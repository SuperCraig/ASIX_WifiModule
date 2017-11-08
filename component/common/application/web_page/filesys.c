/*
 ******************************************************************************
 *     Copyright (c) ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
/*=============================================================================
 * Module Name: filesys.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "filesys.h"
#include "httpd.h"


/* GLOBAL VARIABLE DECLARATIONS */
#if (MAX_POST_RECORDS)
POST_RECORD POST_Record[MAX_POST_RECORDS];
#endif
#if (MAX_POST_BUF_SUBMIT)
static BUF_SUBMIT BUF_Submit[MAX_POST_BUF_SUBMIT];
#endif
#if (MAX_POST_BUF_TEXT)
static BUF_TEXT BUF_Text[MAX_POST_BUF_TEXT];
#endif
#if (MAX_POST_BUF_RADIO)
static BUF_RADIO BUF_Radio[MAX_POST_BUF_RADIO];
#endif
#if (MAX_POST_BUF_SELECT)
static BUF_SELECT BUF_Select[MAX_POST_BUF_SELECT];
#endif
#if (MAX_POST_BUF_TAG)
static BUF_TAG BUF_Tag[MAX_POST_BUF_TAG];
#endif
#if (MAX_POST_BUF_PASSWORD)
static BUF_PASSWORD BUF_Password[MAX_POST_BUF_PASSWORD];
#endif
#if (MAX_POST_BUF_CHECKBOX)
static BUF_CHECKBOX BUF_Checkbox[MAX_POST_BUF_CHECKBOX];
#endif


/* STATIC VARIABLE DECLARATIONS */
extern const U8_T WebImg_asix[];
extern const U8_T WebImg_Logo[];
extern const U8_T WebImg_tbbg[];
extern const U8_T WebAdvanced[];
extern const U8_T WebAxcss0[];
extern const U8_T WebAxcss1[];
extern const U8_T WebAxjs0[];
extern const U8_T WebBasic[];
extern const U8_T WebCgireq[];
extern const U8_T WebHctent[];
extern const U8_T WebIndex[];
extern const U8_T WebRtc[];
extern const U8_T WebSecurity[];
extern const U8_T WebStatus[];
extern const U8_T WebSysconfirm[];
extern const U8_T WebSysmsg[];
extern const U8_T WebWifi[];
extern const U8_T WebWifiwz_cfirm[];
extern const U8_T WebWifiwz_key[];
extern const U8_T WebWifiwz_svy[];
extern const U8_T WebWifi_wz[];

#if (MAX_STORE_FILE_NUM)
//const FILE_MANAGEMEMT FSYS_Manage[MAX_STORE_FILE_NUM] = 
FILE_MANAGEMEMT FSYS_Manage[MAX_STORE_FILE_NUM] = 
{
    /* for file 0 */ {1, 1, "img_asix.gif", 1993, (U8_T*)WebImg_asix, 0, (void*)0},
    /* for file 1 */ {1, 1, "img_Logo.jpg", 2841, (U8_T*)WebImg_Logo, 1, (void*)0},
    /* for file 2 */ {1, 1, "img_tbbg.gif", 200, (U8_T*)WebImg_tbbg, 2, (void*)0},
    /* for file 3 */ {1, 0, "advanced.htm", 9867, (U8_T*)WebAdvanced, 3, (void*)0},
    /* for file 4 */ {1, 3, "axcss0.css", 1885, (U8_T*)WebAxcss0, 4, (void*)0},
    /* for file 5 */ {1, 3, "axcss1.css", 1344, (U8_T*)WebAxcss1, 5, (void*)0},
    /* for file 6 */ {1, 2, "axjs0.js", 9291, (U8_T*)WebAxjs0, 6, (void*)0},
    /* for file 7 */ {1, 0, "basic.htm", 7131, (U8_T*)WebBasic, 7, (void*)0},
    /* for file 8 */ {1, 0, "cgireq.htm", 748, (U8_T*)WebCgireq, 8, (void*)0},
    /* for file 9 */ {1, 0, "hctent.htm", 609, (U8_T*)WebHctent, 9, (void*)0},
    /* for file 10 */ {1, 0, "index.htm", 1394, (U8_T*)WebIndex, 10, (void*)0},
    /* for file 11 */ {1, 0, "rtc.htm", 6066, (U8_T*)WebRtc, 11, (void*)0},
    /* for file 12 */ {1, 0, "security.htm", 6691, (U8_T*)WebSecurity, 12, (void*)0},           //20170724 Craig WebPageSet
    /* for file 13 */ {1, 0, "status.htm", 5570, (U8_T*)WebStatus, 13, (void*)0},
    /* for file 14 */ {1, 0, "sysconfirm.htm", 2161, (U8_T*)WebSysconfirm, 14, (void*)0},
    /* for file 15 */ {1, 0, "sysmsg.htm", 2216, (U8_T*)WebSysmsg, 15, (void*)0},
    /* for file 16 */ {1, 0, "wifi.htm", 6582, (U8_T*)WebWifi, 16, (void*)0},
    /* for file 17 */ {1, 0, "wifiwz_cfirm.htm", 3300, (U8_T*)WebWifiwz_cfirm, 17, (void*)0},
    /* for file 18 */ {1, 0, "wifiwz_key.htm", 2975, (U8_T*)WebWifiwz_key, 18, (void*)0},
    /* for file 19 */ {1, 0, "wifiwz_svy.htm", 6005, (U8_T*)WebWifiwz_svy, 19, (void*)0},
    /* for file 20 */ {1, 0, "wifi_wz.htm", 1695, (U8_T*)WebWifi_wz, 20, (void*)0},
};
#endif

static U8_T PostbufText0[2][36];
static U8_T PostbufText1[2][6];
static U8_T PostbufText2[2][36];
static U8_T PostbufText3[2][36];
static U8_T PostbufText4[2][36];
static U8_T PostbufText5[2][36];
static U8_T PostbufText6[2][6];
static U8_T PostbufText7[2][6];
static U8_T PostbufText8[2][6];
static U8_T PostbufText9[2][6];
static U8_T PostbufText10[2][17];
static U8_T PostbufText11[2][17];
static U8_T PostbufText12[2][6];
static U8_T PostbufText13[2][6];
static U8_T PostbufText14[2][36];
static U8_T PostbufText15[2][6];
static U8_T PostbufText16[2][16];
static U8_T PostbufText17[2][16];
static U8_T PostbufText18[2][16];
static U8_T PostbufText19[2][16];
static U8_T PostbufText20[2][16];
static U8_T PostbufText21[2][16];
static U8_T PostbufText22[2][16];
static U8_T PostbufText23[2][6];
static U8_T PostbufText24[2][17];
static U8_T PostbufText25[2][17];
static U8_T PostbufText26[2][5];
static U8_T PostbufText27[2][3];
static U8_T PostbufText28[2][3];
static U8_T PostbufText29[2][3];
static U8_T PostbufText30[2][3];
static U8_T PostbufText31[2][3];
static U8_T PostbufText32[2][36];
static U8_T PostbufText33[2][36];
static U8_T PostbufText34[2][36];
static U8_T PostbufText35[2][16];
static U8_T PostbufText36[2][36];
static U8_T PostbufText37[2][65];
static U8_T PostbufText38[2][64];
static U8_T PostbufText39[2][32];
static U8_T PostbufText40[2][7];
static U8_T PostbufText41[2][6];
static U8_T PostbufText42[2][12];
static U8_T PostbufText43[2][13];
static U8_T PostbufText44[2][33];
static U8_T PostbufText45[2][33];
static U8_T PostbufText46[2][65];
static U8_T PostbufText47[2][33];
static U8_T PostbufText48[2][12];
static U8_T PostbufText49[2][11];
static U8_T PostbufText50[2][32];
static U8_T PostbufText51[2][27];
static U8_T PostbufText52[2][27];
static U8_T PostbufText53[2][27];
static U8_T PostbufText54[2][27];
static U8_T PostbufText55[2][64];
static U8_T PostbufText56[2][16];
static U8_T PostbufText57[2][16];
static U8_T PostbufText58[2][16];
static U8_T PostbufText59[2][16];
static U8_T PostbufText60[2][65];
static U8_T PostbufPassword0[2][17];
static U8_T PostbufPassword1[2][16];
static U8_T PostbufPassword2[2][16];
static U8_T PostbufPassword3[2][16];
static U8_T PostbufPassword4[2][36];
static U8_T PostbufPassword5[2][32];
static U8_T PostbufPassword6[2][65];

/* LOCAL SUBPROGRAM DECLARATIONS */
static void fsys_InitPostRecord(void);

