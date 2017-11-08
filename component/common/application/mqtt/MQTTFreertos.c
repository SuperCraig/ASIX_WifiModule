/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Ian Craggs - convert to FreeRTOS
 *******************************************************************************/

#include "MQTTFreeRTOS.h"
#include "netdb.h"
#include "misc.h"

#define LWIP_IPV6 0
#if LWIP_IPV6
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((src),(dst),(size)) \
     : (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL))
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? inet6_aton((src),(dst)) \
     : (((af) == AF_INET) ? inet_aton((src),(dst)) : 0))
#else /* LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? inet_aton((src),(dst)) : 0)
#endif /* LWIP_IPV6 */

int ThreadStart(Thread* thread, void (*fn)(void*), void* arg)
{
	int rc = 0;
	uint16_t usTaskStackSize = (configMINIMAL_STACK_SIZE * 5);
	UBaseType_t uxTaskPriority = uxTaskPriorityGet(NULL); /* set the priority as the same as the calling task*/

	rc = xTaskCreate(fn,	/* The function that implements the task. */
		"MQTTTask",			/* Just a text name for the task to aid debugging. */
		usTaskStackSize,	/* The stack size is defined in FreeRTOSIPConfig.h. */
		arg,				/* The task parameter, not used in this case. */
		uxTaskPriority,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
		&thread->task);		/* The task handle is not used. */

	return rc;
}


void MutexInit(Mutex* mutex)
{
	mutex->sem = xSemaphoreCreateMutex();
}

int MutexLock(Mutex* mutex)
{
	return xSemaphoreTake(mutex->sem, portMAX_DELAY);
}

int MutexUnlock(Mutex* mutex)
{
	return xSemaphoreGive(mutex->sem);
}


void TimerCountdownMS(Timer* timer, unsigned int timeout_ms)
{
	timer->xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	vTaskSetTimeOutState(&timer->xTimeOut); /* Record the time at which this function was entered. */
}


void TimerCountdown(Timer* timer, unsigned int timeout) 
{
	TimerCountdownMS(timer, timeout * 1000);
}


int TimerLeftMS(Timer* timer) 
{
	xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait); /* updates xTicksToWait to the number left */
	return (timer->xTicksToWait * portTICK_PERIOD_MS);
}


char TimerIsExpired(Timer* timer)
{
	return xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait) == pdTRUE;
}


void TimerInit(Timer* timer)
{
	timer->xTicksToWait = 0;
	memset(&timer->xTimeOut, '\0', sizeof(timer->xTimeOut));
}


int FreeRTOS_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t		xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t		xTimeOut;
	int				recvLen = 0;
	int				rc = 0;
	
	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &xTicksToWait, sizeof(xTicksToWait));
		
		rc = recv(n->my_socket, buffer + recvLen, len - recvLen, 0);
		if (rc > 0)
		{
			recvLen += rc;
		}
		else if (rc < 0)
		{
			recvLen = rc;
			break;
		}
		
	} while (recvLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return recvLen;
}


int FreeRTOS_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t		xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t		xTimeOut;
	int				sentLen = 0;
	int				rc = 0;
	
	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
//		setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &xTicksToWait, sizeof(xTicksToWait));
		rc = send(n->my_socket, buffer + sentLen, len - sentLen, 0);
		if (rc > 0)
		{
			sentLen += rc;
		}
		else if (rc < 0)
		{
			sentLen = rc;
			break;
		}
		
	} while (sentLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return sentLen;
}


void FreeRTOS_disconnect(Network* n)
{
	shutdown(n->my_socket, SHUT_RDWR);
	close(n->my_socket);
	n->my_socket = -1;
}

int FreeRTOS_sslRead(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t		xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t		xTimeOut;
	int				recvLen = 0;
	int				rc = 0;
	
	if (n->sslInited == 0)
		return -1;
	
	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &xTicksToWait, sizeof(xTicksToWait));
		rc = ssl_read(&n->SSL, buffer + recvLen, len - recvLen);
		if (rc > 0)
		{
			recvLen += rc;
		}
		else if (rc < 0)
		{
			recvLen = rc;
			break;
		}
		
	} while (recvLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return recvLen;
}


int FreeRTOS_sslWrite(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t		xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t		xTimeOut;
	int				sentLen = 0;
	int				rc = 0;
	
	if (n->sslInited == 0)
		return -1;
	
	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
//		setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &xTicksToWait, sizeof(xTicksToWait));
		rc = ssl_write(&n->SSL, buffer + sentLen, len - sentLen);	 	  
		if (rc > 0)
		{
			sentLen += rc;
		}
		else if (rc < 0)
		{
			sentLen = rc;
			break;
		}
		
	} while (sentLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return sentLen;
}


void FreeRTOS_sslDisconnect(Network* n)
{
	shutdown(n->my_socket, SHUT_RDWR);
	close(n->my_socket);
	n->my_socket = -1;
	if (n->sslInited)
	{
		ssl_close_notify(&n->SSL);
		ssl_free(&n->SSL);
		n->sslInited = 0;
	}
}

