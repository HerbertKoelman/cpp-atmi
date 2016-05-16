#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([cpp-atmi],[m4_esyscmd_s(echo $(git describe --always --abbrev=0))],[herbert.koelman@me.com],[],[http://herbertkoelman.github.io/atmiplusplus/])
AC_CONFIG_HEADERS([../include/atmi/config.h])

AC_DEFINE_UNQUOTED(CPP_ATMI_VERSION, ["$PACKAGE_NAME $PACKAGE_VERSION - $(git log --pretty='%H' | head -1) - compiled on `uname -sv` for `basename $TUXDIR`."], [Tuxedo ATMI C++ wrapper.])

AC_CONFIG_SRCDIR([tuxedo.cpp])

AC_ARG_ENABLE([localdir], AS_HELP_STRING([--enable-localdir],[adds -I and -L to /usr/local include and lib respectively.]), CFLAGS="-I /usr/local/include -L /usr/local/lib" )

AC_ARG_VAR([TUXDIR], [Tuxedo home directory])
AC_ARG_ENABLE([tuxdir], AS_HELP_STRING([--enable-tuxdir=PATH],[set TUXDIR to point to this path]), TUXDIR=$enable_tuxdir)

AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_CHECK_PROG([AR],[ar],[ar -rv])
AC_CHECK_PROG([BEAUTIFIER],[uncrustify],[uncrustify -l CPP --replace -c ../crust.cfg])
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create],[],[AC_ERROR([missing pthread library.])])

# Setting things up for ATMI builds
CFLAGS="$CFLAGS -I../include"

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
  LDFLAGS="$LDFLAGS -L$TUXLIB"

  # This cannot be checked here. The library doesn't exist until configure and make is run
  # AC_CHECK_LIB([tux], [tpcall])

else
  AC_MSG_ERROR([set env variable TUXDIR or use --enable-tuxdir=PATH.])
fi

# Checks for header files.
#AC_HEADER_STDC
#AC_CHECK_HEADERS([limits.h nl_types.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST
#AC_C_INLINE
#AC_HEADER_TIME

# Checks for library functions.
#AC_TYPE_SIGNAL
#AC_FUNC_MALLOC
#AC_CHECK_FUNCS([memset])

# Checks for typedefs, structures, and compiler characteristics.
#AC_HEADER_STDBOOL

AC_MSG_CHECKING(whether we have specific compiler optimization options for $CXX )
case "$CXX" in
  xlC_r)
	CFLAGS="-O5 -qmaxmem=-1 -qsmp $CFLAGS "
	CXXFLAGS="-O5 -qmaxmem=-1 -qsmp $CXXFLAGS "
  AC_MSG_RESULT(-O5 -qmaxmem=-1 -qsmp.)
  ;;
  *)
  AC_MSG_RESULT(none found.)
esac

AC_CONFIG_FILES([atmi++.cpp Makefile])
AC_OUTPUT

