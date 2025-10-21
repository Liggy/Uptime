// Uptime.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#include <iostream>
#include "Uptime.h"


static void HowTo()
{
    std::cout
        << "Usage: Uptime [server] {/s} {/?}\n\n"
        << "       server  optionally connect to remote machine\n"
        << "       /s      Show all available startup and shutdown times\n"
        << "       /?      Show this help\n";
}


int main(int argc, char* argv[])
{
    HowTo();

    return 0;
}

