PHONY += man
man: $(S)/doc/man/ip-dedup.1

# read version from version.h, prereq for sed-editing doc files
$(O)/VERSION: $(SRC)/app/ip-dedup/version.h | $(O)
	{ \
		set -e; \
		\
		unset -v IPDEDUP_VER_MAJOR; \
		unset -v IPDEDUP_VER_MINOR; \
		unset -v IPDEDUP_VER_SUFFIX; \
		\
		while read -r a b c; do \
			[ "$${a}" = '#define' ] || continue; \
			case "$${b}" in \
				IPDEDUP_VER_MAJOR) IPDEDUP_VER_MAJOR="$${c}" ;; \
				IPDEDUP_VER_MINOR) IPDEDUP_VER_MINOR="$${c}" ;; \
				IPDEDUP_VER_SUFFIX) \
					if [ "$${c}" = "NULL" ]; then \
						IPDEDUP_VER_SUFFIX=''; \
					else \
						c="$${c#\"}"; c="$${c%\"}"; \
						IPDEDUP_VER_SUFFIX="$${c}"; \
					fi; \
				;; \
			esac; \
		done < $(<); \
		\
		: "$${IPDEDUP_VER_MAJOR:?}"; \
		: "$${IPDEDUP_VER_MINOR:?}"; \
		: "$${IPDEDUP_VER_SUFFIX?}"; \
		\
		> $@ printf '%d.%d%s\n' \
			"$${IPDEDUP_VER_MAJOR}" \
			"$${IPDEDUP_VER_MINOR}" \
			"$${IPDEDUP_VER_SUFFIX}"; \
	}

# sed-edit expressions for doc files
$(O)/sed_edit_doc: $(O)/VERSION | $(O)
	{ set -e; \
		read ver < $(<); \
		\
		printf 's|@%s@|%s|g\n' DATADIR '$(IPDEDUP_DATADIR)'; \
		printf 's|@%s@|%s|g\n' VERSION "$${ver}"; \
	} > $@

$(S)/doc/man:
	$(MKDIRP) -- $(@)

$(S)/doc/man/%: $(S)/doc/src/man/%.md $(O)/sed_edit_doc | $(S)/doc/man
	sed -r -f $(O)/sed_edit_doc $(<) | pandoc -s -t man > $@
