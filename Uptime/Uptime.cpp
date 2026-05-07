#include "pch.h"
#include "I18N.h"
#include "Uptime.h"
#include "UptimeEvaluate.h"


static I18N i18n;
static WCHAR lpDateBuffer[0x100], lpTimeBuffer[0x100];


static int MessageCallback(const TCHAR* format, va_list arg_ptr)
{
    return _vtprintf_p(format, arg_ptr);
}


static void HowTo()
{
    i18n.ProcessMessage(IDS_HOWTO);
}


static SYSTEMTIME UnixTimeToSystemTime(DWORD UnixTime)
{
    SYSTEMTIME SystemTime;
    FILETIME FileTime{}, LocalFileTime{};
    LARGE_INTEGER li{};

    li.QuadPart = ((LONGLONG) UnixTime * 10000000) + 0x019DB1DED53E8000;

    FileTime.dwLowDateTime = li.LowPart;
    FileTime.dwHighDateTime = li.HighPart;

    ::FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
    ::FileTimeToSystemTime(&LocalFileTime, &SystemTime);

    return SystemTime;
}


static DWORD FileTimeToUnixTime(FILETIME FileTime)
{
    LARGE_INTEGER li{};

    li.LowPart = FileTime.dwLowDateTime;
    li.HighPart = FileTime.dwHighDateTime;

    // Convert ticks into seconds since 1/1/1970
    return (DWORD) (LONGLONG) ((li.QuadPart - 0x019DB1DED53E8000) / 10000000);
}


static int LastStartup(LPCTSTR lpRemoteHost, bool bAbsolute)
{
    DWORD Uptime = RetrieveUptime(lpRemoteHost);
    if (!Uptime)
    {
        // Error
        i18n.ProcessMessage(IDS_ERROR_RETRIEVING_DATA);
        return 1;
    }

    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);
    DWORD CurrentTime = FileTimeToUnixTime(ft);

    if (!bAbsolute)
    {
        CurrentTime -= Uptime;

        DWORD days = CurrentTime / 86400; CurrentTime %= 86400;
        DWORD hours = CurrentTime / 3600; CurrentTime %= 3600;
        DWORD minutes = CurrentTime / 60; CurrentTime %= 60;
        DWORD seconds = CurrentTime;
        i18n.ProcessMessage(IDS_UP_SINCE, days, hours, minutes, seconds);
    }
    else
    {
        SYSTEMTIME SystemTime = UnixTimeToSystemTime(Uptime);
        ::GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &SystemTime, nullptr, lpDateBuffer, 0x100, nullptr);
        ::GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &SystemTime, nullptr, lpTimeBuffer, 0x100);
        i18n.ProcessMessage(IDS_UP_AT, lpDateBuffer, lpTimeBuffer);
    }

    return 0;
}


static bool ListAllCallback(DWORD dwEventID, WORD wEventCategory, DWORD dwTimestamp)
{
    static DWORD dwLastTimestamp = 0;
    static UINT uLastEventResourceID = 0;

// First call
    if (!dwLastTimestamp)
    {

        i18n.ProcessMessage(IDS_UPTIME_LIST_HEADER);
        printf("---------- -----------  -------------------  -----------------------------------\n");
    }

    SYSTEMTIME SystemTime = UnixTimeToSystemTime(dwTimestamp);
    ::GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &SystemTime, nullptr, lpDateBuffer, 0x100, nullptr);
    ::GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &SystemTime, nullptr, lpTimeBuffer, 0x100);

    TCHAR event[100]{}, comment[100]{};
    UINT uEventResourceID = IDS_EVENT_UNKNOWN;

    if ((dwEventID == 12) && (wEventCategory == 1))
        uEventResourceID = IDS_EVENT_STARTUP;
    else if ((dwEventID == 13) && (wEventCategory == 2))
        uEventResourceID = IDS_EVENT_SHUTDOWN;
    else if ((dwEventID == 42) && (wEventCategory == 64))
        uEventResourceID = IDS_EVENT_HIBERNATE;
    else if ((dwEventID == 107) && (wEventCategory == 102))
        uEventResourceID = IDS_EVENT_RESUME; // Timetstamp can be inaccurate
    else if ((dwEventID == 506) && (wEventCategory == 157))
        uEventResourceID = IDS_EVENT_SLEEP;
    else if ((dwEventID == 507) && (wEventCategory == 158))
        uEventResourceID = IDS_EVENT_WAKEUP; // Timetstamp can be inaccurate

    i18n.LoadString(uEventResourceID, event, 100);

    if (dwLastTimestamp)
    {
        if (dwTimestamp >= dwLastTimestamp)
        {
            TCHAR lpPriorState[100];
            DWORD duration = dwTimestamp - dwLastTimestamp;
            DWORD days = duration / 86400; duration %= 86400;
            DWORD hours = duration / 3600; duration %= 3600;
            DWORD minutes = duration / 60; duration %= 60;
            DWORD seconds = duration;

            if ((uLastEventResourceID == IDS_EVENT_STARTUP) || (uLastEventResourceID == IDS_EVENT_WAKEUP) || (uLastEventResourceID == IDS_EVENT_RESUME))
                i18n.LoadString(IDS_UPTIME, lpPriorState, 100);
            else
                i18n.LoadString(IDS_DOWNTIME, lpPriorState, 100);

            i18n.ProcessMessageToBuffer(comment, 100, IDS_PRIOR_EVENT, lpPriorState, days, hours, minutes, seconds);
        }
        else
        {
            i18n.LoadString(IDS_INVALID_ORDER, comment, 100);
        }
    }

    i18n.ProcessMessage(IDS_LIST_ENTRY, lpDateBuffer, lpTimeBuffer, event, comment);

    dwLastTimestamp = dwTimestamp;
    uLastEventResourceID = uEventResourceID;

    return true;
}


