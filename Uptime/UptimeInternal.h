#pragma once

#define EVENT_ID_STARTUP 1
#define EVENT_ID_WAKEUP 2
#define EVENT_ID_RESUME 4
#define EVENT_ID_SHUTDOWN 8
#define EVENT_ID_SLEEP 16
#define EVENT_ID_HIBERNATE 32


class UPTIME_DATA
{
private:
	LPTSTR lpComputerName;

	HANDLE hEventLog;
	LPBYTE lpBuffer;
	LPBYTE lpBufferCurrent;
	DWORD dwBufferSize;
	DWORD dwBytesRead;
	bool bDirectionBackwards;

	DWORD dwEventFilter;

	bool GetEventRecord();

public:
	UPTIME_DATA();
	~UPTIME_DATA();
	
	bool OpenEventLog(bool bReadBackwards, LPCTSTR lpRemoteHost=nullptr, DWORD dwFilter = -1);
	bool GetNextEvent();

	const LPBYTE GetEventDetails() const;
};