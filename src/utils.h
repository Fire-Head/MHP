#pragma once
#include <vector>
#include <functional>

#define NAK                              __declspec(naked)
#define EAXJMP(a)                        { _asm mov eax, a _asm jmp eax }

void GetIniPath(char *buffer, size_t bufferlen);
void GetFilePath(const char *path, char *buffer, size_t bufferlen);
void ErrorMsg(const char *msg, ...);

class CInitializer
{
	std::function<void()> func;
	static std::vector<CInitializer *> init;
public:
	
	CInitializer(std::function<void()> f) : func(f)
	{		
		init.push_back(this);
	}
	
	void Patch() { func(); }
	
	static void Apply()
	{
		for( auto i = init.begin(); i != init.end(); ++i )
			(*i)->Patch();
	}
};

#define STARTPATCH static CInitializer init( [] 
#define ENDPATCH );