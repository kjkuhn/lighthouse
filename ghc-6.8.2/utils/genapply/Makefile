TOP=../..
include $(TOP)/mk/boilerplate.mk

HS_PROG = $(GHC_GENAPPLY_PGM)

# genapply is needed to boot in rts/...
ifneq "$(BootingFromHc)" "YES"
boot :: all
endif

ifeq "$(GhcUnregisterised)" "YES"
SRC_HC_OPTS += -DNO_REGS
endif

ifeq "$(ghc_ge_607)" "YES"
SRC_HC_OPTS += -package pretty
endif

# Try to get dependencies right...
SRC_HC_OPTS += -no-recomp
GenApply.o : $(GHC_INCLUDE_DIR)/ghcconfig.h
GenApply.o : $(GHC_INCLUDE_DIR)/MachRegs.h
GenApply.o : $(GHC_INCLUDE_DIR)/Constants.h

include $(TOP)/mk/target.mk
