/*============================================================================*/
/*             National Instruments / System Configuration API                */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 2010-2017.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:   nisyscfg_controller.h                                             */
/* Purpose: Include file for NI Controller specific attributes to be used     */
/*          with the nisyscfg interface defined in nisyscfg.h                 */
/*                                                                            */
/*============================================================================*/

#ifndef ___NISYSCFG_CONTROLLER_H___
#define ___NISYSCFG_CONTROLLER_H___

#include <nisyscfg.h>

typedef enum
{
   /* memory modules attribute */
   NISysCfgControllerPropertyMemoryModuleLocation        =   469807104,   // char *
   NISysCfgControllerPropertyMemoryModuleModelName       =   469815296,   // char *
   NISysCfgControllerPropertyMemoryModuleCapacity        =   469823488    // double
}  NISysCfgControllerProperty;

#endif
