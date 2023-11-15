#include "common.h"
#include "Ini.h"

static char tempBuffer[MAX_PATH];

CIni::CIni(void)
{
	GetIniPath(m_szPath, MAX_PATH);
}

CIni::~CIni(void)
{
}

const char *CIni::ToA(int value)
{
	_itoa_s(value,tempBuffer,10);
	return tempBuffer;
}

int CIni::Get(char *section, char *key, int defaultValue)
{
	return GetPrivateProfileInt(section,  key, defaultValue, m_szPath); 
}

void CIni::Set(char *section, char *key, const char *value)
{
	WritePrivateProfileString(section, key, value, m_szPath);
}

void CIni::Set(char *section, char *key, int value)
{
	Set(section, key, ToA(value));
}