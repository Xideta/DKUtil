#pragma once

#ifndef	DKUT_VERSION_INCLUDED
#define DKUT_VERSION_INCLUDED

#define MAKE_STR_HELPER(x) #x
#define MAKE_STR(x) MAKE_STR_HELPER(x)

#define DKUT_VERSION_MAJOR		1
#define DKUT_VERSION_MINOR		0
#define DKUT_VERSION_PATCH		0
#define DKUT_VERSION_BETA		0
#define DKUT_VERSION_VERSTRING	MAKE_STR(DKUT_VERSION_MAJOR) "." MAKE_STR(DKUT_VERSION_MINOR) "." MAKE_STR(DKUT_VERSION_PATCH) "." MAKE_STR(DKUT_VERSION_BETA)

#endif