/*
 * ----------------------------------------------------------------------------
 * Function Name: FSYS_Init
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void FSYS_Init(void)
{


	fsys_InitPostRecord();
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: fsys_InitPostRecord
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void fsys_InitPostRecord(void)
{
#if (MAX_POST_BUF_SUBMIT)
	BUF_SUBMIT*	pSubmit;
#endif
#if (MAX_POST_BUF_TEXT)
	BUF_TEXT*	pText;
#endif
#if (MAX_POST_BUF_RADIO)
	BUF_RADIO*	pRadio;
#endif
#if (MAX_POST_BUF_SELECT)
	BUF_SELECT*	pSelect;
#endif
#if (MAX_POST_BUF_TAG)
	BUF_TAG*	pTag;
#endif
#if (MAX_POST_BUF_PASSWORD)
	BUF_PASSWORD*	pPassword;
#endif
#if (MAX_POST_BUF_CHECKBOX)
	BUF_CHECKBOX*	pCheckbox;
#endif
	/* for record 0 */
	POST_Record[0].Occupy = 1;
	POST_Record[0].PostType = POST_TYPE_SUBMIT;
	POST_Record[0].Name[0] = 'U';
	POST_Record[0].Name[1] = 'p';
	POST_Record[0].Name[2] = 'g';
	POST_Record[0].Name[3] = 'r';
	POST_Record[0].Name[4] = 'a';
	POST_Record[0].Name[5] = 'd';
	POST_Record[0].Name[6] = 'e';
	POST_Record[0].NameLen = 7;
	POST_Record[0].FileIndex = 255;
	POST_Record[0].UpdateSelf = FALSE;
	POST_Record[0].PValue = &BUF_Submit[0];
	pSubmit = POST_Record[0].PValue;
	pSubmit->Value[0] = 'U';
	pSubmit->Value[1] = 'p';
	pSubmit->Value[2] = 'g';
	pSubmit->Value[3] = 'r';
	pSubmit->Value[4] = 'a';
	pSubmit->Value[5] = 'd';
	pSubmit->Value[6] = 'e';
	pSubmit->DefaultVlaueLen = 7;
	pSubmit->IsApply = 1;

	/* for record 1 */
	POST_Record[1].Occupy = 1;
	POST_Record[1].PostType = POST_TYPE_TEXT;
	POST_Record[1].Name[0] = 'e';
	POST_Record[1].Name[1] = 'm';
	POST_Record[1].Name[2] = 'a';
	POST_Record[1].Name[3] = 'i';
	POST_Record[1].Name[4] = 'l';
	POST_Record[1].Name[5] = '_';
	POST_Record[1].Name[6] = 'a';
	POST_Record[1].Name[7] = 'd';
	POST_Record[1].Name[8] = 'd';
	POST_Record[1].Name[9] = 'r';
	POST_Record[1].NameLen = 10;
	POST_Record[1].FileIndex = 3;
	POST_Record[1].UpdateSelf = FALSE;
	POST_Record[1].PValue = &BUF_Text[0];
	pText = POST_Record[1].PValue;
	pText->CurrValue = PostbufText0[0];
	pText->UserValue = PostbufText0[1];
	pText->Offset = 2033;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 2 */
	POST_Record[2].Occupy = 1;
	POST_Record[2].PostType = POST_TYPE_SELECT;
	POST_Record[2].Name[0] = 's';
	POST_Record[2].Name[1] = 'm';
	POST_Record[2].Name[2] = 't';
	POST_Record[2].Name[3] = 'p';
	POST_Record[2].Name[4] = '_';
	POST_Record[2].Name[5] = 's';
	POST_Record[2].Name[6] = 'e';
	POST_Record[2].Name[7] = 'c';
	POST_Record[2].Name[8] = 'u';
	POST_Record[2].Name[9] = 'r';
	POST_Record[2].Name[10] = 'i';
	POST_Record[2].Name[11] = 't';
	POST_Record[2].Name[12] = 'y';
	POST_Record[2].NameLen = 13;
	POST_Record[2].FileIndex = 3;
	POST_Record[2].UpdateSelf = FALSE;
	POST_Record[2].PValue = &BUF_Select[0];
	pSelect = POST_Record[2].PValue;
	pSelect->Offset[0] = 2287;
	pSelect->Offset[1] = 2347;
	pSelect->Offset[2] = 2389;
	pSelect->Offset[3] = 2431;
	pSelect->Count = 4;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 3 */
	POST_Record[3].Occupy = 1;
	POST_Record[3].PostType = POST_TYPE_TEXT;
	POST_Record[3].Name[0] = 's';
	POST_Record[3].Name[1] = 'm';
	POST_Record[3].Name[2] = 't';
	POST_Record[3].Name[3] = 'p';
	POST_Record[3].Name[4] = '_';
	POST_Record[3].Name[5] = 'p';
	POST_Record[3].Name[6] = 'o';
	POST_Record[3].Name[7] = 'r';
	POST_Record[3].Name[8] = 't';
	POST_Record[3].NameLen = 9;
	POST_Record[3].FileIndex = 3;
	POST_Record[3].UpdateSelf = FALSE;
	POST_Record[3].PValue = &BUF_Text[1];
	pText = POST_Record[3].PValue;
	pText->CurrValue = PostbufText1[0];
	pText->UserValue = PostbufText1[1];
	pText->Offset = 2551;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 4 */
	POST_Record[4].Occupy = 1;
	POST_Record[4].PostType = POST_TYPE_TEXT;
	POST_Record[4].Name[0] = 'e';
	POST_Record[4].Name[1] = 'm';
	POST_Record[4].Name[2] = 'a';
	POST_Record[4].Name[3] = 'i';
	POST_Record[4].Name[4] = 'l';
	POST_Record[4].Name[5] = '_';
	POST_Record[4].Name[6] = 'f';
	POST_Record[4].Name[7] = 'r';
	POST_Record[4].Name[8] = 'o';
	POST_Record[4].Name[9] = 'm';
	POST_Record[4].NameLen = 10;
	POST_Record[4].FileIndex = 3;
	POST_Record[4].UpdateSelf = FALSE;
	POST_Record[4].PValue = &BUF_Text[2];
	pText = POST_Record[4].PValue;
	pText->CurrValue = PostbufText2[0];
	pText->UserValue = PostbufText2[1];
	pText->Offset = 2668;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 5 */
	POST_Record[5].Occupy = 1;
	POST_Record[5].PostType = POST_TYPE_TEXT;
	POST_Record[5].Name[0] = 'e';
	POST_Record[5].Name[1] = 'm';
	POST_Record[5].Name[2] = 'a';
	POST_Record[5].Name[3] = 'i';
	POST_Record[5].Name[4] = 'l';
	POST_Record[5].Name[5] = '_';
	POST_Record[5].Name[6] = 't';
	POST_Record[5].Name[7] = 'o';
	POST_Record[5].Name[8] = '1';
	POST_Record[5].NameLen = 9;
	POST_Record[5].FileIndex = 3;
	POST_Record[5].UpdateSelf = FALSE;
	POST_Record[5].PValue = &BUF_Text[3];
	pText = POST_Record[5].PValue;
	pText->CurrValue = PostbufText3[0];
	pText->UserValue = PostbufText3[1];
	pText->Offset = 2803;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 6 */
	POST_Record[6].Occupy = 1;
	POST_Record[6].PostType = POST_TYPE_TEXT;
	POST_Record[6].Name[0] = 'e';
	POST_Record[6].Name[1] = 'm';
	POST_Record[6].Name[2] = 'a';
	POST_Record[6].Name[3] = 'i';
	POST_Record[6].Name[4] = 'l';
	POST_Record[6].Name[5] = '_';
	POST_Record[6].Name[6] = 't';
	POST_Record[6].Name[7] = 'o';
	POST_Record[6].Name[8] = '2';
	POST_Record[6].NameLen = 9;
	POST_Record[6].FileIndex = 3;
	POST_Record[6].UpdateSelf = FALSE;
	POST_Record[6].PValue = &BUF_Text[4];
	pText = POST_Record[6].PValue;
	pText->CurrValue = PostbufText4[0];
	pText->UserValue = PostbufText4[1];
	pText->Offset = 2938;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 7 */
	POST_Record[7].Occupy = 1;
	POST_Record[7].PostType = POST_TYPE_TEXT;
	POST_Record[7].Name[0] = 'e';
	POST_Record[7].Name[1] = 'm';
	POST_Record[7].Name[2] = 'a';
	POST_Record[7].Name[3] = 'i';
	POST_Record[7].Name[4] = 'l';
	POST_Record[7].Name[5] = '_';
	POST_Record[7].Name[6] = 't';
	POST_Record[7].Name[7] = 'o';
	POST_Record[7].Name[8] = '3';
	POST_Record[7].NameLen = 9;
	POST_Record[7].FileIndex = 3;
	POST_Record[7].UpdateSelf = FALSE;
	POST_Record[7].PValue = &BUF_Text[5];
	pText = POST_Record[7].PValue;
	pText->CurrValue = PostbufText5[0];
	pText->UserValue = PostbufText5[1];
	pText->Offset = 3073;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 8 */
	POST_Record[8].Occupy = 1;
	POST_Record[8].PostType = POST_TYPE_SELECT;
	POST_Record[8].Name[0] = 'c';
	POST_Record[8].Name[1] = 'o';
	POST_Record[8].Name[2] = 'l';
	POST_Record[8].Name[3] = 'd';
	POST_Record[8].Name[4] = 's';
	POST_Record[8].Name[5] = 't';
	POST_Record[8].Name[6] = 'a';
	POST_Record[8].Name[7] = 'r';
	POST_Record[8].Name[8] = 't';
	POST_Record[8].NameLen = 9;
	POST_Record[8].FileIndex = 3;
	POST_Record[8].UpdateSelf = FALSE;
	POST_Record[8].PValue = &BUF_Select[1];
	pSelect = POST_Record[8].PValue;
	pSelect->Offset[0] = 3202;
	pSelect->Offset[1] = 3258;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 9 */
	POST_Record[9].Occupy = 1;
	POST_Record[9].PostType = POST_TYPE_SELECT;
	POST_Record[9].Name[0] = 'a';
	POST_Record[9].Name[1] = 'u';
	POST_Record[9].Name[2] = 't';
	POST_Record[9].Name[3] = 'h';
	POST_Record[9].Name[4] = 'f';
	POST_Record[9].Name[5] = 'a';
	POST_Record[9].Name[6] = 'i';
	POST_Record[9].Name[7] = 'l';
	POST_Record[9].NameLen = 8;
	POST_Record[9].FileIndex = 3;
	POST_Record[9].UpdateSelf = FALSE;
	POST_Record[9].PValue = &BUF_Select[2];
	pSelect = POST_Record[9].PValue;
	pSelect->Offset[0] = 3388;
	pSelect->Offset[1] = 3444;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 10 */
	POST_Record[10].Occupy = 1;
	POST_Record[10].PostType = POST_TYPE_SELECT;
	POST_Record[10].Name[0] = 'i';
	POST_Record[10].Name[1] = 'p';
	POST_Record[10].Name[2] = 'c';
	POST_Record[10].Name[3] = 'h';
	POST_Record[10].Name[4] = 'g';
	POST_Record[10].NameLen = 5;
	POST_Record[10].FileIndex = 3;
	POST_Record[10].UpdateSelf = FALSE;
	POST_Record[10].PValue = &BUF_Select[3];
	pSelect = POST_Record[10].PValue;
	pSelect->Offset[0] = 3573;
	pSelect->Offset[1] = 3629;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 11 */
	POST_Record[11].Occupy = 1;
	POST_Record[11].PostType = POST_TYPE_SELECT;
	POST_Record[11].Name[0] = 'p';
	POST_Record[11].Name[1] = 's';
	POST_Record[11].Name[2] = 'w';
	POST_Record[11].Name[3] = 'c';
	POST_Record[11].Name[4] = 'h';
	POST_Record[11].Name[5] = 'g';
	POST_Record[11].NameLen = 6;
	POST_Record[11].FileIndex = 3;
	POST_Record[11].UpdateSelf = FALSE;
	POST_Record[11].PValue = &BUF_Select[4];
	pSelect = POST_Record[11].PValue;
	pSelect->Offset[0] = 3751;
	pSelect->Offset[1] = 3807;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 12 */
	POST_Record[12].Occupy = 1;
	POST_Record[12].PostType = POST_TYPE_SUBMIT;
	POST_Record[12].Name[0] = 'S';
	POST_Record[12].Name[1] = 'a';
	POST_Record[12].Name[2] = 'v';
	POST_Record[12].Name[3] = 'e';
	POST_Record[12].NameLen = 4;
	POST_Record[12].FileIndex = 255;
	POST_Record[12].UpdateSelf = FALSE;
	POST_Record[12].PValue = &BUF_Submit[1];
	pSubmit = POST_Record[12].PValue;
	pSubmit->Value[0] = 'S';
	pSubmit->Value[1] = 'a';
	pSubmit->Value[2] = 'v';
	pSubmit->Value[3] = 'e';
	pSubmit->DefaultVlaueLen = 4;
	pSubmit->IsApply = 1;

	/* for record 13 */
	POST_Record[13].Occupy = 1;
	POST_Record[13].PostType = POST_TYPE_SUBMIT;
	POST_Record[13].Name[0] = 'A';
	POST_Record[13].Name[1] = 'p';
	POST_Record[13].Name[2] = 'p';
	POST_Record[13].Name[3] = 'l';
	POST_Record[13].Name[4] = 'y';
	POST_Record[13].NameLen = 5;
	POST_Record[13].FileIndex = 255;
	POST_Record[13].UpdateSelf = FALSE;
	POST_Record[13].PValue = &BUF_Submit[2];
	pSubmit = POST_Record[13].PValue;
	pSubmit->Value[0] = 'A';
	pSubmit->Value[1] = 'p';
	pSubmit->Value[2] = 'p';
	pSubmit->Value[3] = 'l';
	pSubmit->Value[4] = 'y';
	pSubmit->DefaultVlaueLen = 5;
	pSubmit->IsApply = 1;

	/* for record 14 */
	POST_Record[14].Occupy = 1;
	POST_Record[14].PostType = POST_TYPE_SELECT;
	POST_Record[14].Name[0] = 'm';
	POST_Record[14].Name[1] = 'b';
	POST_Record[14].Name[2] = 't';
	POST_Record[14].Name[3] = 'c';
	POST_Record[14].Name[4] = 'p';
	POST_Record[14].Name[5] = '_';
	POST_Record[14].Name[6] = 'x';
	POST_Record[14].Name[7] = 'f';
	POST_Record[14].Name[8] = 'e';
	POST_Record[14].Name[9] = 'r';
	POST_Record[14].NameLen = 10;
	POST_Record[14].FileIndex = 3;
	POST_Record[14].UpdateSelf = FALSE;
	POST_Record[14].PValue = &BUF_Select[5];
	pSelect = POST_Record[14].PValue;
	pSelect->Offset[0] = 4279;
	pSelect->Offset[1] = 4338;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 15 */
	POST_Record[15].Occupy = 1;
	POST_Record[15].PostType = POST_TYPE_TEXT;
	POST_Record[15].Name[0] = 'm';
	POST_Record[15].Name[1] = 'b';
	POST_Record[15].Name[2] = 't';
	POST_Record[15].Name[3] = 'c';
	POST_Record[15].Name[4] = 'p';
	POST_Record[15].Name[5] = '_';
	POST_Record[15].Name[6] = 'p';
	POST_Record[15].Name[7] = 'o';
	POST_Record[15].Name[8] = 'r';
	POST_Record[15].Name[9] = 't';
	POST_Record[15].NameLen = 10;
	POST_Record[15].FileIndex = 3;
	POST_Record[15].UpdateSelf = FALSE;
	POST_Record[15].PValue = &BUF_Text[6];
	pText = POST_Record[15].PValue;
	pText->CurrValue = PostbufText6[0];
	pText->UserValue = PostbufText6[1];
	pText->Offset = 4465;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 16 */
	POST_Record[16].Occupy = 1;
	POST_Record[16].PostType = POST_TYPE_TEXT;
	POST_Record[16].Name[0] = 'm';
	POST_Record[16].Name[1] = 'b';
	POST_Record[16].Name[2] = '_';
	POST_Record[16].Name[3] = 'r';
	POST_Record[16].Name[4] = 's';
	POST_Record[16].Name[5] = 'p';
	POST_Record[16].Name[6] = 'T';
	POST_Record[16].Name[7] = 'i';
	POST_Record[16].Name[8] = 'm';
	POST_Record[16].Name[9] = 'e';
	POST_Record[16].NameLen = 10;
	POST_Record[16].FileIndex = 3;
	POST_Record[16].UpdateSelf = FALSE;
	POST_Record[16].PValue = &BUF_Text[7];
	pText = POST_Record[16].PValue;
	pText->CurrValue = PostbufText7[0];
	pText->UserValue = PostbufText7[1];
	pText->Offset = 4579;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 17 */
	POST_Record[17].Occupy = 1;
	POST_Record[17].PostType = POST_TYPE_TEXT;
	POST_Record[17].Name[0] = 'm';
	POST_Record[17].Name[1] = 'b';
	POST_Record[17].Name[2] = '_';
	POST_Record[17].Name[3] = 'f';
	POST_Record[17].Name[4] = 'r';
	POST_Record[17].Name[5] = 'a';
	POST_Record[17].Name[6] = 'm';
	POST_Record[17].Name[7] = 'e';
	POST_Record[17].Name[8] = 'I';
	POST_Record[17].Name[9] = 'n';
	POST_Record[17].Name[10] = 't';
	POST_Record[17].Name[11] = 'v';
	POST_Record[17].NameLen = 12;
	POST_Record[17].FileIndex = 3;
	POST_Record[17].UpdateSelf = FALSE;
	POST_Record[17].PValue = &BUF_Text[8];
	pText = POST_Record[17].PValue;
	pText->CurrValue = PostbufText8[0];
	pText->UserValue = PostbufText8[1];
	pText->Offset = 4777;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 18 */
	POST_Record[18].Occupy = 1;
	POST_Record[18].PostType = POST_TYPE_TEXT;
	POST_Record[18].Name[0] = 'm';
	POST_Record[18].Name[1] = 'b';
	POST_Record[18].Name[2] = '_';
	POST_Record[18].Name[3] = 'C';
	POST_Record[18].Name[4] = 'h';
	POST_Record[18].Name[5] = 'a';
	POST_Record[18].Name[6] = 'r';
	POST_Record[18].Name[7] = 'D';
	POST_Record[18].Name[8] = 'e';
	POST_Record[18].Name[9] = 'l';
	POST_Record[18].Name[10] = 'a';
	POST_Record[18].Name[11] = 'y';
	POST_Record[18].NameLen = 12;
	POST_Record[18].FileIndex = 3;
	POST_Record[18].UpdateSelf = FALSE;
	POST_Record[18].PValue = &BUF_Text[9];
	pText = POST_Record[18].PValue;
	pText->CurrValue = PostbufText9[0];
	pText->UserValue = PostbufText9[1];
	pText->Offset = 4977;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 19 */
	POST_Record[19].Occupy = 1;
	POST_Record[19].PostType = POST_TYPE_SELECT;
	POST_Record[19].Name[0] = 'n';
	POST_Record[19].Name[1] = 'e';
	POST_Record[19].Name[2] = 't';
	POST_Record[19].Name[3] = 'b';
	POST_Record[19].Name[4] = 'a';
	POST_Record[19].Name[5] = 'c';
	POST_Record[19].Name[6] = 'k';
	POST_Record[19].Name[7] = '_';
	POST_Record[19].Name[8] = 'm';
	POST_Record[19].Name[9] = 'o';
	POST_Record[19].Name[10] = 'd';
	POST_Record[19].Name[11] = 'e';
	POST_Record[19].NameLen = 12;
	POST_Record[19].FileIndex = 3;
	POST_Record[19].UpdateSelf = FALSE;
	POST_Record[19].PValue = &BUF_Select[6];
	pSelect = POST_Record[19].PValue;
	pSelect->Offset[0] = 5532;
	pSelect->Offset[1] = 5591;
	pSelect->Offset[2] = 5644;
	pSelect->Offset[3] = 5696;
	pSelect->Count = 4;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 20 */
	POST_Record[20].Occupy = 1;
	POST_Record[20].PostType = POST_TYPE_TEXT;
	POST_Record[20].Name[0] = 'h';
	POST_Record[20].Name[1] = 'i';
	POST_Record[20].Name[2] = 'd';
	POST_Record[20].Name[3] = 'e';
	POST_Record[20].Name[4] = 'B';
	POST_Record[20].Name[5] = 'a';
	POST_Record[20].Name[6] = 'c';
	POST_Record[20].Name[7] = 'k';
	POST_Record[20].Name[8] = 'u';
	POST_Record[20].Name[9] = 'p';
	POST_Record[20].Name[10] = 'M';
	POST_Record[20].Name[11] = 'o';
	POST_Record[20].Name[12] = 'd';
	POST_Record[20].Name[13] = 'e';
	POST_Record[20].NameLen = 14;
	POST_Record[20].FileIndex = 3;
	POST_Record[20].UpdateSelf = FALSE;
	POST_Record[20].PValue = &BUF_Text[10];
	pText = POST_Record[20].PValue;
	pText->CurrValue = PostbufText10[0];
	pText->UserValue = PostbufText10[1];
	pText->Offset = 6065;
	pText->DefaultLength = 2;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '"';
	pText->CurrLength = 2;

	/* for record 21 */
	POST_Record[21].Occupy = 1;
	POST_Record[21].PostType = POST_TYPE_SUBMIT;
	POST_Record[21].Name[0] = 'r';
	POST_Record[21].Name[1] = 'e';
	POST_Record[21].Name[2] = 's';
	POST_Record[21].Name[3] = 't';
	POST_Record[21].Name[4] = 'o';
	POST_Record[21].Name[5] = 'r';
	POST_Record[21].Name[6] = 'e';
	POST_Record[21].NameLen = 7;
	POST_Record[21].FileIndex = 255;
	POST_Record[21].UpdateSelf = FALSE;
	POST_Record[21].PValue = &BUF_Submit[3];
	pSubmit = POST_Record[21].PValue;
	pSubmit->Value[0] = 'R';
	pSubmit->Value[1] = 'e';
	pSubmit->Value[2] = 's';
	pSubmit->Value[3] = 't';
	pSubmit->Value[4] = 'o';
	pSubmit->Value[5] = 'r';
	pSubmit->Value[6] = 'e';
	pSubmit->Value[7] = '_';
	pSubmit->Value[8] = 'D';
	pSubmit->Value[9] = 'e';
	pSubmit->Value[10] = 'f';
	pSubmit->Value[11] = 'a';
	pSubmit->Value[12] = 'u';
	pSubmit->Value[13] = 'l';
	pSubmit->Value[14] = 't';
	pSubmit->DefaultVlaueLen = 15;
	pSubmit->IsApply = 1;

	/* for record 22 */
	POST_Record[22].Occupy = 1;
	POST_Record[22].PostType = POST_TYPE_SUBMIT;
	POST_Record[22].Name[0] = 'r';
	POST_Record[22].Name[1] = 'e';
	POST_Record[22].Name[2] = 'b';
	POST_Record[22].Name[3] = 'o';
	POST_Record[22].Name[4] = 'o';
	POST_Record[22].Name[5] = 't';
	POST_Record[22].NameLen = 6;
	POST_Record[22].FileIndex = 255;
	POST_Record[22].UpdateSelf = FALSE;
	POST_Record[22].PValue = &BUF_Submit[4];
	pSubmit = POST_Record[22].PValue;
	pSubmit->Value[0] = 'R';
	pSubmit->Value[1] = 'e';
	pSubmit->Value[2] = 'b';
	pSubmit->Value[3] = 'o';
	pSubmit->Value[4] = 'o';
	pSubmit->Value[5] = 't';
	pSubmit->DefaultVlaueLen = 6;
	pSubmit->IsApply = 1;

	/* for record 23 */
	POST_Record[23].Occupy = 1;
	POST_Record[23].PostType = POST_TYPE_TEXT;
	POST_Record[23].Name[0] = 'd';
	POST_Record[23].Name[1] = 's';
	POST_Record[23].Name[2] = 'm';
	POST_Record[23].Name[3] = '_';
	POST_Record[23].Name[4] = 'n';
	POST_Record[23].Name[5] = 'a';
	POST_Record[23].Name[6] = 'm';
	POST_Record[23].Name[7] = 'e';
	POST_Record[23].NameLen = 8;
	POST_Record[23].FileIndex = 7;
	POST_Record[23].UpdateSelf = FALSE;
	POST_Record[23].PValue = &BUF_Text[11];
	pText = POST_Record[23].PValue;
	pText->CurrValue = PostbufText11[0];
	pText->UserValue = PostbufText11[1];
	pText->Offset = 1355;
	pText->DefaultLength = 5;
	pText->CurrValue[0] = 'D';
	pText->CurrValue[1] = 'S';
	pText->CurrValue[2] = 'M';
	pText->CurrValue[3] = '1';
	pText->CurrValue[4] = '"';
	pText->CurrLength = 5;

	/* for record 24 */
	POST_Record[24].Occupy = 1;
	POST_Record[24].PostType = POST_TYPE_SELECT;
	POST_Record[24].Name[0] = 'd';
	POST_Record[24].Name[1] = 'b';
	POST_Record[24].Name[2] = 'r';
	POST_Record[24].NameLen = 3;
	POST_Record[24].FileIndex = 7;
	POST_Record[24].UpdateSelf = FALSE;
	POST_Record[24].PValue = &BUF_Select[7];
	pSelect = POST_Record[24].PValue;
	pSelect->Offset[0] = 1561;
	pSelect->Offset[1] = 1616;
	pSelect->Offset[2] = 1661;
	pSelect->Offset[3] = 1705;
	pSelect->Offset[4] = 1749;
	pSelect->Offset[5] = 1793;
	pSelect->Offset[6] = 1836;
	pSelect->Offset[7] = 1879;
	pSelect->Offset[8] = 1922;
	pSelect->Count = 9;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 25 */
	POST_Record[25].Occupy = 1;
	POST_Record[25].PostType = POST_TYPE_SELECT;
	POST_Record[25].Name[0] = 'd';
	POST_Record[25].Name[1] = 'a';
	POST_Record[25].Name[2] = 't';
	POST_Record[25].Name[3] = 'a';
	POST_Record[25].NameLen = 4;
	POST_Record[25].FileIndex = 7;
	POST_Record[25].UpdateSelf = FALSE;
	POST_Record[25].PValue = &BUF_Select[8];
	pSelect = POST_Record[25].PValue;
	pSelect->Offset[0] = 2033;
	pSelect->Offset[1] = 2083;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 26 */
	POST_Record[26].Occupy = 1;
	POST_Record[26].PostType = POST_TYPE_SELECT;
	POST_Record[26].Name[0] = 'p';
	POST_Record[26].Name[1] = 'a';
	POST_Record[26].Name[2] = 'r';
	POST_Record[26].Name[3] = 'i';
	POST_Record[26].Name[4] = 't';
	POST_Record[26].Name[5] = 'y';
	POST_Record[26].NameLen = 6;
	POST_Record[26].FileIndex = 7;
	POST_Record[26].UpdateSelf = FALSE;
	POST_Record[26].PValue = &BUF_Select[9];
	pSelect = POST_Record[26].PValue;
	pSelect->Offset[0] = 2195;
	pSelect->Offset[1] = 2248;
	pSelect->Offset[2] = 2290;
	pSelect->Count = 3;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 27 */
	POST_Record[27].Occupy = 1;
	POST_Record[27].PostType = POST_TYPE_SELECT;
	POST_Record[27].Name[0] = 's';
	POST_Record[27].Name[1] = 't';
	POST_Record[27].Name[2] = 'o';
	POST_Record[27].Name[3] = 'p';
	POST_Record[27].NameLen = 4;
	POST_Record[27].FileIndex = 7;
	POST_Record[27].UpdateSelf = FALSE;
	POST_Record[27].PValue = &BUF_Select[10];
	pSelect = POST_Record[27].PValue;
	pSelect->Offset[0] = 2401;
	pSelect->Offset[1] = 2451;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 28 */
	POST_Record[28].Occupy = 1;
	POST_Record[28].PostType = POST_TYPE_SELECT;
	POST_Record[28].Name[0] = 'f';
	POST_Record[28].Name[1] = 'l';
	POST_Record[28].Name[2] = 'o';
	POST_Record[28].Name[3] = 'w';
	POST_Record[28].NameLen = 4;
	POST_Record[28].FileIndex = 7;
	POST_Record[28].UpdateSelf = FALSE;
	POST_Record[28].PValue = &BUF_Select[11];
	pSelect = POST_Record[28].PValue;
	pSelect->Offset[0] = 2562;
	pSelect->Offset[1] = 2615;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 29 */
	POST_Record[29].Occupy = 1;
	POST_Record[29].PostType = POST_TYPE_SELECT;
	POST_Record[29].Name[0] = 'r';
	POST_Record[29].Name[1] = 's';
	POST_Record[29].Name[2] = '4';
	POST_Record[29].Name[3] = '8';
	POST_Record[29].Name[4] = '5';
	POST_Record[29].NameLen = 5;
	POST_Record[29].FileIndex = 7;
	POST_Record[29].UpdateSelf = FALSE;
	POST_Record[29].PValue = &BUF_Select[12];
	pSelect = POST_Record[29].PValue;
	pSelect->Offset[0] = 2726;
	pSelect->Offset[1] = 2782;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 30 */
	POST_Record[30].Occupy = 1;
	POST_Record[30].PostType = POST_TYPE_SELECT;
	POST_Record[30].Name[0] = 's';
	POST_Record[30].Name[1] = '2';
	POST_Record[30].Name[2] = 'n';
	POST_Record[30].Name[3] = '_';
	POST_Record[30].Name[4] = 'm';
	POST_Record[30].Name[5] = 'o';
	POST_Record[30].Name[6] = 'd';
	POST_Record[30].Name[7] = 'e';
	POST_Record[30].NameLen = 8;
	POST_Record[30].FileIndex = 7;
	POST_Record[30].UpdateSelf = FALSE;
	POST_Record[30].PValue = &BUF_Select[13];
	pSelect = POST_Record[30].PValue;
	pSelect->Offset[0] = 2961;
	pSelect->Offset[1] = 3016;
	pSelect->Offset[2] = 3059;
	pSelect->Offset[3] = 3105;
	pSelect->Count = 4;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 31 */
	POST_Record[31].Occupy = 1;
	POST_Record[31].PostType = POST_TYPE_SELECT;
	POST_Record[31].Name[0] = 'c';
	POST_Record[31].Name[1] = 'o';
	POST_Record[31].Name[2] = 'n';
	POST_Record[31].Name[3] = 'n';
	POST_Record[31].Name[4] = 't';
	POST_Record[31].Name[5] = 'y';
	POST_Record[31].Name[6] = 'p';
	POST_Record[31].Name[7] = 'e';
	POST_Record[31].NameLen = 8;
	POST_Record[31].FileIndex = 7;
	POST_Record[31].UpdateSelf = FALSE;
	POST_Record[31].PValue = &BUF_Select[14];
	pSelect = POST_Record[31].PValue;
	pSelect->Offset[0] = 3236;
	pSelect->Offset[1] = 3288;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 32 */
	POST_Record[32].Occupy = 1;
	POST_Record[32].PostType = POST_TYPE_TEXT;
	POST_Record[32].Name[0] = 't';
	POST_Record[32].Name[1] = 'x';
	POST_Record[32].Name[2] = 't';
	POST_Record[32].Name[3] = 'i';
	POST_Record[32].Name[4] = 'm';
	POST_Record[32].Name[5] = 'e';
	POST_Record[32].Name[6] = 'r';
	POST_Record[32].NameLen = 7;
	POST_Record[32].FileIndex = 7;
	POST_Record[32].UpdateSelf = FALSE;
	POST_Record[32].PValue = &BUF_Text[12];
	pText = POST_Record[32].PValue;
	pText->CurrValue = PostbufText12[0];
	pText->UserValue = PostbufText12[1];
	pText->Offset = 3408;
	pText->DefaultLength = 4;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '0';
	pText->CurrValue[2] = '0';
	pText->CurrValue[3] = '"';
	pText->CurrLength = 4;

	/* for record 33 */
	POST_Record[33].Occupy = 1;
	POST_Record[33].PostType = POST_TYPE_SELECT;
	POST_Record[33].Name[0] = 'c';
	POST_Record[33].Name[1] = 's';
	POST_Record[33].Name[2] = '_';
	POST_Record[33].Name[3] = 'm';
	POST_Record[33].Name[4] = 'o';
	POST_Record[33].Name[5] = 'd';
	POST_Record[33].Name[6] = 'e';
	POST_Record[33].NameLen = 7;
	POST_Record[33].FileIndex = 7;
	POST_Record[33].UpdateSelf = FALSE;
	POST_Record[33].PValue = &BUF_Select[15];
	pSelect = POST_Record[33].PValue;
	pSelect->Offset[0] = 3622;
	pSelect->Offset[1] = 3677;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 34 */
	POST_Record[34].Occupy = 1;
	POST_Record[34].PostType = POST_TYPE_TEXT;
	POST_Record[34].Name[0] = 's';
	POST_Record[34].Name[1] = '_';
	POST_Record[34].Name[2] = 'l';
	POST_Record[34].Name[3] = 's';
	POST_Record[34].Name[4] = 't';
	POST_Record[34].Name[5] = 'p';
	POST_Record[34].Name[6] = 'o';
	POST_Record[34].Name[7] = 'r';
	POST_Record[34].Name[8] = 't';
	POST_Record[34].NameLen = 9;
	POST_Record[34].FileIndex = 7;
	POST_Record[34].UpdateSelf = FALSE;
	POST_Record[34].PValue = &BUF_Text[13];
	pText = POST_Record[34].PValue;
	pText->CurrValue = PostbufText13[0];
	pText->UserValue = PostbufText13[1];
	pText->Offset = 3804;
	pText->DefaultLength = 5;
	pText->CurrValue[0] = '5';
	pText->CurrValue[1] = '0';
	pText->CurrValue[2] = '0';
	pText->CurrValue[3] = '0';
	pText->CurrValue[4] = '"';
	pText->CurrLength = 5;

	/* for record 35 */
	POST_Record[35].Occupy = 1;
	POST_Record[35].PostType = POST_TYPE_SELECT;
	POST_Record[35].Name[0] = 'r';
	POST_Record[35].Name[1] = '2';
	POST_Record[35].Name[2] = 'w';
	POST_Record[35].Name[3] = '_';
	POST_Record[35].Name[4] = 'c';
	POST_Record[35].Name[5] = 'o';
	POST_Record[35].Name[6] = 'n';
	POST_Record[35].Name[7] = 'n';
	POST_Record[35].Name[8] = 's';
	POST_Record[35].NameLen = 9;
	POST_Record[35].FileIndex = 7;
	POST_Record[35].UpdateSelf = FALSE;
	POST_Record[35].PValue = &BUF_Select[16];
	pSelect = POST_Record[35].PValue;
	pSelect->Offset[0] = 4025;
	pSelect->Offset[1] = 4075;
	pSelect->Offset[2] = 4115;
	pSelect->Offset[3] = 4155;
	pSelect->Count = 4;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 36 */
	POST_Record[36].Occupy = 1;
	POST_Record[36].PostType = POST_TYPE_TEXT;
	POST_Record[36].Name[0] = 'c';
	POST_Record[36].Name[1] = '_';
	POST_Record[36].Name[2] = 'd';
	POST_Record[36].Name[3] = 'e';
	POST_Record[36].Name[4] = 's';
	POST_Record[36].Name[5] = 'h';
	POST_Record[36].Name[6] = 'n';
	POST_Record[36].NameLen = 7;
	POST_Record[36].FileIndex = 7;
	POST_Record[36].UpdateSelf = FALSE;
	POST_Record[36].PValue = &BUF_Text[14];
	pText = POST_Record[36].PValue;
	pText->CurrValue = PostbufText14[0];
	pText->UserValue = PostbufText14[1];
	pText->Offset = 4403;
	pText->DefaultLength = 12;
	pText->CurrValue[0] = 'a';
	pText->CurrValue[1] = 's';
	pText->CurrValue[2] = 'i';
	pText->CurrValue[3] = 'x';
	pText->CurrValue[4] = '.';
	pText->CurrValue[5] = 'c';
	pText->CurrValue[6] = 'o';
	pText->CurrValue[7] = 'm';
	pText->CurrValue[8] = '.';
	pText->CurrValue[9] = 't';
	pText->CurrValue[10] = 'w';
	pText->CurrValue[11] = '"';
	pText->CurrLength = 12;

	/* for record 37 */
	POST_Record[37].Occupy = 1;
	POST_Record[37].PostType = POST_TYPE_TEXT;
	POST_Record[37].Name[0] = 'c';
	POST_Record[37].Name[1] = '_';
	POST_Record[37].Name[2] = 'd';
	POST_Record[37].Name[3] = 'e';
	POST_Record[37].Name[4] = 's';
	POST_Record[37].Name[5] = 'p';
	POST_Record[37].Name[6] = 'o';
	POST_Record[37].Name[7] = 'r';
	POST_Record[37].Name[8] = 't';
	POST_Record[37].NameLen = 9;
	POST_Record[37].FileIndex = 7;
	POST_Record[37].UpdateSelf = FALSE;
	POST_Record[37].PValue = &BUF_Text[15];
	pText = POST_Record[37].PValue;
	pText->CurrValue = PostbufText15[0];
	pText->UserValue = PostbufText15[1];
	pText->Offset = 4676;
	pText->DefaultLength = 5;
	pText->CurrValue[0] = '5';
	pText->CurrValue[1] = '0';
	pText->CurrValue[2] = '0';
	pText->CurrValue[3] = '0';
	pText->CurrValue[4] = '"';
	pText->CurrLength = 5;

	/* for record 38 */
	POST_Record[38].Occupy = 1;
	POST_Record[38].PostType = POST_TYPE_SELECT;
	POST_Record[38].Name[0] = 'c';
	POST_Record[38].Name[1] = 'l';
	POST_Record[38].Name[2] = 'i';
	POST_Record[38].Name[3] = 'c';
	POST_Record[38].Name[4] = 'o';
	POST_Record[38].Name[5] = 'n';
	POST_Record[38].Name[6] = 'n';
	POST_Record[38].Name[7] = 'm';
	POST_Record[38].Name[8] = 'o';
	POST_Record[38].Name[9] = 'd';
	POST_Record[38].Name[10] = 'e';
	POST_Record[38].NameLen = 11;
	POST_Record[38].FileIndex = 7;
	POST_Record[38].UpdateSelf = FALSE;
	POST_Record[38].PValue = &BUF_Select[17];
	pSelect = POST_Record[38].PValue;
	pSelect->Offset[0] = 4918;
	pSelect->Offset[1] = 4982;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 39 */
	POST_Record[39].Occupy = 1;
	POST_Record[39].PostType = POST_TYPE_TEXT;
	POST_Record[39].Name[0] = 's';
	POST_Record[39].Name[1] = 't';
	POST_Record[39].Name[2] = 'a';
	POST_Record[39].Name[3] = 't';
	POST_Record[39].Name[4] = 'i';
	POST_Record[39].Name[5] = 'c';
	POST_Record[39].Name[6] = '_';
	POST_Record[39].Name[7] = 'i';
	POST_Record[39].Name[8] = 'p';
	POST_Record[39].NameLen = 9;
	POST_Record[39].FileIndex = 7;
	POST_Record[39].UpdateSelf = FALSE;
	POST_Record[39].PValue = &BUF_Text[16];
	pText = POST_Record[39].PValue;
	pText->CurrValue = PostbufText16[0];
	pText->UserValue = PostbufText16[1];
	pText->Offset = 5165;
	pText->DefaultLength = 12;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '9';
	pText->CurrValue[2] = '2';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '1';
	pText->CurrValue[5] = '6';
	pText->CurrValue[6] = '8';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '0';
	pText->CurrValue[9] = '.';
	pText->CurrValue[10] = '3';
	pText->CurrValue[11] = '"';
	pText->CurrLength = 12;

	/* for record 40 */
	POST_Record[40].Occupy = 1;
	POST_Record[40].PostType = POST_TYPE_TEXT;
	POST_Record[40].Name[0] = 'g';
	POST_Record[40].Name[1] = 'a';
	POST_Record[40].Name[2] = 't';
	POST_Record[40].Name[3] = 'e';
	POST_Record[40].Name[4] = 'w';
	POST_Record[40].Name[5] = 'a';
	POST_Record[40].Name[6] = 'y';
	POST_Record[40].Name[7] = '_';
	POST_Record[40].Name[8] = 'i';
	POST_Record[40].Name[9] = 'p';
	POST_Record[40].NameLen = 10;
	POST_Record[40].FileIndex = 7;
	POST_Record[40].UpdateSelf = FALSE;
	POST_Record[40].PValue = &BUF_Text[17];
	pText = POST_Record[40].PValue;
	pText->CurrValue = PostbufText17[0];
	pText->UserValue = PostbufText17[1];
	pText->Offset = 5297;
	pText->DefaultLength = 12;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '9';
	pText->CurrValue[2] = '2';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '1';
	pText->CurrValue[5] = '6';
	pText->CurrValue[6] = '8';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '0';
	pText->CurrValue[9] = '.';
	pText->CurrValue[10] = '1';
	pText->CurrValue[11] = '"';
	pText->CurrLength = 12;

	/* for record 41 */
	POST_Record[41].Occupy = 1;
	POST_Record[41].PostType = POST_TYPE_TEXT;
	POST_Record[41].Name[0] = 'm';
	POST_Record[41].Name[1] = 'a';
	POST_Record[41].Name[2] = 's';
	POST_Record[41].Name[3] = 'k';
	POST_Record[41].NameLen = 4;
	POST_Record[41].FileIndex = 7;
	POST_Record[41].UpdateSelf = FALSE;
	POST_Record[41].PValue = &BUF_Text[18];
	pText = POST_Record[41].PValue;
	pText->CurrValue = PostbufText18[0];
	pText->UserValue = PostbufText18[1];
	pText->Offset = 5419;
	pText->DefaultLength = 14;
	pText->CurrValue[0] = '2';
	pText->CurrValue[1] = '5';
	pText->CurrValue[2] = '5';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '2';
	pText->CurrValue[5] = '5';
	pText->CurrValue[6] = '5';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '2';
	pText->CurrValue[9] = '5';
	pText->CurrValue[10] = '5';
	pText->CurrValue[11] = '.';
	pText->CurrValue[12] = '0';
	pText->CurrValue[13] = '"';
	pText->CurrLength = 14;

	/* for record 42 */
	POST_Record[42].Occupy = 1;
	POST_Record[42].PostType = POST_TYPE_TEXT;
	POST_Record[42].Name[0] = 'd';
	POST_Record[42].Name[1] = 'n';
	POST_Record[42].Name[2] = 's';
	POST_Record[42].Name[3] = '_';
	POST_Record[42].Name[4] = 'i';
	POST_Record[42].Name[5] = 'p';
	POST_Record[42].NameLen = 6;
	POST_Record[42].FileIndex = 7;
	POST_Record[42].UpdateSelf = FALSE;
	POST_Record[42].PValue = &BUF_Text[19];
	pText = POST_Record[42].PValue;
	pText->CurrValue = PostbufText19[0];
	pText->UserValue = PostbufText19[1];
	pText->Offset = 5544;
	pText->DefaultLength = 11;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '6';
	pText->CurrValue[2] = '8';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '9';
	pText->CurrValue[5] = '5';
	pText->CurrValue[6] = '.';
	pText->CurrValue[7] = '1';
	pText->CurrValue[8] = '.';
	pText->CurrValue[9] = '1';
	pText->CurrValue[10] = '"';
	pText->CurrLength = 11;

	/* for record 43 */
	POST_Record[43].Occupy = 1;
	POST_Record[43].PostType = POST_TYPE_SELECT;
	POST_Record[43].Name[0] = 'd';
	POST_Record[43].Name[1] = 'h';
	POST_Record[43].Name[2] = 'c';
	POST_Record[43].Name[3] = 'p';
	POST_Record[43].Name[4] = 'c';
	POST_Record[43].NameLen = 5;
	POST_Record[43].FileIndex = 7;
	POST_Record[43].UpdateSelf = FALSE;
	POST_Record[43].PValue = &BUF_Select[18];
	pSelect = POST_Record[43].PValue;
	pSelect->Offset[0] = 5706;
	pSelect->Offset[1] = 5762;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 44 */
	POST_Record[44].Occupy = 1;
	POST_Record[44].PostType = POST_TYPE_SELECT;
	POST_Record[44].Name[0] = 'd';
	POST_Record[44].Name[1] = 'h';
	POST_Record[44].Name[2] = 'c';
	POST_Record[44].Name[3] = 'p';
	POST_Record[44].Name[4] = 's';
	POST_Record[44].NameLen = 5;
	POST_Record[44].FileIndex = 7;
	POST_Record[44].UpdateSelf = FALSE;
	POST_Record[44].PValue = &BUF_Select[19];
	pSelect = POST_Record[44].PValue;
	pSelect->Offset[0] = 5878;
	pSelect->Offset[1] = 5934;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 45 */
	POST_Record[45].Occupy = 1;
	POST_Record[45].PostType = POST_TYPE_TEXT;
	POST_Record[45].Name[0] = 'd';
	POST_Record[45].Name[1] = 'h';
	POST_Record[45].Name[2] = 'c';
	POST_Record[45].Name[3] = 'p';
	POST_Record[45].Name[4] = 's';
	POST_Record[45].Name[5] = '_';
	POST_Record[45].Name[6] = 's';
	POST_Record[45].Name[7] = 't';
	POST_Record[45].Name[8] = 'a';
	POST_Record[45].Name[9] = 'r';
	POST_Record[45].Name[10] = 't';
	POST_Record[45].Name[11] = 'i';
	POST_Record[45].Name[12] = 'p';
	POST_Record[45].NameLen = 13;
	POST_Record[45].FileIndex = 7;
	POST_Record[45].UpdateSelf = FALSE;
	POST_Record[45].PValue = &BUF_Text[20];
	pText = POST_Record[45].PValue;
	pText->CurrValue = PostbufText20[0];
	pText->UserValue = PostbufText20[1];
	pText->Offset = 6069;
	pText->DefaultLength = 12;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '9';
	pText->CurrValue[2] = '2';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '1';
	pText->CurrValue[5] = '6';
	pText->CurrValue[6] = '8';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '0';
	pText->CurrValue[9] = '.';
	pText->CurrValue[10] = '4';
	pText->CurrValue[11] = '"';
	pText->CurrLength = 12;

	/* for record 46 */
	POST_Record[46].Occupy = 1;
	POST_Record[46].PostType = POST_TYPE_TEXT;
	POST_Record[46].Name[0] = 'd';
	POST_Record[46].Name[1] = 'h';
	POST_Record[46].Name[2] = 'c';
	POST_Record[46].Name[3] = 'p';
	POST_Record[46].Name[4] = 's';
	POST_Record[46].Name[5] = '_';
	POST_Record[46].Name[6] = 'e';
	POST_Record[46].Name[7] = 'n';
	POST_Record[46].Name[8] = 'd';
	POST_Record[46].Name[9] = 'i';
	POST_Record[46].Name[10] = 'p';
	POST_Record[46].NameLen = 11;
	POST_Record[46].FileIndex = 7;
	POST_Record[46].UpdateSelf = FALSE;
	POST_Record[46].PValue = &BUF_Text[21];
	pText = POST_Record[46].PValue;
	pText->CurrValue = PostbufText21[0];
	pText->UserValue = PostbufText21[1];
	pText->Offset = 6203;
	pText->DefaultLength = 13;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '9';
	pText->CurrValue[2] = '2';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '1';
	pText->CurrValue[5] = '6';
	pText->CurrValue[6] = '8';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '0';
	pText->CurrValue[9] = '.';
	pText->CurrValue[10] = '1';
	pText->CurrValue[11] = '0';
	pText->CurrValue[12] = '"';
	pText->CurrLength = 13;

	/* for record 47 */
	POST_Record[47].Occupy = 1;
	POST_Record[47].PostType = POST_TYPE_TEXT;
	POST_Record[47].Name[0] = 'd';
	POST_Record[47].Name[1] = 'h';
	POST_Record[47].Name[2] = 'c';
	POST_Record[47].Name[3] = 'p';
	POST_Record[47].Name[4] = 's';
	POST_Record[47].Name[5] = '_';
	POST_Record[47].Name[6] = 'g';
	POST_Record[47].Name[7] = 'w';
	POST_Record[47].NameLen = 8;
	POST_Record[47].FileIndex = 7;
	POST_Record[47].UpdateSelf = FALSE;
	POST_Record[47].PValue = &BUF_Text[22];
	pText = POST_Record[47].PValue;
	pText->CurrValue = PostbufText22[0];
	pText->UserValue = PostbufText22[1];
	pText->Offset = 6475;
	pText->DefaultLength = 12;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '9';
	pText->CurrValue[2] = '2';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '1';
	pText->CurrValue[5] = '6';
	pText->CurrValue[6] = '8';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '0';
	pText->CurrValue[9] = '.';
	pText->CurrValue[10] = '3';
	pText->CurrValue[11] = '"';
	pText->CurrLength = 12;

	/* for record 48 */
	POST_Record[48].Occupy = 1;
	POST_Record[48].PostType = POST_TYPE_TEXT;
	POST_Record[48].Name[0] = 'd';
	POST_Record[48].Name[1] = 'h';
	POST_Record[48].Name[2] = 'c';
	POST_Record[48].Name[3] = 'p';
	POST_Record[48].Name[4] = 's';
	POST_Record[48].Name[5] = '_';
	POST_Record[48].Name[6] = 'l';
	POST_Record[48].Name[7] = 't';
	POST_Record[48].NameLen = 8;
	POST_Record[48].FileIndex = 7;
	POST_Record[48].UpdateSelf = FALSE;
	POST_Record[48].PValue = &BUF_Text[23];
	pText = POST_Record[48].PValue;
	pText->CurrValue = PostbufText23[0];
	pText->UserValue = PostbufText23[1];
	pText->Offset = 6614;
	pText->DefaultLength = 5;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '4';
	pText->CurrValue[2] = '4';
	pText->CurrValue[3] = '0';
	pText->CurrValue[4] = '"';
	pText->CurrLength = 5;

	/* for record 49 */
	POST_Record[49].Occupy = 1;
	POST_Record[49].PostType = POST_TYPE_TEXT;
	POST_Record[49].Name[0] = 'c';
	POST_Record[49].Name[1] = 'm';
	POST_Record[49].Name[2] = 'd';
	POST_Record[49].NameLen = 3;
	POST_Record[49].FileIndex = 9;
	POST_Record[49].UpdateSelf = FALSE;
	POST_Record[49].PValue = &BUF_Text[24];
	pText = POST_Record[49].PValue;
	pText->CurrValue = PostbufText24[0];
	pText->UserValue = PostbufText24[1];
	pText->Offset = 326;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 50 */
	POST_Record[50].Occupy = 1;
	POST_Record[50].PostType = POST_TYPE_TEXT;
	POST_Record[50].Name[0] = 'u';
	POST_Record[50].Name[1] = 's';
	POST_Record[50].Name[2] = 'e';
	POST_Record[50].Name[3] = 'r';
	POST_Record[50].Name[4] = 'n';
	POST_Record[50].Name[5] = 'a';
	POST_Record[50].Name[6] = 'm';
	POST_Record[50].Name[7] = 'e';
	POST_Record[50].NameLen = 8;
	POST_Record[50].FileIndex = 10;
	POST_Record[50].UpdateSelf = FALSE;
	POST_Record[50].PValue = &BUF_Text[25];
	pText = POST_Record[50].PValue;
	pText->CurrValue = PostbufText25[0];
	pText->UserValue = PostbufText25[1];
	pText->Offset = 1005;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 51 */
	POST_Record[51].Occupy = 1;
	POST_Record[51].PostType = POST_TYPE_PASSWORD;
	POST_Record[51].Name[0] = 'p';
	POST_Record[51].Name[1] = 'a';
	POST_Record[51].Name[2] = 's';
	POST_Record[51].Name[3] = 's';
	POST_Record[51].Name[4] = 'w';
	POST_Record[51].Name[5] = 'o';
	POST_Record[51].Name[6] = 'r';
	POST_Record[51].Name[7] = 'd';
	POST_Record[51].NameLen = 8;
	POST_Record[51].FileIndex = 10;
	POST_Record[51].UpdateSelf = FALSE;
	POST_Record[51].PValue = &BUF_Password[0];
	pPassword = POST_Record[51].PValue;
	pPassword->CurrValue = PostbufPassword0[0];
	pPassword->UserValue = PostbufPassword0[1];
	pPassword->Offset = 1114;
	pPassword->DefaultLength = 1;
	pPassword->CurrValue[0] = '"';
	pPassword->CurrLength = 1;

	/* for record 52 */
	POST_Record[52].Occupy = 1;
	POST_Record[52].PostType = POST_TYPE_SUBMIT;
	POST_Record[52].Name[0] = 'L';
	POST_Record[52].Name[1] = 'o';
	POST_Record[52].Name[2] = 'g';
	POST_Record[52].Name[3] = 'i';
	POST_Record[52].Name[4] = 'n';
	POST_Record[52].NameLen = 5;
	POST_Record[52].FileIndex = 255;
	POST_Record[52].UpdateSelf = FALSE;
	POST_Record[52].PValue = &BUF_Submit[5];
	pSubmit = POST_Record[52].PValue;
	pSubmit->Value[0] = 'L';
	pSubmit->Value[1] = 'o';
	pSubmit->Value[2] = 'g';
	pSubmit->Value[3] = 'i';
	pSubmit->Value[4] = 'n';
	pSubmit->DefaultVlaueLen = 5;
	pSubmit->IsApply = 1;

	/* for record 53 */
	POST_Record[53].Occupy = 1;
	POST_Record[53].PostType = POST_TYPE_RADIO;
	POST_Record[53].Name[0] = 'r';
	POST_Record[53].Name[1] = 't';
	POST_Record[53].Name[2] = 'c';
	POST_Record[53].Name[3] = '_';
	POST_Record[53].Name[4] = 'c';
	POST_Record[53].Name[5] = 'l';
	POST_Record[53].Name[6] = 'b';
	POST_Record[53].NameLen = 7;
	POST_Record[53].FileIndex = 11;
	POST_Record[53].UpdateSelf = FALSE;
	POST_Record[53].PValue = &BUF_Radio[0];
	pRadio = POST_Record[53].PValue;
	pRadio->Offset[0] = 1418;
	pRadio->Value[0][0] = '0';
	pRadio->Length[0] = 1;
	pRadio->Offset[1] = 1493;
	pRadio->Value[1][0] = '1';
	pRadio->Length[1] = 1;
	pRadio->Count = 2;
	pRadio->DefaultSet = 1;
	pRadio->CurrentSet = 1;
	pRadio->UserSet = 1;

	/* for record 54 */
	POST_Record[54].Occupy = 1;
	POST_Record[54].PostType = POST_TYPE_SELECT;
	POST_Record[54].Name[0] = 'd';
	POST_Record[54].Name[1] = 's';
	POST_Record[54].Name[2] = 't';
	POST_Record[54].Name[3] = '_';
	POST_Record[54].Name[4] = 'o';
	POST_Record[54].Name[5] = 'n';
	POST_Record[54].Name[6] = 'o';
	POST_Record[54].Name[7] = 'f';
	POST_Record[54].Name[8] = 'f';
	POST_Record[54].NameLen = 9;
	POST_Record[54].FileIndex = 11;
	POST_Record[54].UpdateSelf = FALSE;
	POST_Record[54].PValue = &BUF_Select[20];
	pSelect = POST_Record[54].PValue;
	pSelect->Offset[0] = 1672;
	pSelect->Offset[1] = 1726;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 55 */
	POST_Record[55].Occupy = 1;
	POST_Record[55].PostType = POST_TYPE_TEXT;
	POST_Record[55].Name[0] = 'r';
	POST_Record[55].Name[1] = 't';
	POST_Record[55].Name[2] = 'c';
	POST_Record[55].Name[3] = '_';
	POST_Record[55].Name[4] = 'y';
	POST_Record[55].Name[5] = 'y';
	POST_Record[55].NameLen = 6;
	POST_Record[55].FileIndex = 11;
	POST_Record[55].UpdateSelf = FALSE;
	POST_Record[55].PValue = &BUF_Text[26];
	pText = POST_Record[55].PValue;
	pText->CurrValue = PostbufText26[0];
	pText->UserValue = PostbufText26[1];
	pText->Offset = 2030;
	pText->DefaultLength = 5;
	pText->CurrValue[0] = '2';
	pText->CurrValue[1] = '0';
	pText->CurrValue[2] = '1';
	pText->CurrValue[3] = '1';
	pText->CurrValue[4] = '"';
	pText->CurrLength = 5;

	/* for record 56 */
	POST_Record[56].Occupy = 1;
	POST_Record[56].PostType = POST_TYPE_TEXT;
	POST_Record[56].Name[0] = 'r';
	POST_Record[56].Name[1] = 't';
	POST_Record[56].Name[2] = 'c';
	POST_Record[56].Name[3] = '_';
	POST_Record[56].Name[4] = 'm';
	POST_Record[56].Name[5] = 'm';
	POST_Record[56].NameLen = 6;
	POST_Record[56].FileIndex = 11;
	POST_Record[56].UpdateSelf = FALSE;
	POST_Record[56].PValue = &BUF_Text[27];
	pText = POST_Record[56].PValue;
	pText->CurrValue = PostbufText27[0];
	pText->UserValue = PostbufText27[1];
	pText->Offset = 2116;
	pText->DefaultLength = 3;
	pText->CurrValue[0] = '0';
	pText->CurrValue[1] = '4';
	pText->CurrValue[2] = '"';
	pText->CurrLength = 3;

	/* for record 57 */
	POST_Record[57].Occupy = 1;
	POST_Record[57].PostType = POST_TYPE_TEXT;
	POST_Record[57].Name[0] = 'r';
	POST_Record[57].Name[1] = 't';
	POST_Record[57].Name[2] = 'c';
	POST_Record[57].Name[3] = '_';
	POST_Record[57].Name[4] = 'd';
	POST_Record[57].Name[5] = 'd';
	POST_Record[57].NameLen = 6;
	POST_Record[57].FileIndex = 11;
	POST_Record[57].UpdateSelf = FALSE;
	POST_Record[57].PValue = &BUF_Text[28];
	pText = POST_Record[57].PValue;
	pText->CurrValue = PostbufText28[0];
	pText->UserValue = PostbufText28[1];
	pText->Offset = 2200;
	pText->DefaultLength = 3;
	pText->CurrValue[0] = '0';
	pText->CurrValue[1] = '6';
	pText->CurrValue[2] = '"';
	pText->CurrLength = 3;

	/* for record 58 */
	POST_Record[58].Occupy = 1;
	POST_Record[58].PostType = POST_TYPE_TEXT;
	POST_Record[58].Name[0] = 'r';
	POST_Record[58].Name[1] = 't';
	POST_Record[58].Name[2] = 'c';
	POST_Record[58].Name[3] = '_';
	POST_Record[58].Name[4] = 'h';
	POST_Record[58].Name[5] = 'h';
	POST_Record[58].NameLen = 6;
	POST_Record[58].FileIndex = 11;
	POST_Record[58].UpdateSelf = FALSE;
	POST_Record[58].PValue = &BUF_Text[29];
	pText = POST_Record[58].PValue;
	pText->CurrValue = PostbufText29[0];
	pText->UserValue = PostbufText29[1];
	pText->Offset = 2472;
	pText->DefaultLength = 3;
	pText->CurrValue[0] = '0';
	pText->CurrValue[1] = '0';
	pText->CurrValue[2] = '"';
	pText->CurrLength = 3;

	/* for record 59 */
	POST_Record[59].Occupy = 1;
	POST_Record[59].PostType = POST_TYPE_TEXT;
	POST_Record[59].Name[0] = 'r';
	POST_Record[59].Name[1] = 't';
	POST_Record[59].Name[2] = 'c';
	POST_Record[59].Name[3] = '_';
	POST_Record[59].Name[4] = 'n';
	POST_Record[59].Name[5] = 'n';
	POST_Record[59].NameLen = 6;
	POST_Record[59].FileIndex = 11;
	POST_Record[59].UpdateSelf = FALSE;
	POST_Record[59].PValue = &BUF_Text[30];
	pText = POST_Record[59].PValue;
	pText->CurrValue = PostbufText30[0];
	pText->UserValue = PostbufText30[1];
	pText->Offset = 2555;
	pText->DefaultLength = 3;
	pText->CurrValue[0] = '0';
	pText->CurrValue[1] = '4';
	pText->CurrValue[2] = '"';
	pText->CurrLength = 3;

	/* for record 60 */
	POST_Record[60].Occupy = 1;
	POST_Record[60].PostType = POST_TYPE_TEXT;
	POST_Record[60].Name[0] = 'r';
	POST_Record[60].Name[1] = 't';
	POST_Record[60].Name[2] = 'c';
	POST_Record[60].Name[3] = '_';
	POST_Record[60].Name[4] = 's';
	POST_Record[60].Name[5] = 's';
	POST_Record[60].NameLen = 6;
	POST_Record[60].FileIndex = 11;
	POST_Record[60].UpdateSelf = FALSE;
	POST_Record[60].PValue = &BUF_Text[31];
	pText = POST_Record[60].PValue;
	pText->CurrValue = PostbufText31[0];
	pText->UserValue = PostbufText31[1];
	pText->Offset = 2638;
	pText->DefaultLength = 3;
	pText->CurrValue[0] = '0';
	pText->CurrValue[1] = '6';
	pText->CurrValue[2] = '"';
	pText->CurrLength = 3;

	/* for record 61 */
	POST_Record[61].Occupy = 1;
	POST_Record[61].PostType = POST_TYPE_SELECT;
	POST_Record[61].Name[0] = 'n';
	POST_Record[61].Name[1] = 't';
	POST_Record[61].Name[2] = 'p';
	POST_Record[61].Name[3] = 's';
	POST_Record[61].Name[4] = '_';
	POST_Record[61].Name[5] = 't';
	POST_Record[61].Name[6] = 'z';
	POST_Record[61].NameLen = 7;
	POST_Record[61].FileIndex = 11;
	POST_Record[61].UpdateSelf = FALSE;
	POST_Record[61].PValue = &BUF_Select[21];
	pSelect = POST_Record[61].PValue;
	pSelect->Offset[0] = 3145;
	pSelect->Offset[1] = 3222;
	pSelect->Offset[2] = 3291;
	pSelect->Offset[3] = 3345;
	pSelect->Offset[4] = 3398;
	pSelect->Offset[5] = 3465;
	pSelect->Offset[6] = 3526;
	pSelect->Offset[7] = 3592;
	pSelect->Offset[8] = 3655;
	pSelect->Offset[9] = 3710;
	pSelect->Offset[10] = 3778;
	pSelect->Offset[11] = 3845;
	pSelect->Offset[12] = 3899;
	pSelect->Offset[13] = 3961;
	pSelect->Offset[14] = 4022;
	pSelect->Offset[15] = 4085;
	pSelect->Offset[16] = 4148;
	pSelect->Offset[17] = 4211;
	pSelect->Offset[18] = 4277;
	pSelect->Offset[19] = 4338;
	pSelect->Offset[20] = 4402;
	pSelect->Offset[21] = 4470;
	pSelect->Offset[22] = 4530;
	pSelect->Offset[23] = 4596;
	pSelect->Offset[24] = 4656;
	pSelect->Count = 25;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 62 */
	POST_Record[62].Occupy = 1;
	POST_Record[62].PostType = POST_TYPE_TEXT;
	POST_Record[62].Name[0] = 'n';
	POST_Record[62].Name[1] = 't';
	POST_Record[62].Name[2] = 'p';
	POST_Record[62].Name[3] = 's';
	POST_Record[62].Name[4] = '_';
	POST_Record[62].Name[5] = 'n';
	POST_Record[62].Name[6] = 'a';
	POST_Record[62].Name[7] = 'm';
	POST_Record[62].Name[8] = 'e';
	POST_Record[62].Name[9] = '1';
	POST_Record[62].NameLen = 10;
	POST_Record[62].FileIndex = 11;
	POST_Record[62].UpdateSelf = FALSE;
	POST_Record[62].PValue = &BUF_Text[32];
	pText = POST_Record[62].PValue;
	pText->CurrValue = PostbufText32[0];
	pText->UserValue = PostbufText32[1];
	pText->Offset = 4806;
	pText->DefaultLength = 17;
	pText->CurrValue[0] = 't';
	pText->CurrValue[1] = 'i';
	pText->CurrValue[2] = 'm';
	pText->CurrValue[3] = 'e';
	pText->CurrValue[4] = '.';
	pText->CurrValue[5] = 's';
	pText->CurrValue[6] = 't';
	pText->CurrValue[7] = 'd';
	pText->CurrValue[8] = 't';
	pText->CurrValue[9] = 'i';
	pText->CurrValue[10] = 'm';
	pText->CurrValue[11] = 'e';
	pText->CurrValue[12] = '.';
	pText->CurrValue[13] = 'g';
	pText->CurrValue[14] = 'o';
	pText->CurrValue[15] = 'v';
	pText->CurrValue[16] = '"';
	pText->CurrLength = 17;

	/* for record 63 */
	POST_Record[63].Occupy = 1;
	POST_Record[63].PostType = POST_TYPE_TEXT;
	POST_Record[63].Name[0] = 'n';
	POST_Record[63].Name[1] = 't';
	POST_Record[63].Name[2] = 'p';
	POST_Record[63].Name[3] = 's';
	POST_Record[63].Name[4] = '_';
	POST_Record[63].Name[5] = 'n';
	POST_Record[63].Name[6] = 'a';
	POST_Record[63].Name[7] = 'm';
	POST_Record[63].Name[8] = 'e';
	POST_Record[63].Name[9] = '2';
	POST_Record[63].NameLen = 10;
	POST_Record[63].FileIndex = 11;
	POST_Record[63].UpdateSelf = FALSE;
	POST_Record[63].PValue = &BUF_Text[33];
	pText = POST_Record[63].PValue;
	pText->CurrValue = PostbufText33[0];
	pText->UserValue = PostbufText33[1];
	pText->Offset = 4948;
	pText->DefaultLength = 17;
	pText->CurrValue[0] = 't';
	pText->CurrValue[1] = 'i';
	pText->CurrValue[2] = 'c';
	pText->CurrValue[3] = 'k';
	pText->CurrValue[4] = '.';
	pText->CurrValue[5] = 's';
	pText->CurrValue[6] = 't';
	pText->CurrValue[7] = 'd';
	pText->CurrValue[8] = 't';
	pText->CurrValue[9] = 'i';
	pText->CurrValue[10] = 'm';
	pText->CurrValue[11] = 'e';
	pText->CurrValue[12] = '.';
	pText->CurrValue[13] = 'g';
	pText->CurrValue[14] = 'o';
	pText->CurrValue[15] = 'v';
	pText->CurrValue[16] = '"';
	pText->CurrLength = 17;

	/* for record 64 */
	POST_Record[64].Occupy = 1;
	POST_Record[64].PostType = POST_TYPE_TEXT;
	POST_Record[64].Name[0] = 'n';
	POST_Record[64].Name[1] = 't';
	POST_Record[64].Name[2] = 'p';
	POST_Record[64].Name[3] = 's';
	POST_Record[64].Name[4] = '_';
	POST_Record[64].Name[5] = 'n';
	POST_Record[64].Name[6] = 'a';
	POST_Record[64].Name[7] = 'm';
	POST_Record[64].Name[8] = 'e';
	POST_Record[64].Name[9] = '3';
	POST_Record[64].NameLen = 10;
	POST_Record[64].FileIndex = 11;
	POST_Record[64].UpdateSelf = FALSE;
	POST_Record[64].PValue = &BUF_Text[34];
	pText = POST_Record[64].PValue;
	pText->CurrValue = PostbufText34[0];
	pText->UserValue = PostbufText34[1];
	pText->Offset = 5090;
	pText->DefaultLength = 17;
	pText->CurrValue[0] = 't';
	pText->CurrValue[1] = 'o';
	pText->CurrValue[2] = 'c';
	pText->CurrValue[3] = 'k';
	pText->CurrValue[4] = '.';
	pText->CurrValue[5] = 's';
	pText->CurrValue[6] = 't';
	pText->CurrValue[7] = 'd';
	pText->CurrValue[8] = 't';
	pText->CurrValue[9] = 'i';
	pText->CurrValue[10] = 'm';
	pText->CurrValue[11] = 'e';
	pText->CurrValue[12] = '.';
	pText->CurrValue[13] = 'g';
	pText->CurrValue[14] = 'o';
	pText->CurrValue[15] = 'v';
	pText->CurrValue[16] = '"';
	pText->CurrLength = 17;

	/* for record 65 */
	POST_Record[65].Occupy = 1;
	POST_Record[65].PostType = POST_TYPE_TEXT;
	POST_Record[65].Name[0] = 'n';
	POST_Record[65].Name[1] = 'e';
	POST_Record[65].Name[2] = 'w';
	POST_Record[65].Name[3] = '_';
	POST_Record[65].Name[4] = 'u';
	POST_Record[65].Name[5] = 's';
	POST_Record[65].Name[6] = 'n';
	POST_Record[65].NameLen = 7;
	POST_Record[65].FileIndex = 12;
	POST_Record[65].UpdateSelf = FALSE;
	POST_Record[65].PValue = &BUF_Text[35];
	pText = POST_Record[65].PValue;
	pText->CurrValue = PostbufText35[0];
	pText->UserValue = PostbufText35[1];
	pText->Offset = 1368;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 66 */
	POST_Record[66].Occupy = 1;
	POST_Record[66].PostType = POST_TYPE_PASSWORD;
	POST_Record[66].Name[0] = 'o';
	POST_Record[66].Name[1] = 'l';
	POST_Record[66].Name[2] = 'd';
	POST_Record[66].Name[3] = '_';
	POST_Record[66].Name[4] = 'p';
	POST_Record[66].Name[5] = 's';
	POST_Record[66].Name[6] = 'w';
	POST_Record[66].NameLen = 7;
	POST_Record[66].FileIndex = 12;
	POST_Record[66].UpdateSelf = FALSE;
	POST_Record[66].PValue = &BUF_Password[1];
	pPassword = POST_Record[66].PValue;
	pPassword->CurrValue = PostbufPassword1[0];
	pPassword->UserValue = PostbufPassword1[1];
	pPassword->Offset = 1779;
	pPassword->DefaultLength = 1;
	pPassword->CurrValue[0] = '"';
	pPassword->CurrLength = 1;

	/* for record 67 */
	POST_Record[67].Occupy = 1;
	POST_Record[67].PostType = POST_TYPE_PASSWORD;
	POST_Record[67].Name[0] = 'n';
	POST_Record[67].Name[1] = 'e';
	POST_Record[67].Name[2] = 'w';
	POST_Record[67].Name[3] = '_';
	POST_Record[67].Name[4] = 'p';
	POST_Record[67].Name[5] = 's';
	POST_Record[67].Name[6] = 'w';
	POST_Record[67].NameLen = 7;
	POST_Record[67].FileIndex = 12;
	POST_Record[67].UpdateSelf = FALSE;
	POST_Record[67].PValue = &BUF_Password[2];
	pPassword = POST_Record[67].PValue;
	pPassword->CurrValue = PostbufPassword2[0];
	pPassword->UserValue = PostbufPassword2[1];
	pPassword->Offset = 1891;
	pPassword->DefaultLength = 1;
	pPassword->CurrValue[0] = '"';
	pPassword->CurrLength = 1;

	/* for record 68 */
	POST_Record[68].Occupy = 1;
	POST_Record[68].PostType = POST_TYPE_PASSWORD;
	POST_Record[68].Name[0] = 'c';
	POST_Record[68].Name[1] = 'f';
	POST_Record[68].Name[2] = 'm';
	POST_Record[68].Name[3] = '_';
	POST_Record[68].Name[4] = 'p';
	POST_Record[68].Name[5] = 's';
	POST_Record[68].Name[6] = 'w';
	POST_Record[68].NameLen = 7;
	POST_Record[68].FileIndex = 12;
	POST_Record[68].UpdateSelf = FALSE;
	POST_Record[68].PValue = &BUF_Password[3];
	pPassword = POST_Record[68].PValue;
	pPassword->CurrValue = PostbufPassword3[0];
	pPassword->UserValue = PostbufPassword3[1];
	pPassword->Offset = 2007;
	pPassword->DefaultLength = 1;
	pPassword->CurrValue[0] = '"';
	pPassword->CurrLength = 1;

	/* for record 69 */
	POST_Record[69].Occupy = 1;
	POST_Record[69].PostType = POST_TYPE_TEXT;
	POST_Record[69].Name[0] = 's';
	POST_Record[69].Name[1] = 'm';
	POST_Record[69].Name[2] = 't';
	POST_Record[69].Name[3] = 'p';
	POST_Record[69].Name[4] = '_';
	POST_Record[69].Name[5] = 'u';
	POST_Record[69].Name[6] = 'n';
	POST_Record[69].NameLen = 7;
	POST_Record[69].FileIndex = 12;
	POST_Record[69].UpdateSelf = FALSE;
	POST_Record[69].PValue = &BUF_Text[36];
	pText = POST_Record[69].PValue;
	pText->CurrValue = PostbufText36[0];
	pText->UserValue = PostbufText36[1];
	pText->Offset = 2426;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 70 */
	POST_Record[70].Occupy = 1;
	POST_Record[70].PostType = POST_TYPE_PASSWORD;
	POST_Record[70].Name[0] = 's';
	POST_Record[70].Name[1] = 'm';
	POST_Record[70].Name[2] = 't';
	POST_Record[70].Name[3] = 'p';
	POST_Record[70].Name[4] = '_';
	POST_Record[70].Name[5] = 'p';
	POST_Record[70].Name[6] = 'w';
	POST_Record[70].NameLen = 7;
	POST_Record[70].FileIndex = 12;
	POST_Record[70].UpdateSelf = FALSE;
	POST_Record[70].PValue = &BUF_Password[4];
	pPassword = POST_Record[70].PValue;
	pPassword->CurrValue = PostbufPassword4[0];
	pPassword->UserValue = PostbufPassword4[1];
	pPassword->Offset = 2553;
	pPassword->DefaultLength = 1;
	pPassword->CurrValue[0] = '"';
	pPassword->CurrLength = 1;

	/* for record 71 */
	POST_Record[71].Occupy = 1;
	POST_Record[71].PostType = POST_TYPE_TEXT;
	POST_Record[71].Name[0] = 'g';
	POST_Record[71].Name[1] = 'n';
	POST_Record[71].Name[2] = '_';
	POST_Record[71].Name[3] = 'h';
	POST_Record[71].Name[4] = 'o';
	POST_Record[71].Name[5] = 's';
	POST_Record[71].Name[6] = 't';
	POST_Record[71].NameLen = 7;
	POST_Record[71].FileIndex = 12;
	POST_Record[71].UpdateSelf = FALSE;
	POST_Record[71].PValue = &BUF_Text[37];
	pText = POST_Record[71].PValue;
	pText->CurrValue = PostbufText37[0];
	pText->UserValue = PostbufText37[1];
	pText->Offset = 3033;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 72 */
	POST_Record[72].Occupy = 1;
	POST_Record[72].PostType = POST_TYPE_TEXT;
	POST_Record[72].Name[0] = 'b';
	POST_Record[72].Name[1] = 'm';
	POST_Record[72].Name[2] = '_';
	POST_Record[72].Name[3] = 'h';
	POST_Record[72].Name[4] = 'o';
	POST_Record[72].Name[5] = 's';
	POST_Record[72].Name[6] = 't';
	POST_Record[72].NameLen = 7;
	POST_Record[72].FileIndex = 12;
	POST_Record[72].UpdateSelf = FALSE;
	POST_Record[72].PValue = &BUF_Text[38];
	pText = POST_Record[72].PValue;
	pText->CurrValue = PostbufText38[0];
	pText->UserValue = PostbufText38[1];
	pText->Offset = 3608;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 73 */
	POST_Record[73].Occupy = 1;
	POST_Record[73].PostType = POST_TYPE_TEXT;
	POST_Record[73].Name[0] = 'b';
	POST_Record[73].Name[1] = 'm';
	POST_Record[73].Name[2] = '_';
	POST_Record[73].Name[3] = 'u';
	POST_Record[73].Name[4] = 'n';
	POST_Record[73].NameLen = 5;
	POST_Record[73].FileIndex = 12;
	POST_Record[73].UpdateSelf = FALSE;
	POST_Record[73].PValue = &BUF_Text[39];
	pText = POST_Record[73].PValue;
	pText->CurrValue = PostbufText39[0];
	pText->UserValue = PostbufText39[1];
	pText->Offset = 3729;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 74 */
	POST_Record[74].Occupy = 1;
	POST_Record[74].PostType = POST_TYPE_PASSWORD;
	POST_Record[74].Name[0] = 'b';
	POST_Record[74].Name[1] = 'm';
	POST_Record[74].Name[2] = '_';
	POST_Record[74].Name[3] = 'p';
	POST_Record[74].Name[4] = 'w';
	POST_Record[74].NameLen = 5;
	POST_Record[74].FileIndex = 12;
	POST_Record[74].UpdateSelf = FALSE;
	POST_Record[74].PValue = &BUF_Password[5];
	pPassword = POST_Record[74].PValue;
	pPassword->CurrValue = PostbufPassword5[0];
	pPassword->UserValue = PostbufPassword5[1];
	pPassword->Offset = 3854;
	pPassword->DefaultLength = 1;
	pPassword->CurrValue[0] = '"';
	pPassword->CurrLength = 1;

	/* for record 75 */
	POST_Record[75].Occupy = 1;
	POST_Record[75].PostType = POST_TYPE_TEXT;
	POST_Record[75].Name[0] = 'b';
	POST_Record[75].Name[1] = 'm';
	POST_Record[75].Name[2] = '_';
	POST_Record[75].Name[3] = 'o';
	POST_Record[75].Name[4] = 'r';
	POST_Record[75].Name[5] = 'g';
	POST_Record[75].Name[6] = 'i';
	POST_Record[75].Name[7] = 'd';
	POST_Record[75].NameLen = 8;
	POST_Record[75].FileIndex = 12;
	POST_Record[75].UpdateSelf = FALSE;
	POST_Record[75].PValue = &BUF_Text[40];
	pText = POST_Record[75].PValue;
	pText->CurrValue = PostbufText40[0];
	pText->UserValue = PostbufText40[1];
	pText->Offset = 3985;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 76 */
	POST_Record[76].Occupy = 1;
	POST_Record[76].PostType = POST_TYPE_TEXT;
	POST_Record[76].Name[0] = 'b';
	POST_Record[76].Name[1] = 'm';
	POST_Record[76].Name[2] = '_';
	POST_Record[76].Name[3] = 't';
	POST_Record[76].Name[4] = 'y';
	POST_Record[76].Name[5] = 'p';
	POST_Record[76].Name[6] = 'i';
	POST_Record[76].Name[7] = 'd';
	POST_Record[76].NameLen = 8;
	POST_Record[76].FileIndex = 12;
	POST_Record[76].UpdateSelf = FALSE;
	POST_Record[76].PValue = &BUF_Text[41];
	pText = POST_Record[76].PValue;
	pText->CurrValue = PostbufText41[0];
	pText->UserValue = PostbufText41[1];
	pText->Offset = 4164;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 77 */
	POST_Record[77].Occupy = 1;
	POST_Record[77].PostType = POST_TYPE_TEXT;
	POST_Record[77].Name[0] = 'b';
	POST_Record[77].Name[1] = 'm';
	POST_Record[77].Name[2] = '_';
	POST_Record[77].Name[3] = 'd';
	POST_Record[77].Name[4] = 'e';
	POST_Record[77].Name[5] = 'v';
	POST_Record[77].Name[6] = 'i';
	POST_Record[77].Name[7] = 'd';
	POST_Record[77].NameLen = 8;
	POST_Record[77].FileIndex = 12;
	POST_Record[77].UpdateSelf = FALSE;
	POST_Record[77].PValue = &BUF_Text[42];
	pText = POST_Record[77].PValue;
	pText->CurrValue = PostbufText42[0];
	pText->UserValue = PostbufText42[1];
	pText->Offset = 4345;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 78 */
	POST_Record[78].Occupy = 1;
	POST_Record[78].PostType = POST_TYPE_TEXT;
	POST_Record[78].Name[0] = 'b';
	POST_Record[78].Name[1] = 'm';
	POST_Record[78].Name[2] = '_';
	POST_Record[78].Name[3] = 'a';
	POST_Record[78].Name[4] = 'p';
	POST_Record[78].Name[5] = 'i';
	POST_Record[78].Name[6] = 'v';
	POST_Record[78].Name[7] = 'e';
	POST_Record[78].Name[8] = 'r';
	POST_Record[78].NameLen = 9;
	POST_Record[78].FileIndex = 12;
	POST_Record[78].UpdateSelf = FALSE;
	POST_Record[78].PValue = &BUF_Text[43];
	pText = POST_Record[78].PValue;
	pText->CurrValue = PostbufText43[0];
	pText->UserValue = PostbufText43[1];
	pText->Offset = 4531;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 79 */
	POST_Record[79].Occupy = 1;
	POST_Record[79].PostType = POST_TYPE_TEXT;
	POST_Record[79].Name[0] = 'b';
	POST_Record[79].Name[1] = 'm';
	POST_Record[79].Name[2] = '_';
	POST_Record[79].Name[3] = 'a';
	POST_Record[79].Name[4] = 'p';
	POST_Record[79].Name[5] = 'i';
	POST_Record[79].Name[6] = 'k';
	POST_Record[79].Name[7] = 'e';
	POST_Record[79].Name[8] = 'y';
	POST_Record[79].NameLen = 9;
	POST_Record[79].FileIndex = 12;
	POST_Record[79].UpdateSelf = FALSE;
	POST_Record[79].PValue = &BUF_Text[44];
	pText = POST_Record[79].PValue;
	pText->CurrValue = PostbufText44[0];
	pText->UserValue = PostbufText44[1];
	pText->Offset = 4738;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 80 */
	POST_Record[80].Occupy = 1;
	POST_Record[80].PostType = POST_TYPE_TEXT;
	POST_Record[80].Name[0] = 'b';
	POST_Record[80].Name[1] = 'm';
	POST_Record[80].Name[2] = '_';
	POST_Record[80].Name[3] = 'a';
	POST_Record[80].Name[4] = 'p';
	POST_Record[80].Name[5] = 'i';
	POST_Record[80].Name[6] = 't';
	POST_Record[80].Name[7] = 'o';
	POST_Record[80].Name[8] = 'k';
	POST_Record[80].Name[9] = 'e';
	POST_Record[80].Name[10] = 'n';
	POST_Record[80].NameLen = 11;
	POST_Record[80].FileIndex = 12;
	POST_Record[80].UpdateSelf = FALSE;
	POST_Record[80].PValue = &BUF_Text[45];
	pText = POST_Record[80].PValue;
	pText->CurrValue = PostbufText45[0];
	pText->UserValue = PostbufText45[1];
	pText->Offset = 4970;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 81 */
	POST_Record[81].Occupy = 1;
	POST_Record[81].PostType = POST_TYPE_TEXT;
	POST_Record[81].Name[0] = 'a';
	POST_Record[81].Name[1] = 'z';
	POST_Record[81].Name[2] = '_';
	POST_Record[81].Name[3] = 'h';
	POST_Record[81].Name[4] = 'o';
	POST_Record[81].Name[5] = 's';
	POST_Record[81].Name[6] = 't';
	POST_Record[81].NameLen = 7;
	POST_Record[81].FileIndex = 12;
	POST_Record[81].UpdateSelf = FALSE;
	POST_Record[81].PValue = &BUF_Text[46];
	pText = POST_Record[81].PValue;
	pText->CurrValue = PostbufText46[0];
	pText->UserValue = PostbufText46[1];
	pText->Offset = 5543;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 82 */
	POST_Record[82].Occupy = 1;
	POST_Record[82].PostType = POST_TYPE_TEXT;
	POST_Record[82].Name[0] = 'a';
	POST_Record[82].Name[1] = 'z';
	POST_Record[82].Name[2] = '_';
	POST_Record[82].Name[3] = 'h';
	POST_Record[82].Name[4] = 'n';
	POST_Record[82].NameLen = 5;
	POST_Record[82].FileIndex = 12;
	POST_Record[82].UpdateSelf = FALSE;
	POST_Record[82].PValue = &BUF_Text[47];
	pText = POST_Record[82].PValue;
	pText->CurrValue = PostbufText47[0];
	pText->UserValue = PostbufText47[1];
	pText->Offset = 5664;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 83 */
	POST_Record[83].Occupy = 1;
	POST_Record[83].PostType = POST_TYPE_TEXT;
	POST_Record[83].Name[0] = 'a';
	POST_Record[83].Name[1] = 'z';
	POST_Record[83].Name[2] = '_';
	POST_Record[83].Name[3] = 'd';
	POST_Record[83].Name[4] = 'i';
	POST_Record[83].NameLen = 5;
	POST_Record[83].FileIndex = 12;
	POST_Record[83].UpdateSelf = FALSE;
	POST_Record[83].PValue = &BUF_Text[48];
	pText = POST_Record[83].PValue;
	pText->CurrValue = PostbufText48[0];
	pText->UserValue = PostbufText48[1];
	pText->Offset = 5786;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 84 */
	POST_Record[84].Occupy = 1;
	POST_Record[84].PostType = POST_TYPE_PASSWORD;
	POST_Record[84].Name[0] = 'a';
	POST_Record[84].Name[1] = 'z';
	POST_Record[84].Name[2] = '_';
	POST_Record[84].Name[3] = 's';
	POST_Record[84].Name[4] = 'k';
	POST_Record[84].NameLen = 5;
	POST_Record[84].FileIndex = 12;
	POST_Record[84].UpdateSelf = FALSE;
	POST_Record[84].PValue = &BUF_Password[6];
	pPassword = POST_Record[84].PValue;
	pPassword->CurrValue = PostbufPassword6[0];
	pPassword->UserValue = PostbufPassword6[1];
	pPassword->Offset = 6016;
	pPassword->DefaultLength = 1;
	pPassword->CurrValue[0] = '"';
	pPassword->CurrLength = 1;

	/* for record 85 */
	POST_Record[85].Occupy = 1;
	POST_Record[85].PostType = POST_TYPE_TEXT;
	POST_Record[85].Name[0] = 'a';
	POST_Record[85].Name[1] = 'z';
	POST_Record[85].Name[2] = '_';
	POST_Record[85].Name[3] = 'e';
	POST_Record[85].Name[4] = 't';
	POST_Record[85].NameLen = 5;
	POST_Record[85].FileIndex = 12;
	POST_Record[85].UpdateSelf = FALSE;
	POST_Record[85].PValue = &BUF_Text[49];
	pText = POST_Record[85].PValue;
	pText->CurrValue = PostbufText49[0];
	pText->UserValue = PostbufText49[1];
	pText->Offset = 6140;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 86 */
	POST_Record[86].Occupy = 1;
	POST_Record[86].PostType = POST_TYPE_SELECT;
	POST_Record[86].Name[0] = 'n';
	POST_Record[86].Name[1] = 'e';
	POST_Record[86].Name[2] = 't';
	POST_Record[86].Name[3] = 'w';
	POST_Record[86].Name[4] = 'o';
	POST_Record[86].Name[5] = 'r';
	POST_Record[86].Name[6] = 'k';
	POST_Record[86].Name[7] = '_';
	POST_Record[86].Name[8] = 'm';
	POST_Record[86].Name[9] = 'o';
	POST_Record[86].Name[10] = 'd';
	POST_Record[86].Name[11] = 'e';
	POST_Record[86].NameLen = 12;
	POST_Record[86].FileIndex = 16;
	POST_Record[86].UpdateSelf = FALSE;
	POST_Record[86].PValue = &BUF_Select[22];
	pSelect = POST_Record[86].PValue;
	pSelect->Offset[0] = 1415;
	pSelect->Offset[1] = 1472;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 87 */
	POST_Record[87].Occupy = 1;
	POST_Record[87].PostType = POST_TYPE_SELECT;
	POST_Record[87].Name[0] = 'c';
	POST_Record[87].Name[1] = 'h';
	POST_Record[87].Name[2] = 'a';
	POST_Record[87].Name[3] = 'n';
	POST_Record[87].Name[4] = 'n';
	POST_Record[87].Name[5] = 'e';
	POST_Record[87].Name[6] = 'l';
	POST_Record[87].NameLen = 7;
	POST_Record[87].FileIndex = 16;
	POST_Record[87].UpdateSelf = FALSE;
	POST_Record[87].PValue = &BUF_Select[23];
	pSelect = POST_Record[87].PValue;
	pSelect->Offset[0] = 1592;
	pSelect->Offset[1] = 1643;
	pSelect->Offset[2] = 1684;
	pSelect->Offset[3] = 1725;
	pSelect->Offset[4] = 1766;
	pSelect->Offset[5] = 1807;
	pSelect->Offset[6] = 1848;
	pSelect->Offset[7] = 1889;
	pSelect->Offset[8] = 1930;
	pSelect->Offset[9] = 1971;
	pSelect->Offset[10] = 2013;
	pSelect->Offset[11] = 2055;
	pSelect->Offset[12] = 2097;
	pSelect->Offset[13] = 2139;
	pSelect->Count = 14;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 88 */
	POST_Record[88].Occupy = 1;
	POST_Record[88].PostType = POST_TYPE_TEXT;
	POST_Record[88].Name[0] = 's';
	POST_Record[88].Name[1] = 's';
	POST_Record[88].Name[2] = 'i';
	POST_Record[88].Name[3] = 'd';
	POST_Record[88].NameLen = 4;
	POST_Record[88].FileIndex = 16;
	POST_Record[88].UpdateSelf = FALSE;
	POST_Record[88].PValue = &BUF_Text[50];
	pText = POST_Record[88].PValue;
	pText->CurrValue = PostbufText50[0];
	pText->UserValue = PostbufText50[1];
	pText->Offset = 2261;
	pText->DefaultLength = 9;
	pText->CurrValue[0] = 'a';
	pText->CurrValue[1] = 's';
	pText->CurrValue[2] = 'i';
	pText->CurrValue[3] = 'x';
	pText->CurrValue[4] = 't';
	pText->CurrValue[5] = 'e';
	pText->CurrValue[6] = 's';
	pText->CurrValue[7] = 't';
	pText->CurrValue[8] = '"';
	pText->CurrLength = 9;

	/* for record 89 */
	POST_Record[89].Occupy = 1;
	POST_Record[89].PostType = POST_TYPE_CHECKBOX;
	POST_Record[89].Name[0] = 'h';
	POST_Record[89].Name[1] = 'i';
	POST_Record[89].Name[2] = 'd';
	POST_Record[89].Name[3] = 'e';
	POST_Record[89].Name[4] = '_';
	POST_Record[89].Name[5] = 's';
	POST_Record[89].Name[6] = 's';
	POST_Record[89].Name[7] = 'i';
	POST_Record[89].Name[8] = 'd';
	POST_Record[89].NameLen = 9;
	POST_Record[89].FileIndex = 16;
	POST_Record[89].UpdateSelf = FALSE;
	POST_Record[89].PValue = &BUF_Checkbox[0];
	pCheckbox = POST_Record[89].PValue;
	pCheckbox->Offset = 2452;
	pCheckbox->Value[0] = 'h';
	pCheckbox->Value[1] = 'i';
	pCheckbox->Value[2] = 'd';
	pCheckbox->Value[3] = 'e';
	pCheckbox->Value[4] = '_';
	pCheckbox->Value[5] = 's';
	pCheckbox->Value[6] = 's';
	pCheckbox->Value[7] = 'i';
	pCheckbox->Value[8] = 'd';
	pCheckbox->DefaultVlaueLen = 9;
	pCheckbox->DefaultSet = 0;
	pCheckbox->CurrentSet = 0;
	pCheckbox->UserSet = 0;

	/* for record 90 */
	POST_Record[90].Occupy = 1;
	POST_Record[90].PostType = POST_TYPE_SELECT;
	POST_Record[90].Name[0] = 's';
	POST_Record[90].Name[1] = 'e';
	POST_Record[90].Name[2] = 'c';
	POST_Record[90].Name[3] = 'u';
	POST_Record[90].Name[4] = 'r';
	POST_Record[90].Name[5] = 'i';
	POST_Record[90].Name[6] = 't';
	POST_Record[90].Name[7] = 'y';
	POST_Record[90].Name[8] = '_';
	POST_Record[90].Name[9] = 'm';
	POST_Record[90].Name[10] = 'o';
	POST_Record[90].Name[11] = 'd';
	POST_Record[90].Name[12] = 'e';
	POST_Record[90].NameLen = 13;
	POST_Record[90].FileIndex = 16;
	POST_Record[90].UpdateSelf = FALSE;
	POST_Record[90].PValue = &BUF_Select[24];
	pSelect = POST_Record[90].PValue;
	pSelect->Offset[0] = 2569;
	pSelect->Offset[1] = 2623;
	pSelect->Offset[2] = 2666;
	pSelect->Count = 3;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 91 */
	POST_Record[91].Occupy = 1;
	POST_Record[91].PostType = POST_TYPE_SELECT;
	POST_Record[91].Name[0] = 'k';
	POST_Record[91].Name[1] = 'e';
	POST_Record[91].Name[2] = 'y';
	POST_Record[91].Name[3] = '_';
	POST_Record[91].Name[4] = 'l';
	POST_Record[91].Name[5] = 'e';
	POST_Record[91].Name[6] = 'n';
	POST_Record[91].Name[7] = 'g';
	POST_Record[91].Name[8] = 't';
	POST_Record[91].Name[9] = 'h';
	POST_Record[91].NameLen = 10;
	POST_Record[91].FileIndex = 16;
	POST_Record[91].UpdateSelf = FALSE;
	POST_Record[91].PValue = &BUF_Select[25];
	pSelect = POST_Record[91].PValue;
	pSelect->Offset[0] = 2889;
	pSelect->Offset[1] = 2946;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 92 */
	POST_Record[92].Occupy = 1;
	POST_Record[92].PostType = POST_TYPE_SELECT;
	POST_Record[92].Name[0] = 'k';
	POST_Record[92].Name[1] = 'e';
	POST_Record[92].Name[2] = 'y';
	POST_Record[92].Name[3] = '_';
	POST_Record[92].Name[4] = 'i';
	POST_Record[92].Name[5] = 'n';
	POST_Record[92].Name[6] = 'd';
	POST_Record[92].Name[7] = 'e';
	POST_Record[92].Name[8] = 'x';
	POST_Record[92].NameLen = 9;
	POST_Record[92].FileIndex = 16;
	POST_Record[92].UpdateSelf = FALSE;
	POST_Record[92].PValue = &BUF_Select[26];
	pSelect = POST_Record[92].PValue;
	pSelect->Offset[0] = 3072;
	pSelect->Offset[1] = 3133;
	pSelect->Offset[2] = 3184;
	pSelect->Offset[3] = 3235;
	pSelect->Count = 4;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 93 */
	POST_Record[93].Occupy = 1;
	POST_Record[93].PostType = POST_TYPE_TEXT;
	POST_Record[93].Name[0] = 'k';
	POST_Record[93].Name[1] = 'e';
	POST_Record[93].Name[2] = 'y';
	POST_Record[93].Name[3] = '_';
	POST_Record[93].Name[4] = 'i';
	POST_Record[93].Name[5] = 'n';
	POST_Record[93].Name[6] = 'd';
	POST_Record[93].Name[7] = 'e';
	POST_Record[93].Name[8] = 'x';
	POST_Record[93].Name[9] = '_';
	POST_Record[93].Name[10] = '0';
	POST_Record[93].NameLen = 11;
	POST_Record[93].FileIndex = 16;
	POST_Record[93].UpdateSelf = FALSE;
	POST_Record[93].PValue = &BUF_Text[51];
	pText = POST_Record[93].PValue;
	pText->CurrValue = PostbufText51[0];
	pText->UserValue = PostbufText51[1];
	pText->Offset = 3376;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 94 */
	POST_Record[94].Occupy = 1;
	POST_Record[94].PostType = POST_TYPE_TEXT;
	POST_Record[94].Name[0] = 'k';
	POST_Record[94].Name[1] = 'e';
	POST_Record[94].Name[2] = 'y';
	POST_Record[94].Name[3] = '_';
	POST_Record[94].Name[4] = 'i';
	POST_Record[94].Name[5] = 'n';
	POST_Record[94].Name[6] = 'd';
	POST_Record[94].Name[7] = 'e';
	POST_Record[94].Name[8] = 'x';
	POST_Record[94].Name[9] = '_';
	POST_Record[94].Name[10] = '1';
	POST_Record[94].NameLen = 11;
	POST_Record[94].FileIndex = 16;
	POST_Record[94].UpdateSelf = FALSE;
	POST_Record[94].PValue = &BUF_Text[52];
	pText = POST_Record[94].PValue;
	pText->CurrValue = PostbufText52[0];
	pText->UserValue = PostbufText52[1];
	pText->Offset = 3522;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 95 */
	POST_Record[95].Occupy = 1;
	POST_Record[95].PostType = POST_TYPE_TEXT;
	POST_Record[95].Name[0] = 'k';
	POST_Record[95].Name[1] = 'e';
	POST_Record[95].Name[2] = 'y';
	POST_Record[95].Name[3] = '_';
	POST_Record[95].Name[4] = 'i';
	POST_Record[95].Name[5] = 'n';
	POST_Record[95].Name[6] = 'd';
	POST_Record[95].Name[7] = 'e';
	POST_Record[95].Name[8] = 'x';
	POST_Record[95].Name[9] = '_';
	POST_Record[95].Name[10] = '2';
	POST_Record[95].NameLen = 11;
	POST_Record[95].FileIndex = 16;
	POST_Record[95].UpdateSelf = FALSE;
	POST_Record[95].PValue = &BUF_Text[53];
	pText = POST_Record[95].PValue;
	pText->CurrValue = PostbufText53[0];
	pText->UserValue = PostbufText53[1];
	pText->Offset = 3668;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 96 */
	POST_Record[96].Occupy = 1;
	POST_Record[96].PostType = POST_TYPE_TEXT;
	POST_Record[96].Name[0] = 'k';
	POST_Record[96].Name[1] = 'e';
	POST_Record[96].Name[2] = 'y';
	POST_Record[96].Name[3] = '_';
	POST_Record[96].Name[4] = 'i';
	POST_Record[96].Name[5] = 'n';
	POST_Record[96].Name[6] = 'd';
	POST_Record[96].Name[7] = 'e';
	POST_Record[96].Name[8] = 'x';
	POST_Record[96].Name[9] = '_';
	POST_Record[96].Name[10] = '3';
	POST_Record[96].NameLen = 11;
	POST_Record[96].FileIndex = 16;
	POST_Record[96].UpdateSelf = FALSE;
	POST_Record[96].PValue = &BUF_Text[54];
	pText = POST_Record[96].PValue;
	pText->CurrValue = PostbufText54[0];
	pText->UserValue = PostbufText54[1];
	pText->Offset = 3814;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 97 */
	POST_Record[97].Occupy = 1;
	POST_Record[97].PostType = POST_TYPE_TEXT;
	POST_Record[97].Name[0] = 'a';
	POST_Record[97].Name[1] = 'e';
	POST_Record[97].Name[2] = 's';
	POST_Record[97].Name[3] = '_';
	POST_Record[97].Name[4] = 'p';
	POST_Record[97].Name[5] = 'a';
	POST_Record[97].Name[6] = 's';
	POST_Record[97].Name[7] = 's';
	POST_Record[97].Name[8] = 'p';
	POST_Record[97].Name[9] = 'h';
	POST_Record[97].Name[10] = 'r';
	POST_Record[97].Name[11] = 'a';
	POST_Record[97].Name[12] = 's';
	POST_Record[97].Name[13] = 'e';
	POST_Record[97].NameLen = 14;
	POST_Record[97].FileIndex = 16;
	POST_Record[97].UpdateSelf = FALSE;
	POST_Record[97].PValue = &BUF_Text[55];
	pText = POST_Record[97].PValue;
	pText->CurrValue = PostbufText55[0];
	pText->UserValue = PostbufText55[1];
	pText->Offset = 4094;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 98 */
	POST_Record[98].Occupy = 1;
	POST_Record[98].PostType = POST_TYPE_SELECT;
	POST_Record[98].Name[0] = 'w';
	POST_Record[98].Name[1] = 'z';
	POST_Record[98].Name[2] = '_';
	POST_Record[98].Name[3] = 'd';
	POST_Record[98].Name[4] = 'h';
	POST_Record[98].Name[5] = 'c';
	POST_Record[98].Name[6] = 'p';
	POST_Record[98].Name[7] = 'c';
	POST_Record[98].NameLen = 8;
	POST_Record[98].FileIndex = 17;
	POST_Record[98].UpdateSelf = FALSE;
	POST_Record[98].PValue = &BUF_Select[27];
	pSelect = POST_Record[98].PValue;
	pSelect->Offset[0] = 1754;
	pSelect->Offset[1] = 1810;
	pSelect->Count = 2;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 99 */
	POST_Record[99].Occupy = 1;
	POST_Record[99].PostType = POST_TYPE_TEXT;
	POST_Record[99].Name[0] = 'w';
	POST_Record[99].Name[1] = 'z';
	POST_Record[99].Name[2] = '_';
	POST_Record[99].Name[3] = 's';
	POST_Record[99].Name[4] = 't';
	POST_Record[99].Name[5] = 'a';
	POST_Record[99].Name[6] = 't';
	POST_Record[99].Name[7] = 'i';
	POST_Record[99].Name[8] = 'c';
	POST_Record[99].Name[9] = '_';
	POST_Record[99].Name[10] = 'i';
	POST_Record[99].Name[11] = 'p';
	POST_Record[99].NameLen = 12;
	POST_Record[99].FileIndex = 17;
	POST_Record[99].UpdateSelf = FALSE;
	POST_Record[99].PValue = &BUF_Text[56];
	pText = POST_Record[99].PValue;
	pText->CurrValue = PostbufText56[0];
	pText->UserValue = PostbufText56[1];
	pText->Offset = 1936;
	pText->DefaultLength = 12;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '9';
	pText->CurrValue[2] = '2';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '1';
	pText->CurrValue[5] = '6';
	pText->CurrValue[6] = '8';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '0';
	pText->CurrValue[9] = '.';
	pText->CurrValue[10] = '3';
	pText->CurrValue[11] = '"';
	pText->CurrLength = 12;

	/* for record 100 */
	POST_Record[100].Occupy = 1;
	POST_Record[100].PostType = POST_TYPE_TEXT;
	POST_Record[100].Name[0] = 'w';
	POST_Record[100].Name[1] = 'z';
	POST_Record[100].Name[2] = '_';
	POST_Record[100].Name[3] = 'g';
	POST_Record[100].Name[4] = 'a';
	POST_Record[100].Name[5] = 't';
	POST_Record[100].Name[6] = 'e';
	POST_Record[100].Name[7] = 'w';
	POST_Record[100].Name[8] = 'a';
	POST_Record[100].Name[9] = 'y';
	POST_Record[100].Name[10] = '_';
	POST_Record[100].Name[11] = 'i';
	POST_Record[100].Name[12] = 'p';
	POST_Record[100].NameLen = 13;
	POST_Record[100].FileIndex = 17;
	POST_Record[100].UpdateSelf = FALSE;
	POST_Record[100].PValue = &BUF_Text[57];
	pText = POST_Record[100].PValue;
	pText->CurrValue = PostbufText57[0];
	pText->UserValue = PostbufText57[1];
	pText->Offset = 2071;
	pText->DefaultLength = 12;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '9';
	pText->CurrValue[2] = '2';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '1';
	pText->CurrValue[5] = '6';
	pText->CurrValue[6] = '8';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '0';
	pText->CurrValue[9] = '.';
	pText->CurrValue[10] = '1';
	pText->CurrValue[11] = '"';
	pText->CurrLength = 12;

	/* for record 101 */
	POST_Record[101].Occupy = 1;
	POST_Record[101].PostType = POST_TYPE_TEXT;
	POST_Record[101].Name[0] = 'w';
	POST_Record[101].Name[1] = 'z';
	POST_Record[101].Name[2] = '_';
	POST_Record[101].Name[3] = 'm';
	POST_Record[101].Name[4] = 'a';
	POST_Record[101].Name[5] = 's';
	POST_Record[101].Name[6] = 'k';
	POST_Record[101].NameLen = 7;
	POST_Record[101].FileIndex = 17;
	POST_Record[101].UpdateSelf = FALSE;
	POST_Record[101].PValue = &BUF_Text[58];
	pText = POST_Record[101].PValue;
	pText->CurrValue = PostbufText58[0];
	pText->UserValue = PostbufText58[1];
	pText->Offset = 2196;
	pText->DefaultLength = 14;
	pText->CurrValue[0] = '2';
	pText->CurrValue[1] = '5';
	pText->CurrValue[2] = '5';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '2';
	pText->CurrValue[5] = '5';
	pText->CurrValue[6] = '5';
	pText->CurrValue[7] = '.';
	pText->CurrValue[8] = '2';
	pText->CurrValue[9] = '5';
	pText->CurrValue[10] = '5';
	pText->CurrValue[11] = '.';
	pText->CurrValue[12] = '0';
	pText->CurrValue[13] = '"';
	pText->CurrLength = 14;

	/* for record 102 */
	POST_Record[102].Occupy = 1;
	POST_Record[102].PostType = POST_TYPE_TEXT;
	POST_Record[102].Name[0] = 'w';
	POST_Record[102].Name[1] = 'z';
	POST_Record[102].Name[2] = '_';
	POST_Record[102].Name[3] = 'd';
	POST_Record[102].Name[4] = 'n';
	POST_Record[102].Name[5] = 's';
	POST_Record[102].Name[6] = '_';
	POST_Record[102].Name[7] = 'i';
	POST_Record[102].Name[8] = 'p';
	POST_Record[102].NameLen = 9;
	POST_Record[102].FileIndex = 17;
	POST_Record[102].UpdateSelf = FALSE;
	POST_Record[102].PValue = &BUF_Text[59];
	pText = POST_Record[102].PValue;
	pText->CurrValue = PostbufText59[0];
	pText->UserValue = PostbufText59[1];
	pText->Offset = 2324;
	pText->DefaultLength = 11;
	pText->CurrValue[0] = '1';
	pText->CurrValue[1] = '6';
	pText->CurrValue[2] = '8';
	pText->CurrValue[3] = '.';
	pText->CurrValue[4] = '9';
	pText->CurrValue[5] = '5';
	pText->CurrValue[6] = '.';
	pText->CurrValue[7] = '1';
	pText->CurrValue[8] = '.';
	pText->CurrValue[9] = '1';
	pText->CurrValue[10] = '"';
	pText->CurrLength = 11;

	/* for record 103 */
	POST_Record[103].Occupy = 1;
	POST_Record[103].PostType = POST_TYPE_SUBMIT;
	POST_Record[103].Name[0] = 'A';
	POST_Record[103].Name[1] = 'c';
	POST_Record[103].Name[2] = 'c';
	POST_Record[103].Name[3] = 'e';
	POST_Record[103].Name[4] = 'p';
	POST_Record[103].Name[5] = 't';
	POST_Record[103].NameLen = 6;
	POST_Record[103].FileIndex = 255;
	POST_Record[103].UpdateSelf = FALSE;
	POST_Record[103].PValue = &BUF_Submit[6];
	pSubmit = POST_Record[103].PValue;
	pSubmit->Value[0] = 'A';
	pSubmit->Value[1] = 'c';
	pSubmit->Value[2] = 'c';
	pSubmit->Value[3] = 'e';
	pSubmit->Value[4] = 'p';
	pSubmit->Value[5] = 't';
	pSubmit->DefaultVlaueLen = 6;
	pSubmit->IsApply = 1;

	/* for record 104 */
	POST_Record[104].Occupy = 1;
	POST_Record[104].PostType = POST_TYPE_SUBMIT;
	POST_Record[104].Name[0] = 'C';
	POST_Record[104].Name[1] = 'a';
	POST_Record[104].Name[2] = 'n';
	POST_Record[104].Name[3] = 'c';
	POST_Record[104].Name[4] = 'e';
	POST_Record[104].Name[5] = 'l';
	POST_Record[104].NameLen = 6;
	POST_Record[104].FileIndex = 255;
	POST_Record[104].UpdateSelf = FALSE;
	POST_Record[104].PValue = &BUF_Submit[7];
	pSubmit = POST_Record[104].PValue;
	pSubmit->Value[0] = 'C';
	pSubmit->Value[1] = 'a';
	pSubmit->Value[2] = 'n';
	pSubmit->Value[3] = 'c';
	pSubmit->Value[4] = 'e';
	pSubmit->Value[5] = 'l';
	pSubmit->DefaultVlaueLen = 6;
	pSubmit->IsApply = 0;

	/* for record 105 */
	POST_Record[105].Occupy = 1;
	POST_Record[105].PostType = POST_TYPE_SELECT;
	POST_Record[105].Name[0] = 'w';
	POST_Record[105].Name[1] = 'z';
	POST_Record[105].Name[2] = '_';
	POST_Record[105].Name[3] = 'k';
	POST_Record[105].Name[4] = 'e';
	POST_Record[105].Name[5] = 'y';
	POST_Record[105].Name[6] = 'i';
	POST_Record[105].Name[7] = 'd';
	POST_Record[105].NameLen = 8;
	POST_Record[105].FileIndex = 18;
	POST_Record[105].UpdateSelf = FALSE;
	POST_Record[105].PValue = &BUF_Select[28];
	pSelect = POST_Record[105].PValue;
	pSelect->Offset[0] = 1535;
	pSelect->Offset[1] = 1595;
	pSelect->Offset[2] = 1645;
	pSelect->Offset[3] = 1695;
	pSelect->Count = 4;
	pSelect->DefaultSet = 0;
	pSelect->CurrentSet = 0;
	pSelect->UserSet = 0;

	/* for record 106 */
	POST_Record[106].Occupy = 1;
	POST_Record[106].PostType = POST_TYPE_TEXT;
	POST_Record[106].Name[0] = 'w';
	POST_Record[106].Name[1] = 'z';
	POST_Record[106].Name[2] = '_';
	POST_Record[106].Name[3] = 'k';
	POST_Record[106].Name[4] = 'e';
	POST_Record[106].Name[5] = 'y';
	POST_Record[106].NameLen = 6;
	POST_Record[106].FileIndex = 18;
	POST_Record[106].UpdateSelf = FALSE;
	POST_Record[106].PValue = &BUF_Text[60];
	pText = POST_Record[106].PValue;
	pText->CurrValue = PostbufText60[0];
	pText->UserValue = PostbufText60[1];
	pText->Offset = 1850;
	pText->DefaultLength = 1;
	pText->CurrValue[0] = '"';
	pText->CurrLength = 1;

	/* for record 107 */
	POST_Record[107].Occupy = 1;
	POST_Record[107].PostType = POST_TYPE_SUBMIT;
	POST_Record[107].Name[0] = 'N';
	POST_Record[107].Name[1] = 'e';
	POST_Record[107].Name[2] = 'x';
	POST_Record[107].Name[3] = 't';
	POST_Record[107].NameLen = 4;
	POST_Record[107].FileIndex = 255;
	POST_Record[107].UpdateSelf = FALSE;
	POST_Record[107].PValue = &BUF_Submit[8];
	pSubmit = POST_Record[107].PValue;
	pSubmit->Value[0] = 'N';
	pSubmit->Value[1] = 'e';
	pSubmit->Value[2] = 'x';
	pSubmit->Value[3] = 't';
	pSubmit->DefaultVlaueLen = 4;
	pSubmit->IsApply = 1;

}

/*
 * ----------------------------------------------------------------------------
 * Function Name: FSYS_FindFile
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
U8_T FSYS_FindFile(U8_T* pFName)
{
	U8_T index, find;
	U8_T *pSour;
	U8_T *pDesc;

	for (index = 0; index < MAX_STORE_FILE_NUM; index++)
	{
		if (!FSYS_Manage[index].Occupy)
			continue;

		pSour = pFName;
		pDesc = (U8_T*)FSYS_Manage[index].FName;
		find = 1;
		while (*pDesc != 0)
		{
			if (*pSour != *pDesc)
			{
				find = 0;
				break;
			}
			pSour++;
			pDesc++;
		}
		if (find == 1)
			return index;
	}
	return index;
}


/* End of filesys.c */
