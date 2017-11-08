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
 * Module name: console.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */
#ifndef _CONSOLE_HEADER_
#define _CONSOLE_HEADER_

#define CLI_MAX_COMMAND_LEN 80
#define CLI_MAX_CMD_LEN 32
#define CLI_MAX_MESSAGE_LEN 128
#define CLI_MAX_RETRIES 3
#define MAX_USER_ACCOUNT 1
#define MAX_USERNAME_LEN 32
#define MAX_PASSWD_LEN 12
#define CLI_MAX_ARGS 16
#define CLI_MAX_PROMSTR 12
#define CLI_CLOSED -1 
#define CLI_TIMEOUT -2

typedef struct _CLICtl_ 
{
    int (*putChar)(char c);   /* write one character */
    int (*getChar)(int ms);   /* read one character */  
    int llaId;                /* LLA identification */
	char promStr[CLI_MAX_PROMSTR];
} CONSOLE_CLICtl;

enum LLAID_Ind 
{
    LLA_UART,
    LLA_TELNET,
    LLA_MAX
};

/* terminal code */
#define CTRL_C  0x03
#define BACKSP  0x08
#define BELL    0x07
#define CLI_CR      0x0d
#define CLI_LF      0x0a
#define CLI_ECHO_ON "\010 \010"

typedef int (*CmdPtr)(CONSOLE_CLICtl *icb, int argc, char **argv);
typedef int (*HelpPtr)(CONSOLE_CLICtl *icb);

typedef struct _CmdEntry_
{
    char cmd[CLI_MAX_CMD_LEN];
    CmdPtr cmdFunc;
    HelpPtr help;
    int level;
} CONSOLE_CmdEntry;

typedef struct _UserAccount_
{
    char Name[MAX_USERNAME_LEN];
    char Passwd[MAX_PASSWD_LEN];
    char Level;
} CONSOLE_Account;

int CONSOLE_PutChar(char inChar);
int CONSOLE_GetChar(int ms);
int CONSOLE_PutMessage(CONSOLE_CLICtl *icb, char *fmt, ...);
int CONSOLE_PutString(CONSOLE_CLICtl *icb, char *strData, int len);
int Console(CONSOLE_CLICtl *icb);

#endif /* _CONSOLE_HEADER_ */

