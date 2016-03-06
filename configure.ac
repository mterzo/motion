#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([motion], esyscmd(['./version.sh']),
        [https://github.com/mterzo/motion/issues])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

AC_MSG_CHECKING(for Darwin)
Darwin=`uname -a | grep "Darwin"`
if test "${Darwin}" = ""; then
    AC_MSG_RESULT(no)
else
    AC_MSG_RESULT($Darwin)
fi

AC_MSG_CHECKING(for *Bsd)
FreeBSD=`uname -a | grep "BSD"`
if test "${FreeBSD}" = ""; then
    AC_MSG_RESULT(no)
else
    AC_MSG_RESULT($FreeBSD)
fi

AM_CONDITIONAL([BSD_VIDEO], [test x$FreeBSD != x"" -a x$Darwin != x""])

if test "${Darwin}" = ""; then

    if test "${FreeBSD}" = ""; then
        AC_MSG_RESULT(no)
        VIDEO="video.o video2.o video_common.o"
    else
        AC_MSG_RESULT(yes)
        if test "${LINUXTHREADS}" = "no"; then
            AC_MSG_CHECKING(Linuxthreads)
            AC_MSG_RESULT(skipping)
        else
            THREAD_CHECK="/usr/local/include/pthread/linuxthreads/pthread.h"
            THREAD_LIB_CHECK="/usr/local/lib/liblthread.so"
        fi

        if test "${PWCBSD}" != "no"; then
            VIDEO="video.o video2.o video_common.o"
            TEMP_CFLAGS="${CFLAGS} -I/usr/local/include -DPWCBSD"
        else
            VIDEO="video_freebsd.o"
            TEMP_CFLAGS="${CFLAGS} -I/usr/local/include"
        fi

        TEMP_LDFLAGS="${LDFLAGS} -L/usr/local/lib"
        TEMP_LIBS="-L/usr/local/lib"

    fi
else
    TEMP_CFLAGS="${CFLAGS} -I/sw/include"
    TEMP_LDFLAGS="${LDFLAGS} -L/sw/lib"
    TEMP_LIBS="-L/sw/lib"
    VIDEO="video_freebsd.o"
    FINK_LIB="-L/sw/lib"
    Darwin="yes"
    V4L="no"
    AC_MSG_RESULT($Darwin)
fi

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

#
# Check for sdl library
#
SDL_SUPPORT="no"
AC_ARG_WITH(sdl,
[  --without-sdl           Compile without sdl support to get stream in SDL window.
],
[],
[])
AC_MSG_CHECKING(for sdl)
if test "x$withval" = "xno"; then
    AC_MSG_RESULT(skipped)
else
    if test "${FreeBSD}" != ""; then
        CONFIG_SDL='sdl11-config'
    else
        CONFIG_SDL='sdl-config'
    fi
    if test -z "`($CONFIG_SDL --version) 2>/dev/null`" ;then
        AC_MSG_RESULT(no)
        if test "$withval" = "yes"; then
            echo ""
            echo "****************************************************"
            echo "* sdl-config could not be found. Please install it *"
            echo "* and remove the --with-sdl configure argument.    *"
            echo "* libSDL can be found at http://www.libsdl.org     *"
            echo "****************************************************"
            echo ""
        fi
    else
        AC_MSG_RESULT(yes)
        SDL_SUPPORT="yes"
        TEMP_LIBS="$TEMP_LIBS `${CONFIG_SDL} --libs`"
        TEMP_CFLAGS="${TEMP_CFLAGS} `${CONFIG_SDL} --cflags`"
        AC_DEFINE([HAVE_SDL],1,[Define to 1 if you have SDL support])
        SDL_OBJ="sdl.o"
        AC_SUBST(SDL_OBJ)
    fi
fi
AM_CONDITIONAL([HAVE_SDL], [ test x$SDL_SUPPORT = xyes])

if test x$JPEG_SUPPORT != xyes ; then
  # Checks for libraries
  JPEG_LIBS="-ljpeg"
  AC_CHECK_LIB(jpeg, jpeg_set_defaults, [
        AC_SUBST(JPEG_LIBS)
        JPEG_SUPPORT="yes"
    ], [
        AC_MSG_ERROR([Could not find jpeg library])
    ]
  )
fi

#
# Check for libavcodec and libavformat from ffmpeg
#

FFMPEG_DIR="yes"
FFMPEG_OK="no_found"
FFMPEG_OBJ=""
AC_ARG_WITH(ffmpeg,
[  --with-ffmpeg[=DIR]       Specify the prefix for the install path for
                          libavcodec/libavformat (part of ffmpeg) be able to
                          encode mpeg movies realtime.
                          If this is not specified motion will try to find
                          the libraries in /usr and /usr/local.
                          ],
FFMPEG_DIR="$withval"
)

#
# ffmpeg headers custom location
#
FFMPEG_HEADERS_DIR="yes"
AC_ARG_WITH(ffmpeg_headers,
[  --with-ffmpeg-headers[=DIR] Specify the prefix for ffmpeg headers.
                               ],
FFMPEG_HEADERS_DIR="$withval"
)

#
# ffmpeg custom extra libraries
#
FFMPEG_EXTRALIBS="avformat avcodec avutil m z swscale "
AC_ARG_WITH(ffmpeg-libs,
[  --with-ffmpeg-libs[=libs]  Specify the extra libs for ffmpeg
                               ],
FFMPEG_EXTRALIBS="$withval"
)

#
# --without-ffmpeg or with-ffmpeg=no
#
if test "${FFMPEG_DIR}" = "no"; then
    AC_MSG_CHECKING(for ffmpeg)
    AC_MSG_RESULT(skipping)
elif test "${FFMPEG_DIR}" = "yes"; then
    PKG_CHECK_MODULES([LIBAVCODEC], [libavcodec >= 54])
    PKG_CHECK_MODULES([LIBAVFORMAT], [libavformat >= 54])
else
   echo "todo"
fi

#
# sqllite support
#

SQLITE3_SUPPORT="no"
AC_ARG_WITH(sqlite, [--with-sqlite[=dir] Enable sqlite database support],
    [
        if test x${witharg} = x""; then
            PKG_CHECK_MODULES([SQLITE3], [sqlite3])
        else
            tmp_cflags=${CFLAGS}
            tmp_ldflags=${LDFLAGS}
            CFLAGS += "-I${withval}/include"
            LDFLAGS += "-L${withval}/lib"
            AC_CHECK_LIB([SQLITE3],[sqlite3_version][],,
                AC_MSG_ERROR([Could not find sqlite3 library]))
            CFLAGS=$tmp_cflags
            LDFLAGS=$tmp_ldflags
        fi
    ])


# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([bzero dup2 getcwd gethostbyname gettimeofday localtime_r memchr memmove memset mkdir munmap putenv regcomp select setenv socket strcasecmp strchr strdup strerror strncasecmp strstr uname])


SDL_SUPPORT="no"
AC_CHECK_LIB([pcap],[pcap_create])
AC_CHECK_LIB(pthread, pthread_create)

AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile])
AC_OUTPUT

