// Uptime.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#include <iostream>
#include "Uptime.h"


static void HowTo()
{
    std::wcout
        << L"Usage: Uptime [server] {/s} {/?}\n\n"
        << L"       server  optionally connect to remote machine\n"
        << L"       /s      Show all available startup and shutdown times\n"
        << L"       /?      Show this help\n";
}


int main(int argc, char* argv[])
{
    bool DoList = false;
    bool DoHelp = false;
    char* RemoteHost = nullptr;

    for (int cmdidx = 1; cmdidx < argc; cmdidx++)
    {
        if ((argv[cmdidx][0] == '-') || (argv[cmdidx][0] == '/')) // Option
        {
            char* option = &argv[cmdidx][1];
            if (!_stricmp(option, "?"))
            {
                DoHelp = true;
            }
            else if (!_stricmp(option, "s"))
            {
                DoList = true;
            }
            else
            {
                DoHelp = true;
                break;
            }
        }
        else // argument
        {
            if (!RemoteHost)
                RemoteHost = argv[cmdidx];
            else // We only support one remote host
            {
                DoHelp = true;
                break;
            }
        }
    }
    if (DoHelp)
    {
        HowTo();
    }
    else if (DoList)
    {
// List all related events
        std::wcout << "Listing all events\n";
    }
    else
    {
// Just show last startup
        std::wcout << "Host started at xyz. This is abc seconds ago\n";
    }
    return 0;
}

