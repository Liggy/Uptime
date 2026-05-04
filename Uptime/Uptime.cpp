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


static I18N i18n;


static int MessageCallback(const TCHAR* format, va_list arg_ptr)
{
    return _vtprintf_p(format, arg_ptr);
}


static void HowTo()
{
    i18n.ProcessMessage(IDS_HOWTO);
}


static SYSTEMTIME UnixTimeToSystemTime(DWORD unixtime)
{
    SYSTEMTIME systemtime;
    FILETIME filetime, localfiletime;
    LARGE_INTEGER li{};

    li.QuadPart = ((LONGLONG) unixtime * 10000000) + 0x019DB1DED53E8000;

    filetime.dwLowDateTime = li.LowPart;
    filetime.dwHighDateTime = li.HighPart;

    FileTimeToLocalFileTime(&filetime, &localfiletime);
    FileTimeToSystemTime(&localfiletime, &systemtime);

    return systemtime;
}


static DWORD FileTimeToUnixTime(FILETIME filetime)
{
    LARGE_INTEGER li{};

    li.LowPart = filetime.dwLowDateTime;
    li.HighPart = filetime.dwHighDateTime;

    // Convert ticks into seconds since 1/1/1970
    return (DWORD) (LONGLONG) ((li.QuadPart - 0x019DB1DED53E8000) / 10000000);
}


static int LastStartup(bool absolute)
{
    DWORD Uptime = RetrieveUptime(nullptr);
    if (!Uptime)
    {
        // Error
//        i18n.ProcessMessage();
        return 1;
    }

    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    DWORD CurrentTime = FileTimeToUnixTime(ft);

    if (!absolute)
    {
        CurrentTime -= Uptime;

        int days = CurrentTime / 86400; CurrentTime %= 86400;
        int hours = CurrentTime / 3600; CurrentTime %= 3600;
        int minutes = CurrentTime / 60; CurrentTime %= 60;
        int seconds = CurrentTime;
        i18n.ProcessMessage(IDS_UP_SINCE, days, hours, minutes, seconds);
    }
    else
    {
        SYSTEMTIME st = UnixTimeToSystemTime(Uptime);
        TCHAR DateBuffer[0x100], TimeBuffer[0x100];
        GetDateFormatEx(
            LOCALE_NAME_USER_DEFAULT,
            DATE_SHORTDATE,
            &st,
            nullptr,
            DateBuffer,
            0x100,
            nullptr
        );
        GetTimeFormatEx(
            LOCALE_NAME_USER_DEFAULT,
            0,
            &st,
            nullptr,
            TimeBuffer,
            0x100
        );
        i18n.ProcessMessage(IDS_UP_AT, DateBuffer, TimeBuffer);
    }


    return 0;
}

static int ListAllEvents()
{
    // List all related events
    std::t_cout << _T("Listing all events\n");

    return 0;
}

int main(int argc, char* argv[])
{
    ::SetConsoleOutputCP(1250);
//For language debugging purposes only
#if 0
    DWORD langCount = 1;
    const WCHAR lang[] = { L"en-US\0" };
    SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, lang, &langCount);
#endif

    i18n.Init(&MessageCallback);
    bool DoList = false;
    bool DoHelp = false;
    bool DoAbsolute = false;
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
            else if (!_stricmp(option, "a"))
            {
                DoAbsolute = true;
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
    if (DoList && DoAbsolute)
        DoHelp = true;

    if (DoHelp)
    {
        HowTo();
    }
    else if (DoList)
    {
        ListAllEvents();
    }
    else
    {
        LastStartup(DoAbsolute);
    }
    return 0;
}

