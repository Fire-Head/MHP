#pragma once
#include <windows.h>

class CIni
{
	char m_szPath[MAX_PATH];
public:
	CIni(void);
	~CIni(void);
	
	const char *ToA(int value);
	
	int Get(char *section, char *key, int defaultValue);
	void Set(char *section, char *key, const char *value);
	void Set(char *section, char *key, int value);
};

