#include "pch.h"
#include "I18N.h"

I18N::I18N(CallbackPrototype output)
{
	hResFile = LoadLibraryEx(_T("Uptime.dll"), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
	hCallback = output;
}

I18N::~I18N()
{
	if (hResFile)
		FreeLibrary(hResFile);
}

int I18N::ProcessMessage(UINT uID, ...) const
{
	TCHAR buffer[1024];
	va_list arg_ptr;
	int rc;
	
	if (!hResFile || !hCallback)
		return 0;

	va_start(arg_ptr, uID);
	LoadString(hResFile, uID, buffer, 1024);

	rc = hCallback(buffer, arg_ptr);

	va_end(arg_ptr);

	return rc;
}

