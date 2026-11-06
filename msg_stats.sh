#
#	$Id$
#
msg stats blt.all.all.all ;
msg stats brd.all.all.all ;
msg stats coi.all.all.all ;
msg stats csh.all.all.all ;
msg stats sdx.all.all.all ;
msg stats spx.all.all.all ;
echo "";
case	`uname -n` in

cashbox | coinbox | breadbox | ballotbox )
	msg stats tali.1 ;
	msg stats tali.2 ;
	msg stats tali.3 ;
	msg stats tali.4 ;
	echo "";
	msg stats sccp.1 ;
	msg stats dip.1 ;
	echo "";
	msg stats lnp.1 ;
	msg stats lnp.2 ;
	echo "";
	msg stats n00.1 ;
	msg stats n00.2 ;
	echo "";
	msg stats cnam.1 ;
	msg stats cnam.2 ;
	echo "";
	;;

sandbox | soapbox | strongbox | shoebox )
	msg stats tali.0 ;
	msg stats tali.1 ;
	msg stats tali.2 ;
	msg stats tali.3 ;
	msg stats tali.4 ;
	echo "";
	msg stats lsw.0 ;
	msg stats lw.0 ;
	msg stats lw.1 ;
	msg stats lw.2 ;
	msg stats lw.3 ;
	msg stats lw.4 ;
	msg stats lw.5 ;
	echo "";
	msg stats sccp.1 ;
	msg stats swt.all ;
	echo "";
	msg stats psaxs.1 ;
	msg stats psaxs.2 ;
	msg stats psaxs.3 ;
	msg stats psaxs.4 ;
	msg stats psaxs.5 ;
	msg stats psaxs.6 ;
	msg stats psaxs.7 ;
	msg stats psaxs.8 ;
	msg stats psaxs.9 ;
	msg stats psaxs.10 ;
	msg stats psaxs.11 ;
	msg stats psaxs.12 ;
	msg stats psaxs.13 ;
	msg stats psaxs.14 ;
	msg stats psaxs.15 ;
	msg stats psaxs.16 ;
	msg stats psaxs.17 ;
	msg stats psaxs.18 ;
	msg stats psaxs.19 ;
	echo "";
	msg stats i2q.16 ;
	msg stats q2i.16 ;
	echo "";
	;;

#ballotbox | breadbox )
#	msg stats bill.1 ;
#	echo "";
#	msg stats csp.1 ;
#	echo "";
#	msg stats svc.1 ;
#	msg stats svc.2 ;
#	msg stats svc.3 ;
#	msg stats svc.4 ;
#	msg stats svc.5 ;
#	echo "";
#	;;

esac

