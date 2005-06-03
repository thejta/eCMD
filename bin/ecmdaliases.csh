##########################################
# Alias file for ecmd command
#

##########################################
# alias for setting target script
##########################################
alias target 'setenv TARGET_VARIABLES '\''\!*'\''; source $PWD/target.csh; unsetenv TARGET_VARIABLES'

##########################################
# alias for setting ecmd_setup script
##########################################
alias ecmdsetup 'eval `$PWD/ecmdsetup.pl csh \!*`'

##########################################
# Include Cronus specific aliases
#
source $PWD/../plugins/cro/croaliases.csh
