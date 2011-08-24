include ../Makefile.include
include ../config.site.mk

LIBNAME=boost
VERSION=1_47_0
LIBVERSION=1.47.0
SOURCE=$(LIBNAME)_$(VERSION)
ARCHIVE=$(SOURCE).tar.bz2
BASE_URL=http://downloads.sourceforge.net/project/boost/boost/1.47.0

all: .installed

$(TARBALLS_LOCATION)/$(ARCHIVE):
	$(RETRIEVE_TOOL) $(RETRIEVE_TOOL_FLAGS) $(BASE_URL)/$(ARCHIVE)

.installed: $(TARBALLS_LOCATION)/$(ARCHIVE)
	rm -rf $(SOURCE)
	$(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	echo $(SOURCE) > .gitignore
	sed -e "s?@boost_flags@?$(boost_flags)?g" \
            -e "s?@platform_gcc_version@?$(platform_gcc_version)?g" \
            user-config.jam.in >> $(SOURCE)/tools/build/v2/user-config.jam
	cd $(SOURCE); echo $(PREFIX) | sed 's/\//\\\\\//g' | xargs -I '%' sed -i '' -e's/-install_name "/-install_name "%\/lib\//' tools/build/v2/tools/darwin.jam
	cd $(SOURCE); ./bootstrap.sh --prefix=$(PREFIX) --with-libraries=thread,date_time,system
	cd $(SOURCE); ./bjam --prefix=$(PREFIX) $(bjam_args) install
	# boost builds .so instead of .dylib for iphone, fix it.
	if test "$(DARWIN)" = "ios"; then \
		mv $(PREFIX)/lib/libboost_thread.so.$(LIBVERSION) $(PREFIX)/lib/libboost_thread.dylib; \
		rm $(PREFIX)/lib/libboost_thread.so; \
		mv $(PREFIX)/lib/libboost_date_time.so.$(LIBVERSION) $(PREFIX)/lib/libboost_date_time.dylib; \
		rm $(PREFIX)/lib/libboost_date_time.so; \
	fi
	install_name_tool -id $(PREFIX)/lib/libboost_thread.dylib $(PREFIX)/lib/libboost_thread.dylib
	install_name_tool -id $(PREFIX)/lib/libboost_date_time.dylib $(PREFIX)/lib/libboost_date_time.dylib
	touch .installed

clean:
	rm -rf $(SOURCE) .installed

distclean::
	rm -rf $(SOURCE) .installed

