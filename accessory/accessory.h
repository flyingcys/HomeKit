/****************************************************************************
 *
 * MODULE:             accessory.h
 *
 * COMPONENT:          homekit interface
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

#ifndef __H_ACCESSORY_H_
#define __H_ACCESSORY_H_

#if defined __cplusplus   
extern "C" {
#endif
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "utils.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct {
    char auName[MIBF];
    char auSerialNumber[MIBF];
    char auManufacturer[MIBF];
    char auModel[MIBF];
} tsAccessoryInfo;

typedef struct {
    tsServices *psService;
    tsCharacteristics sName;
    tsCharacteristics sIndentify;
    tsCharacteristics sManufacturer;
    tsCharacteristics sModel;
    tsCharacteristics sSerialNumber;
    
    
} tsAccessoryInformation;

typedef struct{
    uint64 u64AIDs; //Accessory Instance Id
    tsServices *psServices;
    teAccessoryType eAccessoryType;
    //tsAccessoryInformation *psAccessoryInformation;
    int64 idCount;
    func ;
} tsAccessroy;
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************/
/***        Local    Functions                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif /*__H_ACCESSORY_H_*/

