#pragma once
typedef int (*CallbackPrototype)(const TCHAR*, va_list);

class I18N
{
	HMODULE hResFile;
	CallbackPrototype hCallback;

public:
	I18N(CallbackPrototype);
	~I18N();


	int ProcessMessage(UINT, ...) const;
};
