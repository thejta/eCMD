# This is the ecmdDataBuffer package / class for use in Cronus Perl Modules #

package ecmdDataBuffer;

use strict;
#use warnings;
#require Exporter;

#use vars qw(@ISA @EXPORT);

#@ISA = qw(Exporter);
#@EXPORT = qw(new getWordLength getstr setstr );


################################################
# This is the only data string global variable #
################################################
my $iv_DataStr = "";

sub new {
	my $self = \$iv_DataStr;
	bless $self;
	$iv_DataStr = @_[1];
	return $self;
}


#  // Member Functions
#  /** @name Buffer Size Function */
#  //@{
#  /**
#   * @brief Return the length of the buffer in words
#   * @retval Buffer length in words rounded up
#   */
sub getWordLength {
	my ($len) =0;
	my ($needExtra) =0;
	$len = length($iv_DataStr);
	$needExtra = $len % 32;
	$len = $len /32;
	if ($needExtra!=0) {$len++;}
	return $len;
}

#  /**
#   * @brief Return the length of the buffer in bytes
#   * @retval Buffer length in bytes rounded up
#   */
sub getByteLength() {
	my ($len) =0;
	my ($needExtra) =0;
	$len = length($iv_DataStr);
	$needExtra = $len % 8;
	$len = $len /8;
	if ($needExtra!=0) {$len++;}
	return $len;
}

#  /**
#   * @brief Return the length of the buffer in bits
#   * @retval Buffer length in bits
#   */
sub getBitLength() {
	return length($iv_DataStr);
}

#  /**
#   * @brief Reinitialize the Buffer to specified length
#   * @param i_newNumWords Length of new buffer in words
#   * @post Buffer is reinitialized
#   *
#   * CAUTION : All data stored in buffer will be lost
#   */
sub setWordLength() {
	my ($i_newNumWords)= @_[1];
	my ($looper)=0;
	$iv_DataStr= "";
	for($looper=0;$looper<$i_newNumWords;$looper++) {
		$iv_DataStr = $iv_DataStr . "00000000000000000000000000000000";
	}
}  


#  /**
#   * @brief Reinitialize the Buffer to specified length
#   * @param i_newNumBits Length of new buffer in bits
#   * @post Buffer is reinitialized
#   *
#   * CAUTION : All data stored in buffer will be lost
#   */
sub setBitLength() {
	my ($i_newNumBits)= @_[1];
	my ($looper)=0;
	$iv_DataStr= "";
	for($looper=0;$looper<$i_newNumBits;$looper++) {
		$iv_DataStr = $iv_DataStr . "0";
	}
}




#  //@}



#/** @name Bit/Word Manipulation Functions */
#  //@{


#  /**
#   * @brief Turn on a bit in buffer
#   * @param i_bit Bit in buffer to turn on
#   */
sub setBit() {
	my ($i_bit,$len) = @_[1],@_[2];
	if (($i_bit        >= getBitLength()) || 
	    ($i_bit + $len >= getBitLength())   ) {
		printf("**** ERROR : ecmdDataBuffer::setBit: bit %d >= NumBits (%d)\n", $i_bit, getBitLength());
        } else {
		if ($len) {
			while ($len != 0) {
				substr($iv_DataStr,$i_bit+$len,1) ='1';
				$len--;
			}
		} else {
			substr($iv_DataStr,$i_bit,1) ='1';
		}
	}
}

sub clearBit() {
	my ($i_bit) = @_[1];
	my ($len)   = @_[2];
	if (($i_bit        >= getBitLength()) || 
	    ($i_bit + $len >= getBitLength())   ) {
		printf("**** ERROR : ecmdDataBuffer::clearBit: bit %d >= NumBits (%d)\n", $i_bit, getBitLength());
        } else {
		if ($len) {
			while ($len != 0) {
				substr($iv_DataStr,$i_bit+$len,1) ='0';
				$len--;
			}
		} else {
			substr($iv_DataStr,$i_bit,1) ='0';
		}
	}
}


