AC_PREREQ([2.71])
AC_INIT([rakarrack],[0.6.2],[Josep Andreu holborn@telefonica.net]) 
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.C])
AC_CONFIG_HEADERS([src/config.h])

AC_ARG_ENABLE(optimizations,
AS_HELP_STRING([--enable-optimizations[=yes/no]],[enable optimizations (default=yes)]),
[ac_optimizations="$enableval"], [ac_optimizations="yes"])

if test "x$ac_optimizations" = "xyes"; then
OPTS=""
else
OPTS="no"
fi

AC_ARG_ENABLE(vectorize,
AS_HELP_STRING([--enable-vectorize[=yes/no]],[enable vectorize (default=yes)]),
[ac_vectorize="$enableval"], [ac_vectorize="yes"])

if test "x$ac_vectorize" = "xyes"; then
VECTOR="-ftree-vectorize -fvect-cost-model"
else
ac_vectorize="no"
fi


AC_ARG_ENABLE(sse,
AS_HELP_STRING([--enable-sse],[enable sse (default=automatic check)]),
[ac_sse="$enableval"])

if test "x$ac_sse" = "xyes"; then
SSE="-msse -mfpmath=sse"
else
ac_sse="no"
fi

AC_ARG_ENABLE(sse2,
AS_HELP_STRING([--enable-sse2],[enable sse2 (default=automatic check)]),
[ac_sse2="$enableval"])

if test "x$ac_sse2" = "xyes"; then
SSE="-msse2 -mfpmath=sse"
else 
ac_sse2="no"
fi

AC_ARG_ENABLE(altivec,
AS_HELP_STRING([--enable-altivec],[enable altivec (default=automatic check)]),
[ac_altivec="$enableval"])

if test "x$ac_altivec" = "xyes"; then
ALTIVEC="-maltivec"
else
ac_altivec="no"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_CHECK_PROG([FLTKCONFIG], [fltk-config], [fltk-config])
if test -z "$FLTKCONFIG"; then
  AC_MSG_ERROR([fltk not installed!])
else
  ac_fltk="yes"
fi
AC_CHECK_PROG([ACONNECT],[aconnect],[aconnect])
AC_DEFINE_UNQUOTED(ACONNECT,"$ACONNECT",[ACONNECT])
AC_DEFINE_UNQUOTED(ENABLE_MIDI,1,[ENABLE_MIDI])

# Checks gcc version

GCC_VERSION=`$CC -dumpversion | cut -f 2 -d "."`
if test "$GCC_VERSION" = "5"; then
ADDS="-fvar-tracking-assignments-toggle -fexcess-precision=fast"
else
ADDS=""
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([floor memset memmove pow select sqrt strdup strstr])





# Checks for libraries.

AC_CHECK_LIB([X11], [XOpenDisplay],[ac_X11="yes"],[ac_X11="no"])
if test "x$ac_X11" = "xyes"; then
   LIBS="$LIBS -lX11"
fi


AC_CHECK_LIB([Xft], [main], [ac_libXft="yes"], [ac_libXft="no"])
if test "x$ac_libXft" = "xyes"; then
   LIBS="$LIBS -lXft"
fi

AC_CHECK_LIB([Xrender], [main], [ac_Xrender="yes"], [ac_Xrender="no"])
if test "x$ac_Xrender" = "xyes"; then
   LIBS="$LIBS -lXrender"

fi


AC_CHECK_LIB([dl], [main], [ac_dl="yes"], [ac_dl="no"])
if test "x$ac_dl" = "xno"; then
AC_MSG_ERROR([dl library not found.])
else 
   LIBS="$LIBS -ldl"

fi

AC_CHECK_LIB([Xpm],[main], [ac_Xpm="yes"], [ac_Xpm="no"])
if test "x$ac_Xpm" = "xno"; then
AC_MSG_ERROR([Xpm library not found.])
else 
   LIBS="$LIBS -lXpm"

fi

AC_CHECK_LIB([fontconfig], [main], [ac_fontconfig="yes"], [ac_fontconfig="no"])
if test "x$ac_fontconfig" = "xno"; then
AC_MSG_ERROR([fontconfig library not found.])
else 
   LIBS="$LIBS -lfontconfig"

