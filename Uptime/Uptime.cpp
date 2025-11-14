#include "pch.h"
#include "I18N.h"
#include "Uptime.h"
#include "UptimeEvaluate.h"


#ifdef UNICODE
#define t_cout wcout
#define t_cin wcin
#define t_cerr wcerr
#else
#define t_cout cout
#define t_cin cin
#define t_cerr cerr
#endif


static int MessageCallback(const TCHAR* format, va_list arg_ptr)
{
    return _vtprintf_p(format, arg_ptr);
}


static void HowTo(I18N* i18n)
{
    i18n->ProcessMessage(IDS_HOWTO);
}


int main(int argc, char* argv[])
{
    DWORD langCount = 1;
    const WCHAR lang[] = { L"en-US\0" };
    ::SetConsoleOutputCP(1250);
//For debugging purposes only
//    SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, lang, &langCount);

    I18N i18n(&MessageCallback);
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
        HowTo(&i18n);
    }
    else if (DoList)
    {
// List all related events
        std::t_cout << _T("Listing all events\n");
    }
    else
    {
// Just show last startup
        int days = 0, hours = 4, minutes = 6, seconds = 1;
        i18n.ProcessMessage(IDS_UP_SINCE, days, hours, minutes, seconds);
    }
    return 0;
}

