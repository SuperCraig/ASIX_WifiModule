/*
 ******************************************************************************
 *     Copyright (c) 2015	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
/*=============================================================================
 * Module Name: ax88796C_if.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "AX88796C.h"
#include "AX88796C_if.h"
#include <stdio.h>
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "netif/etharp.h"
#include "wifi_conf.h"
#include "misc.h"
#include "gconfig.h"
#include "ethernetif.h"

extern struct netif xnetif[NET_IF_NUM]; 

/* NAMING CONSTANT DECLARATIONS */
#define AX88796CIF_MAX_BUF_LIST_DEPTH	32
#define IFNAME0         'e'
#define IFNAME1         '2'

/* GLOBAL VARIABLES DECLARATIONS */

/* LOCAL VARIABLES DECLARATIONS */
static struct buf_list tx_buf_list[AX88796CIF_MAX_BUF_LIST_DEPTH], rx_buf_list[AX88796CIF_MAX_BUF_LIST_DEPTH];
static u8 AX88796IF_ready = 0;
static u8 AX88796IF_linkupStatus = 0;
static xSemaphoreHandle ax88796cif_mutex = NULL;

/* LOCAL SUBPROGRAM DECLARATIONS */

/* LOCAL SUBPROGRAM BODIES */

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void ax88796cif_low_level_init(struct netif *netif)
{
	ax88796cif_mutex = xSemaphoreCreateMutex();
	
	/* set netif MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	
	/* set netif MAC hardware address */
	MISC_GetMacAddr(netif->hwaddr);
	
	/* set netif maximum transfer unit */
	netif->mtu = 1500;

	/* Accept broadcast address and ARP traffic */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	/* initialize hardware */
	if (ax88796c_init(netif->hwaddr) < 0)
		return;
	
	AX88796IF_ready = 1;
	
	if(xTaskCreate(ax88796cif_input, "ax796if_input", 1024, netif, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
	{
		printf("AX88796CIF: Create input process task fail!\r\n");
	}
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 */
err_t ax88796cif_low_level_output(struct netif *netif, struct pbuf *p)
{
	err_t ret = ERR_OK;
	struct pbuf *q;	
	unsigned short tx_buf_depth = 0;
	
	if (AX88796IF_ready == 0)
		return ERR_IF;
	
	for (q = p; (q != NULL) && (tx_buf_depth < AX88796CIF_MAX_BUF_LIST_DEPTH); q = q->next)
	{
		tx_buf_list[tx_buf_depth].pbuf = (unsigned char*) q->payload;
		tx_buf_list[tx_buf_depth++].len = q->len;
	}
	
	if (tx_buf_depth)
	{
		while(xSemaphoreTake(ax88796cif_mutex, portMAX_DELAY) != pdTRUE);
		if (ax88796c_pkt_send(tx_buf_list, tx_buf_depth, p->tot_len) < 0)
		{
			printf("AX88796CIF: Send packet fail\r\n");		  
			ret = ERR_BUF;		  
		}
		xSemaphoreGive(ax88796cif_mutex);		
	}
	return ret;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
struct pbuf *ax88796cif_low_level_input(struct netif *netif)
{
	struct pbuf *p = NULL, *q;
	unsigned short rx_buf_depth = 0;
	unsigned short pkt_len, tmp16;

	while(xSemaphoreTake(ax88796cif_mutex, portMAX_DELAY) != pdTRUE);
	pkt_len = ax88796c_check_rev_pkt_count(netif->mtu);
	if (pkt_len)
	{
		/* long word alignment */
		tmp16 = (pkt_len + AX88796C_RX_HDR_LEN + 3) & 0xFFFC;
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
		if (MISC_GetNetworkBackupSwitchStatus() == NETWORK_SWITCH_TO_WIFI_IF)
		{		
			ax88796c_pkt_drop(tmp16);
			xSemaphoreGive(ax88796cif_mutex);			
			return NULL;
		}
#endif		  
		p = pbuf_alloc(PBUF_RAW, tmp16, PBUF_POOL);
		if (p)
		{
			for (q = p; (q != NULL) && (rx_buf_depth < AX88796CIF_MAX_BUF_LIST_DEPTH); q = q->next)
			{
				rx_buf_list[rx_buf_depth].pbuf = (unsigned char*) q->payload;
				rx_buf_list[rx_buf_depth++].len = q->len;
			}
	  
			if (ax88796c_pkt_recv(rx_buf_list, rx_buf_depth, tmp16) < 0)
			{
				pbuf_free(p);
		        p=NULL;
				printf("AX88796CIF: Rcv packet fail\r\n");		  			  
			}
		}
		else
		{
			ax88796c_pkt_drop(tmp16);
			printf("AX88796CIF: Buffer allocation fail, dropped packet!\n");		  
		}
	}
	xSemaphoreGive(ax88796cif_mutex);	
	return p;
}

/**
 * This function is the ax88796cif_input task, it is processed when a packet 
 * is ready to be read from the interface. It uses the function ax88796cif_low_level_input() 
 * that should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 */
void ax88796cif_input(void *param)
{
	struct pbuf *p;  
	AX88796C_EVENT_TYPES events;
	unsigned char full_duplex, rx_flow_ctrl, tx_flow_ctrl, tmp8;
	unsigned short speed;
	struct netif *netif = param;
	
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	int err, backupMode = GCONFIG_GetNetworkBackupMode();
	
	/* Enable network backup switcher in MISC module */
	MISC_SetNetworkBackupMode(backupMode);
#endif
	
	while (1)
	{
		vTaskDelay(2);

		/* Get events */
		while(xSemaphoreTake(ax88796cif_mutex, portMAX_DELAY) != pdTRUE);
		events = ax88796c_get_event();
		xSemaphoreGive(ax88796cif_mutex);
		
		/* Received packets process */
		if (events & EVENT_RX_PKT)
		{
			tmp8 = 5;

			while (p = ax88796cif_low_level_input(netif))
			{
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
				if (ETHERNETIF_LockNetworkSwitchSection())
				{
					err = netif->input(p, netif);
					ETHERNETIF_UnLockNetworkSwitchSection();
					if (err != ERR_OK)
					{
						pbuf_free(p);
					    p=NULL;
						printf("AX88796CIF: Input packet error!\r\n");
					}
				}
				else
				{
					pbuf_free(p);
					p=NULL;
				}
#else
				if (netif->input(p, netif) != ERR_OK)
				{
					pbuf_free(p);
				    p=NULL;
					printf("AX88796CIF: Input packet error!\r\n");						  
				}
#endif				
				if (!(tmp8--))
				{
					break;
				}
			}
		}
		
		/* Link change process */				
		if (events & EVENT_LINK_UP)
		{
			AX88796IF_linkupStatus = 1;
			while(xSemaphoreTake(ax88796cif_mutex, portMAX_DELAY) != pdTRUE);
			ax88796c_get_mac_cfg(&full_duplex, &speed, &rx_flow_ctrl, &tx_flow_ctrl);
			xSemaphoreGive(ax88796cif_mutex);
			printf("AX88796CIF: Link Up! %s duplex, %dM, RFC/TFC=%s/%s\r\n"
					 , full_duplex ? "full":"half"
					 , speed
					 , rx_flow_ctrl ? "enable":"disable"
					 , tx_flow_ctrl ? "enable":"disable");
		}
		else if (events & EVENT_LINK_DOWN)
		{
			AX88796IF_linkupStatus = 0;	  
			printf("AX88796CIF: Link Down!\r\n");
		}
	}
}
#if 0
/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function ax88796cif_low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ax88796cif_init(struct netif *netif)
{
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
//	netif->hostname = "lwip2";
	netif->hostname = (char*)GCONFIG_GetDeviceName();
#endif /* LWIP_NETIF_HOSTNAME */

	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;

  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */	
	netif->output     = etharp_output;
	netif->linkoutput = ax88796cif_low_level_output;

	/* initialize the hardware */
	ax88796cif_low_level_init(netif);

	return ERR_OK;
}
#endif
int ax88796cif_is_linkup(void)
{
	return AX88796IF_linkupStatus;
}

/* End of ax88796cif.c */