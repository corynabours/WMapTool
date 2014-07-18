#pragma once
class Preferences
{
private:
	LPWSTR currentFolder;
public:
	void RemovePreference(LPCWSTR keyName);
	void SaveStringPreference(LPCWSTR keyName, LPCWSTR value);
	LPWSTR GetStringPreference(LPCWSTR keyName);
	int GetNumericPreference(LPCWSTR keyName);
	void SaveNumericPreference(LPCWSTR keyName, int value);
	void SetSubFolder(LPWSTR subfolder);
	Preferences();
	~Preferences();
};

