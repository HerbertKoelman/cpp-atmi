#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([substation],[m4_esyscmd_s(echo $(git describe --always --abbrev=0))],[herbert.koelman@pmu.fr],[],[http://mut-mut-scmv1.dev.parimutuel.local/applications/cpp-atmi])
AC_LANG(C++)

CFLAGS="-D GIT_COMMIT=\"`git log --pretty='%H' | head -1`\""
#CFLAGS=""
CPPFLAGS="-I ./ -I ../include "
CXXFLAGS="-g"
LDFLAGS="$LDFLAGS -L ../lib "

AC_CONFIG_AUX_DIR([/usr/bin ../ ../bin])

AC_ARG_VAR([AUXDIR], [auxiliary include and lib directories])
AC_ARG_ENABLE([auxdir], AS_HELP_STRING([--enable-auxdir=PATH],[set AUXDIR to point to this path]), AUXDIR=$enable_auxdir)

AC_ARG_VAR([TUXDIR], [Tuxedo home directory])
AC_ARG_ENABLE([tuxdir], \
  AS_HELP_STRING([--enable-tuxdir=PATH], [set TUXDIR to point to this path]), \
  TUXDIR=$enable_tuxdir)

# Checks for programs.
AC_PROG_CXX([xlC_r xlC g++ c++ gcc cl KCC CC cxx cc++ aCC ])
AC_PROG_CC([xlc_r xlc gcc cl cc])
AC_CHECK_PROG([AR],[ar],[ar -rv])
AC_CHECK_PROG([MV],[mv],[mv],[echo "no mv command found"])
AC_CHECK_PROG([MKFLDHDR],[mkfldhdr32],[mkfldhdr32],[AC_ERROR([missing program mkfldhdr32.])])
AC_CHECK_PROG([RM],[rm],[rm -f])
AC_PROG_RANLIB

AC_CHECK_SIZEOF([long])
if test $ac_cv_sizeof_long == "8" 
then
  AC_SUBST(BITS,"64")
  bits=64
else
  AC_MSG_RESULT([using 32 bits (default)])
  AC_SUBST(BITS,"32")
  bits=32
fi

if test -n "$AUXDIR"
then
  CPPFLAGS="$CPPFLAGS -I$AUXDIR/include"
  LDFLAGS="$LDFLAGS -L$AUXDIR/lib"
  AC_MSG_NOTICE([WARNING using auxiliary directory $AUXDIR (CPPFLAGS=$CPPFLAGS, LDFLAGS=$LDFLAGS)])
fi

OBJECTS=""
for src in *.cpp
do
  obj=`basename $src .cpp`.o
  OBJECTS="$OBJECTS $obj"
done
AC_SUBST(OBJECTS,$OBJECTS)

TARGETS=""
for src in *.bcl
do
  obj=`basename $src .bcl`
  TARGETS="$TARGETS $obj"
done
for src in *.bsr
do
  obj=`basename $src .bsr`
  TARGETS="$TARGETS $obj"
done
AC_SUBST(TARGETS,$TARGETS)

# Setting things up for ATMI builds
if test -n "$TUXDIR"
then
  TUXLIB="$TUXDIR/lib"
  TUXINC="$TUXDIR/include"

  AC_MSG_CHECKING([for tuxedo libraries])
  test -e $TUXLIB && AC_MSG_RESULT(found $TUXLIB.) || AC_MSG_ERROR(failed to find $TUXLIB directory.)

  AC_MSG_CHECKING([for tuxedo includes])
  test -e $TUXINC && AC_MSG_RESULT(found $TUXINC.) || AC_MSG_ERROR(failed to find $TUXINC directory.)

  CPPFLAGS="$CPPFLAGS -I$TUXINC "
  LDFLAGS="$LDFLAGS -L$TUXLIB"

  AC_CHECK_LIB([tux], [tpcall], [void="yes"])
  AC_CHECK_LIB([fml32], [Falloc32], [void="yes"])
else
  AC_MSG_ERROR([set env variable TUXDIR or use --enable-tuxdir=PATH.])
fi

AC_MSG_CHECKING([for specific $CXX compiler options])
case "$CXX" in
  xlC_r | xlC)
  CFLAGS="-q$bits -qmaxmem=-1 -D__IBMCPP_TR1__ -qlanglvl=extended0x:decltype:static_assert:rvaluereferences $CFLAGS"
  LDFLAGS="$LDFLAGS -bh:5 -brtl -bmaxdata:0x80000000"
  AC_SUBST(BFLAGS,"-+")
  AC_MSG_RESULT([yes])
  ;;
  g++ | gcc)
	CFLAGS="-x c++ -std=c++11 -frtti $CFLAGS "
#  AC_SUBST(BFLAGS,"")
  AC_MSG_RESULT([yes])
  ;;
  *)
  AC_MSG_RESULT(none found.)
esac

# Checks for libraries.
#
# WARNING if some library depends on other, they must check in the order of dependency
#
AC_CHECK_LIB([fml++], [strerror],[],[AC_ERROR([missing fml++ library.])])
AC_CHECK_LIB([cltatmi++], [getenv],[CLT_LIBS="$CLT_LIBS -lcltatmi++"],[AC_ERROR([missing cltatmi++ library.])])
AC_CHECK_LIB([atmi++],    [getenv],[SRV_LIBS="$SRV_LIBS -latmi++"],[AC_ERROR([missing atmi++ library.])])
AC_SUBST(CLT_LIBS)
AC_SUBST(SRV_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gettimeofday])

# Checks for typedefs, structures, and compiler characteristics.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

