CC ?= gcc
DESTDIR ?= /usr/local
SRC ?= ipv4-merger.c
BIN ?= ipv4-merger

ipv4-merger: $(SRC)
	$(CC) -O3 -ffast-math $^ -o $@

install: $(BIN)
	install -d $(DESTDIR)/bin/
	install $(BIN) $(DESTDIR)/bin/

uninstall:
	rm -f $(DESTDIR)/bin/$(BIN)

clean:
	rm -vf *.o $(BIN)
