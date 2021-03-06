#################################################################################
#
#			    mk/boilerplate.mk
#
#		The Glorious GHC Boilerplate Makefile
#
# This one file should be included (directly or indirectly) by all Makefiles 
# in the GHC hierarchy.
#
#################################################################################

# We want to disable all the built-in rules that make uses; having them
# just slows things down, and we write all the rules ourselves.
# Setting .SUFFIXES to empty disables them all.
MAKEFLAGS += --no-builtin-rules

# GHC_TOP is the *relative* path to the fptools toplevel directory from the
# location where a project Makefile was invoked. It is set by looking at the
# current value of TOP.
#
GHC_TOP := $(TOP)

# $(FPTOOLS_TOP) is the old name for $(GHC_TOP), kept for backwards compat
FPTOOLS_TOP := $(TOP)


# This rule makes sure that "all" is the default target, regardless of where it appears
#		THIS RULE MUST REMAIN FIRST!
default: all

# -----------------------------------------------------------------------------
# Misc bits

# If $(way) is set then we define $(way_) and $(_way) from it in the
# obvious fashion.  This must be done before suffix.mk is included,
# because the pattern rules in that file depend on these variables.

ifneq "$(way)" ""
  way_ := $(way)_
  _way := _$(way)
endif


# When using $(patsubst ...) and friends, you can't use a literal comma
# freely - so we use ${comma} instead.  (See PACKAGE_CPP_OPTS in package.mk
# for an example usage.)
comma=,

# -----------------------------------------------------------------------------
# 	Now follow the pieces of boilerplate
#	The "-" signs tell make not to complain if they don't exist

include $(TOP)/mk/config.mk
# All configuration information
#	(generated by "configure" from config.mk.in)
#


include $(TOP)/mk/paths.mk
# Variables that say where things belong (e.g install directories)
# and where we are right now
# Also defines variables for standard files (SRCS, LIBS etc)


include $(TOP)/mk/opts.mk
# Variables that control the option flags for all the
# language processors

ifeq "$(BootingFromHc)" "YES"
include $(TOP)/mk/bootstrap.mk
endif

# (Optional) build-specific configuration
include $(TOP)/mk/custom-settings.mk

ifndef FAST
-include .depend
endif
# The dependencies file from the current directory
