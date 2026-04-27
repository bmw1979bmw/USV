// Это инклудить в конец maindoc.cpp
// #pragma hdrstop
// #include "Printer.inl"

#include "WinSpool.h"

BOOL
WINAPI
OpenPrinter(
   LPSTR    pPrinterName,
   LPHANDLE phPrinter,
   LPPRINTER_DEFAULTSA pDefault
){return 0;};

LONG
WINAPI
DocumentProperties(
    HWND      hWnd,
    HANDLE    hPrinter,
    LPSTR   pDeviceName,
    PDEVMODEA pDevModeOutput,
    PDEVMODEA pDevModeInput,
    DWORD     fMode
){return 0;};

BOOL
WINAPI
ClosePrinter(
    HANDLE hPrinter
){return 0;};
