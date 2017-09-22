
AM_CPPFLAGS = -Wall \
	-I${top_srcdir}/deps/wvstreams/include

bin_PROGRAMS = wvdial

wvdial_SOURCES = wvdial.cc

wvdial_LDADD = \
	${top_builddir}/deps/wvstreams/libwvbase.a \
	${top_builddir}/deps/wvstreams/utils/libwvutils.a \
	${top_builddir}/deps/wvstreams/uniconf/libuniconf.a \
	${top_builddir}/deps/wvstreams/streams/libwvstreams.a
