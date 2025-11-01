#include "pch.h"
#include "Uptime.h"
#include "UptimeEvaluate.h"


//#define SUFFICIENTLY_LARGE_STRING_BUFFER (MAX_PATH*2)
//#define USER_CONFIGURATION_STRING_BUFFER (((LOCALE_NAME_MAX_LENGTH+1)*5)+1)
//#define SUFFICIENTLY_LARGE_ERROR_BUFFER (1024*2)
//#define HELLO_MODULE_CONTRIVED_FILE_PATH  (L"Uptime.dll")

#ifdef UNICODE
#define t_cout wcout
#else
#define t_cout cout
#endif

static int MessageCallback(const TCHAR* format, ...)
{
    int rc = 0;
    va_list arg_ptr;

    va_start(arg_ptr, format);
    _vtprintf_p(format, arg_ptr);
}


static void HowTo()
{
    std::t_cout
        << _T("Usage: Uptime [server] {/s} {/?}\n\n")
        << _T("       server  optionally connect to remote machine\n")
        << _T("       /s      Show all available startup and shutdown times\n")
        << _T("       / ? Show this help\n");
}

static void InitI18N()
{
    HMODULE i18nModule = LoadLibraryEx(_T("Uptime.dll"), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);

}


int main(int argc, char* argv[])
{
    bool DoList = false;
    bool DoHelp = false;
    char* RemoteHost = nullptr;


    for (int cmdidx = 1; cmdidx < argc; cmdidx++)
    {
        if ((argv[cmdidx][0] == '-') || (argv[cmdidx][0] == '/')) // Option
        {
            char* option = &argv[cmdidx][1];
            if (!_stricmp(option, "?"))
            {
                DoHelp = true;
            }
            else if (!_stricmp(option, "s"))
            {
                DoList = true;
            }
            else
            {
                DoHelp = true;
                break;
            }
        }
        else // argument
        {
            if (!RemoteHost)
                RemoteHost = argv[cmdidx];
            else // We only support one remote host
            {
                DoHelp = true;
                break;
            }
        }
    }

    if (DoHelp)
    {
        HowTo();
    }
    else if (DoList)
    {
// List all related events
        std::t_cout << _T("Listing all events\n");
    }
    else
    {
// Just show last startup
        std::t_cout << _T("Host has been up for: w day(s), x hour(s), y minute(s), z second(s)\n");
    }
    return 0;
}

