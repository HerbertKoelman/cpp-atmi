#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([cpp-atmi],[Unit tests],[herbert.koelman@pmu.fr])
AC_LANG(C++)

AC_ARG_VAR([TUXDIR], [Tuxedo home directory])
AC_ARG_ENABLE([tuxdir], \
  AS_HELP_STRING([--enable-tuxdir=PATH], [set TUXDIR to point to this path]), \
  TUXDIR=$enable_tuxdir)

AC_ARG_VAR([IMSDIR], [ART IMS home directory])
AC_ARG_ENABLE([imsdir], \
  AS_HELP_STRING([--enable-imsdir=PATH], [set IMSDIR to point to this path]), \
  IMSDIR=$enable_imsdir)

AC_ARG_VAR([AUXDIR], [auxilary directory, extends with AUXDIR/include and AUXDIR/lib])
AC_ARG_ENABLE([auxdir], \
  AS_HELP_STRING([--enable-auxdir=PATH], [set AUXDIR to extend include and lib search]), \
  AUXDIR=$enable_auxdir )

AC_CONFIG_SRCDIR([exceptions_tests.bcl])
AC_CONFIG_AUX_DIR([/usr/bin ../../ ../bin])

# Checks for programs.
AC_PROG_CXX([xlC_r xlC g++ c++ gcc cl KCC CC cxx cc++ aCC ])
AC_PROG_CC([xlc xlc_r gcc cl cc])
AC_CHECK_PROG([AR],[ar],[ar -rv])
AC_CHECK_PROG([MV],[mv],[mv],[echo "no mv command found"])
AC_CHECK_PROG([RM],[rm],[rm -f],[echo "no rm command found"])
AC_CHECK_PROG([MKFLDHDR],[mkfldhdr32],[mkfldhdr32],[echo "missing program mkfldhdr32." && exit -1])
AC_PROG_RANLIB

CFLAGS="-I ../../include -I ./"
LDFLAGS="$LDFLAGS -L ../../lib "

if test -n "$AUXDIR"
then
  CFLAGS="$CFLAGS -I$AUXDIR/include"
  LDFLAGS="$LDFLAGS -L$AUXDIR/lib"
fi

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

# Setting things up for ATMI builds
if test -n "$TUXDIR"
then
  TUXLIB="$TUXDIR/lib"
  TUXINC="$TUXDIR/include"

  AC_MSG_CHECKING([for tuxedo libraries])
  test -e $TUXLIB && AC_MSG_RESULT(found $TUXLIB.) || AC_MSG_ERROR(failed to find $TUXLIB directory.)

  AC_MSG_CHECKING([for tuxedo includes])
  test -e $TUXINC && AC_MSG_RESULT(found $TUXINC.) || AC_MSG_ERROR(failed to find $TUXINC directory.)

  CFLAGS="$CFLAGS -I$TUXINC "
  LDFLAGS="$LDFLAGS -L$TUXLIB"

  AC_CHECK_LIB([tux], [tpcall], [_void="found tux lib."])
  AC_CHECK_LIB([fml32], [Falloc32], [_void="found fml32 lib."])
else
  AC_MSG_ERROR([set env variable TUXDIR or use --enable-tuxdir=PATH.])
fi

#if test -n "$IMSDIR"
#then
#  IMSLIB="$IMSDIR/lib"
#  IMSINC="$IMSDIR/include"
#
#  AC_MSG_CHECKING([for ART IMS libraries])
#  test -e $IMSLIB && AC_MSG_RESULT(found $IMSLIB.) || AC_MSG_ERROR(failed to find $IMSLIB directory.)
#
#  AC_MSG_CHECKING([for ART IMS includes])
#  test -e $IMSINC && AC_MSG_RESULT(found $IMSINC.) || AC_MSG_ERROR(failed to find $IMSINC directory.)
#
#  CFLAGS="$CFLAGS -I$IMSINC "
#  LDFLAGS="$LDFLAGS -L$IMSLIB"
#
#else
#  AC_MSG_ERROR([set env variable IMSDIR or use --enable-imsdir=PATH.])
#fi

OBJECTS=""
for src in *.cpp
do
  obj=`basename $src .cpp`.o
  OBJECTS="$OBJECTS $obj"
done
AC_SUBST(OBJECTS,$OBJECTS)

AC_MSG_CHECKING([for specific $CXX compiler options])
case "$CXX" in
  xlC_r | xlC)
	CPPFLAGS="-brtl -bh:5 -qlanglvl=extended0x:decltype:static_assert::rvaluereferences:rvaluereferences" # this is not needed anymore -qsourcetype=c++ "
  CFLAGS="-q$bits -O -qmaxmem=-1 $CFLAGS"
  AC_MSG_RESULT([yes])
  ;;
  g++ | gcc)
	#permissive is not required anymore - CXXFLAGS="-x c++ -std=c++11 -frtti -fpermissive $CXXFLAGS "
	CXXFLAGS="-x c++ -std=c++11 -frtti $CXXFLAGS "
  AC_MSG_RESULT([yes])
  ;;
  *)
  AC_MSG_RESULT(none found.)
esac

# Checks for libraries.
# not used here - AC_CHECK_LIB([pthread], [pthread_create],[],[AC_ERROR([missing pthread library.])])
AC_CHECK_LIB([fml++], [strncmp],[],[AC_ERROR([missing fml++ library.])])
AC_CHECK_LIB([cltatmi++], [getenv],[],[AC_ERROR([missing cltatmi++ library.])])
AC_CHECK_LIB([pmulog],[log_get_header],[],[AC_ERROR([missing pmulog.h header.])])
AC_CHECK_LIB([pthread], [pthread_create],[],[AC_ERROR([missing pthread library.])])
AC_CHECK_LIB([cpp-pthread], [cpp_pthread_version],[],[AC_ERROR([missing cpp-thread library.])])

# Checks for header files.
# Changed for a AC_CHECK_LIB call - AC_CHECK_HEADERS([pmulog.h],[],[AC_ERROR([missing pmulog.h header.])])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST
#AC_C_INLINE
#AC_HEADER_TIME

# Checks for library functions.
AC_TYPE_SIGNAL

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