echo ""
echo "   **************************"
echo "      Configure status       "
echo "      ${PACKAGE_NAME} ${PACKAGE_VERSION}"
echo "   **************************"
echo

if test "${Darwin}" != ""; then
    echo "OS             :     Darwin"
elif test "${FreeBSD}" != ""; then
    echo "OS             :     *BSD"
else
    echo "OS             :     Linux"
fi

if test "${JPEG_SUPPORT_TURBO}" = "yes"; then
    echo "JPEG turbo support:  Yes"
elif test "${JPEG_SUPPORT_MMX}" = "yes"; then
    echo "JPEG-mmx support:    Yes"
elif test "${JPEG_SUPPORT}" = "yes"; then
    echo "JPEG support:        Yes"
else
    AC_MSG_ERROR([Could not find jpeg library])
fi

if test "${SDL_SUPPORT}" = "yes"; then
    echo "SDL support:         Yes"
else
    echo "SDL support:         No"
fi

if test x${LIBAVFORMAT_LIBS} != x""; then
    echo "FFmpeg support:      Yes"
else
    echo "FFmpeg support:      No"
fi

if test x${SQLITE3_LIBS} != x""; then
    echo "SQLite support:      Yes"
else
    echo "SQLite support:      No"
fi

echo
echo "CFLAGS: $CFLAGS"
echo
echo "Install prefix:       $prefix"
echo
