#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([cpp-atmi],[m4_esyscmd_s(echo $(git describe --always --abbrev=0))],[herbert.koelman@me.com],[],[http://herbertkoelman.github.io/atmiplusplus/])
AC_CONFIG_HEADERS([../include/atmi/config.h])

AC_CONFIG_SRCDIR([tuxedo.cpp])

# AC_ARG_ENABLE([localdir], AS_HELP_STRING([--enable-localdir],[adds -I and -L to /usr/local include and lib respectively.]))

AC_ARG_VAR([AUXDIR], [auxiliary include and lib directories])
AC_ARG_ENABLE([auxdir], AS_HELP_STRING([--enable-auxdir=PATH],[set AUXDIR to point to this path]), AUXDIR=$enable_auxdir)

AC_ARG_VAR([TUXDIR], [Tuxedo home directory])
AC_ARG_ENABLE([tuxdir], AS_HELP_STRING([--enable-tuxdir=PATH],[set TUXDIR to point to this path]), TUXDIR=$enable_tuxdir)

AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_CHECK_PROG([AR],[ar],[ar -rv])
AC_PROG_RANLIB

CXXFLAGS=""
CFLAGS="-I../include"

if test -n "$AUXDIR"
then
  CFLAGS="$CFLAGS -I$AUXDIR/include"
  LDFLAGS="$LDFLAGS -L$AUXDIR/lib"
  AC_MSG_NOTICE([WARNING using auxiliary directory $AUXDIR (CFLAGS=$CFLAGS, LDFLAGS=$LDFLAGS)])
fi

if test -n "$enable_localdir"
then
  CFLAGS="$CFLAGS -I$enable_localdir/include"
  LDFLAGS="$LDFLAGS -L$wenable_localdir/lib"
  AC_MSG_NOTICE([WARNING using local directory $enable_localdir (CFLAGS=$CFLAGS, LDFLAGS=$LDFLAGS)])
fi

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create],[],[AC_ERROR([missing pthread library.])])

# C++11 checks
AC_CHECK_HEADER([mutex], [AC_DEFINE([HAVE_CPP11_MUTEX])])

if test "$ac_cv_header_mutex" = "no" 
then
#   AC_CHECK_LIB([cpp-pthread], [cpp_pthread_version],[AC_DEFINE([HAVE_CPP_PTHREAD])],[AC_ERROR([missing cpp-pthread library.])])
   AC_CHECK_LIB([cpp-pthread], [cpp_pthread_version],[AC_DEFINE([HAVE_CPP_PTHREAD]) LIBS="$LIBS -lcpp-pthread"],[ AC_ERROR([missing cpp-pthread library.]) ])
fi

OBJECTS="atmi++.o"
for src in *.cpp
do
  OBJECTS="$OBJECTS `basename $src .cpp`.o"
done
AC_SUBST(OBJECTS,$OBJECTS)

# Setting things up for ATMI builds
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
  LDFLAGS="$LDFLAGS -L$TUXLIB"

else
  AC_MSG_ERROR([set env variable TUXDIR or use --enable-tuxdir=PATH.])
fi

AC_DEFINE_UNQUOTED(CPP_ATMI_VERSION, ["$PACKAGE_NAME $PACKAGE_VERSION - $(git log --pretty='%H' | head -1) - compiled on `uname -sv` for `basename $TUXDIR`."], [Tuxedo ATMI C++ wrapper.])

# Checks for header files.
#AC_CHECK_HEADERS([limits.h nl_types.h cstdlib cstring sys/time.h cunistd])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_MSG_CHECKING(whether we have specific compiler optimization options for $CXX )
case "$CXX" in
  xlC_r | xlC)
	  CPPFLAGS="-qlanglvl=extended0x:decltype:static_assert:rvaluereferences"
    CFLAGS="-qmaxmem=-1 -qsmp $CFLAGS"
    CXXFLAGS="-O"
    LDFLAGS="-brtl -bh:5 $LDFLAGS"
    AC_MSG_RESULT([$CFLAGS $CPPFLAGS $CXXFLAGS])
    ;;
  g++ | gcc)
    CXXFLAGS="-O2"
    CPPFLAGS="-std=c++11 $CPPFLAGS "
    AC_MSG_RESULT([$CFLAGS $CPPFLAGS $CXXFLAGS])
    ;;
  *)
    AC_MSG_RESULT([none found.])
esac

AC_CONFIG_FILES([atmi++.cpp Makefile])
AC_OUTPUT

