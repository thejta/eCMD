#!/usr/bin/perl

# &*&*&*&*&*&*&&*&*&*&*&*&*&&*&*&*&*&*&*&*&*&*&*&*&*&*&*&&*&*&*&*&*&*&&*&*&*&*&*&*&*&*
#
# WARNING DO NOT EDIT THIS SCRIPT UNLESS YOU ARE EDITING 'ecmdWrapper.pl'
# The files are all linked together in CVS and you will cause headaches if you edit the individual commands (ie getscom)
# Edit ecmdWrapper.pl - commit - then update and all the other files will get updated
#
# &*&*&*&*&*&*&&*&*&*&*&*&*&&*&*&*&*&*&*&*&*&*&*&*&*&*&*&&*&*&*&*&*&*&&*&*&*&*&*&*&*&*

# ***************************************************************************
# Get rid of any path information from the command that came in
# ***************************************************************************
@filespecifiers = split(/\//, $0); 
$functionname = $filespecifiers[@filespecifiers-1];
	
# ***************************************************************************
# Figure out what the user's ecmd executable is set to, if they have one.
# ***************************************************************************
$ecmd_exe = $ENV{"ECMD_EXE"};

$stat = lstat($ecmd_exe);

#check if the cronus exe file exists or not
if($stat eq "1"){
    #nothing the file exists
    ; 
} else{
    # the cronus_exe does not exist tell that to the user ans exit
    print "*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****\n";
    print "The eCMD executable \n \t\"$ecmd_exe\" \n\t\t does NOT exist\n\n";
    print "Please modify your ECMD_EXE variable to point to a valid eCMD executable before running \n";
    print "*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****ERROR*****\n";
    exit(1);
}	

# ***************************************************************************
# Cat all of the argument from the command line together to get one
# big command line.
# ***************************************************************************
foreach (@ARGV) {
    $functionname = $functionname . " $_";
}


$temp = $ecmd_exe . " " . $functionname;
print $temp
exec($temp);
exit($?/256);

