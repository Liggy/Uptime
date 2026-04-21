#pragma once

class UPTIME_DATA
{
private:
	LPTSTR ComputerName;

	HANDLE hEventLog;
	LPBYTE lpBuffer;
	LPBYTE lpBufferCurrent;
	DWORD dwBufferSize;
	DWORD dwBytesRead;
	bool bDirectionBackwards;

	bool GetEventRecord();

	bool GetLocalHostname();

public:
	UPTIME_DATA();
	~UPTIME_DATA();
	
	bool OpenEventLog(bool bReadBackwards);
	bool GetNextEvent();

	const LPBYTE GetEventDetails() const;
};