
package ChipTarget;

=for mainBrief
/**
 * @file ChipTarget.H
 * @brief Perl Class to hold eCMD Targetting information

 * Usage : 
   <pre>
     require ChipTarget;
     my $target = new ChipTarget("pu -p1");
     $target->node(2);
   </pre>


*/

/**
 * @brief Perl Class to hold eCMD Targetting information
 * 
 * Usage : 
   <pre>
     require ChipTarget;
     my $target = new ChipTarget("pu -p1");
     $target->node(2);
   </pre>
*/
=cut


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

=for functionBrief
  /** 	
   * @brief Set the Cage
   * @param i_cage New Cage Value

   * Usage : $target->cage(3);
  */
  void cage(int i_cage);
=cut
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

=for functionBrief
  /** 	
   * @brief Set the Node
   * @param i_node New Node Value

   * Usage : $target->node(2);
  */
  void node(int i_node);
=cut
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

=for functionBrief
  /** 	
   * @brief Set the Slot
   * @param i_slot New Slot Value

   * Usage : $target->slot(0);
  */
  void slot(int i_slot);
=cut
sub slot
{
	my $self = shift;

	if (@_) {  #a set
		if ($$self =~ /-s\w+/) {

			my @halves = split(/-s\w+/, $$self);
			$$self = join("", $halves[0], "-s", $_[0], $halves[1]);

		} 
		else {
			$$self .= " -s".$_[0];
		}
	}
	else {  #a get
		if ($$self =~ /-s(\w+)/) {			
			return $1;		
		}
		else {
			return "0";
		}
	}
}


=for functionBrief
  /** 	
   * @brief Set the chip name
   * @param i_chipname New Chipname

   * Usage : $target->chip("pu");
  */
  void chip(const char* i_chipname);
=cut
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

=for functionBrief
  /** 	
   * @brief Set the chip position
   * @param i_pos New Chip Position

   * Usage : $target->pos(1);
  */
  void pos(int i_pos);
=cut
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


=for functionBrief
  /** 	
   * @brief Set the chip core
   * @param i_core New Core Value

   * Usage : $target->core(1);
  */
  void core(int i_core);
=cut
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


=for functionBrief
  /** 	
   * @brief Set the chip thread
   * @param i_thread New Thread Value

   * Usage : $target->thread(0);
  */
  void thread(int i_thread);
=cut
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

