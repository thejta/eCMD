package ChipTarget;

sub new 
{
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $data = shift;

	if (!$data) 
	{
		$data = "";
	}

	my $self = \$data;

	bless $self, $class;
	return $self;
}

sub chip
{
	my $self = shift;

	my @fields = split(/\s+/,$$self);

	if ($fields[0] =~ /^-/) 
	{
		die "Invalid chip target: Cannot have non-chip arg as first position in string: $$self\n";
	}
	
	if (@_) 
	{
		$$self =~ s/$fields[0]/$_[0]/;
	}
	else 
	{
		return $fields[0];
	}

}

sub cage
{
	my $self = shift;

	if (@_) {  #a set
		if ($$self =~ /-k\w+/) {

			my @halves = split(/-k\w+/, $$self);
			$$self = join("", $halves[0], "-k", $_[0], $halves[1]);

		} 
		else {
			$$self .= " -k".$_[0];
		}
	}
	else {  #a get
		if ($$self =~ /-k(\w+)/) {			
			return $1;		
		}
		else {
			return "0";
		}
	}
}

sub node
{
	my $self = shift;

	if (@_) {  #a set
		if ($$self =~ /-n\w+/) {

			my @halves = split(/-n\w+/, $$self);
			$$self = join("", $halves[0], "-n", $_[0], $halves[1]);

		} 
		else {
			$$self .= " -n".$_[0];
		}
	}
	else {  #a get
		if ($$self =~ /-n(\w+)/) {			
			return $1;		
		}
		else {
			return "0";
		}
	}
}

sub pos
{
	my $self = shift;

	if (@_) {  #a set
		if ($$self =~ /-p\w+/) {

			my @halves = split(/-p\w+/, $$self);
			$$self = join("", $halves[0], "-p", $_[0], $halves[1]);

		} 
		else {
			$$self .= " -p".$_[0];
		}
	}
	else {  #a get
		if ($$self =~ /-p(\w+)/) {			
			return $1;		
		}
		else {
			return "0";
		}
	}
}

sub core
{
	my $self = shift;

	if (@_) {  #a set
		if ($$self =~ /-c\w+/) {

			my @halves = split(/-c\w+/, $$self);
			$$self = join("", $halves[0], "-c", $_[0], $halves[1]);

		} 
		else {
			$$self .= " -c".$_[0];
		}
	}
	else {  #a get
		if ($$self =~ /-c(\w+)/) {			
			return $1;		
		}
		else {
			return "0";
		}
	}
}

sub thread
{
	my $self = shift;

	if (@_) {  #a set
		if ($$self =~ /-t\w+/) {

			my @halves = split(/-t\w+/, $$self);
			$$self = join("", $halves[0], "-t", $_[0], $halves[1]);

		} 
		else {
			$$self .= " -t".$_[0];
		}
	}
	else {  #a get
		if ($$self =~ /-t(\w+)/) {			
			return $1;		
		}
		else {
			return "0";
		}
	}
}

1;
