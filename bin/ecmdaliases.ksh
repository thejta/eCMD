##########################################
# Alias file for ecmd command
#

##########################################
# If we don't do this, then perl starts complaining.
# I don't know the reason for this, we just have to.
#
export LANG=C

##########################################
# alias for setting target script
##########################################
alias target=_target

function _target
{
   export TARGET_VARIABLES="$*"
   if [[ $ECMD_RELEASE != "" ]]
   then
      . $CTEPATH/tools/ecmd/$ECMD_RELEASE/bin/target.ksh
   else 
      . $PWD/target.ksh
   fi
   unset TARGET_VARIABLES
}

##########################################
# alias for setting ecmd_setup script
##########################################
alias ecmdsetup=_ecmdsetup

function _ecmdsetup
{
   if [[ $ECMD_RELEASE != "" ]]
   then
      eval `$CTEPATH/tools/ecmd/$ECMD_RELEASE/bin/ecmdsetup.pl ksh $*`
   else 
      eval `$PWD/ecmdsetup.pl ksh $*`
   fi
}

##########################################
# Include Cronus specific aliases
#
# I'm pulling these, no one has used them - JTA 10/27/06
#. $PWD/../plugins/cro/croaliases.ksh
