#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)

AC_INIT([cpp-atmi],[m4_esyscmd_s(echo $(git describe --always --abbrev=0))],[herbert.koelman@me.com],[],[http://herbertkoelman.github.io/atmiplusplus/])
AC_LANG(C++)

AC_CONFIG_AUX_DIR([/usr/bin . bin])
AC_CANONICAL_BUILD

AC_ARG_ENABLE([localdir], AS_HELP_STRING([--enable-localdir],[adds -I and -L to /usr/local include and lib respectively.]), CFLAGS="-I /usr/local/include -L /usr/local/lib" )
AC_ARG_ENABLE([build], AS_HELP_STRING([--enable-build],[build build.]), BUILD=$enable_build)
AC_ARG_ENABLE([release], AS_HELP_STRING([--enable-release],[build as release.]), BUILD=$PACKAGE_VERSION)
AC_ARG_ENABLE([repository], AS_HELP_STRING([--enable-repository],[use the given name to locate the remote repository]), AC_MSG_NOTICE([repository set to $enable_repository]) )

# TODO remove this AC_DEFINE_UNQUOTED(CPP_ATMI_VERSION, ["$PACKAGE_VERSION compiled on `uname -sv`.`uname -r` for `basename $TUXDIR`"], [package description string])

# Check for optional use of doxygen
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen])
AC_CHECK_PROG([RM],[rm],[rm -f])
AC_PROG_MKDIR_P
AC_PROG_LN_S
AC_CHECK_PROG([DEPLOY],[basic_deploy.sh],[basic_deploy.sh],[echo \"please install basic_deploy.sh, make sure it\'s in your PATH and re-run ./configure\"])

if test -z "$BUILD" 
then
  BUILD="SNAPSHOT"
  AC_MSG_NOTICE([using default BUILD name $BUILD])
fi
AC_SUBST(BUILD,$BUILD)

if test ! -z "$enable_repository"
then
  AC_SUBST(REPOSITORY,"$enable_repository/$PACKAGE_NAME")
fi

# Okay, no cppcheck found, maybe we have sonar
if test -z "$CODECHECK" 
then
  AC_CHECK_PROG([CODECHECK],[sonar-runner],[sonar-runner -Dsonar.cfamily.library.directories=include,/usr/include,$TUXDIR/include],[])
fi

if test -z "$CODECHECK" 
then
  AC_CHECK_PROG([CODECHECK],[cppcheck],[cppcheck --template='{file}:{line},{severity},{id}:\n    {message}\n' --enable=all -I include ./src ./include],[])
fi

AC_MSG_CHECKING([whether target directory ./app exists])
if test -e ./app
then
  AC_MSG_RESULT([already there.])
else
  mkdir -p ./app
  AC_MSG_RESULT([./app created.])
fi

AC_MSG_CHECKING([whether target directory ./lib exists])
if test -e ./lib
then
  AC_MSG_RESULT([already there.])
else
  mkdir -p ./lib
  AC_MSG_RESULT([./lib created.])
fi

AC_MSG_CHECKING([whether target directory ./bin exists])
if test -d ./bin
then
  AC_MSG_RESULT([already there.])
else
  mkdir -p ./bin
  AC_MSG_RESULT([./bin created.])
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
AC_CONFIG_SUBDIRS(src utls)
AC_CONFIG_FILES([Makefile sonar-project.properties doxyfile])
AC_OUTPUT

