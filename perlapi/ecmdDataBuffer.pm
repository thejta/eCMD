# This is the ecmdDataBuffer package / class for use in Cronus Perl Modules #

package ecmdDataBuffer;

=for mainBrief
/**
 * @file ecmdDataBuffer.H
 * @brief Provides a means to handle data from the eCMD Perl API
 *
 * DataBuffers handle data in a Big Endian fashion with Bit 0 being the MSB
*/
=cut

use strict;


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


#######################################################################################
=for functionBrief
  /**
   * @brief Return the length of the buffer in words
   * @retval Buffer length in words rounded up
   */
  int   getWordLength() const;
=cut

sub getWordLength {
	my ($len) =0;
	my ($needExtra) =0;
	$len = length($iv_DataStr);
	$needExtra = $len % 32;
	$len = $len /32;
	if ($needExtra!=0) {$len++;}
	return $len;
}


#######################################################################################
=for functionBrief
  /**
   * @brief Return the length of the buffer in bytes
   * @retval Buffer length in bytes rounded up
   */
  int   getByteLength() const;
=cut

sub getByteLength() {
	my ($len) =0;
	my ($needExtra) =0;
	$len = length($iv_DataStr);
	$needExtra = $len % 8;
	$len = $len /8;
	if ($needExtra!=0) {$len++;}
	return $len;
}


#######################################################################################
=for functionBrief
  /**
   * @brief Return the length of the buffer in bits
   * @retval Buffer length in bits
   */
  int   getBitLength() const;
=cut

