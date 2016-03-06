bin_PROGRAMS = test

test_SOURCES = motion_test.c

test_CFLAGS=-Dsysconfdir='"$(sysconfdir)"' @LIBAVCODEC_CFLAGS@           \
	@LIBAVFORMAT_CFLAGS@ @SQLITE3_CFLAGS@

test_LDADD=@JPEG_LIBS@ @LIBAVCODEC_LIBS@ @LIBAVFORMAT_LIBS@ @SQLITE3_LIBS@

TESTS = test
