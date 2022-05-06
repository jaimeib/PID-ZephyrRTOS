/*
 * error_checks.h
 * 
 * Macros to check error codes in POSIX functions.
 * 
 * Copyright 2016 Hector Perez <perezh@unican.es>
 * 
 * File based on the error_checks.h included in MaRTE OS.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef	ERROR_CHECKS_H_
#define ERROR_CHECKS_H_

#include <string.h>
#include <stdio.h>

// Macro 'CHK'
//
// Error check for POSIX functions that do NOT modify "errno"
// ("modern" POSIX functions). Functions that return zero if
// successful; otherwise, they return an error number to indicate the
// error.
#define CHK(p) { int ret;                                      \
                 if ((ret = p)) {			       \
                   printf ("Error:"#p":%s\n", strerror (ret)); \
                   exit (-1);                                  \
                 }                                             \
               }

#endif // ERROR_CHECKS_H_
