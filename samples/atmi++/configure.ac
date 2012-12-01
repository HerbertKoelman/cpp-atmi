#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([atmi++],[2.1],[herbert.koelman@me.com])
AC_CONFIG_SRCDIR([tptest.bcl])
#AC_CONFIG_HEADERS([../../include/atmi++.h])

AC_ARG_VAR([TUXDIR], [Tuxedo home directory])
AC_ARG_ENABLE([tuxdir], AS_HELP_STRING([--enable-tuxdir=PATH],[set TUXDIR to point to this path]), TUXDIR=$enable_tuxdir)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_CHECK_PROG([AR],[ar],[ar -rv])
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create])

# Setting things up for ATMI builds
CFLAGS="$CFLAGS -I../../include "

AC_MSG_CHECKING([for tuxedo home directory])
if test -n "$TUXDIR"
then

  AC_MSG_RESULT([$TUXDIR])

  TUXLIB="$TUXDIR/lib"
  TUXINC="$TUXDIR/include"

  AC_MSG_CHECKING([for tuxedo libraries])
  test -e $TUXLIB && AC_MSG_RESULT(found $TUXLIB.) || AC_MSG_ERROR(failed to find $TUXLIB directory.)

  AC_MSG_CHECKING([for tuxedo includes])
  test -e $TUXINC && AC_MSG_RESULT(found $TUXINC.) || AC_MSG_ERROR(failed to find $TUXINC directory.)

  CFLAGS="$CFLAGS -I$TUXINC "
  CXXFLAGS="$CFLAGS"
  LDFLAGS="$LDFLAGS -L$TUXLIB -L../../lib"

  AC_CHECK_LIB([tux], [tpcall])

else
  AC_MSG_ERROR([set env variable TUXDIR or use --enable-tuxdir=PATH.])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h nl_types.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gettimeofday])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

AC_MSG_CHECKING(whether we have specific compiler optimization options for $CXX )
case "$CXX" in
  xlC_r)
	CFLAGS="-+ $CFLAGS "
	CXXFLAGS="-+ $CXXFLAGS "
  AC_MSG_RESULT([-+ .])
  ;;
  g++)
	CFLAGS="-+ $CFLAGS "
	CXXFLAGS="-x c++ $CXXFLAGS "
  AC_MSG_RESULT([-x c++ .])
  ;;
  *)
  AC_MSG_RESULT(none found.)
esac

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

