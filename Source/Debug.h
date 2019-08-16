/**
 * @file  Debug.h
 *
 * @brief  Provides debugging macros that are removed in release builds.
 */


#pragma once
#ifdef DEBUG
#   include <iostream>
#   include <cassert>
#   include <cstdio>

// Prints a line of debug output:
#   define DBG(toPrint) std::cout << toPrint << "\n";

// Prints a line of verbose debug output:
#   ifdef VERBOSE
#       define DBG_V(toPrint) DBG(toPrint)
#   else
#       define DBG_V(toPrint)
#   endif

// Prints a C-style error message:
#   define DBG_PERROR(toPrint) perror(toPrint);

// Terminates the program if a test condition is not met:
#   define DBG_ASSERT(condition) assert(condition);

// Redefine debug macros as empty statements outside of debug builds:
#else
#   define DBG(toPrint)
#   define DBG_V(toPrint)
#   define DBG_PERROR(toPrint)
#   define DBG_ASSERT(condition)
#endif
