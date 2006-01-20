#!/bin/ksh

##############################################
# Used to do target locking to prevent ecmd
# plugins from talking to a machine by mistake
# It also sets the ECMD_TARGET variable if
# one of the keywords wasn't specified
#
# By: Jason Albert
# Date: 04/25/02
#
# Ver 2.0
#
##############################################

numFields=`echo $TARGET_VARIABLES | awk '{print NF}'`

# Very first thing, do the help if nothing is passed in
if [[ $numFields = 0 ]]
then
   echo "help: target lock|override|unlock|query"
fi

if [[ $numFields > 1 ]]
then
   message=`echo $@ | awk '{print substr($0,5) }'`
   export TARGET_VARIABLES=lock
fi

if [[ $TARGET_VARIABLES = "override" || $TARGET_VARIABLES = "lock" ]]
then

   if [[ $ECMD_PLUGIN != "scand" ]]
   then
     # Print initial message
     temp1=`cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | grep -c "#LOCK#"`
     if [[ $TARGET_VARIABLES = "override" ]]
     then
     	if [[ $temp1 = 0 ]]
     	then
     	   echo 'This machine has no lock on it, override aborted'
     	   return
     	fi
     	echo "Overriding lock on target $ECMD_TARGET"
     	cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | awk -F\# '{ if($2 ~ /LOCK_COMMENT/) {printf("Message:%s\n",$3)} else if($2 ~ /LOCK_OWNER/) {printf("Locked By:%s\n",$3)}  }'
     else
     	if [[ $temp1 != 0 ]]
     	then
     	   echo 'This machine is already locked, if you wish to access please run "target override"'
     	   return
     	fi

     	if [[ $message = "" ]]
     	then
     	   echo "Please enter message about the machine lock:"
     	   read message
     	fi
     	echo "#LOCK# yes" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
     	echo "#LOCK_COMMENT# $message" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
     	echo "#LOCK_OWNER# `whoami` on `date`" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
     fi

     # Check and see if we have lock, create one if not
     temp1=`printenv ECMD_LOCK`
     if [[ $temp1 = "" ]]
     then
     	temp2=$(($$ + 10000))
     	export ECMD_LOCK=$ECMD_TARGET""_$temp2
     fi

     # Now see if the #LOCK_ALLOWED# entry is in the info file, if not add it
     temp3=`cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | grep -c "#LOCK_ALLOWED# $ECMD_LOCK"`
     if [[ $temp3 = 0 ]]
     then
     	echo "#LOCK_ALLOWED# $ECMD_LOCK" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
     fi

     # Print exit message
     if [[ $TARGET_VARIABLES = "override" ]]
     then
     	echo "Lock override complete"
     else
     	echo "Target is now locked"
     fi
   fi
   
elif [[ $TARGET_VARIABLES = "unlock" ]]
then
   if [[ $ECMD_PLUGIN != "scand" ]]
   then
     echo "Releasing lock on target $ECMD_TARGET"
     cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | awk '{ if($1 !~ /LOCK/) {path = sprintf("%s/targets/%s_info_temp",ENVIRON["CRONUS_HOME"],ENVIRON["ECMD_TARGET"]); print $0 > path} }'
     mv $CRONUS_HOME/targets/$ECMD_TARGET""_info_temp $CRONUS_HOME/targets/$ECMD_TARGET""_info
     echo "Target is now unlocked"
   fi
   
elif [[ $TARGET_VARIABLES = "query" || $TARGET_VARIABLES = "q" ]]
then

   # First tell the user what their target is
   echo Current target is \"$ECMD_TARGET\"
   if [[ $ECMD_PLUGIN != "scand" ]]
   then
     # Then mention if the target is locked
     temp3=`cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | grep -c "#LOCK# yes"`
     if [[ $temp3 = 0 ]] #Not Locked
     then
       echo "The target \"$ECMD_TARGET\" is NOT locked"
     else #LOCKED
     	echo "The target \"$ECMD_TARGET\" is locked!"
     	cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | awk -F\# '{ if($2 ~ /LOCK_COMMENT/) {printf("Message:%s\n",$3)} else if($2 ~ /LOCK_OWNER/) {printf("Locked By:%s\n",$3)}  }'
     fi
   fi
elif [[ $numFields = 1 ]]
then
   # Must be a real target, so set the environment variable
   export ECMD_TARGET=$TARGET_VARIABLES
   if [[ $ECMD_PLUGIN = "scand" ]]
   then
     export SCAND_DFN=$ECMD_TARGET
   fi
   echo eCMD Target is now \"$ECMD_TARGET\"
fi

unset numFields
unset message
unset temp1
unset temp2
unset temp3
