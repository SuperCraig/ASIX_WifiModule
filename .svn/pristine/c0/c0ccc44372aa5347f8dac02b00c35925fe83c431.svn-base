/*
 *********************************************************************************
 *     Copyright (c) 2015	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
 /*============================================================================
 * Module name: ax88796C_if.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __AX88796C_IF_H__
#define __AX88796C_IF_H__
   
#include "lwip/netif.h"
   
void			ax88796cif_low_level_init(struct netif *netif);
err_t			ax88796cif_low_level_output(struct netif *netif, struct pbuf *p);
struct pbuf*	ax88796cif_low_level_input(struct netif *netif);
void			ax88796cif_input(void *param);
//err_t			ax88796cif_init(struct netif *netif);
int				ax88796cif_is_linkup(void);

#endif /* __AX88796C_IF_H__ */
