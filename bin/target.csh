#!/bin/csh

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

set numFields = `echo $TARGET_VARIABLES | awk '{print NF}'`

# Very first thing, do the help if nothing is passed in
if (!($numFields)) then
   echo "help: target lock|unlock|override|query"
   exit
endif

if ($numFields > 1) then
   set message = `echo $TARGET_VARIABLES | awk '{print substr($0,5) }'`
   setenv TARGET_VARIABLES lock
endif

if ($TARGET_VARIABLES == "override" || $TARGET_VARIABLES == "lock") then

   # Print initial message
   set temp1 = `cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | grep -c "#LOCK#"`
   if ($TARGET_VARIABLES == "override") then
      if (!($temp1)) then
         echo 'This machine has no lock on it, override aborted'
         exit
      endif
      echo "Overriding lock on target $ECMD_TARGET"
      cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | awk -F\# '{ if($2 ~ /LOCK_COMMENT/) {printf("Message:%s\n",$3)} else if($2 ~ /LOCK_OWNER/) {printf("Locked By:%s\n",$3)}  }'
   else
      if ($temp1) then
         echo 'This machine is already locked, if you wish to access please run "target override"'
         exit
      endif

      if (!($?message)) then
         echo "Please enter reason for machine lock:"
         set message = "$<"
      endif
      echo "#LOCK# yes" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
      echo "#LOCK_COMMENT# $message" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
      echo "#LOCK_OWNER# `whoami` on `date`" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
   endif


   # Check and see if we have lock, create one if not
   set temp1 = `printenv ECMD_LOCK`
   if ($temp1 == "") then
      @ temp2 = ($$ + 10000)
      setenv ECMD_LOCK $ECMD_TARGET""_$temp2
   endif

   # Now see if the #LOCK_ALLOWED# entry is in the info file, if not add it 
   set temp3 = `cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | grep -c "#LOCK_ALLOWED# $ECMD_LOCK"`
   if (!($temp3)) then
      echo "#LOCK_ALLOWED# $ECMD_LOCK" >> $CRONUS_HOME/targets/$ECMD_TARGET""_info
   endif

   # Print exit message
   if ($TARGET_VARIABLES == "override") then
      echo "Lock override complete"
   else
      echo "Target is now locked"
   endif

else if ($TARGET_VARIABLES == "unlock") then

   echo "Releasing lock on target $ECMD_TARGET"
   cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | awk '{ if($1 !~ /LOCK/) {path = sprintf("%s/targets/%s_info_temp",ENVIRON["CRONUS_HOME"],ENVIRON["ECMD_TARGET"]); print $0 > path} }'
   mv $CRONUS_HOME/targets/$ECMD_TARGET""_info_temp $CRONUS_HOME/targets/$ECMD_TARGET""_info
   echo "Target is now unlocked"

else if ($TARGET_VARIABLES == "query") then

   # First tell the user what their target is
   echo Current target is \"$ECMD_TARGET\"
   # Then mention if the target is locked
   set temp3 = `cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | grep -c "#LOCK# yes"`
   if (!($temp3)) then
     echo The target \"$ECMD_TARGET\" is NOT locked
   else
     echo The target \"$ECMD_TARGET\" is locked!
     cat $CRONUS_HOME/targets/$ECMD_TARGET""_info | awk -F\# '{ if($2 ~ /LOCK_COMMENT/) {printf("Message:%s\n",$3)} else if($2 ~ /LOCK_OWNER/) {printf("Locked By:%s\n",$3)}  }'
   endif

else
   # Must be a real target, so set the environment variable
   setenv ECMD_TARGET $TARGET_VARIABLES
   echo eCMD Target is now \"$ECMD_TARGET\"
endif

unset numFields
unset message
unset temp1
unset temp2
unset temp3
