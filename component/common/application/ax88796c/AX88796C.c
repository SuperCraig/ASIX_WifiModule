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
 * Module Name: ax88796C.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "AX88796C.h"
#include <stdio.h>
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "spi_api.h"
#include "spi_ex_api.h"

/* NAMING CONSTANT DECLARATIONS */
#define SPI0_MOSI  	PC_2
#define SPI0_MISO  	PC_3
#define SPI0_SCLK  	PC_1
#define SPI0_CS  	PC_0
#define SPI0_IRQ   	PC_5

/* EXTERNAL VARIABLES DECLARATIONS */
	
/* GLOBAL VARIABLES DECLARATIONS */

/* LOCAL VARIABLES DECLARATIONS */
static spi_t spi0_master;
static unsigned char ax88796c_txbuf[AX88796C_TX_BUF_SIZE], ax88796c_rxbuf[AX88796C_RX_BUF_SIZE];
static unsigned short seq_num = 0;
static unsigned char ax88796c_tmpbuf[AX88796C_RX_BUF_SIZE];

/* LOCAL SUBPROGRAM DECLARATIONS */
static unsigned short ax88796c_read_reg(unsigned char reg);
static void ax88796c_write_reg(unsigned short value, unsigned char reg);
static int ax88796c_write_fifo_pio(unsigned char *buf, unsigned short count);
static int ax88796c_read_fifo_pio(unsigned char *buf, unsigned short count);
static unsigned short ax88796c_mdio_read(unsigned short phy_id, unsigned short loc);
static void ax88796c_mdio_write(unsigned short phy_id, unsigned short loc, unsigned short val);
static void ax88796c_phy_init(void);
static void ax88796c_timeout_init(unsigned long *recordtime);
static int ax88796c_check_timeout(unsigned long recordtime, int timeout);
static int ax88796c_check_free_pages(unsigned char need_pages);

