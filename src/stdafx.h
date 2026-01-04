#pragma once

#include <cassert>
#include <cerrno>
#include <cstdio>

// Windows configuration - must be defined before including any Windows headers
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#define NOMINMAX             // Prevent Windows from defining min/max macros
#define STRICT               // Enable strict type checking for Windows handles

#include <cstdint>    // For SIZE_MAX and standard integer types

#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <objbase.h>  // For COM types (LPMALLOC, IMalloc) excluded by WIN32_LEAN_AND_MEAN
#include <winternl.h>

#include "dbghelp.h"    // Thread-safe wrapper around DbgHelp symbol functions
