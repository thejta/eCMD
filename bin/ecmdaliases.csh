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
# I'm pulling these, no one has used them - JTA 10/27/06
#source $PWD/../plugins/cro/croaliases.csh