#  /**
#   * @brief Set a word of data in buffer
#   * @param i_wordoffset Offset of word to set
#   * @param i_value 32 bits of data to put into word
#   */
sub setWord() {
	my ($i_wordOffset) = @_[1];
	my ($i_value) = @_[2];
	my ($offset) =0;

	if ($i_wordOffset >= getWordLength()) {
		printf("**** ERROR : ecmdDataBuffer::setWord: wordoffset %d >= NumWords (%d)\n", $i_wordOffset, getWordLength());
        } else {

		my ($startBit) = $i_wordOffset * 32;
    		my ($i) =0;
		my ($mask) = 0x80000000;
    		for ($i = 0; $i < 32; $i++) {
			$offset = $startBit+$i;
	      		if ($i_value & $mask) {
				substr($iv_DataStr,$offset,1) ='1';
		      	}
      			else {
				substr($iv_DataStr,$offset,1) ='0';
	      		}
			$mask >>= 1;
		}
	}
}


#/**
#  * @brief Fetch a word from ecmdDataBuffer
#  * @param i_wordoffset Offset of word to fetch
#  * @retval Value of word requested
#  */
sub getWord() {
	my ($i_wordOffset) = @_[1];
	my ($extracted) ="";
	$extracted = substr($iv_DataStr,$i_wordOffset*32,32);
	return $extracted;
}

#  /**
#   * @brief Set a byte of data in buffer
#   * @param i_byteoffset Offset of byte to set
#   * @param i_value 8 bits of data to put into byte
#   */
sub setByte() {
	my ($i_byteOffset) = @_[1];
	my ($i_value) = @_[2];
	my ($offset) =0;

	if ($i_byteOffset >= getByteLength()) {
		printf("**** ERROR : ecmdDataBuffer::setByte: Byteoffset %d >= NumBytes (%d)\n", $i_byteOffset, getByteLength());
        } else {

		my ($startBit) = $i_byteOffset * 8;
    		my ($i) =0;
		my ($mask) = 0x80;
    		for ($i = 0; $i < 8; $i++) {
			$offset = $startBit+$i;
	      		if ($i_value & $mask) {
				substr($iv_DataStr,$offset,1) ='1';
		      	}
      			else {
				substr($iv_DataStr,$offset,1) ='0';
	      		}
			$mask >>= 1;
		}
	}
}



sub flipBit() {
	my ($bit) = @_[1];
	my ($len) = @_[2];
	my ($looper) =0;
	if ($len) {

	} else {
		$len =1;
	}
	
	if (($bit+$len-1) >= getBitLength()) {
		printf("**** ERROR : ecmdDataBuffer::flipBit: bit %d + %d >= NumBits (%d)\n", $bit, $len, getBitLength());
	}
	$len -=1;

	for($looper=0;$looper<=$len; $looper++) {
		if (substr($iv_DataStr,$bit+$looper,1) == "1") {
			substr($iv_DataStr,$bit+$looper,1) ='0';
      		} elsif (substr($iv_DataStr,$bit+$looper,1) == "0") {
			substr($iv_DataStr,$bit+$looper,1) ='1';
		} else {
			printf("**** ERROR : ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d\n", $bit+$looper);
		}
	}
}


sub isBitSet() {
	my ($bit) = @_[1];
	my ($len) = @_[2];
	my ($looper) =0;

	if ($len) {
	} else {
		$len =1;
	}
	
	if (($bit+$len-1) >= getBitLength()) {
		printf("**** ERROR : ecmdDataBuffer::isBitSet: bit %d + %d >= NumBits (%d)\n", $bit, $len, getBitLength());
	}
	$len -=1;

	for($looper=0;$looper<=$len; $looper++) {
      		if (substr($iv_DataStr,$bit+$looper,1) == "1") {
#do nothing but know that a 1 is valid.
		} elsif (substr($iv_DataStr,$bit+$looper,1) == "0") {
			return 0;
		} else {
		      printf("**** ERROR : ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d \n", $bit+$len);
			return 0;
		}
	}
	return 1;
}