void NetworkInit(Network* n, unsigned char sslEnable)
{
	n->my_socket = -1;
	n->sslEnable = sslEnable;
	n->sslInited = 0;
	  
	if (sslEnable)
	{
		n->mqttread = FreeRTOS_sslRead;
		n->mqttwrite = FreeRTOS_sslWrite;
		n->disconnect = FreeRTOS_sslDisconnect;
	}
	else
	{
		n->mqttread = FreeRTOS_read;
		n->mqttwrite = FreeRTOS_write;
		n->disconnect = FreeRTOS_disconnect;
	}
}


int NetworkConnect(Network* n, char* addr, int port)
{
	struct sockaddr_in sAddr;
	int retVal = -1;
	struct hostent *hptr;
	int	keepalive_enable = 1;
	int keep_idle = 30;
	
	if (n->my_socket >= 0)
	{
		n->disconnect(n);
	}

	/* Host address resolution */
	sAddr.sin_addr.s_addr = inet_addr(addr);
	if (sAddr.sin_addr.s_addr	== IPADDR_NONE)
	{
    	if((hptr = gethostbyname(addr)) == NULL)
		{
			printf("\n\r Fail to get host IP address!!\n");
			goto exit;
		}
    	memcpy((void *)&sAddr.sin_addr, (void *)hptr->h_addr, hptr->h_length);
	}
	else
	{
    	sAddr.sin_len = sizeof(sAddr);	  
	}
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(port);	

	if ((n->my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("\n\r socket failed!!\n");
		goto exit;
	}
	setsockopt( n->my_socket, SOL_SOCKET, SO_KEEPALIVE,
			(const char *) &keepalive_enable, sizeof( keepalive_enable ) );	
	setsockopt( n->my_socket, IPPROTO_TCP, TCP_KEEPIDLE,
			(const char *) &keep_idle, sizeof( keep_idle ) );	
	if ((retVal = connect(n->my_socket, (struct sockaddr*)&sAddr, sizeof(sAddr))) < 0)
	{
		close(n->my_socket);
		n->my_socket = -1;
		printf("\n\r connect failed!!\n");
		goto exit;
	}

	/* Start SSL handshake for securety connection */
	if (n->sslEnable!=0)
	{
		if (n->sslInited)
		{
			ssl_close_notify(&n->SSL);
			ssl_free(&n->SSL);
			n->sslInited = 0;
		}
		memset(&n->SSL, 0, sizeof(ssl_context));
		
		if (ssl_init(&n->SSL) != 0)
		{
			close(n->my_socket);
			n->my_socket = -1;
			printf("\n\r init ssl failed!!\n");
			goto exit;
		}
		ssl_set_endpoint(&n->SSL, SSL_IS_CLIENT);
		ssl_set_authmode(&n->SSL, SSL_VERIFY_NONE);
		ssl_set_rng(&n->SSL, MISC_GetRandom, NULL);
		ssl_set_dbg(&n->SSL, NULL, NULL);
		ssl_set_bio(&n->SSL, net_recv, &n->my_socket, net_send, &n->my_socket);
		n->sslInited = 1;
		while ((retVal = ssl_handshake(&n->SSL)) != 0)
		{
			if (retVal != POLARSSL_ERR_NET_WANT_READ && retVal != POLARSSL_ERR_NET_WANT_WRITE)
			{
				n->disconnect(n);
				printf("\n\r ssl handshake failed!!\n");
				goto exit;
			}
		}
		printf("\n\r establish secure connection successfully!!\n");
	}
	
exit:
	return retVal;
}


#if 0
int NetworkConnectTLS(Network *n, char* addr, int port, SlSockSecureFiles_t* certificates, unsigned char sec_method, unsigned int cipher, char server_verify)
{
	SlSockAddrIn_t sAddr;
	int addrSize;
	int retVal;
	unsigned long ipAddress;

	retVal = sl_NetAppDnsGetHostByName(addr, strlen(addr), &ipAddress, AF_INET);
	if (retVal < 0) {
		return -1;
	}

	sAddr.sin_family = AF_INET;
	sAddr.sin_port = sl_Htons((unsigned short)port);
	sAddr.sin_addr.s_addr = sl_Htonl(ipAddress);

	addrSize = sizeof(SlSockAddrIn_t);

	n->my_socket = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_SEC_SOCKET);
	if (n->my_socket < 0) {
		return -1;
	}

	SlSockSecureMethod method;
	method.secureMethod = sec_method;
	retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECMETHOD, &method, sizeof(method));
	if (retVal < 0) {
		return retVal;
	}

	SlSockSecureMask mask;
	mask.secureMask = cipher;
	retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_MASK, &mask, sizeof(mask));
	if (retVal < 0) {
		return retVal;
	}

	if (certificates != NULL) {
		retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_FILES, certificates->secureFiles, sizeof(SlSockSecureFiles_t));
		if (retVal < 0)
		{
			return retVal;
		}
	}

	retVal = sl_Connect(n->my_socket, (SlSockAddr_t *)&sAddr, addrSize);
	if (retVal < 0) {
		if (server_verify || retVal != -453) {
			sl_Close(n->my_socket);
			return retVal;
		}
	}

	SysTickIntRegister(SysTickIntHandler);
	SysTickPeriodSet(80000);
	SysTickEnable();

	return retVal;
}
#endif
