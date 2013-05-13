#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <SDKDDKVer.h>


#define PYKD_VERSION_MAJOR      0
#define PYKD_VERSION_MINOR      3
#define PYKD_VERSION_SUBVERSION 0
#define PYKD_VERSION_BUILDNO    0

#define __VER_STR2__(x) #x
#define __VER_STR1__(x) __VER_STR2__(x)

#define PYKD_VERSION_BUILD      PYKD_VERSION_MAJOR, PYKD_VERSION_MINOR, PYKD_VERSION_SUBVERSION, PYKD_VERSION_BUILDNO

#define PYKD_VERSION_BUILD_STR  __VER_STR1__(PYKD_VERSION_BUILD)