fi

AC_CHECK_LIB([freetype], [main], [ac_freetype="yes"], [ac_freetype="no"])
if test "x$ac_freetype" = "xno"; then
AC_MSG_ERROR([freetype library not found.])
else 
   LIBS="$LIBS -lfreetype"

fi

AC_CHECK_LIB([m], [main], [ac_m="yes"], [ac_m="no"])
if test "x$ac_m" = "xno"; then
AC_MSG_ERROR([Math library not found.])
else 
   LIBS="$LIBS -lm"

fi

AC_CHECK_LIB([pthread], [main], [ac_pthread="yes"], [ac_pthread="no"] )
if test "x$ac_pthread" = "xno"; then
AC_MSG_ERROR([pthread library not found.])
else 
   LIBS="$LIBS -lpthread"
fi

AC_CHECK_LIB([rt], [main], [ac_rt="yes"], [ac_rt="no"])
if test "x$ac_rt" = "xyes"; then
   LIBS="$LIBS -lrt"

fi

AC_CHECK_LIB([z], [main], [ac_z="yes"], [ac_zm="no"])
if test "x$ac_z" = "xno"; then
AC_MSG_ERROR([z library not found.])
else 
   LIBS="$LIBS -lz"

fi

AC_CHECK_LIB([asound], [main], [ac_asound="yes"], [ac_asound="no"])
if test "x$ac_asound" = "xyes"; then
   LIBS="$LIBS -lasound"

fi

AC_CHECK_LIB([jack],main, [ac_jack="yes"], [ac_jack="no"])
if test "x$ac_jack" = "xyes"; then
   LIBS="$LIBS -ljack"
fi

AC_CHECK_LIB([jack64],main, [ac_jack64="yes"], [ac_jack64="no"])
if test "x$ac_jack64" = "xyes"; then
   LIBS="$LIBS -ljack64"
fi

AC_CHECK_LIB([sndfile],sf_open, [ac_sndfile="yes"], [ac_sndfile="no"])
if test "x$ac_sndfile" = "xno"; then
AC_MSG_ERROR([sndfile library not found.])
else 
   LIBS="$LIBS -lsndfile"

fi

AC_CHECK_LIB([samplerate],src_simple, [ac_samplerate="yes"], [ac_samplerate="no"])
if test "x$ac_samplerate" = "xno"; then
AC_MSG_ERROR([samplerate library not found.])
else 
   LIBS="$LIBS -lsamplerate"

fi


AC_CHECK_LIB([fftw3], [main], [ac_fftw3="yes"], [ac_fftw3="no"])
if test "x$ac_m" = "xno"; then
AC_MSG_ERROR([fftw library not found.])
else 
   LIBS="$LIBS -lfftw3"
fi

if test "x$prefix" = "xNONE"; then
AC_DEFINE_UNQUOTED(PREFIX, "/usr/local", [Install prefix])
else
AC_DEFINE_UNQUOTED(PREFIX, "$prefix", [Install prefix])
fi
AC_HEADER_DIRENT

AC_CHECK_HEADERS([fcntl.h libintl.h stdlib.h string.h sys/ioctl.h unistd.h sndfile.h samplerate.h fftw3.h X11/xpm.h])
#AC_CHECK_HEADER(alsa/asoundlib.h, true, AC_MSG_ERROR([Alsa is required]))
AC_SUBST(SAMPLERATE_CFLAG)
AC_SUBST(SAMPLERATE_LIBS)


if test "x$prefix" = "xNONE"; then
prefix=/usr/local
fi



AC_ARG_ENABLE(datadir,
AS_HELP_STRING([--enable-datadir[=yes/no]],[use datadir (default=no)]),
[ac_dd="$enableval"], [ac_dd="no"])

if test "x$ac_dd" = "xyes"; then
DATA_DIR="$datadir"
else
DATA_DIR="$prefix/share/${PACKAGE}"
fi


AC_ARG_ENABLE(docdir,
AS_HELP_STRING([--enable-docdir[=yes/no]],[use docdir (default=no)]),
[ac_hd="$enableval"], [ac_hd="no"])

