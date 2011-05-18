#  A quick and dirty Makefile to download/build and install
#
#  Usage:
#    make
#    sudo make install

include ../config.mk

LIBNAME=boost
VERSION=1_44_0
SOURCE=$(LIBNAME)_$(VERSION)
# download location and format
BASE_URL=http://mirrors.xbmc.org/build-deps/darwin-libs

RETRIEVE_TOOL=/usr/bin/curl
RETRIEVE_TOOL_FLAGS=-Ls --create-dirs --output $(TARBALLS_LOCATION)/$(ARCHIVE)
ARCHIVE=$(SOURCE).tar.bz2
ARCHIVE_TOOL=tar
ARCHIVE_TOOL_FLAGS=xf

LIBDYLIB=$(PREFIX)/lib/libboost_thread.dylib

all: $(LIBDYLIB)

$(TARBALLS_LOCATION)/$(ARCHIVE):
	$(RETRIEVE_TOOL) $(RETRIEVE_TOOL_FLAGS) $(BASE_URL)/$(ARCHIVE)

$(LIBDYLIB): $(TARBALLS_LOCATION)/$(ARCHIVE)
	rm -rf $(SOURCE)
	$(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	echo $(SOURCE) > .gitignore
	cat user-config.jam-osx-10.4_i386.in >> $(SOURCE)/tools/build/v2/user-config.jam
	cd $(SOURCE); echo $(PREFIX) | sed 's/\//\\\\\//g' | xargs -I '%' sed -i '' -e's/-install_name "/-install_name "%\/lib\//' tools/build/v2/tools/darwin.jam
	cd $(SOURCE); ./bootstrap.sh --prefix=$(PREFIX) --with-libraries=thread,system
	cd $(SOURCE); ./bjam --prefix=$(PREFIX) toolset=darwin-4.0 --architecture=x86 address-model=32 target-os=darwin macosx-version=${SDK_VERSION} macosx-version-min=${SDK_VERSION} --link=static install

clean:
	cd $(SOURCE); ./bjam --clean
	rm -rf $(PREFIX)/include/boost
	rm -f  $(PREFIX)/lib/libboost*
	rm -f .installed

distclean::
	rm -rf $(PREFIX)/include/boost
	rm -f  $(PREFIX)/lib/libboost*
	rm -rf $(SOURCE) .installed

