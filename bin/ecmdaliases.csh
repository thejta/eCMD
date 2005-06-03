##########################################
# Alias file for ecmd command
#

##########################################
# alias for setting target script
##########################################
alias target 'setenv TARGET_VARIABLES '\''\!*'\''; source $CTEPATH/tools/ecmd/.common/bin/target.csh; unsetenv TARGET_VARIABLES'

##########################################
# alias for setting ecmd_setup script
##########################################
alias ecmdsetup 'eval `$CTEPATH/tools/ecmd/.common/bin/ecmdsetup.pl csh \!*`'

##########################################
# Include Cronus specific aliases
#
source $CTEPATH/tools/ecmd/.common/bin/.cro/croaliases.csh
