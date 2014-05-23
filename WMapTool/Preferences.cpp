#include "stdafx.h"
#include "Preferences.h"

void Preferences::SetSubFolder(LPWSTR subfolder)
{
	int currentFolderLen = lstrlenW(currentFolder);
	int subfolderLen = lstrlenW(subfolder);
	LPWSTR newfolder = new wchar_t[currentFolderLen + subfolderLen + 2];
	wsprintf(newfolder, L"%s\\%s", currentFolder, subfolder);
	delete currentFolder;
	currentFolder = newfolder;
}

int Preferences::GetNumericPreference(LPCWSTR keyName)
{
	DWORD value = 0;
	DWORD size = MAXSIZE_T;
	int result = RegGetValue(HKEY_CURRENT_USER, currentFolder, keyName, RRF_RT_DWORD, NULL, &value, &size);
	if (result == ERROR_SUCCESS)
		return (int)value;
	else
		return 0;
}

LPWSTR Preferences::GetStringPreference(LPCWSTR keyName)
{
	wchar_t value[255];
	memset(value, 0, 255);
	DWORD size = 255;
	int result = RegGetValue(HKEY_CURRENT_USER, currentFolder, keyName, RRF_RT_REG_SZ, NULL, &value, &size);
	value[size/2] = 0;
	size = lstrlen(value);
	LPWSTR wvalue = new wchar_t[size+1];
	wsprintf(wvalue, L"%s", value);
	return wvalue;
}

void Preferences::SaveStringPreference(LPCWSTR keyName, LPCWSTR value)
{

}

void Preferences::SaveNumericPreference(LPCWSTR keyName, int value)
{
	HKEY key;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, currentFolder, &key)) return;
	RegSetValueEx(key, keyName, 0, REG_DWORD, (BYTE *)&value, sizeof(int));
	RegCloseKey(key);
}


Preferences::Preferences()
{
	HKEY hTestKey, hLibraryKey;
	currentFolder = new wchar_t[28];
	wsprintf(currentFolder,L"%s",TEXT("SOFTWARE\\Naboursoft\\Maptool"));
	LSTATUS status = RegCreateKeyEx(HKEY_CURRENT_USER, currentFolder, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hTestKey, NULL);
	if (status != ERROR_SUCCESS) return;
	RegCloseKey(hTestKey);
	status = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Naboursoft\\Maptool\\Library"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hLibraryKey, NULL);
	if (status != ERROR_SUCCESS) return;
	RegCloseKey(hLibraryKey);
}


Preferences::~Preferences()
{
	delete currentFolder;
	currentFolder = 0;
}
