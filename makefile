# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
# Copyright (c) 2009-2026 John R. Smythe Jr.
#

#
#	Global Definitions
#

include		../rules.mak

#
#	Unit and Common Header Definitions
#

CHDRS =	$(HDR)/$(UNIT).h

#
#	Unit's Library Definitions
#

LIBRARIES = -laddr -lmap -lsem -lfar -lnow -lsys

MSGS = $(wildcard *.msg)

#
#	Product Definitions
#

PRODUCTS += $(addprefix $(FILE_PATH),$(MSGS))

SOURCE +=	$(MSGS)

#
#	Global Targets
#

include		../targets.mak

#
#	Unit's Targets
#


#
#	Include	Object Dependency Files
#

$(FILE_PATH)%.msg :     %.msg
	cp	-pf	$? $@ ;
	chmod   666	$@ ;

include	.depend

# Guardian legacy message conversions live in addr; retain that real dependency.
msg: LIBRARIES := -lmiomsg -lmio -lmsg -laddr -lmap -lsem -lfar -lnow -lsys
