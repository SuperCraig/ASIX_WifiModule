/*
 ******************************************************************************
 *     Copyright (c) 2006	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
#ifndef __CLICMD_H__
#define __CLICMD_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */

/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */ 

extern CONSOLE_CmdEntry CLICMD_userCmdTable[];
extern CONSOLE_Account CLICMD_userTable[];

u16 CLICMD_GetCmdTableSize(void);

#endif /* __CLICMD_H__ */

/* End of clicmd.h */