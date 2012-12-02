#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([atmi++],[trunk],[herbert.koelman@me.com],[http://redmine.urbix-software.fr/projects/atmiplusplus])

AC_CONFIG_AUX_DIR([/usr/bin . bin])
AC_CONFIG_HEADERS([include/atmi_config.h])
AC_DEFINE_UNQUOTED(ATMI_VERSION, ["v$PACKAGE_VERSION compiled on `uname -sv`.`uname -r` for `basename $TUXDIR`"], [package description string])
AC_CHECK_PROG([RM],[rm],[rm -f])

# Check for optional use of doxygen
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen])

AC_CANONICAL_BUILD

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
AC_CONFIG_SUBDIRS(samples/utl)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