static int ListAllEvents(LPCTSTR lpRemoteHost, bool bHibernate)
{
    DWORD dwComputerNameSize = 0;
    LPTSTR lpComputerName = nullptr;

    if (lpRemoteHost)
        i18n.ProcessMessage(IDS_UPTIME_REPORT, lpRemoteHost);
    else
    {
        ::GetComputerName(lpComputerName, &dwComputerNameSize);
        if (lpComputerName)
        {
            lpComputerName = new TCHAR[dwComputerNameSize +2];
            lpComputerName[0] = '\\';
            lpComputerName[1] = '\\';
            ::GetComputerName(lpComputerName + 2, &dwComputerNameSize);
            i18n.ProcessMessage(IDS_UPTIME_REPORT, lpComputerName);
            delete[] lpComputerName;
        }
    }

    putchar('\n');
//TODO: Get OS version or drop code
//    i18n.ProcessMessage(IDS_CURRENT_OS, _T(""));

//TODO: This is local time. Check if NetRemoteTOD should be used instead
    DYNAMIC_TIME_ZONE_INFORMATION TimeZoneInformation;
    DWORD rc = ::GetDynamicTimeZoneInformation(&TimeZoneInformation);
    if (rc == 1)
        i18n.ProcessMessage(IDS_TIME_ZONE, TimeZoneInformation.StandardName);
    else if (rc == 2)
        i18n.ProcessMessage(IDS_TIME_ZONE, TimeZoneInformation.DaylightName);
    putchar('\n');

    SYSTEMTIME SystemTime;
    ::GetLocalTime(&SystemTime);
    ::GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &SystemTime, nullptr, lpDateBuffer, 0x100, nullptr);
    ::GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &SystemTime, nullptr, lpTimeBuffer, 0x100);
    i18n.ProcessMessage(IDS_EXECUTION_TIMESTAMP, lpDateBuffer, lpTimeBuffer);
    putchar('\n');


    RetrieveAllEvents(ListAllCallback, bHibernate, lpRemoteHost);

    return 0;
}


int main(int argc, char* argv[])
{
    ::SetConsoleOutputCP(1250);
//For language debugging purposes only
#if 0
    DWORD langCount = 1;
    const WCHAR lang[] = { L"en-US\0" };
    ::SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, lang, &langCount);
#endif

    i18n.Init(&MessageCallback);
    bool DoList = false;
    bool DoHelp = false;
    bool DoAbsolute = false;
    bool DoHibernate = false;
    char* HostArg = nullptr;
    LPTSTR RemoteHost = nullptr;
    int rc = 0;


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
            else if (!_stricmp(option, "h"))
            {
                DoHibernate = true;
            }
            else
            {
                DoHelp = true;
                break;
            }
        }
        else // argument
        {
            if (!HostArg)
                HostArg = argv[cmdidx];
            else // We only support one remote host
            {
                DoHelp = true;
                break;
            }
        }
    }
    if (DoList && DoAbsolute)
        DoHelp = true;
    if (!DoList && DoHibernate)
        DoHelp = true;

#ifdef UNICODE
    if (HostArg)
    {
        int size = (int) strlen(HostArg) + 1;
        RemoteHost = new _TCHAR[size];
        ::MultiByteToWideChar(CP_ACP, 0, HostArg, -1, RemoteHost, size);
    }
#else
    RemoteHost = HostArg;
#endif

    if (DoHelp)
    {
        HowTo();
    }
    else if (DoList)
    {
        rc = ListAllEvents(RemoteHost, DoHibernate);
    }
    else
    {
        rc = LastStartup(RemoteHost, DoAbsolute);
    }

    if (RemoteHost)
        delete[] RemoteHost;

    return rc;
}

