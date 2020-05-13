SED_EXPRV =
SED_EXPRV += 's,@@IPDEDUP_DATADIR@@,$(IPDEDUP_DATADIR),g'
RUN_SED_EXPRV = sed -r $(SED_EXPRV)

PHONY += clean
clean:
	test ! -d '$(O_OBJ)' || find '$(O_OBJ)' -type f -name '*.o' -delete
	test ! -d '$(O_OBJ)' || find '$(O_OBJ)' -depth -type d -empty -delete
	test ! -f '$(O)/ip-dedup' || rm -- '$(O)/ip-dedup'
	test ! -f '$(O)/ip-dedup.bashcomp' || rm -- '$(O)/ip-dedup.bashcomp'

PHONY += install
install: install-bin install-data install-man

PHONY += install-bin
install-bin:
	$(DOEXE) -- $(O)/ip-dedup $(DESTDIR)$(BINDIR)/ip-dedup

PHONY += install-data
install-data: install-data-ip4 install-data-ip6

PHONY += install-data-ip4
install-data-ip4:
	$(DOINS) -- $(S_IPLIST)/inet/doc $(DESTDIR)$(IPDEDUP_DATADIR)/inet/doc
	$(DOINS) -- $(S_IPLIST)/inet/link-local $(DESTDIR)$(IPDEDUP_DATADIR)/inet/link-local
	$(DOINS) -- $(S_IPLIST)/inet/lo $(DESTDIR)$(IPDEDUP_DATADIR)/inet/lo
	$(DOINS) -- $(S_IPLIST)/inet/martians $(DESTDIR)$(IPDEDUP_DATADIR)/inet/martians
	$(DOINS) -- $(S_IPLIST)/inet/priv $(DESTDIR)$(IPDEDUP_DATADIR)/inet/priv

PHONY += install-data-ip6
install-data-ip6:
	$(DOINS) -- $(S_IPLIST)/inet6/doc $(DESTDIR)$(IPDEDUP_DATADIR)/inet6/doc
	$(DOINS) -- $(S_IPLIST)/inet6/link-local $(DESTDIR)$(IPDEDUP_DATADIR)/inet6/link-local
	$(DOINS) -- $(S_IPLIST)/inet6/lo $(DESTDIR)$(IPDEDUP_DATADIR)/inet6/lo
	$(DOINS) -- $(S_IPLIST)/inet6/martians $(DESTDIR)$(IPDEDUP_DATADIR)/inet6/martians
	$(DOINS) -- $(S_IPLIST)/inet6/ula $(DESTDIR)$(IPDEDUP_DATADIR)/inet6/ula

PHONY += install-man
install-man:
	$(DOINS) -- $(S)/doc/man/ip-dedup.1 $(DESTDIR)$(MANDIR)/man1/ip-dedup.1

PHONY += bashcomp
bashcomp: ip-dedup.bashcomp

PHONY += install-bashcomp
install-bashcomp:
	$(DOINS) -- $(O)/ip-dedup.bashcomp $(DESTDIR)$(BASHCOMPDIR)/ip-dedup