sub getBitLength() {
	return length($iv_DataStr);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Reinitialize the Buffer to specified length
   * @param i_newNumWords Length of new buffer in words
   * @post Buffer is reinitialized
   *
   * CAUTION : All data stored in buffer will be lost
   */
  void  setWordLength(int i_newNumWords);   
=cut

sub setWordLength() {
	my ($i_newNumWords)= @_[1];
	my ($looper)=0;
	$iv_DataStr= "";
	for($looper=0;$looper<$i_newNumWords;$looper++) {
		$iv_DataStr = $iv_DataStr . "00000000000000000000000000000000";
	}
}  


#######################################################################################
=for functionBrief
  /**
   * @brief Reinitialize the Buffer to specified length
   * @param i_newNumBits Length of new buffer in bits
   * @post Buffer is reinitialized
   *
   * CAUTION : All data stored in buffer will be lost
   */
  void  setBitLength(int i_newNumBits);
=cut

sub setBitLength() {
	my ($i_newNumBits)= @_[1];
	my ($looper)=0;
	$iv_DataStr= "";
	for($looper=0;$looper<$i_newNumBits;$looper++) {
		$iv_DataStr = $iv_DataStr . "0";
	}
}


#######################################################################################
=for functionBrief
  /**
   * @brief Turn on a bit in buffer
   * @param i_bit Bit in buffer to turn on
   * @param i_len Number of consecutive bits from start bit to turn on
   */
  void  setBit(int i_bit, int i_len);
=cut

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


#######################################################################################
=for functionBrief
  /**
   * @brief Clear a bit in buffer
   * @param i_bit Bit in buffer to turn off
   * @param i_len Number of consecutive bits from start bit to off
   */
  void  clearBit(int i_bit, int i_len);
=cut

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


#######################################################################################
=for functionBrief
  /**
   * @brief Set a word of data in buffer
   * @param i_wordoffset Offset of word to set
   * @param i_value 32 bits of data to put into word
   */
  void  setWord(int i_wordoffset, uint32_t i_value);
=cut

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


#######################################################################################
=for functionBrief
/**
  * @brief Fetch a word from ecmdDataBuffer
  * @param i_wordoffset Offset of word to fetch
  * @retval Value of word requested
  */
  uint32_t getWord(int i_wordoffset);
=cut

sub getWord() {
	my ($i_wordOffset) = @_[1];
	my ($extracted) ="";
	$extracted = substr($iv_DataStr,$i_wordOffset*32,32);
	return $extracted;
}


#######################################################################################
=for functionBrief
  /**
   * @brief Set a byte of data in buffer
   * @param i_byteoffset Offset of byte to set
   * @param i_value 8 bits of data to put into byte
   */
  void  setByte(int i_byteoffset, uint32_t i_value);
=cut

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


#######################################################################################
=for functionBrief
  /**
   * @brief Invert bit
   * @param i_bit Bit in buffer to invert
   * @param i_len Number of consecutive bits to invert
   */
  void  flipBit(int i_bit, int i_len);
=cut

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


#######################################################################################
=for functionBrief
 /**
   * @brief Test if bit is set
   * @param i_bit Bit to test
   * @param i_len Number of consecutive bits to test
   * @retval true if bit is set - false if bit is clear
   */
  int   isBitSet(int i_bit, int i_len);
=cut

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


#######################################################################################
=for functionBrief
/**
   * @brief Test if bit is clear
   * @param i_bit Bit to test
   * @param i_len Number of consecutive bits to test
   * @retval true if bit is clear - false if bit is set
   */
  int   isBitClear(int i_bit, int i_len);
=cut

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


#######################################################################################
=for functionBrief
/**
   * @brief Count number of bits set in a range
   * @param i_bit Start bit to test
   * @param i_len Number of consecutive bits to test
   * @retval Number of bits set in range
   */
  int   getNumBitsSet(int i_bit, int i_len);
=cut

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


#######################################################################################
=for functionBrief
  /**
   * @brief Shift data to right
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to right by specified number of bits - data is shifted off the end
   * @post Buffer size is unchanged
   */
  void  shiftRight(int i_shiftnum);
=cut

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


#######################################################################################
=for functionBrief
 /**
   * @brief Shift data to left
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to left by specified number of bits - data is shifted off the beginning
   * @post Buffer size is unchanged
   */
  void  shiftLeft(int i_shiftnum);
=cut

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


#######################################################################################
=for functionBrief
  /**
   * @brief Rotate data to right
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the right by specified number of bits - data is rotated to the beginning
   */
  void  rotateRight(int i_rotatenum);
=cut

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


#######################################################################################
=for functionBrief
 /**
   * @brief Rotate data to left
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the left by specified number of bits - data is rotated to the end
   */
  void  rotateLeft(int i_rotatenum);
=cut

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


#######################################################################################
=for functionBrief
  /**
   * @brief Clear entire buffer to 0's
   */
  void  flushTo0();
=cut

sub flushTo0() {
	my ($len) =0;
	my ($looper) =0;

	$len = length($iv_DataStr);
	$iv_DataStr =0;
	for($looper=0;$looper<$len-1; $looper++) {
		$iv_DataStr = $iv_DataStr . 0;
	}

}


#######################################################################################
=for functionBrief
  /**
   * @brief Set entire buffer to 1's
   */
  void  flushTo1();
=cut

sub flushTo1() {
	my ($len) =0;
	my ($looper) =0;

	$len = length($iv_DataStr);
	$iv_DataStr =1;
	for($looper=0;$looper<$len-1; $looper++) {
		$iv_DataStr = $iv_DataStr . 1;
	}
}


#######################################################################################
=for functionBrief
  /**
   * @brief Invert entire buffer
   */
  void  invert();  /* Performs bit inversion on entire DataBuffer class */
=cut

sub invert() {
#  /* Performs bit inversion on entire DataBuffer class */
}



#######################################################################################
=for functionBrief
  /**
   * @brief Apply an inversion mask to data inside buffer
   * @param i_invMask Buffer that stores inversion mask
   * @param i_invByteLen Buffer length provided in bytes
   */
   void applyInversionMask(uint32_t * i_invMask, int i_invByteLen);
=cut
sub applyInversionMask() {
#void applyInversionMask(uint32_t * i_invMask, int i_invByteLen);

}


#######################################################################################
=for functionBrief
  /**
   * @brief Insert part of another DataBuffer into this one
   * @param i_bufferIn DataBuffer to copy data from - data is taken left aligned
   * @param i_start Start bit to insert to
   * @param i_len Length of bits to insert
   * @post Data is copied from bufferIn to this DataBuffer in specified location
   */
  void  insert(ecmdDataBuffer & i_bufferIn, int i_start, int i_len);
=cut
sub insert() {
#  void  insert(ecmdDataBuffer & i_bufferIn, int i_start, int i_len);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Copy data from this DataBuffer into another
   * @param o_bufferOut DataBuffer to copy into - data is placed left aligned
   * @param i_start Start bit of data in this DataBuffer to copy
   * @param i_len Length of consecutive bits to copy
   * @post Data is copied from specified location in this DataBuffer to bufferOut
   */
  void  extract(ecmdDataBuffer & o_bufferOut, int i_start, int i_len);
=cut
sub extract() {
#  void  extract(ecmdDataBuffer & o_bufferOut, int i_start, int i_len);
}


#######################################################################################
=for functionBrief
  /* these functions OR the datain into the DataBuffer buffer */
  /**
   * @brief OR data into DataBuffer
   * @param i_bufferIn DataBuffer to OR data from - data is taken left aligned
   * @param i_startbit Start bit to OR to
   * @param i_len Length of bits to OR
   * @post Data is ORed from i_bufferIn to this DataBuffer in specified location
   */
  void setOr(ecmdDataBuffer & i_bufferIn, int i_startbit, int i_len);
=cut
sub setOr() {
#  void setOr(ecmdDataBuffer & i_bufferIn, int i_startbit, int i_len);
}


#######################################################################################
=for functionBrief
  /**
   * @brief OR data into DataBuffer
   * @param i_bufferIn DataBuffer to OR data from - data is taken left aligned
   * @post Entire data is ORed from bufferIn to this DataBuffer
   */
  void merge(ecmdDataBuffer & i_bufferIn); // does a setor on the whole buffer
=cut
sub merge() {
#  void merge(ecmdDataBuffer & i_bufferIn); // does a setor on the whole buffer
}


#######################################################################################
=for functionBrief
  /* these functions AND the datain into the DataBuffer buffer */
  /**
   * @brief AND data into DataBuffer
   * @param i_bufferIn Bitvector to AND data from - data is taken left aligned
   * @param i_startbit Start bit to AND to
   * @param i_len Length of bits to AND
   * @post Data is ANDed from bufferIn to this DataBuffer in specified location
   */
  void setAnd(ecmdDataBuffer & i_bufferIn, int i_startbit, int i_len);
=cut
sub setAnd() {
#  void setAnd(ecmdDataBuffer & i_bufferIn, int i_startbit, int i_len);
}



#######################################################################################
=for functionBrief
  /**
   * @brief Copy entire contents of this ecmdDataBuffer into o_copyBuffer 
   * @param o_copyBuffer DataBuffer to copy data into
   * @post copyBuffer is an exact duplicate of this DataBuffer
   */
  void  copy(ecmdDataBuffer & o_copyBuffer); 
=cut
sub copy() {
#  void  copy(ecmdDataBuffer & o_copyBuffer); 
}


#######################################################################################
=for functionBrief
  /* These are only to be used to apply a buffer to the entire ecmdDataBuffer, not just sections */
  /**
   * @brief Copy buffer into this ecmdDataBuffer
   * @param i_buf Buffer to copy from
   * @param i_bytes Byte length to copy
   * @post  Xstate and Raw buffer are set to value in i_buf for smaller of i_bytes or buffer capacity
   */
  void  memCopyIn(uint32_t * i_buf, int i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
=cut
sub memCopyIn() {
#  void  memCopyIn(uint32_t * i_buf, int i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
}


#######################################################################################
=for functionBrief
  /**
   * @brief Copy DataBuffer into supplied uint32_t buffer
   * @param o_buf Buffer to copy into - must be pre-allocated
   * @param i_bytes Byte length to copy
   * @post o_buf has contents of databuffer for smaller of i_bytes or buffer capacity
   */
  void  memCopyOut(uint32_t * o_buf, int i_bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */
=cut
sub memCopyOut() {
#  void  memCopyOut(uint32_t * o_buf, int i_bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */
}


#######################################################################################
=for functionBrief
  /**
   * @brief Generate odd parity over a range of bits and insert into DataBuffer
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @param i_insertpos Bit position to insert parity
   * @retval 0 on success - nonzero on failure
   */
  int  oddParity(int i_start, int i_stop, int i_insertpos); 
=cut
sub oddParity() { 
#  int  oddParity(int i_start, int i_stop, int i_insertpos); 
}


#######################################################################################
=for functionBrief
  /**
   * @brief Generate even parity over a range of bits and insert into DataBuffer
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @param i_insertpos Bit position to insert parity
   * @retval 0 on success - nonzero on failure
   */
  int  oddParity(int i_start, int i_stop, int i_insertpos); 
=cut
sub evenParity() {
#  int  oddParity(int i_start, int i_stop, int i_insertpos); 
}


#######################################################################################
=for functionBrief
  /**
   * @brief Return Data as a hex left aligned char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genHexLeftStr(int i_start, int i_bitlen);
=cut
sub genHexLeftStr() {
#  std::string genHexLeftStr(int i_start, int i_bitlen);
#  std::string genHexLeftStr();

}


#######################################################################################
=for functionBrief
  /**
   * @brief Return Data as a hex right aligned char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genHexRightStr(int i_start, int i_bitlen); 
=cut
sub genHexRightStr() {
#  std::string genHexRightStr(int i_start, int i_bitlen); 
#  std::string genHexRightStr(); 
}


#######################################################################################
=for functionBrief
  /**
   * @brief Return Data as a binary char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genBinStr(int i_start, int i_bitlen); 
=cut
sub genBinStr() {
#  std::string genBinStr(int i_start, int i_bitlen); 
#  std::string genBinStr(); 
}


#######################################################################################
=for functionBrief
  /**
   * @brief Retrieve a section of the Xstate Data
   * @param i_start Start bit of data to retrieve
   * @param i_bitlen Number of consecutive bits to retrieve
   * @retval String containing requested data
   */
  std::string genXstateStr(int i_start, int i_bitlen);
=cut
sub genXstateStr() {
#  std::string genXstateStr(int i_start, int i_bitlen);
#  std::string genXstateStr();
}


#######################################################################################
=for functionBrief
  /**
   * @brief Convert data from a hex left-aligned string and insert it into this data buffer
   * @param i_hexChars Hex Left-aligned string of data to insert 
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @param i_length Length of data to insert, defaults to length of i_hexChars, zeroes are padded or data dropped from right if necessary
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-hex chars detected in i_hexChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  int insertFromHexLeft (const char * i_hexChars, int i_start = 0, int i_length = 0);
=cut
sub insertFromHexLeft() {
#  int insertFromHexLeft (const char * i_hexChars, int i_start = 0, int i_length = 0);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Convert data from a hex right-aligned string and insert it into this data buffer
   * @param i_hexChars Hex Right-aligned string of data to insert
   * @param i_expectedLength The expected length of the string data, zeros are padded or data dropped from the left if necessary
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-hex chars detected in i_hexChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  int insertFromHexRight (const char * i_hexChars, int i_start = 0, int i_expectedLength = 0);
=cut
sub insertFromHexRight() {
#  int insertFromHexRight (const char * i_hexChars, int i_start = 0, int i_expectedLength = 0);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Convert data from a binary string and insert it into this data buffer
   * @retval 0 on success- non-zero on failure
   * @param i_binChars String of 0's and 1's to insert 
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-binary chars detected in i_binChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  int insertFromBin (const char * i_binChars, int i_start = 0);
=cut
sub insertFromBin () {
#  int insertFromBin (const char * i_binChars, int i_start = 0);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Check section of buffer for any X-state values
   * @param i_start Start bit to test
   * @param i_length Number of consecutive bits to test
   * @retval 1 if xstate found 0 if none
   */
  int   hasXstate(int i_start, int i_length); /* check subset */
=cut
sub hasXstate() {
#  int   hasXstate(int i_start, int i_length); /* check subset */
#  int   hasXstate(); 
}


#######################################################################################
=for functionBrief
  /**
   * @brief Retrieve an Xstate value from the buffer
   * @param i_bit Bit to retrieve

   * NOTE - To retrieve multiple bits use genXstateStr
   */
  char  getXstate(int i_bit);
=cut
sub getXstate() {
#  char  getXstate(int i_bit);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Set an Xstate value in the buffer
   * @param i_bit Bit to set
   * @param i_value Xstate value to set
   */
  void setXstate(int i_bit, char i_value);
=cut
sub setXstate() {
#  void setXstate(int i_bit, char i_value);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Copy buffer into the Xstate data of this ecmdDataBuffer
   * @param i_buf Buffer to copy from
   * @param i_bytes Byte length to copy (char length)
   * @post  Xstate and Raw buffer are set to value in i_buf for smaller of i_bytes or buffer capacity
   */
  void  memCopyInXstate(char * i_buf, int i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
=cut
sub memCopyInXstate() {
#  void  memCopyInXstate(char * i_buf, int i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
}


#######################################################################################
=for functionBrief
  /**
   * @brief Copy DataBuffer into supplied char buffer from Xstate data
   * @param o_buf Buffer to copy into - must be pre-allocated
   * @param i_bytes Byte length to copy (char length)
   * @post o_buf has contents of databuffer for smaller of i_bytes or buffer capacity
   */
   void  memCopyOutXstate(char * o_buf, int i_bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */
=cut
sub memCopyOutXstate() {
#  void  memCopyOutXstate(char * o_buf, int i_bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */
}



sub getstr {
	return $iv_DataStr;
}

sub setstr {
	$iv_DataStr = @_[1];
}


1;