/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_timeout_init()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void ax88796c_timeout_init(unsigned long *recordtime)
{
	if (recordtime)
	{
		(*recordtime) = xTaskGetTickCount();	  
	}
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_check_timeout()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static int ax88796c_check_timeout(unsigned long recordtime, int timeout)
{
	unsigned long currtime = xTaskGetTickCount();
	unsigned long elapse_time;
	
	if (currtime >= recordtime)
	{
		elapse_time = currtime - recordtime;
	}
	else
	{
		elapse_time = portMAX_DELAY - recordtime + currtime;
	}
			
	if ((elapse_time * portTICK_RATE_MS) >= timeout)
	{
		return 1;
	}  
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_check_free_pages()
 * Purpose : Check free pages of TX buffer
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static int ax88796c_check_free_pages(unsigned char need_pages)
{
	u8 free_pages;
	u16 tmp;

	free_pages = ax88796c_read_reg(P0_TFBFCR) & TX_FREEBUF_MASK;
	if (free_pages < need_pages) 
	{
		return -1;
	}

	return 0; 
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_read_reg()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static unsigned short ax88796c_read_reg(unsigned char reg)
{
	unsigned char txbuf[6], rxbuf[6];

	memset(txbuf, 0, 6);
	memset(rxbuf, 0, 6);

	txbuf[0] = AX_SPICMD_READ_REG;
	txbuf[1] = reg;
	txbuf[2] = 0xff;
	txbuf[3] = 0xff;
	
	spi_flush_rx_fifo(&spi0_master);
	spi_master_write_read_stream(&spi0_master, txbuf, rxbuf, 6);	
	while (spi_busy(&spi0_master));
	return (unsigned short)((rxbuf[5]<<8) + rxbuf[4]);
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_write_reg()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void ax88796c_write_reg(unsigned short value, unsigned char reg)
{
	unsigned char txbuf[4];

	txbuf[0] = AX_SPICMD_WRITE_REG;
	txbuf[1] = reg;
	txbuf[2] = (u8)value;
	txbuf[3] = (u8)(value >> 8);
	
	spi_master_write_stream(&spi0_master, txbuf, 4);
	while (spi_busy(&spi0_master));
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_write_fifo_pio()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static int ax88796c_write_fifo_pio(unsigned char *buf, unsigned short count)
{
  	int ret;
	unsigned short len;
	
	buf[0] = AX_SPICMD_WRITE_TXQ;
	buf[1] = 0xff;
	buf[2] = 0xff;
	buf[3] = 0xff;
 	len = count + AX88796C_FIFO_WRITE_OFFSET;

	ret = spi_master_write_stream_dma(&spi0_master, buf, len);
	while (spi_busy(&spi0_master));
	return ret;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_read_fifo_pio()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static int ax88796c_read_fifo_pio(unsigned char *buf, unsigned short count)
{
	int ret;
	unsigned short len;
	
	ax88796c_tmpbuf[0] = AX_SPICMD_READ_RXQ;
	ax88796c_tmpbuf[1] = 0xff;
	ax88796c_tmpbuf[2] = 0xff;
	ax88796c_tmpbuf[3] = 0xff;
	ax88796c_tmpbuf[4] = 0xff;
 	len = count + AX88796C_FIFO_READ_OFFSET;
	
	spi_flush_rx_fifo(&spi0_master);
	ret = spi_master_write_read_stream_dma(&spi0_master, ax88796c_tmpbuf, buf, len);
	while (spi_busy(&spi0_master));	
	return ret;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_mdio_read()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static unsigned short ax88796c_mdio_read(unsigned short phy_id, unsigned short loc)
{
	unsigned long tmp32;

	ax88796c_write_reg((MDIOCR_RADDR(loc) | MDIOCR_FADDR(phy_id) | MDIOCR_READ), P2_MDIOCR);

	ax88796c_timeout_init(&tmp32);		
	while ((ax88796c_read_reg(P2_MDIOCR) & MDIOCR_VALID) == 0)	
	{
		if (ax88796c_check_timeout(tmp32, AX88796C_OP_TIMEOUT))
		{
			AX88796C_PRINT("Wait MDIO read timeout\r\n");	  			
			break;
		}
	}
	return (ax88796c_read_reg(P2_MDIODR));
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_mdio_write()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void ax88796c_mdio_write(unsigned short phy_id, unsigned short loc, unsigned short val)
{
	unsigned long tmp32;

	ax88796c_write_reg(val, P2_MDIODR);
	ax88796c_write_reg((MDIOCR_RADDR(loc) | MDIOCR_FADDR(phy_id) | MDIOCR_WRITE), P2_MDIOCR);

	ax88796c_timeout_init(&tmp32);		
	while ((ax88796c_read_reg(P2_MDIOCR) & MDIOCR_VALID) == 0)	
	{
		if (ax88796c_check_timeout(tmp32, AX88796C_OP_TIMEOUT))
		{
			AX88796C_PRINT("Wait MDIO write timeout\r\n");	  
			break;
		}
	}
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_phy_init()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ax88796c_phy_init(void)
{
	unsigned short tmp16;

	/* Setup LED mode */
	ax88796c_write_reg((LCR_LED0_EN | LCR_LED0_DUPLEX | LCR_LED1_EN | LCR_LED1_100MODE), P2_LCR0);

	tmp16 = ax88796c_read_reg(P2_LCR1);
	ax88796c_write_reg((tmp16 & LCR_LED2_MASK) | LCR_LED2_EN | LCR_LED2_LINK, P2_LCR1);

	tmp16 = (POOLCR_PHYID(0x10) | POOLCR_POLL_EN | POOLCR_POLL_FLOWCTRL | POOLCR_POLL_BMCR);
	ax88796c_write_reg(tmp16, P2_POOLCR);

	ax88796c_mdio_write(AX88796C_PHY_ID, PHY_REG_AN_ADVERTISEMENT, SPEED_DUPLEX_AUTO);
	ax88796c_mdio_write(AX88796C_PHY_ID, PHY_REG_BASIC_MODE_CTRL, RESTART_AUTONEG);
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_spi_init()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ax88796c_spi_init(void)
{
	/* SPI hardware configuration */
    spi_init(&spi0_master, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi0_master, 8, 3, 0);
	spi_frequency(&spi0_master, AX88796C_SPI_SPEED);
#if AX88796C_USE_CS2
	spi_slave_select(&spi0_master, CS_2);
#else
	spi_slave_select(&spi0_master, CS_0);
#endif
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_init()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
signed short ax88796c_init(unsigned char *phwaddr)
{
	unsigned short tmp16;
	unsigned long tmp32;
	
	memset(ax88796c_tmpbuf, 0, AX88796C_RX_BUF_SIZE);	
	
	/* Chip Reset */
	ax88796c_write_reg(PSR_RESET, P0_PSR);	
	tmp32 = 10;
	while (tmp32--);
	ax88796c_write_reg(PSR_RESET_CLR, P0_PSR);

	/* Make sure AX88796C is ready */
	tmp16 = 5;		
	while (1)
	{
		if ((ax88796c_read_reg(P0_BOR) == 0x1234) && (ax88796c_read_reg(P0_PSR) & PSR_DEV_READY))
		{
			break;
		}
		else if (tmp16 == 0)
		{
			AX88796C_PRINT("Fail to wait for chip ready.\r\n");
			return -1;
		}
		else
		{
			ax88796c_write_reg(PSR_RESET, P0_PSR);	
			tmp32 = 10;	
			while (tmp32--);
			ax88796c_write_reg(PSR_RESET_CLR, P0_PSR);
		}
		tmp16--;
		vTaskDelay(100);		
	}
	/* Set sequence number */
	seq_num = (ax88796c_read_reg(P0_TSNR) & 0x1F);

	/* Register compresion = 0 */
	//tmp16 = ax88796c_read_reg(P4_SPICR) & (~(SPICR_RCEN | SPICR_QCEN));
	tmp16 = 0;
	ax88796c_write_reg(tmp16, P4_SPICR);

	/* Set mac for AX88796C chip */
	ax88796c_set_mac_addr(phwaddr);

	/* Enable crc checksum supporting */
	ax88796c_write_reg((COERCR0_RXIPCE | COERCR0_RXTCPE | COERCR0_RXUDPE), P4_COERCR0);
	ax88796c_write_reg((COETCR0_TXIP | COETCR0_TXTCP | COETCR0_TXUDP), P4_COETCR0);

	/* Disable stuffing packet */
	tmp16 = ax88796c_read_reg(P1_RXBSPCR) & (~RXBSPCR_STUF_ENABLE);
	ax88796c_write_reg(tmp16, P1_RXBSPCR);

	/* Enable RX packet process */
	ax88796c_write_reg(RPPER_RXEN, P1_RPPER);

	/* Set INT pin */
	tmp16 = ax88796c_read_reg(P0_FER) | FER_RXEN | FER_TXEN | FER_BSWAP | FER_IRQ_PULL | FER_INTLO;
	ax88796c_write_reg(tmp16, P0_FER);

	/* Set PHY */
	ax88796c_phy_init();
  
	/* Enable interrupt */
	ax88796c_write_reg(~(IMR_LINK | IMR_RXPKT), P0_IMR);
	
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_set_mac_addr()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ax88796c_set_mac_addr(unsigned char *phwaddr)
{
	unsigned short tmp16;

	tmp16 = ((unsigned short)(phwaddr[4] << 8) | (unsigned short)(phwaddr[5]));
	ax88796c_write_reg(tmp16, P3_MACASR0);

	tmp16 = ((unsigned short)(phwaddr[2] << 8) | (unsigned short)(phwaddr[3]));
	ax88796c_write_reg(tmp16, P3_MACASR1);

	tmp16 = ((unsigned short)(phwaddr[0] << 8) | (unsigned short)(phwaddr[1]));
	ax88796c_write_reg(tmp16, P3_MACASR2);
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_load_mac_addr()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ax88796c_load_mac_addr(unsigned char *phwaddr)
{
	unsigned short tmp16;

	tmp16 = ax88796c_read_reg(P3_MACASR0);
	phwaddr[5] = (unsigned char) tmp16;
	phwaddr[4] = (unsigned char)(tmp16 >> 8);

	tmp16 = ax88796c_read_reg(P3_MACASR1);
	phwaddr[3] = (unsigned char) tmp16;
	phwaddr[2] = (unsigned char)(tmp16 >> 8);

	tmp16 = ax88796c_read_reg(P3_MACASR2);
	phwaddr[1] = (unsigned char) tmp16;
	phwaddr[0] = (unsigned char)(tmp16 >> 8);
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_pkt_send()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
signed short ax88796c_pkt_send(struct buf_list *plist, unsigned char list_depth, unsigned short pkt_len)
{
	unsigned char i, align_count, need_pages;
	unsigned short tmp16, len_bar;
	unsigned char *ptxbuf = ax88796c_txbuf + AX88796C_FIFO_WRITE_OFFSET;
#if AX88796C_DEBUG_ENABLE
	struct tx_sop_header		sop_hdr = {0,0};
	struct tx_segment_header	seg_hdr = {0,0};
	struct tx_eop_header		eop_hdr = {0,0};
#endif
	
	/* Check target buffer is valid */	
	if (plist == 0 || 
		list_depth == 0 ||
		(AX88796C_FIFO_WRITE_OFFSET + AX88796C_TX_SOP_SEG_HDR_LEN + pkt_len + 4 + AX88796C_TX_EOP_HDR_LEN) >= AX88796C_TX_BUF_SIZE)	
	{
		AX88796C_PRINT("Wrong tx buffer\r\n");
		return -1;	
	}

	/* Checl tx free buffer */
	need_pages = ((pkt_len + AX88796C_TX_SOP_SEG_HDR_LEN - 1) >> AX88796C_PAGE_SHIFT) + 1;
	if (ax88796c_check_free_pages(need_pages) < 0)
	{
		AX88796C_PRINT("Insufficient TX free page\r\n");	 
		return -1;
	}

	len_bar = (~pkt_len & TX_HDR_SOP_PKTLENBAR);
	/* Build SOP header */
	*((unsigned short*)&ptxbuf[0]) = SWAP16(pkt_len);
	*((unsigned short*)&ptxbuf[2]) = SWAP16(TX_HDR_SEQNUM(seq_num) | len_bar);
#if AX88796C_DEBUG_ENABLE
	sop_hdr.flags_pktlen = *((unsigned short*)&ptxbuf[0]);
	sop_hdr.seqnum_pktlenbar = *((unsigned short*)&ptxbuf[2]);
#endif
			
	/* Build Segment header */
	*((unsigned short*)&ptxbuf[4]) = SWAP16(TX_HDR_SEG_FS | TX_HDR_SEG_LS | pkt_len);
	*((unsigned short*)&ptxbuf[6]) = SWAP16(len_bar);
#if AX88796C_DEBUG_ENABLE
	seg_hdr.f_segnum_seglen = *((unsigned short*)&ptxbuf[4]);
	seg_hdr.eo_so_seglenbar = *((unsigned short*)&ptxbuf[6]);
#endif	
	ptxbuf += AX88796C_TX_SOP_SEG_HDR_LEN;
	
	/* Fill packet content */
	for (i=0; i<list_depth; i++)
	{
		memcpy(ptxbuf, plist[i].pbuf, plist[i].len);
		ptxbuf += plist[i].len;
	}

	/* Build padding for double-word alignment */		  
	align_count = ((pkt_len + 3) & 0x7FC) - pkt_len;
	if (align_count)
	{
		memset(ptxbuf, 0xff, align_count);	  
  		ptxbuf += align_count;
	}
	
	/* Build EOP header */
	*((unsigned short*)&ptxbuf[0]) = SWAP16(TX_HDR_SEQNUM(seq_num) | pkt_len);
	*((unsigned short*)&ptxbuf[2]) = SWAP16(TX_HDR_SEQNUM(~seq_num) | len_bar);
#if AX88796C_DEBUG_ENABLE
	eop_hdr.seqnum_pktlen = *((unsigned short*)&ptxbuf[0]);
	eop_hdr.seqnumbar_pktlenbar = *((unsigned short*)&ptxbuf[2]);
#endif	
	ptxbuf += AX88796C_TX_EOP_HDR_LEN;
	
	
	/* Write TX end of packet */
	ax88796c_write_reg((TSNR_TXB_START | TSNR_PKT_CNT(1)), P0_TSNR);
	ax88796c_write_fifo_pio(ax88796c_txbuf, AX88796C_TX_SOP_SEG_HDR_LEN + pkt_len + align_count + AX88796C_TX_EOP_HDR_LEN);
	
	/* Check result */
	tmp16 = ax88796c_read_reg(P0_PSR);
	if ((tmp16 & PSR_DEV_READY) == 0)
	{
		AX88796C_PRINT("Check device ready fail, P0_PSR=0x%04x\r\n", tmp16);		
		goto low_level_tx_err;
	}
	
	/* Check TX bridge write operation is done */
	tmp16 = ax88796c_read_reg(P0_TSNR);
	if ((tmp16 & TXNR_TXB_IDLE) == 0)	
	{
		AX88796C_PRINT("Check TX bridge idle fail(%d), P0_TSNR=0x%04x\r\n", pkt_len, tmp16);	  			
		goto low_level_tx_err;	  
	}

	tmp16 = ax88796c_read_reg(P0_ISR);
	if (tmp16 & ISR_TXERR)
	{
		AX88796C_PRINT("TX error, P0_ISR=0x%04x\r\n", tmp16);
		goto low_level_tx_err;
	}
	/* Increase sequence number */
	seq_num++;
	seq_num &= 0x1f;
	return 0;

low_level_tx_err:
	AX88796C_PRINT("pkt_len=0x%04x, len_bar=0x%04x, seq_num=0x%04x\r\n", pkt_len, len_bar, seq_num);  
#if AX88796C_DEBUG_ENABLE
	for (tmp16 = 0; tmp16 < 60; tmp16++)
	{
		printf("%s%02x%s", (tmp16==0)?"TxPkt: ":"-", ax88796c_txbuf[tmp16], (tmp16==59)?"\r\n":"");
	}
#endif	
	AX88796C_PRINT("TxSop flags_pktlen=0x%04x, seqnum_pktlenbar=0x%04x\r\n", sop_hdr.flags_pktlen, sop_hdr.seqnum_pktlenbar);	
	AX88796C_PRINT("TxSeg f_segnum_seglen=0x%04x, eo_so_seglenbar=0x%04x\r\n", seg_hdr.f_segnum_seglen, seg_hdr.eo_so_seglenbar);	
	AX88796C_PRINT("TxEop seqnum_pktlen=0x%04x, seqnumbar_pktlenbar=0x%04x\r\n", eop_hdr.seqnum_pktlen, eop_hdr.seqnumbar_pktlenbar);		
	
	ax88796c_write_reg(TXNR_TXB_REINIT, P0_TSNR);
	seq_num	= 0;
	return -1;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_check_rev_pkt_count()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
signed short ax88796c_check_rev_pkt_count(unsigned short mtu)
{
	unsigned short tmp16;
	
	/* Latch RX bridge first */  
	tmp16 = ax88796c_read_reg(P0_RTWCR) | RTWCR_RX_LATCH;
	ax88796c_write_reg(tmp16, P0_RTWCR);
	
	/* Get received packet count */  	
	if ((ax88796c_read_reg(P0_RXBCR2) & RXBCR2_PKT_MASK) == 0)
	{
		return 0;
	}

	/* Get size of topest packet */
	tmp16 = ax88796c_read_reg(P0_RCPHR);
	if (tmp16 & RX_HDR_ERROR)
	{
		/* Skip the packet which has header error */
		ax88796c_write_reg(RXBCR1_RXB_DISCARD, P0_RXBCR1);
		return 0;
	}

	/* Check packet length */
	tmp16 &= 0x7FF;
//	if ((tmp16 < 60) || (tmp16 > mtu))	
	if ((tmp16 < 60))		
	{
		/* Discard the packet which length is invalid */	  
		ax88796c_write_reg(RXBCR1_RXB_DISCARD, P0_RXBCR1);
//		AX88796C_PRINT("Packet size is either larger than %d or small 60(%d).\n", mtu, tmp16);
		AX88796C_PRINT("Packet size is small 60(%d)\r\n", tmp16);
		return 0;
	}
	
	return tmp16;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_pkt_recv()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
signed short ax88796c_pkt_recv(struct buf_list *plist, unsigned char list_depth, unsigned short pkt_len)
{
	unsigned short tmp16;
	unsigned char i, *prxbuf = ax88796c_rxbuf + AX88796C_FIFO_READ_OFFSET + AX88796C_RX_HDR_LEN;

	/* Check target buffer is valid */
	if (plist == 0 || 
		list_depth == 0 ||
		(AX88796C_FIFO_READ_OFFSET + AX88796C_RX_HDR_LEN + pkt_len + 4) >= AX88796C_RX_BUF_SIZE)
	{
		ax88796c_write_reg(RXBCR1_RXB_DISCARD, P0_RXBCR1);/* Discard received packet which size bigger than RX buffer */
		AX88796C_PRINT("Wrong rx buffer\r\n");	  
		return -1;
	}
	
	/* Start RX bridge for data transfer */
	ax88796c_write_reg((RXBCR1_RXB_START | (pkt_len >> 1)), P0_RXBCR1);
	tmp16 = ax88796c_read_reg(P0_RXBCR2);
	if ((tmp16 & RXBCR2_RXB_READY) == 0)	
	{
		ax88796c_write_reg(RXBCR1_RXB_DISCARD, P0_RXBCR1);
		AX88796C_PRINT("Check RX bridge ready fail(%d), P0_RXBCR2=0x%04x\r\n", pkt_len, tmp16);	  			
		return -1;
	}

	/* Read full packet (includes header + packet content) */
    ax88796c_read_fifo_pio(ax88796c_rxbuf, pkt_len);
	
	/* Read packet content */
    for (i=0; i<list_depth; i++)
    {
		memcpy(plist[i].pbuf, prxbuf, plist[i].len);
		prxbuf += plist[i].len;
	}
	
	/* Check RX bridge read operation is done */
	tmp16 = ax88796c_read_reg(P0_RXBCR2);
	if ((tmp16 & RXBCR2_RXB_IDLE) == 0)	
	{
		ax88796c_write_reg(RXBCR2_RXB_REINIT, P0_RXBCR2);
		AX88796C_PRINT("Check RX bridge idle fail(%d), P0_RXBCR2=0x%04x\r\n", pkt_len, tmp16);	
#if AX88796C_DEBUG_ENABLE
		for (tmp16 = 0; tmp16 < 60; tmp16++)
		{
			printf("%s%02x%s", (tmp16==0)?"RxPkt: ":"-", ax88796c_rxbuf[tmp16], (tmp16==59)?"\r\n":"");
		}
#endif
		return 0;/* Forward received packet to uplayer even if RX bridge is not idle */
	}
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_pkt_drop()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ax88796c_pkt_drop(unsigned short pkt_len)
{
	unsigned char *prxbuf = ax88796c_rxbuf + AX88796C_FIFO_READ_OFFSET + AX88796C_RX_HDR_LEN;

	if ((AX88796C_FIFO_READ_OFFSET + AX88796C_RX_HDR_LEN + pkt_len + 4) >= AX88796C_RX_BUF_SIZE)
	{
		ax88796c_write_reg(RXBCR1_RXB_DISCARD, P0_RXBCR1);
		AX88796C_PRINT("Wrong pkt_len > %d\r\n", AX88796C_RX_BUF_SIZE);	  
		return;
	}

	/* Start RX bridge for data transfer */
	ax88796c_write_reg((RXBCR1_RXB_START | (pkt_len >> 1)), P0_RXBCR1);
	if ((ax88796c_read_reg(P0_RXBCR2) & RXBCR2_RXB_READY) == 0)
	{
		ax88796c_write_reg(RXBCR1_RXB_DISCARD, P0_RXBCR1);
		AX88796C_PRINT("Check RX bridge ready fail2\r\n");
		return;
	}

	/* Read full packet (includes header + packet content) */
    ax88796c_read_fifo_pio(ax88796c_rxbuf, pkt_len);

	/* Check RX bridge read operation is done */
	if ((ax88796c_read_reg(P0_RXBCR2) & RXBCR2_RXB_IDLE) == 0)
	{
		ax88796c_write_reg (RXBCR2_RXB_REINIT, P0_RXBCR2);
		AX88796C_PRINT("Check RX bridge idle fail2\r\n");
		return;
	}
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_get_event()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
AX88796C_EVENT_TYPES ax88796c_get_event(void)
{
	unsigned short isr, events = 0;

	isr = ax88796c_read_reg(P0_ISR);
	
	if (isr)
	{
		ax88796c_write_reg(isr, P0_ISR);/* Clear event flag */
	
		if (isr & ISR_RXPKT)
		{
			events |= EVENT_RX_PKT;
		}
	
		if (isr & ISR_LINK)
		{
			if (ax88796c_mdio_read(AX88796C_PHY_ID, PHY_REG_BASIC_MODE_STATUS) & 0x0004)
			{
				events |= EVENT_LINK_UP;
			}
			else
			{
				events |= EVENT_LINK_DOWN;		  
			}
		}
	}
	return events;
}

/*
 * ----------------------------------------------------------------------------
 * Function: ax88796c_get_mac_cfg()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ax88796c_get_mac_cfg(unsigned char *full_duplex, unsigned short *speed, unsigned char *rx_flow_ctrl, unsigned char *tx_flow_ctrl)
{
	unsigned short mac_cfg;

	mac_cfg = ax88796c_read_reg(P0_MACCR);
	
	*full_duplex = (mac_cfg & MACCR_DUPLEXFULL) ? 1:0;
	*speed = (mac_cfg & MACCR_SPEED100) ? 100:10;	
	*rx_flow_ctrl = (mac_cfg & MACCR_RXFC_ENABLE) ? 1:0;		
	*tx_flow_ctrl = (mac_cfg & MACCR_TXFC_ENABLE) ? 1:0;
}

/* End of ax88796c.c */