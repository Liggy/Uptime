#include "pch.h"
#include "UptimeInternal.h"


UPTIME_DATA::UPTIME_DATA()
{
	lpComputerName = nullptr;
	lpBuffer = nullptr;
	dwBufferSize = 0;
	dwEventFilter = -1;

	hEventLog = nullptr;
	lpBufferCurrent = nullptr;
	dwBytesRead = 0;
	bDirectionBackwards = false;
}

UPTIME_DATA::~UPTIME_DATA()
{
	if (lpComputerName)
		delete[] lpComputerName;

	if (lpBuffer)
		free(lpBuffer);

	if (hEventLog)
		::CloseEventLog(hEventLog);
}


bool UPTIME_DATA::GetEventRecord()
{
	DWORD dwMinNumberOfBytesNeeded;

	if (lpBufferCurrent)
	{
		EVENTLOGRECORD* lpEventLogRecord = (PEVENTLOGRECORD) lpBufferCurrent;
		lpBufferCurrent += lpEventLogRecord->Length;
		if (lpBufferCurrent < lpBuffer + dwBytesRead)
			return true;
		lpBufferCurrent = nullptr;
	}

	bool rc = ::ReadEventLog(hEventLog, EVENTLOG_SEQUENTIAL_READ | (bDirectionBackwards ? EVENTLOG_BACKWARDS_READ : EVENTLOG_FORWARDS_READ), 0, lpBuffer, dwBufferSize, &dwBytesRead, &dwMinNumberOfBytesNeeded);
	if (!rc)
	{
		DWORD dwError = ::GetLastError();
		if (dwError == ERROR_INSUFFICIENT_BUFFER)
		{
			LPBYTE lpBufferTemp = (LPBYTE) realloc(lpBuffer, dwMinNumberOfBytesNeeded);
			if (!lpBufferTemp)
				return false;
			dwBufferSize = dwMinNumberOfBytesNeeded;
			lpBuffer = lpBufferTemp;
			rc = ::ReadEventLog(hEventLog, EVENTLOG_SEQUENTIAL_READ | (bDirectionBackwards ? EVENTLOG_BACKWARDS_READ : EVENTLOG_FORWARDS_READ), 0, lpBuffer, dwBufferSize, &dwBytesRead, &dwMinNumberOfBytesNeeded);
		}

		if (!rc)
			return false;
	}

	lpBufferCurrent = lpBuffer;
	return true;
}


bool UPTIME_DATA::OpenEventLog(bool bReadBackwards, LPCTSTR lpRemoteHost, DWORD dwFilter)
{
	if (hEventLog)
		::CloseEventLog(hEventLog);

	lpBufferCurrent = nullptr;

// API currently fails when RemoteHost is set and code is compiled as MBCS instead of Unicode
	hEventLog = ::OpenEventLog(lpRemoteHost, _T("System"));

	if (!hEventLog)
		return false;

	if (!dwBufferSize)
	{
		lpBuffer = (LPBYTE) malloc(0x10000);
		if (!lpBuffer)
			return false;
		dwBufferSize = 0x10000;
	}

	bDirectionBackwards = bReadBackwards;
	dwEventFilter = dwFilter;

	return true;
}


bool UPTIME_DATA::GetNextEvent()
{
	PEVENTLOGRECORD lpEventLogRecord;

	while (GetEventRecord())
	{
		lpEventLogRecord=(PEVENTLOGRECORD) lpBufferCurrent;
		/*
		EventType == EVENTLOG_INFORMATION_TYPE
		EventID:EventCategory -> Event
		12:1    -> EVENT_ID_STARTUP
		507:158 -> EVENT_ID_WAKEUP
		107:102 -> EVENT_ID_RESUME
		13:2    -> EVENT_ID_SHUTDOWN
		506:157 -> EVENT_ID_SLEEP
		42:64   -> EVENT_ID_HIBERNATE

		EventType == EVENTLOG_ERROR_TYPE
		EventID:
		6008->Unexpected Shutdown EVENT_ID_UNEXPECTED, Category = None - occurs after startup event
		*/

		if ((this->dwEventFilter & EVENT_ID_STARTUP) && (lpEventLogRecord->EventID == 12) && (lpEventLogRecord->EventCategory == 1))
			return true;
		if ((this->dwEventFilter & EVENT_ID_WAKEUP) && (lpEventLogRecord->EventID == 507) && (lpEventLogRecord->EventCategory == 158))
			return true;
		if ((this->dwEventFilter & EVENT_ID_RESUME) && (lpEventLogRecord->EventID == 107) && (lpEventLogRecord->EventCategory == 102))
			return true;
		if ((this->dwEventFilter & EVENT_ID_SHUTDOWN) && (lpEventLogRecord->EventID == 13) && (lpEventLogRecord->EventCategory == 2))
			return true;
		if ((this->dwEventFilter & EVENT_ID_SLEEP) && (lpEventLogRecord->EventID == 506) && (lpEventLogRecord->EventCategory == 157))
			return true;
		if ((this->dwEventFilter & EVENT_ID_HIBERNATE) && (lpEventLogRecord->EventID == 42) && (lpEventLogRecord->EventCategory == 64))
			return true;
	}

	return false;
}


const LPBYTE UPTIME_DATA::GetEventDetails() const
{
	if (lpBufferCurrent)
		return LPBYTE(lpBufferCurrent);

	return nullptr;
}
