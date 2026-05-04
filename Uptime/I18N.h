#pragma once
typedef int (*CallbackPrototype)(const TCHAR*, va_list);

class I18N
{
	HMODULE hResFile;
	CallbackPrototype hCallback;

public:
	I18N();
	I18N(CallbackPrototype output);
	~I18N();

	void Init(CallbackPrototype output);


	int ProcessMessage(UINT, ...) const;
};
