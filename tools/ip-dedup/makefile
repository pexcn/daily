# compat bits for combined GNU/BSD make targets, do not use them in this file
S    := ${.CURDIR}
O     = .
O_OBJ = $(O)/src
# ---

PHONY =

PHONY += all
all: ip-dedup

.include "mk/install_vars.mk"
.include "mk/prj.mk"

.if ${STANDALONE} == 1
.include "mk/datadir_standalone.mk"
.else
.include "mk/datadir_install.mk"
.endif

.if !defined(IPDEDUP_DATADIR) || ${IPDEDUP_DATADIR} == ""
CC_OPTS_EXTRA += -UIPDEDUP_DATADIR
.else
CC_OPTS_EXTRA += -DIPDEDUP_DATADIR=\"$(IPDEDUP_DATADIR)\"
.endif

.include "mk/warnflags_base.mk"

.if ${NO_WERROR} != 1
.include "mk/warnflags_werror.mk"
.endif

.if ${STATIC} == 1
.include "mk/static.mk"
.endif

.if ${HARDEN} == 1
.include "mk/c_harden.mk"
.endif

.include "mk/compile_c.mk"
.include "mk/obj_defs.mk"

.include "mk/common_targets.mk"


ODEP_IP_DEDUP := ${OBUNDLE_APP_IP_DEDUP:%=src/%.o}

.SUFFIXES: .c .o
.c.o:
	${MKDIRP} -- ${@D}
	${COMPILE_C} ${.IMPSRC} -o ${@}

ip-dedup: ${ODEP_IP_DEDUP}
	$(LINK_O) ${.ALLSRC} -o ${@}

ip-dedup.bashcomp: ${S_FILES}/ip-dedup.bashcomp.in
	< ${S_FILES}/ip-dedup.bashcomp.in ${RUN_SED_EXPRV} > ${@}

.PHONY: ${PHONY}
