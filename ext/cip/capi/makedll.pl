#!/usr/bin/perl
# File makedll.pl created by Joshua Wills at 12:45:07 on Fri Sep 19 2003. 

my @ignores = qw( ecmdLoadDll ecmdUnloadDll ecmdCommandArgs);
my $ignore_re = join '|', @ignores;

my $printout;
my @enumtable;

while (<>) {

    if (/^int/) {

	next if (/$ignore_re/);

	chomp; chop;  
	my ($func, $args) = split /\(|\)/ , $_;

	my ($type, $funcname) = split /\s+/, $func;
	my @argnames = split /,/ , $args;

        foreach my $i (0..$#argnames) {
            if ($argnames[$i] =~ /=/) {
              $argnames[$i] =~ s/=.*//;
            }
        }

        $" = ",";
        $printout .= "$type $funcname(@argnames) {\n\n";
	$" = " ";

	$printout .= "  $type rc;\n\n";
	$printout .= "#ifdef ECMD_STATIC_FUNCTIONS\n\n";

	$printout .= "  rc = ";

        my $enumname;

        if ($funcname =~ /ecmd/) {

           $funcname =~ s/ecmd//;

           $enumname = "ECMD_".uc($funcname);

           $funcname = "dll".$funcname;
        }
        else {

           $enumname = "ECMD_".uc($funcname);
           $funcname = "dll".ucfirst($funcname);
        }
       
	  
	$printout .= $funcname . "(";

	my $argstring;
	my $typestring;
	foreach my $curarg (@argnames) {

	    my @argsplit = split /\s+/, $curarg;

	    my @typeargs = @argsplit[0..$#argsplit-1];
	    $typestring .= "@typeargs" . ", ";

	    $argstring .= $argsplit[-1] . ", ";
	}

	chop ($typestring, $argstring);
	chop ($typestring, $argstring);

	$printout .= $argstring . ");\n\n";
	    
	$printout .= "#else\n\n";

	$printout .= "  if (DllFnTable[$enumname] == NULL) {\n";
	$printout .= "     DllFnTable[$enumname] = (void*)dlsym(dlHandle, \"$funcname\");\n";
	$printout .= "  }\n\n";

	$printout .= "  $type (*Function)($typestring) = \n";
	$printout .= "      ($type(*)($typestring))DllFnTable[$enumname];\n\n";

	$printout .= "  rc = (*Function)($argstring);\n\n";
	
	$printout .= "#endif\n\n";

	$printout .= "  return rc;\n\n";

	$printout .= "}\n\n";

	push @enumtable, $enumname;
    }

}

push @enumtable, "ECMD_NUMFUNCTIONS";
$" = ",\n";
print "typedef enum {\n@enumtable\n} ecmdFunctionIndex_t;\n\n";
$" = " ";

print "void * dlHandle = NULL;\n";
print "void * DllFnTable[ECMD_NUMFUNCTIONS];\n\n";

print $printout;