if test "x$ac_hd" = "xyes"; then
HELPDIR="$docdir"
else
HELPDIR="$prefix/share/doc/${PACKAGE}"
fi

AC_DEFINE_UNQUOTED(DATA_DIR,"$DATA_DIR",[DATA_DIR])
AC_DEFINE_UNQUOTED(HELPDIR,"$HELPDIR",[HELPDIR])
AC_DEFINE([WEBSITE],["rakarrack.sf.net"],[WEBSITE])

if test -z "$OPTS";then
if test -z "$ALTIVEC";then
if test -z "$SSE"; then
SSE=$(cat /proc/cpuinfo | grep sse2)
if test -z "$SSE"; then
SSE=""
ac_sse2="no"
else
SSE="-msse2 -mfpmath=sse"
ac_sse2="yes"
fi
if test -z "$SSE"; then
SSE=$(cat /proc/cpuinfo | grep sse)
if test -z "$SSE"; then
SSE=""
ac_sse="no"
else
SSE="-msse -mfpmath=sse"
ac_sse="yes"
fi
fi
fi
fi

if test -z "$ALTIVEC"; then
ALTIVEC=$(cat /proc/cpuinfo | grep altivec)
if test -z "$ALTIVEC"; then
ALTIVEC=""
ac_altivec="no"
else
ALTIVEC="-maltivec"
ac_altivec="yes"
fi
fi
fi

LIBS="`$FLTKCONFIG --use-images --ldflags` $LIBS"
CXXFLAGS="-Wall -Werror -Wfatal-errors -Wno-warn_unused_result -Wno-unused-result $SSE $ALTIVEC -ffast-math $VECTOR -pipe $ADDS -fsigned-char `$FLTKCONFIG --use-images --cxxflags` `pkg-config --cflags sndfile` `pkg-config --cflags jack` $CXXFLAGS"
AC_CONFIG_FILES([Makefile src/Makefile man/Makefile data/Makefile icons/Makefile doc/Makefile doc/help/Makefile doc/help/imagenes/Makefile doc/help/css/Makefile extra/Makefile])
AC_OUTPUT

echo
echo  " $PACKAGE_NAME $PACKAGE_VERSION"
echo
echo
echo  " Optimizations__________________________________________________"
echo  " Vectorization. . . . . . . . . . . . . . . . . . . . . .: $ac_vectorize"
echo  " Use altivec. . . . . . . . . . . . . . . . . . . . . . .: $ac_altivec"
echo  " Use sse  . . . . . . . . . . . . . . . . . . . . . . . .: $ac_sse"
echo  " Use sse2 . . . . . . . . . . . . . . . . . . . . . . . .: $ac_sse2"
echo  
echo  " Library________________________________________________________"
echo  " X11 support  . . . . . . . . . . . . . . . . . . . . . .: $ac_X11"
echo  " Xpm support (libxpm) . . . . . . . . . . . . . . . . . .: $ac_Xpm"
echo  " Xft support  . . . . . . . . . . . . . . . . . . . . . .: $ac_libXft"
echo  " Xrender support  . . . . . . . . . . . . . . . . . . . .: $ac_Xrender"
echo  " Fontconfig support . . . . . . . . . . . . . . . . . . .: $ac_fontconfig"
echo  " Freetype support . . . . . . . . . . . . . . . . . . . .: $ac_freetype"
echo  " FLTK toolkit . . . . . . . . . . . . . . . . . . . . . .: $ac_fltk"
echo  " pthread support (libpthread) . . . . . . . . . . . . . .: $ac_pthread"
echo  " JACK Audio Connection Kit library  . . . . . . . . . . .: $ac_jack"
echo  " ALSA MIDI Sequencer support  . . . . . . . . . . . . . .: $ac_asound"
echo  " Samplerate conversion (libsamplerate). . . . . . . . . .: $ac_samplerate"
echo  " General audio file (libsndfile)  . . . . . . . . . . . .: $ac_sndfile"
echo  " Fast Fourier Transform (fftw3)   . . . . . . . . . . . .: $ac_fftw3"
echo  
echo  " Now maybe you can run 'make' to build the sources."
echo
echo

