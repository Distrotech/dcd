/* This file is licensed under the GPL v2	*/
/* (c) Othmar Pasteka <othmar@debian.org>, 2000	*/
/* David E. Smith 2000 -- a few simple changes  */

#include <endian.h>

#ifndef __BYTE_ORDER
#error Unable to automatically determine byte order; check endian.h

/* This is the default -- Linux on x86 machines. */
/* #define SHA_BYTE_ORDER 1234 */
/* A few other options - but if you're here you */
/* probably know what you're doing. */
/* #define SHA_BYTE_ORDER 4321 */
/* #define SHA_BYTE_ORDER 12345678 */
/* #define SHA_BYTE_ORDER 87654321 */

#endif


/*
	This version uses the libc macro __BYTE_ORDER
	to determine the endianess of the architecture
*/ 

#define SHA_BYTE_ORDER __BYTE_ORDER

#define SHA_VERSION 1
