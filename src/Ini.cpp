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

int CIni::Get(char *section, char *key, int defaultValue)
{
	return GetPrivateProfileInt(section,  key, defaultValue, m_szPath); 
}

void CIni::Set(char *section, char *key, int value)
{
	_itoa_s(value,tempBuffer,10);
	WritePrivateProfileString(section, key, tempBuffer, m_szPath);
}