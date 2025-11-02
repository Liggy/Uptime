#pragma once

class Uptime
{
private:

public:
	Uptime();
	~Uptime();
}; 

int RetrieveUptime(const char hostarg[] /*, int since*/);
