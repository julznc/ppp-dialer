# bitbake recipe to build ppp-dialer program

DESCRIPTION = "Point-to-Point Protocol dialer (modified WvDial and WvStreams)"

LICENSE = "LGPLv2.1"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/LGPL-2.1;md5=1a6d268fd218675ffea8be556788b780"

inherit autotools

RDEPENDS_${PN} = "ppp"

# replaces old wvdial package
RPROVIDES_${PN} += "wvdial wvdialconf"

SRC_URI = "git://github.com/julznc/ppp-dialer.git;branch=master"

SRCREV = "${AUTOREV}"
