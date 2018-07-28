CC ?= gcc
DESTDIR ?= /usr/local
SRC ?= netmask.c errors.c
BIN ?= netmask

netmask: $(SRC)
	$(CC) -O3 -ffast-math $^ -o $@ -DPACKAGE=\"netmask\" -DVERSION=\"2.3.7\" -DSTDC_HEADERS=1 -DHAVE_SYSLOG_H=1 -DHAVE_VPRINTF=1 -DHAVE_STRERROR=1 -DHAVE_STRTOUL=1

install: $(BIN)
	install -d $(DESTDIR)/bin/
	install $(BIN) $(DESTDIR)/bin/

uninstall:
	rm -f $(DESTDIR)/bin/$(BIN)

clean:
	rm -vf *.o $(BIN)
