#ifndef _ATCMD_GENERAL_H_
#define _ATCMD_GENERAL_H_
#include "atcmd.h"

int At_Attention (SERIAL_PORT port, char *cmd, stParam *param);
int At_Reboot (SERIAL_PORT port, char *cmd, stParam *param);
int At_Restore (SERIAL_PORT port, char *cmd, stParam *param);
int At_Dfu (SERIAL_PORT port, char *cmd, stParam *param);
#ifndef RUI_BOOTLOADER
int At_Sn (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetBat (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetFwBuildTime (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetFwRepoInfo (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetFwVersion (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetCliVersion (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetApiVersion (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetHwModel (SERIAL_PORT port, char *cmd, stParam *param);
int At_GetHwID (SERIAL_PORT port, char *cmd, stParam *param);
#endif

#endif //_ATCMD_GENERAL_H_
