include config.mk

SUBDIRS = \
	help2man autoconf automake libtool pkg-config yasm cmake \
	pcre expat gettext readline sqlite3 \
	liblzo liblzo2 freetype2 fontconfig \
	openssl libssh2 curl \
	jpeg tiff libpng libogg \
	libvorbis libmad fribidi libwavpack libmpeg2 \
	faad2 libcdio libsamplerate librtmp \
	libvorbis libflac libmad fribidi libwavpack libmpeg2 \
	faad2 libcdio libsamplerate librtmp \
	libGLEW libsdl libsdl_mixer libsdl_image \
	mysqlclient boost \

.PHONY: setup $(SUBDIRS)

all: setup $(SUBDIRS)

setup:
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/slib
	mkdir -p $(PREFIX)/share
	mkdir -p $(PREFIX)/include

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done

distclean::
	for d in $(SUBDIRS); do $(MAKE) -C $$d distclean; done
	rm -rf $(BUILDDIR)
