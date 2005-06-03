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
   . $PWD/target.ksh
   unset TARGET_VARIABLES
}

##########################################
# alias for setting ecmd_setup script
##########################################
alias ecmdsetup=_ecmdsetup

function _ecmdsetup
{
   eval `$PWD/ecmdsetup.pl ksh $*`
}

##########################################
# Include Cronus specific aliases
#
. $PWD/../plugins/cro/croaliases.ksh
