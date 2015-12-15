#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(matrixtools, 1.0.0, gentili@mcpnet.ca)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
                 Makefile
                 display/Makefile
                 mtop/Makefile
                 ])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lncurses':
AC_CHECK_LIB([ncurses], [main], [], AC_MSG_ERROR([Missing required library]))
# FIXME: Replace `main' with a function in `-lprocps':
AC_CHECK_LIB([procps], [main], [], AC_MSG_ERROR([Missing required library]))
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main] [], AC_MSG_ERROR([Missing required library]))
# FIXME: Replace `main' with a function in `-ltinfo':
AC_CHECK_LIB([tinfo], [main] [], AC_MSG_ERROR([Missing required library]))

# Checks for header files.
AC_CHECK_HEADERS([string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([floor gettimeofday memset])

AC_OUTPUT
