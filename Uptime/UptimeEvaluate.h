#pragma once

typedef bool (*UptimeListPrototype)(const DWORD, const WORD, const DWORD);

DWORD RetrieveUptime(LPCTSTR lpHostarg = nullptr);
DWORD RetrieveAllEvents(UptimeListPrototype cbCallbackFunction, bool bHibernate, LPCTSTR lpHostarg = nullptr);