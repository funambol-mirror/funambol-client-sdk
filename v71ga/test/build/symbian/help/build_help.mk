# ============================================================================
#  Name	 : build_help.mk
#  Part of  : symbian
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : symbian
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : symbian_0xEEBB239B.hlp
symbian_0xEEBB239B.hlp : symbian.xml symbian.cshlp Custom.xml
	cshlpcmp symbian.cshlp
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
	copy symbian_0xEEBB239B.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del symbian_0xEEBB239B.hlp
	del symbian_0xEEBB239B.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo symbian_0xEEBB239B.hlp

FINAL : do_nothing
