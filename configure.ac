# Autoconf file for Pana
AC_PREREQ([2.66])
AC_INIT([Pana], [1.5], [pana.bunnies.net])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
						src/Makefile
						doc/Makefile
						doc/da/Makefile
						po/Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_ENABLE_SHARED(yes)

# KDE 
#KDE_SET_PREFIX
#KDE_PROG_LIBTOOL
#AM_KDE_WITH_NLS
#KDE_USE_QT(3.3)
#AC_PATH_KDE
#KDE_INIT_DOXYGEN([The API Reference], [Version $VERSION])
#CXXFLAGS="$CXXFLAGS $KDE_DEFAULT_CXXFLAGS"
#KDE_CHECK_TYPES



# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h limits.h memory.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/mount.h sys/param.h sys/socket.h sys/statvfs.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([alarm atexit dup2 fdatasync floor ftruncate getcwd gethostname gettimeofday localtime_r memmove memset mkdir munmap pow putenv realpath rint rmdir select setenv socket sqrt strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol])

AC_OUTPUT
