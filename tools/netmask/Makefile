netmask: netmask.c errors.c
	gcc $^ -o $@ -DPACKAGE=\"netmask\" -DVERSION=\"2.3.7\" -DSTDC_HEADERS=1 -DHAVE_SYSLOG_H=1 -DHAVE_VPRINTF=1 -DHAVE_STRERROR=1 -DHAVE_STRTOUL=1

install: netmask
	install -d $(TRAVIS_BUILD_DIR)/bin/
	install netmask $(TRAVIS_BUILD_DIR)/bin/

clean:
	rm -vf *.o netmask
