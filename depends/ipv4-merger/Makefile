ipv4-merger: ipv4-merger.c
	gcc -O3 -ffast-math $^ -o $@

install: ipv4-merger
	install -d $(TRAVIS_BUILD_DIR)/bin/
	install ipv4-merger $(TRAVIS_BUILD_DIR)/bin/

clean:
	rm -vf *.o ipv4-merger
