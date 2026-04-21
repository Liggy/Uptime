#include <stdio.h>
#include "pch.h"
#include "UptimeInternal.h"
#include "UptimeEvaluate.h"

static UPTIME_DATA UptimeData;


// Return timestamp in case of success. 0 if not found
DWORD RetrieveUptime(const char hostarg[])
{
    PEVENTLOGRECORD event;

    if (UptimeData.OpenEventLog(true))
    {
        while (UptimeData.GetNextEvent())
        {
            event = (PEVENTLOGRECORD) UptimeData.GetEventDetails();
            if ((event->EventID == 12) && (event->EventCategory == 1))
                return event->TimeGenerated;
        }
    }

    return 0;
}

