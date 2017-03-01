/****************************************************************************
 *
 * MODULE:             ip.c
 *
 * COMPONENT:          home kit interface
 *
 * REVISION:           $Revision:  1.0$
 *
 * DATED:              $Date: 2017-01-11 15:13:17 +0100 (Fri, 12 Dec 2016 $
 *
 * AUTHOR:             PCT
 *
 ****************************************************************************
 *
 * Copyright panchangtao@gmail.com 2017. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <profile.h>
#include "ip.h"
#include "http_handle.h"
#include "light_bulb.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DBG_IP 1
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern tsController sController;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Local    Functions                                            ***/
/****************************************************************************/
static uint16 u16IpGetBinaryData()
{
    return 0;
}
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
tsIpMessage *psIpResponseNew()
{
    tsIpMessage *psIpMsg = (tsIpMessage*)malloc(sizeof(tsIpMessage));
    CHECK_POINTER(psIpMsg, NULL);
    memset(psIpMsg, 0, sizeof(tsIpMessage));
    psIpMsg->psTlvPackage = psTlvPackageNew();
    if(psIpMsg->psTlvPackage == NULL){
        FREE(psIpMsg); return NULL;
    }
    return psIpMsg;
}
tsIpMessage *psIpMessageFormat(const uint8 *psBuffer, uint16 u16Len)
{
    tsIpMessage *psIpMsg = (tsIpMessage*)malloc(sizeof(tsIpMessage));
    CHECK_POINTER(psIpMsg, NULL);
    memset(psIpMsg, 0, sizeof(tsIpMessage));
    psIpMsg->psHttp = psHttpParser(psBuffer, u16Len);
    psIpMsg->psTlvPackage = psTlvPackageFormat(psIpMsg->psHttp->acContentData, psIpMsg->psHttp->u16ContentLen);
    if(psIpMsg->psTlvPackage == NULL){
        ERR_vPrintln(T_TRUE, "psTlvPackageFormat Failed");
        FREE(psIpMsg); return NULL;
    }
    return psIpMsg;
}

teHapStatus eIpMessageRelease(tsIpMessage *psIpMsg)
{
    eTlvPackageRelease(psIpMsg->psTlvPackage);
    FREE(psIpMsg->psHttp);
    FREE(psIpMsg);
    return E_HAP_STATUS_OK;
}

teHapStatus eHapHandlePackage(tsProfile *psProfile, tsBonjour *psBonjour, uint8 *psBuffer, int iLen, int iSocketFd)
{
    tsHttpEntry *psHttpEntry = psHttpParser(psBuffer, (uint16)iLen);

    DBG_vPrintln(DBG_IP, "Data Len:%d", psHttpEntry->u16ContentLen);
    if(strstr((char*)psHttpEntry->acDirectory, "pair-setup"))
    {
        DBG_vPrintln(DBG_IP, "IOS Device Pair Setup");
        teHapStatus eStatus = eHandlePairSetup(psBuffer, iLen, iSocketFd, psBonjour);
        if(E_HAP_STATUS_OK != eStatus){
            ERR_vPrintln(T_TRUE, "eHandlePairSetup Error:%d", eStatus);
            return eStatus;
        }
    }
    else if(strstr((char*)psHttpEntry->acDirectory, "pair-verify"))
    {
        DBG_vPrintln(DBG_IP, "IOS Device Pair Verify");
        eHandlePairVerify(psBuffer, iLen, iSocketFd, psBonjour);
        eHandleAccessoryRequest(psProfile, iSocketFd, psBonjour);
    }
    else if(strstr((char*)psHttpEntry->acDirectory, "identify"))
    {
        close(iSocketFd);
    }
    else
    {
        DBG_vPrintln(DBG_IP, "Handle Controller Request");
    }
    FREE(psHttpEntry);
    return E_HAP_STATUS_OK;
}

teHapStatus eHandleAccessoryPackage(tsProfile *psProfile, const uint8 *psData, uint16 u16Len, uint8 **psResp, uint16 *pu16Len)
{
    CHECK_POINTER(psData, E_HAP_STATUS_ERROR);
    CHECK_POINTER(psResp, E_HAP_STATUS_ERROR);
    CHECK_POINTER(pu16Len, E_HAP_STATUS_ERROR);

    DBG_vPrintln(DBG_IP, "eHandleAccessoryPackage:%s\n", psData);

    if (strstr((char*)psData, "/accessories")) {
        //Publish the characteristics of the accessories
        NOT_vPrintln(DBG_IP, "Ask for accessories info\n");
        json_object *psJsonRet = psProfile->psGetAccessoryJsonInfo(psProfile->psAccessory);
        *pu16Len = u16HttpFormat(E_HTTP_STATUS_SUCCESS_OK, "application/hap+json",
                                 (uint8*)json_object_get_string(psJsonRet), (uint16) strlen(json_object_get_string(psJsonRet)), psResp);
        json_object_put(psJsonRet);
    } else if(strstr((char*)psData, "/characteristics")) {
        tsHttpEntry *psHttp = psHttpParser(psData, u16Len);
        if(strstr((char*)psData, "PUT")){
            NOT_vPrintln(DBG_IP, "Writing Characteristics Attribute:%s\n", psHttp->acContentData);
            *pu16Len = u16HttpFormat(E_HTTP_STATUS_SUCCESS_NO_CONTENT, "application/hap+json", NULL, 0, psResp);

        } else if(strstr((char*)psData, "GET")){
            WAR_vPrintln(DBG_IP, "Reading Characteristics Attribute\n");
            json_object *psJsonRet = psProfile->psGetCharacteristicInfo(psProfile->psAccessory, (char*)psHttp->acDirectory);
            *pu16Len = u16HttpFormat(E_HTTP_STATUS_SUCCESS_OK, "application/hap+json",
                                     (uint8*)json_object_get_string(psJsonRet), (uint16) strlen(json_object_get_string(psJsonRet)), psResp);
            json_object_put(psJsonRet);
        }
        FREE(psHttp);
    } else if(strstr((char*)psData, "/pairings")){
        NOT_vPrintln(DBG_IP, "Controller Request Remove Pairing");
        eHandlePairingRemove(psData, u16Len, psResp, pu16Len);
    }

    return E_HAP_STATUS_OK;
}