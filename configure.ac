#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([cpp-atmi],[m4_esyscmd_s(echo $(git describe --always --abbrev=0))],[herbert.koelman@me.com],[],[http://herbertkoelman.github.io/atmiplusplus/])
AC_LANG(C++)

AC_ARG_ENABLE([localdir], AS_HELP_STRING([--enable-localdir],[adds -I and -L to /usr/local include and lib respectively.]), CFLAGS="-I /usr/local/include -L /usr/local/lib" )
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug],[sets #define _DEBUG_]),[AC_DEFINE_UNQUOTED(_DEBUG_,[1],[activate debug traces.])])

AC_CONFIG_AUX_DIR([/usr/bin . bin])

AC_DEFINE_UNQUOTED(CPP_ATMI_VERSION, ["$PACKAGE_VERSION compiled on `uname -sv`.`uname -r` for `basename $TUXDIR`"], [package description string])

# Check for optional use of doxygen
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen])
AC_CHECK_PROG([RM],[rm],[rm -f])
AC_PROG_MKDIR_P

AC_CANONICAL_BUILD

#HEADERS=""
#for header in include/atmi/*.hpp include/atmi/*.h
#do
#  HEADERS="$HEADERS $header"
#done
#for header in include/atmi/*.in
#do
#  HEADERS="$HEADERS `basename $header .in`"
#done
#AC_SUBST(HEADERS,$HEADERS)

AC_MSG_CHECKING([whether target directory ./lib exists])
if test -e ./lib
then
  AC_MSG_RESULT([already there.])
else
  mkdir -p ./lib
  AC_MSG_RESULT([./lib created.])
fi

AC_MSG_CHECKING([whether wording directory ./locale exists])
if test -e ./locale
then
  AC_MSG_RESULT([already there.])
else
  mkdir -p ./locale/en_US ./locale/fr_FR
  AC_MSG_RESULT([./locale created.])
fi

AC_PROG_INSTALL
AC_CONFIG_SUBDIRS(src)
AC_CONFIG_FILES([Makefile doxyfile])
AC_OUTPUT

