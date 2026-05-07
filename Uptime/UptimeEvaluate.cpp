#include <stdio.h>
#include "pch.h"
#include "UptimeInternal.h"
#include "UptimeEvaluate.h"


// Return timestamp in case of success. 0 if not found
DWORD RetrieveUptime(LPCTSTR lpHostarg)
{
    UPTIME_DATA UptimeData;
    PEVENTLOGRECORD event;

    if (UptimeData.OpenEventLog(true, lpHostarg, EVENT_ID_STARTUP))
    {
        while (UptimeData.GetNextEvent())
        {
            event = (PEVENTLOGRECORD) UptimeData.GetEventDetails();
//            if ((event->EventID == 12) && (event->EventCategory == 1))
            return event->TimeGenerated;
        }
    }

    return 0;
}


DWORD RetrieveAllEvents(UptimeListPrototype cbCallbackFunction, bool bHibernate, LPCTSTR lpHostarg)
{
    UPTIME_DATA UptimeData;
    PEVENTLOGRECORD hEvent;
    
//TODO: restrict events based on command line arguments
    if (UptimeData.OpenEventLog(false, lpHostarg, bHibernate?-1:EVENT_ID_STARTUP | EVENT_ID_SHUTDOWN))
    {
        while (UptimeData.GetNextEvent())
        {
            hEvent = (PEVENTLOGRECORD) UptimeData.GetEventDetails();
            if (!cbCallbackFunction(hEvent->EventID, hEvent->EventCategory, hEvent->TimeGenerated))
                return 2;
        }
    }
    else
        return 1;

    return 0;

}