sub isBitClear() {
	my ($bit) = @_[1];
	my ($len) = @_[2];
	my ($looper) =0;

	if ($len) {
	} else {
		$len =1;
	}
	
	if (($bit+$len-1) >= getBitLength()) {
		printf("**** ERROR : ecmdDataBuffer::isBitClear: bit %d + %d >= NumBits (%d)\n", $bit, $len, getBitLength());
	}
	$len -=1;

	for($looper=0;$looper<=$len; $looper++) {
      		if (substr($iv_DataStr,$bit+$looper,1) == "1") {
			return 0;
		} elsif (substr($iv_DataStr,$bit+$looper,1) == "0") {
#do nothing but know that a 0 is valid.
		} else {
		      printf("**** ERROR : ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d \n", $bit+$len);
			return 0;
		}
	}
	return 1;
}



sub getNumBitsSet() {
	my ($bit) = @_[1];
	my ($len) = @_[2];
	my ($looper) =0;
	my ($count) =0;

	if ($len) {
	} else {
		$len =1;
	}
	
	if (($bit+$len-1) >= getBitLength()) {
		printf("**** ERROR : ecmdDataBuffer::getNumBitsSet: bit %d + %d >= NumBits (%d)\n", $bit, $len, getBitLength());
	}
	$len -=1;

	for($looper=0;$looper<=$len; $looper++) {
      		if (substr($iv_DataStr,$bit+$looper,1) == "1") {
			$count++;
		} elsif (substr($iv_DataStr,$bit+$looper,1) == "0") {
#do nothing but know that a 0 is valid.
		} else {
		      printf("**** ERROR : ecmdDataBuffer::getNumBitSet: non-binary character detected in data at bit %d \n", $bit+$len);
		}
	}
    return $count;
}


  /**
   * @brief Shift data to right
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to right by specified number of bits - data is shifted off the end
   * @post Buffer size is unchanged
   */

sub shiftRight() {
	my ($shiftNum) = @_[1];
	my ($charHolder);
	my ($looper) =0;
	my ($tmpStr);

	if($shiftNum) {
	#if it is zero, then just skip it all.

		for($looper=0;$looper<$shiftNum; $looper++) {
			$charHolder = chop($iv_DataStr);
			$iv_DataStr = 0 . $iv_DataStr;
		}
	}


}

 /**
   * @brief Shift data to left
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to left by specified number of bits - data is shifted off the beginning
   * @post Buffer size is unchanged
   */

sub shiftLeft() {
	my ($shiftNum) = @_[1];
	my ($looper) =0;
	my ($tmpStr);

	if($shiftNum) {
	#if it is zero, then just skip it all.

		for($looper=0;$looper<$shiftNum; $looper++) {
			$tmpStr = substr($iv_DataStr,1,getBitLength()-1);
			$iv_DataStr = $tmpStr . 0;
		}

	}

}

  /**
   * @brief Rotate data to right
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the right by specified number of bits - data is rotated to the beginning
   */

sub rotateRight() {
	my ($shiftNum) = @_[1];
	my ($charHolder);
	my ($looper) =0;
	my ($tmpStr);

	if($shiftNum) {
	#if it is zero, then just skip it all.

		for($looper=0;$looper<$shiftNum; $looper++) {
			$charHolder = chop($iv_DataStr);
			$iv_DataStr = $charHolder . $iv_DataStr;
		}
	}


}

 /**
   * @brief Rotate data to left
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the left by specified number of bits - data is rotated to the end
   */
  
sub rotateLeft() {
	my ($shiftNum) = @_[1];
	my ($charHolder);
	my ($looper) =0;
	my ($tmpStr);

	if($shiftNum) {
	#if it is zero, then just skip it all.

		for($looper=0;$looper<$shiftNum; $looper++) {
	      		$charHolder = substr($iv_DataStr,0,1);
			$tmpStr = substr($iv_DataStr,1,getBitLength()-1);
			$iv_DataStr = $tmpStr . $charHolder;
		}

	}

}




sub getstr {
	return $iv_DataStr;
}

sub setstr {
	$iv_DataStr = @_[1];
}


1;

