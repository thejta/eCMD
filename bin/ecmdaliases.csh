##########################################
# Alias file for ecmd command
#

##########################################
# alias for setting target script
##########################################
#alias target 'setenv TARGET_VARIABLES '\''\!*'\''; source $PWD/target.csh; unsetenv TARGET_VARIABLES'
alias target 'setenv TARGET_VARIABLES '\''\!*'\''; eval `awk '\''BEGIN { if (ENVIRON["ECMD_RELEASE"] == "") {printf("source $PWD/target.csh")} else {printf("source $CTEPATH/tools/ecmd/$ECMD_RELEASE/bin/target.csh")}}'\''`; unsetenv TARGET_VARIABLES'

##########################################
# alias for setting ecmd_setup script
##########################################
#alias ecmdsetup 'eval `$PWD/ecmdsetup.pl csh \!*`'
alias ecmdsetup 'set ecmdsp = `awk '\''BEGIN { if (ENVIRON["ECMD_RELEASE"] == "") {printf("%s/ecmdsetup.pl csh", ENVIRON["PWD"])} else {printf("%s/tools/ecmd/%s/bin/ecmdsetup.pl csh", ENVIRON["CTEPATH"], ENVIRON["ECMD_RELEASE"])}}'\''`; eval `$ecmdsp \!*`; unset ecmdsp'

##########################################
# Include Cronus specific aliases
#
# I'm pulling these, no one has used them - JTA 10/27/06
#source $PWD/../plugins/cro/croaliases.csh
