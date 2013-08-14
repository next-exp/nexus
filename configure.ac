dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)
AC_INIT(version.cpp)
AC_CANONICAL_TARGET([])

dnl Initi automake with the name of the package and the version
AM_INIT_AUTOMAKE(next,"v0r1")
dnl AC_CONFIG_HEADERS(config.h)
AM_CONFIG_HEADER(config.h)

dnl things required by automake

AC_PROG_MAKE_SET([])


dnl Checks for programs.
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX


AC_MSG_CHECKING(whether current system is supported)
case "$host_os" in
*darwin*)
  LDFLAGS="-flat_namespace"
  AC_DISABLE_SHARED	  
esac

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

dnl CXX flags
AC_SUBST(CXXFLAGS)

CXXFLAGS="-g -Wall" 
CXXFLAGS=" $CXXFLAGS -DG4VIS_USE -DG4VIS_BUILD_OPENGLX_DRIVER -DG4VIS_USE_OPENGLX"
CXXFLAGS=" $CXXFLAGS -DG4DEBUG -DG4VERBOSE -DG4_STORE_TRAJECTORY  -DG4USE_STL -DG4UI_USE_TCSH"


dnl includes
AC_SUBST(INCLUDES)
INCLUDES="-I."

dnl Check if HDF5_PREFIX is defined
if test x$HDF5_PREFIX = x; then
   HDF5_PREFIX=/usr/local
else
     HDF5_PREFIX=$HDF5_PREFIX
fi

echo "set HDF5_PREFIX to "$HDF5_PREFIX

dnl Check if G4INCLUDE is defined
if test x$G4INCLUDE = x; then
  AC_MSG_ERROR("G4INCLUDE not found: Check your G4 Installation")
fi

INCLUDES="$INCLUDES $(clhep-config --include) $(bhep-config --include) -I$HDF5_PREFIX/include" 
INCLUDES="$INCLUDES -I$G4INCLUDE"

AC_CHECK_HEADERS([hdf5.h], [], [AC_MSG_ERROR("HDF5 headers not found: Check your installation")])
AC_CHECK_HEADERS([zlib.h], [], [AC_MSG_ERROR("ZLIB headers not found: Check your installation")])
AC_CHECK_HEADERS([bhep/bhep_svc.h], [], [AC_MSG_ERROR("bhep headers not found: Check your installation")])


dnl Use standard <sstream> if found; 
AC_CHECK_HEADERS([sstream])

dnl Use standard <ostream> if found; 
AC_CHECK_HEADERS([ostream])


dnl linker flags 
AC_SUBST(LDFLAGS)

LDFLAGS="$LDFLAGS $(clhep-config --ldflags)"
LDFLAGS="$LDFLAGS $(bhep-config --ldflags)"
#LDFLAGS="$LDFLAGS -L$(root-config --prefix)/lib"
#LDFLAGS="$LDFLAGS -L$HDF5_PREFIX/lib"

dnl Check if G4LIBDIR is defined
if test x$G4LIBDIR = x; then
  AC_MSG_ERROR("G4LIBDIR not found: Check your G4 Installation")
fi

LDFLAGS="$LDFLAGS -L$G4LIBDIR"


dnl change path if your libraries are somewhere else
AC_SUBST(LIBS)

AC_MSG_CHECKING([for CLHEP library])
AC_CHECK_LIB(CLHEP, main, [], [AC_MSG_ERROR("CLHEP library not found: Check your installation")])

AC_MSG_CHECKING([for HDF5 library])
AC_CHECK_LIB(hdf5, main, [], [AC_MSG_ERROR("HDF5 library not found: Check your installation")])

AC_MSG_CHECKING([for bhep library])
AC_CHECK_LIB(bhep, main, [], [AC_MSG_ERROR("BHEP library not found: Check your installation")])

AC_MSG_CHECKING([for ZLIB library])
AC_CHECK_LIB(z, main, [], [AC_MSG_ERROR("ZLIB library not found: Check your installation")])

#LIBS="$LIBS -lhdf5"

LIBS=" -lG4digits_hits -lG4event -lG4FR -lG4geometry -lG4global $LIBS"
LIBS=" -lG4graphics_reps -lG4intercoms -lG4interfaces -lG4materials $LIBS" 
LIBS=" -lG4modeling -lG4OpenGL -lG4parmodels -lG4particles -lG4persistency $LIBS" 
LIBS=" -lG4processes -lG4RayTracer -lG4readout -lG4run -lG4track $LIBS" 
LIBS=" -lG4tracking -lG4Tree -lG4visHepRep -lG4vis_management -lG4visXXX $LIBS" 
LIBS=" -lG4VRML $LIBS"


echo "host               = $host_os "
echo "CXX                = $CXX"
echo "CXXFLAGS           = $CXXFLAGS"
echo "INCLUDES           = $INCLUDES"
echo "LIBS               = $LIBS"
echo "LDFLAGS            = $LDFLAGS"
echo ""


AC_CONFIG_FILES([
	Makefile 
	base/Makefile
	physics/Makefile
	geom/Makefile
	next/Makefile
	bin/Makefile
	])
AC_OUTPUT
