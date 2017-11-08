/*
 *********************************************************************************
 *     Copyright (c) 2016	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
 /*============================================================================
 * Module name: pl7223.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef _PL7223_H_
#define _PL7223_H_

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
/* Task setting */
#define PL7223_PRINT_HEADER		"PL7223: "
#define PL7223_DEBUG			0

/* DSP version checking */
#define PL7223_DSP_VER1				0x03
#define PL7223_DSP_VER2				0x04
#define PL7223_DSP_CHK_MAX_TIMES	10

/* SPI and related timing setting */
#define PL7223_SPI_CLK			100000	// 100KHz
#define PL7223_DELAY_CNT_MULP	((800000 / PL7223_SPI_CLK) * 20)
#define PL7223_INIT_DELAY_CNT	12000	// counts
#define PL7223_RESET_T1_CNT		500		// counts
#define PL7223_RESET_T2_CNT		300		// counts
#define PL7223_WRITE_DELAY_CNT	3		// counts

/* Command type */
#define	READ_PL7223				0x4000
#define	WRITE_PL7223			0x8000
#define	DSPSTATUS_PL7223		0xF000

/* Dummy data */
#define DUM_PL7223				0x00

/* Status check or bit operation */
#define PL7223_DSP_READY_MASK	0x80
#define PL7223_RELAY_MASK		0x20

/* DSP Buffer Mapping Address */
#define PL7223_DBMA_VRMS		0x3000
#define PL7223_DBMA_SAMPLE_CNT	0x3809
#define PL7223_DBMA_RELAY		0x380F
#define PL7223_DBMA_DSP_VER		0x3860

/* MACRO DECLARATIONS */
#define PL7223_Display(name)	printf(#name" %d.%d\n\r", (int)(name * 1000) / 1000, (int)(name * 1000) % 1000)

/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void pl7223_Init();
unsigned char pl7223_IsReadyNow(void);
void pl7223_Send(unsigned char);
void pl7223_ReadStatus(void);
void pl7223_Delay(int);
void pl7223_Reset(void);
void pl7223_Read(unsigned char*, unsigned int, unsigned int);
void pl7223_Write(unsigned char*, unsigned int, unsigned int);
void pl7223_Masurement(void);
void pl7223_RelayControl(int);

#endif /* _PL7223_H_ */

