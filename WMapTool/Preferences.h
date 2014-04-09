#pragma once
class Preferences
{
public:
	int GetNumericPreference(LPCWSTR keyName);
	void SaveNumericPreference(LPCWSTR keyName, int value);
	Preferences();
	~Preferences();
};

