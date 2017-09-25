AM_CPPFLAGS = -DUNSTABLE -Wall \
	-I${srcdir}/include -I${srcdir}/argp

noinst_LIBRARIES =  libwvbase.a libwvutils.a libuniconf.a libwvstreams.a

libwvbase_a_SOURCES = \
	xplc-cxx/factory.cc \
	xplc-cxx/getiface.cc \
	xplc-cxx/strtouuid.cc \
	xplc-cxx/uuidtostr.cc \
	xplc-cxx/xplc.cc

libwvutils_a_SOURCES = \
	utils/wvargs.cc \
	utils/wvbuffer.cc \
	utils/wvbufferstore.cc \
	utils/wvcont.cc \
	utils/wverror.cc \
	utils/wvfork.cc \
	utils/wvhash.cc \
	utils/wvhashtable.cc \
	utils/wvlinklist.cc \
	utils/wvmoniker.cc \
	utils/wvregex.cc \
	utils/wvscatterhash.cc \
	utils/wvsorter.cc \
	utils/wvattrs.cc \
	utils/wvstring.cc \
	utils/wvstringlist.cc \
	utils/wvstringmask.cc \
	utils/strutils.cc \
	utils/wvtask.cc \
	utils/wvtimeutils.cc \
	utils/wvstreamsdebugger.cc \
	utils/wvbackslash.cc \
	utils/wvencoder.cc \
	utils/wvtclstring.cc \
	utils/wvstringcache.cc \
	utils/wvcrashbase.cc \
	utils/wvsubproc.cc

libuniconf_a_SOURCES = \
	uniconf/uniconf.cc \
	uniconf/uniconfgen.cc \
	uniconf/uniconfkey.cc \
	uniconf/uniconfroot.cc \
	uniconf/unihashtree.cc \
	uniconf/unimountgen.cc \
	uniconf/unitempgen.cc \
	uniconf/uniinigen.cc \
	uniconf/unigenhack.cc \
	uniconf/unilistiter.cc \
	uniconf/wvconfemu.cc

libwvstreams_a_SOURCES = \
	streams/wvlog.cc \
	streams/wvstream.cc \
	streams/wvsyslog.cc \
	streams/wvfdstream.cc \
	streams/wvistreamlist.cc \
	streams/wvfile.cc \
	streams/wvstreamclone.cc  \
	streams/wvconstream.cc \
	streams/wvpipe.cc \
	streams/wvmodem.cc \
	streams/wvlockdev.cc

