//
// Created by surajcha on 7/24/18.
//
#pragma once
#ifndef MACRODEF_H
#define MACRODEF_H
#include <math.h>
#include <stdio.h>

#ifndef NULL
    #define NULL 0L
#endif /* ~NULL */

#ifndef TRUE
    #define FALSE 0
    #define TRUE 1
#endif /* ~TRUE */

#ifdef _WIN32
    #define DIR_SEP '\\'
#else /* ~_WIN32 */
    #define DIR_SEP '/'
#endif /* ~_WIN32 */


#ifdef __max
	#define max __max
#else
	#define max(A,B) ( ((A) > (B)) ? (A) : (B) )
#endif /* ~max */


#ifdef  __min
	#define min __min
#else
    #define min(A,B) ( ((A) > (B)) ? (B) : (A) )
#endif /* ~min */

#ifndef round
	#define round(X) (((X) >= 0) ? (int)((X)+0.5) : (int)((X)-0.5))
#endif /* ~round */

#ifndef floor
    #define floor(X) ((long)X)
#endif

#ifndef MAXPATHLENGTH
    #define MAXPATHLENGTH 256
#endif /* ~MAXPATHLENGTH */

#ifndef ERRSTREAM
	#define ERRSTREAM stdout
#endif

#ifndef LOGSTREAM
	#define LOGSTREAM stdout
#endif // !LOGSTREAM


#endif //MACRODEF_H
