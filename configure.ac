dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)
AC_INIT(version.cpp)
AC_CANONICAL_TARGET([])

dnl Init automake with the name of the package and the version
AM_INIT_AUTOMAKE(mindG4,"v0r0")
AM_CONFIG_HEADER(config.h)
dnl things required by automake
AC_PROG_MAKE_SET([])

dnl Checks for programs.
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

dnl MacOSX darwin compatibility 
AC_MSG_CHECKING(whether current system is supported)
case "$host_os" in
*darwin*)
  LDFLAGS="-flat_namespace"
  AC_DISABLE_SHARED	  
esac

define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

AC_MSG_CHECKING([for math library])
AC_CHECK_LIB(m,main)	

AC_MSG_CHECKING([for g2c library])
AC_CHECK_LIB(g2c,main)	

AC_PROG_INSTALL
AC_PROG_LN_S
AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_CHECK_TOOL(AR, ar, :)
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T


dnl Check for config programs
AC_PATH_PROG([CLHEP], [clhep-config], [no])
if test "${CLHEP}" = "no"; then
   AC_MSG_ERROR(clhep-config not found!)
fi

AC_PATH_PROG([BHEP], [bhep-config], [no])
if test "${BHEP}" = "no"; then
   AC_MSG_ERROR(bhep-config not found!)
fi

AC_MSG_CHECKING(if HDF5_PREFIX is defined)
if test x$HDF5_PREFIX = x; then
   HDF5_PREFIX=/usr/local
else
   HDF5_PREFIX=$HDF5_PREFIX
fi
AC_MSG_NOTICE(set HDF5_PREFIX to "$HDF5_PREFIX")

AC_MSG_CHECKING(if G4LIBDIR is defined)
if test x$G4LIBDIR = x; then
  AC_MSG_ERROR(G4LIBDIR not found: Check your G4 Installation)
fi
echo "yes"

AC_MSG_CHECKING(if G4INCLUDE is defined)
if test x$G4INCLUDE = x; then
  AC_MSG_ERROR(G4INCLUDE not found: Check your G4 Installation)
fi
echo "yes"


dnl CXX flags
AC_SUBST(CXXFLAGS)
CXXFLAGS="-g -Wall"

dnl includes
AC_SUBST(INCLUDES)
INCLUDES="-I."
INCLUDES="$INCLUDES $(clhep-config --include) $(bhep-config --include) -I$HDF5_PREFIX/include $(scripts/g4-config --include)"


dnl Use standard <sstream> if found; 
AC_CHECK_HEADERS([sstream])
dnl Use standard <ostream> if found; 
AC_CHECK_HEADERS([ostream])


dnl linker flags 
AC_SUBST(LDFLAGS)
LDFLAGS="$LDFLAGS $(clhep-config --ldflags)"
LDFLAGS="$LDFLAGS $(bhep-config --ldflags)"


dnl change path if your libraries are somewhere else
AC_SUBST(LIBS)

AC_MSG_CHECKING([for CLHEP library])
AC_CHECK_LIB(CLHEP, main)

AC_MSG_CHECKING([for HDF5 library])
AC_CHECK_LIB(hdf5, main)

AC_MSG_CHECKING([for bhep library])
AC_CHECK_LIB(bhep,main)

AC_MSG_CHECKING([for ZLIB library])
AC_CHECK_LIB(z,main)

LIBS="$LIBS $(scripts/g4-config --libs)"


echo ""
echo "host               = $host_os "
echo "CXX                = $CXX"
echo "CXXFLAGS           = $CXXFLAGS"
echo "LDFLAGS            = $LDFLAGS"
echo "INCLUDES           = $INCLUDES"
echo "LIBS               = $LIBS"
echo ""


AC_CONFIG_FILES([
	Makefile 
	nexus/Makefile
	base/Makefile
	physics/Makefile
	generator/Makefile
	geometry/Makefile
	manager/Makefile
	applications/Makefile
	])
AC_OUTPUT
