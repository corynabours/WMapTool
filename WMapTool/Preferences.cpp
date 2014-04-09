#include "stdafx.h"
#include "Preferences.h"

int Preferences::GetNumericPreference(LPCWSTR keyName)
{
	HKEY appKey;
	DWORD value = 0;
	DWORD size = MAXSIZE_T;
	int result = RegGetValue(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Naboursoft\\Maptool"), keyName, RRF_RT_DWORD, NULL, &value, &size);
	return (int)value;
}

void Preferences::SaveNumericPreference(LPCWSTR keyName, int value)
{
	HKEY key;
	RegOpenKey(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Naboursoft\\Maptool"), &key);
	RegSetValueEx(key, keyName, 0, REG_DWORD, (BYTE *)&value, sizeof(int));
	RegCloseKey(key);
}


Preferences::Preferences()
{
	HKEY hTestKey;
	LSTATUS status = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Naboursoft\\Maptool"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hTestKey,NULL);
	RegCloseKey(hTestKey);
}


Preferences::~Preferences()
{
}
