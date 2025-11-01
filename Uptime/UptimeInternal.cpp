#include "pch.h"
#include "UptimeInternal.h"

UPTIME_DATA::UPTIME_DATA()
{
	ComputerName = nullptr;
}

UPTIME_DATA::~UPTIME_DATA()
{
	if (ComputerName)
		delete[] ComputerName;
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

bool UPTIME_DATA::GetLocalTimezone()
{
	GetTimeZoneInformation(&LocalTimeZone);

	return false;
}

void UPTIME_DATA::RetrieveUptimeEvent()
{
}
