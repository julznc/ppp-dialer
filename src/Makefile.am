
AM_CPPFLAGS = -Wall \
	-I${top_srcdir}/deps/wvstreams/include

bin_PROGRAMS = wvdial wvdialconf

wvdial_SOURCES = \
	wvdial.cc \
	wvdialer.cc \
	wvdialbrain.cc \
	wvdialmon.cc \
	wvpapchap.cc

wvdialconf_SOURCES = \
	wvdialconf.cc \
	wvmodemscan.cc

WVSTREAMLIBS = \
	${top_builddir}/deps/wvstreams/libwvstreams.a \
	${top_builddir}/deps/wvstreams/argp/libargp.a

wvdial_LDADD = $(WVSTREAMLIBS)

wvdialconf_LDADD = $(WVSTREAMLIBS)

PPPPEERSDIR = /etc/ppp/peers

install-data-hook:
	[ -d ${PPPPEERSDIR}      ] || install -d ${PPPPEERSDIR}
	cp ${srcdir}/ppp.provider ${PPPPEERSDIR}/wvdial
	cp ${srcdir}/ppp.provider-pipe ${PPPPEERSDIR}/wvdial-pipe

uninstall-hook:
	rm -f ${PPPPEERSDIR}/wvdial
	rm -f ${PPPPEERSDIR}/wvdial-pipe
