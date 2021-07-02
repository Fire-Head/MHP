#include "Ini.h"

static char tempBuffer[MAX_PATH];

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

CIni::CIni(void)
{
	char			moduleName[MAX_PATH];
	char			iniName[MAX_PATH];
	char*			p;

	GetModuleFileName((HINSTANCE)&__ImageBase, moduleName, MAX_PATH);

	p = strrchr(moduleName, '.');
	*p = '\0';

	p = strrchr(moduleName, '\\');
	strncpy_s(iniName, p + 1, MAX_PATH);
	
	strcat_s(iniName, ".ini");
	strncpy_s(m_szPath, moduleName, (p - moduleName + 1));

	m_szPath[p - moduleName + 1] = '\0';
	
	strcat_s(m_szPath, iniName);
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