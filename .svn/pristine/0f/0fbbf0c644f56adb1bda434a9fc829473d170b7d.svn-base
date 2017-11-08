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
 /*============================================================================
 * Module Name: email.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * no message
 *
 *=============================================================================
 */

#ifndef __EMAIL_H__
#define __EMAIL_H__

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"
#include "smtpc.h"
   
/* NAMING CONSTANT DECLARATIONS */
   
/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
s16 EMAIL_SendColdStart(void);
SMTPC_MSG_BUILD_STATUS EMAIL_BuildColdStart(SMTPC_REQUEST *pReq);
s16 EMAIL_SendAuthenFail(void);
SMTPC_MSG_BUILD_STATUS EMAIL_BuildAuthenFail(SMTPC_REQUEST *pReq);
s16 EMAIL_SendIpChanged(void);
SMTPC_MSG_BUILD_STATUS EMAIL_BuildIpChanged(SMTPC_REQUEST *pReq);
s16 EMAIL_SendPasswordChanged(void);
SMTPC_MSG_BUILD_STATUS EMAIL_BuildPasswordChanged(SMTPC_REQUEST *pReq);

#endif /* End of __EMAIL_H__ */


/* End of email.h */
