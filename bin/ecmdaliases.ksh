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
   . $CTEPATH/tools/ecmd/.common/bin/target.ksh
   unset TARGET_VARIABLES
}

##########################################
# alias for setting ecmd_setup script
##########################################
alias ecmdsetup=_ecmdsetup

function _ecmdsetup
{
   eval `$CTEPATH/tools/ecmd/.common/bin/ecmdsetup.pl ksh $*`
}

##########################################
# Include Cronus specific aliases
#
. $CTEPATH/tools/ecmd/.common/bin/.cro/croaliases.ksh
