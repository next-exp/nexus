#! /bin/sh

# Run this to generate all the auto-generated files needed by the GNU
# configure program

if test x$AUTOCONF = x; then
   AUTOCONF=/usr/bin/autoconf
   echo "set AUTOCONF to "$AUTOCONF
fi
if test x$AUTOHEADER = x; then
   AUTOHEADER=/usr/bin/autoheader
   echo "set AUTOHEADER to "$AUTOHEADER
fi

$AUTOHEADER
libtoolize --automake
aclocal
automake --add-missing --gnu
$AUTOCONF
