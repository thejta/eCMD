##########################################
# Alias file for ecmd command
#

##########################################
# alias for setting target script
##########################################
alias target 'setenv TARGET_VARIABLES '\''\!*'\''; source INSTALL_BIN_PATH/target.csh; unsetenv TARGET_VARIABLES'

##########################################
# alias for setting ecmdsetup script
##########################################
alias ecmdsetup 'eval `INSTALL_BIN_PATH/ecmdsetup.pl csh \!*`'
