AC_INIT(xo, 0.0.0-develop)
AM_INIT_AUTOMAKE(foreign)

AC_CHECK_LIB([bdd], [bdd_init], [], [AC_MSG_ERROR([bdd not found!])])

AC_PROG_CC_C99
AC_PROG_CXX

AC_CONFIG_FILES(Makefile src/Makefile tests/Makefile)
AC_OUTPUT
