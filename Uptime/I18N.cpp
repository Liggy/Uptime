#include "pch.h"
#include "I18N.h"


I18N::I18N()
{
	hResFile = nullptr;
	hCallback = nullptr;
}


I18N::I18N(i18nCallbackPrototype cbOutput)
{
	Init(cbOutput);
}


I18N::~I18N()
{
	if (hResFile)
		FreeLibrary(hResFile);
}


void I18N::Init(i18nCallbackPrototype cbOutput)
{
	hResFile = ::LoadLibraryEx(_T("Uptime.dll"), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
	hCallback = cbOutput;
}


int I18N::ProcessMessage(UINT uID, ...) const
{
	TCHAR lpBuffer[1024];
	va_list arg_ptr;
	int rc;
	
	if (!hResFile || !hCallback)
		return 0;

	va_start(arg_ptr, uID);
	::LoadString(hResFile, uID, lpBuffer, 1024);

	rc = hCallback(lpBuffer, arg_ptr);

	va_end(arg_ptr);

	return rc;
}


int I18N::ProcessMessageToBuffer(TCHAR* lpDestination, UINT uDestinationSize, UINT uID, ...) const
{
	TCHAR lpBuffer[1024];
	va_list arg_ptr;
	int rc;

	if (!hResFile || !hCallback)
		return 0;

	va_start(arg_ptr, uID);
	::LoadString(hResFile, uID, lpBuffer, 1024);

	rc = _vstprintf_p(lpDestination, uDestinationSize, lpBuffer, arg_ptr);

	va_end(arg_ptr);

	return rc;
}


int I18N::LoadString(UINT uID, LPTSTR lpBuffer, int cchBufferMax) const
{
	if (!hResFile)
		return 0;
	
	return ::LoadString(hResFile, uID, lpBuffer, cchBufferMax);
}