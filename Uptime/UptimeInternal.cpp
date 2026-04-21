#include "pch.h"
#include "UptimeInternal.h"

UPTIME_DATA::UPTIME_DATA()
{
	ComputerName = nullptr;
	lpBuffer = nullptr;
	dwBufferSize = 0;

	hEventLog = nullptr;
	lpBufferCurrent = nullptr;
	dwBytesRead = 0;
	bDirectionBackwards = false;
}

UPTIME_DATA::~UPTIME_DATA()
{
	if (ComputerName)
		delete[] ComputerName;

	if (lpBuffer)
		free(lpBuffer);

	if (hEventLog)
		::CloseEventLog(hEventLog);
}

bool UPTIME_DATA::GetLocalHostname()
{
	DWORD ComputerNameSize = 0;
	bool rc = false;

	if (ComputerName)
	{
		delete[] ComputerName;
		ComputerName = nullptr;
	}

	::GetComputerName(ComputerName, &ComputerNameSize);
	if (ComputerNameSize)
	{
		ComputerName = new TCHAR[ComputerNameSize];
		rc = ::GetComputerName(ComputerName, &ComputerNameSize);
	}

	return rc;
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


bool UPTIME_DATA::OpenEventLog(bool bReadBackwards)
{
	if (hEventLog)
		::CloseEventLog(hEventLog);

	lpBufferCurrent = nullptr;
	hEventLog = ::OpenEventLog(ComputerName, _T("System"));

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
		EventID:
		12->Start EVENT_ID_STARTUP EventCategory = 1
		13->Shutdown EVENT_ID_SHUTDOWN, Category = 2
		42->Standby EVENT_ID_STANDBY, Category = 64
		107->WakeUp EVENT_ID_WAKEUP, Category = 102
		*/

		if (
			((lpEventLogRecord->EventID == 12) && (lpEventLogRecord->EventCategory == 1))
		 || ((lpEventLogRecord->EventID == 13) && (lpEventLogRecord->EventCategory == 2))
		 || ((lpEventLogRecord->EventID == 42) && (lpEventLogRecord->EventCategory == 64))
		 || ((lpEventLogRecord->EventID == 107) && (lpEventLogRecord->EventCategory == 102))
		   )
		{
			return true;
		}
	}

	return false;
}

const LPBYTE UPTIME_DATA::GetEventDetails() const
{
	if (lpBufferCurrent)
		return LPBYTE(lpBufferCurrent);

	return nullptr;
}
