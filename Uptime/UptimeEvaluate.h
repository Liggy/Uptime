#pragma once

typedef bool (*UptimeListPrototype)(const DWORD, const WORD, const DWORD);

DWORD RetrieveUptime(LPCTSTR lpHostarg = nullptr);
DWORD RetrieveAllEvents(UptimeListPrototype cbCallbackFunction, LPCTSTR lpHostarg = nullptr);