#pragma once


struct UPTIME_DATA
{
private:
	LPTSTR ComputerName;
	TIME_ZONE_INFORMATION LocalTimeZone;

	bool GetLocalHostname();
	bool GetLocalTimezone();

public:
	UPTIME_DATA();
	~UPTIME_DATA();
	
	void RetrieveUptimeEvent(/*handle*/ /*, bool backwards*/);
};