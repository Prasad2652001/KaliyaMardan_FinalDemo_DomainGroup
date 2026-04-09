#pragma once

#include <windows.h> // For BOOL, TEXT(), MessageBox(), etc.
#include <stdio.h>   // For fopen_s(), fprintf(), fclose()
#include <string>
#include "GLHeadersAndMacros.h"

BOOL CreateLogFile(void);
void PrintGLInfo(void);
void PrintLog(const char *, ...);
void PrintLogFunction(std::string, const char *, ...);
void PrintLogString(std::string data);
void CloseLogFile(void);
