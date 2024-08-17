#pragma once

void Wlan_Init(void);
void Wlan_Cyclic(void);
bool Wlan_IsConnected(void);
bool Wlan_ToggleEnable(void);
int Wlan_GetRssi(void);
bool Wlan_ConnectionTryInProgress(void);
