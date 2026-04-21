#pragma once

class Uptime
{
private:

public:
	Uptime();
	~Uptime();
}; 

DWORD RetrieveUptime(const char hostarg[]);
