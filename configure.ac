#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([atmi++],[v3.2],[herbert.koelman@me.com],[atmi++-src-v3.2.tar.gz],[https://github.com/HerbertKoelman/atmiplusplus])

AC_ARG_ENABLE([localdir], AS_HELP_STRING([--enable-localdir],[adds -I and -L to /usr/local include and lib respectively.]), CFLAGS="-I /usr/local/include -L /usr/local/lib" )
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug],[sets #define _DEBUG_]),[AC_DEFINE_UNQUOTED(_DEBUG_,[1],[activate debug traces.])])

AC_CONFIG_AUX_DIR([/usr/bin . bin])
AC_CONFIG_HEADERS([include/atmi++.h])
AC_DEFINE_UNQUOTED(ATMI_VERSION, ["$PACKAGE_VERSION compiled on `uname -sv`.`uname -r` for `basename $TUXDIR`"], [package description string])
AC_CHECK_PROG([RM],[rm],[rm -f])

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create],[],[
	 echo "missing dependency." 
   exit -1])
AC_CHECK_LIB([yaml], [yaml_parser_initialize],[],[
	 echo "missing dependency (check http://pyyaml.org/wiki/LibYAML)." 
   exit -1])


# Check for optional use of doxygen
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen])
AC_CHECK_PROG([BEAUTIFIER], [uncrustify], [uncrustify])

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create],[],[
  echo "you need libpthread, please check your distrib"
    exit -1])

AC_CHECK_LIB([yaml], [yaml_parser_initialize],[],[
  echo "you need libyaml, please check your distrib or you may try yum install libyaml-devel"
    exit -1])

AC_CANONICAL_BUILD

AC_MSG_CHECKING([whether config.h exists])
if test -e ./include/atmi++.h
then
  rm ./include/atmi++.h
  AC_MSG_RESULT([./include/atmi++.h was removed.])
fi

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
AC_CONFIG_SUBDIRS(src/tuxedo)
AC_CONFIG_SUBDIRS(src/utl)
AC_CONFIG_SUBDIRS(samples/atmi++)
# not ready to be released - AC_CONFIG_SUBDIRS(samples/utl)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

