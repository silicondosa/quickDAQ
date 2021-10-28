//
// Created by surajcha on 7/24/18.
//
#pragma once
#ifndef MACRODEF_H
#define MACRODEF_H

#include <math.h>
#include <stdio.h>

#ifdef __cplusplus 
extern "C" {
#endif 

// ---------------------------
// Define NULL, TRUE and FALSE
// ---------------------------
#ifndef NULL
    #define NULL 0L
#endif /* ~NULL */

#ifndef TRUE
    #define FALSE 0
    #define TRUE 1
#endif /* ~TRUE */

// -----------------------------------
// Platform specific macro definitions
// -----------------------------------
#ifdef _WIN32
	// Windows-specific definitions
	// ----------------------------
    #define DIR_SEP '\\'
	#define WIN32_LEAN_AND_MEAN
#else /* ~_WIN32 */
    #define DIR_SEP '/'
#endif /* ~_WIN32 */

// --------------------------------
// Platform-independent math macros
// --------------------------------
#ifndef round
	#define round(X) (((X) >= 0) ? (int)((X)+0.5) : (int)((X)-0.5))
#endif /* ~round */

#ifndef floor
    #define floor(X) ((long)X)
#endif

// -------------------
// C++ specific macros
// -------------------
#ifdef __cplusplus
	#ifdef __max
		#define max __max
	#else
		#ifndef max
			#define max(A,B) ( ((A) > (B)) ? (A) : (B) )
		#endif // max not defined
	#endif /* ~__max */

	#ifdef  __min
		#define min __min
	#else
		#ifndef min
			#define min(A,B) ( ((A) > (B)) ? (B) : (A) )
		#endif // min not defined
	#endif /* ~__min */
#endif // __cplusplus

// -------------------------------------------
// Max length of paths allowed in our programs
// -------------------------------------------
#ifndef MAXPATHLENGTH
    #define MAXPATHLENGTH 256
#endif /* ~MAXPATHLENGTH */

// -------------------------
// Output stream definitions
// -------------------------
#ifndef ERRSTREAM
	#define ERRSTREAM stdout
#endif

#ifndef LOGSTREAM
	#define LOGSTREAM stdout
#endif // !LOGSTREAM

#ifdef __cplusplus 
}
#endif 

#endif //MACRODEF_H
