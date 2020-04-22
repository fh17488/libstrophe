LIBSTROPHE_URL := https://github.com/fh17488/libstrophe/archive/master.tar.gz

PKGS+=libstrophe

$(TARBALLS)/libstrophe.tar.gz:
	$(call download,$(LIBSTROPHE_URL))

.sum-libstrophe: libstrophe.tar.gz
	touch $@

libstrophe: libstrophe.tar.gz 
	$(UNPACK)
	mv $@-master $@

.libstrophe: libstrophe .sum-libstrophe
	cd $< && $(HOSTVARS) ./bootstrap.sh
	cd $< && $(HOSTVARS) ./configure
	cd $< && $(MAKE) 
	touch $@
