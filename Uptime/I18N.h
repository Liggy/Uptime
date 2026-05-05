#pragma once
typedef int (*i18nCallbackPrototype)(const TCHAR*, va_list);

class I18N
{
	HMODULE hResFile;
	i18nCallbackPrototype hCallback;

public:
	I18N();
	I18N(i18nCallbackPrototype cbOutput);
	~I18N();

	void Init(i18nCallbackPrototype cbOutput);


	int ProcessMessage(UINT, ...) const;
	int ProcessMessageToBuffer(TCHAR*, UINT, UINT, ...) const;

	int LoadString(UINT uID, LPTSTR lpBuffer,int cchBufferMax) const;

};
