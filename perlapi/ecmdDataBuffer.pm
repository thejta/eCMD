# This is the ecmdDataBuffer package / class for use in Cronus Perl Modules #

package ecmdDataBuffer;

=for mainBrief
/**
 * @file ecmdDataBuffer.H
 * @brief Provides a means to handle data from the eCMD Perl API
 *
 * DataBuffers handle data as a binary string
 *
 * Usage : 
   <pre>
     require ecmdDataBuffer;
     my $data = new ecmdDataBuffer();
     $data->setWord(2,0xFEEDBEEF);
   </pre>
*/

/**
 * @brief Perl Class to provide a means to handle data from the eCMD Perl API
 * 
 * Usage : 
   <pre>
     require ecmdDataBuffer;
     my $data = new ecmdDataBuffer();
     $data->setWord(2,"FEEDBEEF");
   </pre>
*/
=cut

use strict;


sub new {
  my $class = $_[0];
  my $iv_DataStr = $_[1] || "";
  my $self = \$iv_DataStr;
  bless $self, $class;
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
  my $self = $_[0];  # Me

  my $len =0;
  my $needExtra =0;
  $len = length($$self);
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
  my $self = $_[0];  # Me
  my $len =0;
  my $needExtra =0;
  $len = length($$self);
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
  my $self = $_[0];  # Me
  return length($$self);
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
  my $self = $_[0];  # Me
  my $i_newNumWords= @_[1];
  my $looper=0;
  $$self= "";
  for($looper=0;$looper<$i_newNumWords;$looper++) {
    $$self = $$self . "00000000000000000000000000000000";
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
  my $self = $_[0];  # Me
  my $i_newNumBits= @_[1];
  my $looper=0;
  $$self= "";
  for($looper=0;$looper<$i_newNumBits;$looper++) {
    $$self = $$self . "0";
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
  my $self = $_[0];  # Me
  my $i_bit = @_[1];
  my $len   = @_[2];
  if (($i_bit >= $self->getBitLength()) || ($i_bit + $len >= $self->getBitLength()) ) {
    printf("**** ERROR : ecmdDataBuffer.pm::setBit: bit %d >= NumBits (%d)\n", $i_bit, $self->getBitLength());
  } else {
    if ($len) {
      while ($len != 0) {
        substr($$self,$i_bit+$len-1,1) ='1';
        $len--;
      }
    } else {
      substr($$self,$i_bit,1) ='1';
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
  my $self = $_[0];  # Me
  my $i_bit = @_[1];
  my $len   = @_[2];
  if (($i_bit >= $self->getBitLength()) || ($i_bit + $len >= $self->getBitLength()) ) {
    printf("**** ERROR : ecmdDataBuffer.pm::clearBit: bit %d >= NumBits (%d)\n", $i_bit, $self->getBitLength());
  } else {
    if ($len) {
      while ($len != 0) {
        substr($$self,$i_bit+$len-1,1) ='0';
        $len--;
      }
    } else {
      substr($$self,$i_bit,1) ='0';
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
  my $self = $_[0];  # Me
  my $i_wordOffset = @_[1];
  my $i_value = @_[2];
  my $offset =0;

  if ($i_wordOffset >= $self->getWordLength()) {
    printf("**** ERROR : ecmdDataBuffer.pm::setWord: wordoffset %d >= NumWords (%d)\n", $i_wordOffset, $self->getWordLength());
  } else {

    my $startBit = $i_wordOffset * 32;
    my $i =0;
    my $mask = 0x80000000;
    for ($i = 0; $i < 32; $i++) {
      $offset = $startBit+$i;
      if ($i_value & $mask) {
        substr($$self,$offset,1) ='1';
      }
      else {
        substr($$self,$offset,1) ='0';
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
  my $self = $_[0];  # Me
  my $wordNum = @_[1];
#	a zero is added to the function call because of the way the argument parsing is handled.
  my $word = $self->genHexLeftStr($wordNum*32, 32);
  return $word;
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
  my $self = $_[0];  # Me
  my $i_byteOffset = @_[1];
  my $i_value = @_[2];
  my $offset =0;

  if ($i_byteOffset >= $self->getByteLength()) {
    printf("**** ERROR : ecmdDataBuffer.pm::setByte: Byteoffset %d >= NumBytes (%d)\n", $i_byteOffset, $self->getByteLength());
  } else {

    my $startBit = $i_byteOffset * 8;
    my $i =0;
    my $mask = 0x80;
    for ($i = 0; $i < 8; $i++) {
      $offset = $startBit+$i;
      if ($i_value & $mask) {
        substr($$self,$offset,1) ='1';
      }
      else {
        substr($$self,$offset,1) ='0';
      }
      $mask >>= 1;
    }
  }
}

#######################################################################################
=for functionBrief
/**
   * @brief Fetch a byte from ecmdDataBuffer
   * @param i_byteoffset Offset of byte to fetch
   * @retval Value of byte requested
   */
  uint8_t getByte(int i_byteoffset);
=cut
sub getByte() {
  my $self = $_[0];  # Me
  my $i_byteOffset = @_[1];
  my $extracted ="";
  $extracted = substr($$self,$i_byteOffset*8,8);
  return $extracted;
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
  my $self = $_[0];  # Me
  my $bit = @_[1];
  my $len = @_[2];
  my $looper =0;
  if ($len) {
    
  } else {
    $len =1;
  }
	
  if (($bit+$len-1) >= $self->getBitLength()) {
    printf("**** ERROR : ecmdDataBuffer.pm::flipBit: bit %d + %d >= NumBits (%d)\n", $bit, $len, $self->getBitLength());
  }
  $len -=1;

  for($looper=0;$looper<=$len; $looper++) {
    if (substr($$self,$bit+$looper,1) == "1") {
      substr($$self,$bit+$looper,1) ='0';
    } elsif (substr($$self,$bit+$looper,1) == "0") {
      substr($$self,$bit+$looper,1) ='1';
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::flipBit: cannot flip non-binary data at bit %d\n", $bit+$looper);
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
  my $self = $_[0];  # Me
  my $bit = @_[1];
  my $len = @_[2];
  my $looper =0;

  if ($len) {
  } else {
    $len =1;
  }
	
  if (($bit+$len-1) >= $self->getBitLength()) {
    printf("**** ERROR : ecmdDataBuffer.pm::isBitSet: bit %d + %d >= NumBits (%d)\n", $bit, $len, $self->getBitLength());
  }
  $len -=1;

  for($looper=0;$looper<=$len; $looper++) {
    if (substr($$self,$bit+$looper,1) == "1") {
#do nothing but know that a 1 is valid.
    } elsif (substr($$self,$bit+$looper,1) == "0") {
      return 0;
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::isBitSet: non-binary character detected in data at bit %d \n", $bit+$len);
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
  my $self = $_[0];  # Me
  my $bit = @_[1];
  my $len = @_[2];
  my $looper =0;

  if ($len) {
  } else {
    $len =1;
  }
	
  if (($bit+$len-1) >= $self->getBitLength()) {
    printf("**** ERROR : ecmdDataBuffer.pm::isBitClear: bit %d + %d >= NumBits (%d)\n", $bit, $len, $self->getBitLength());
  }
  $len -=1;

  for($looper=0;$looper<=$len; $looper++) {
    if (substr($$self,$bit+$looper,1) == "1") {
      return 0;
    } elsif (substr($$self,$bit+$looper,1) == "0") {
#do nothing but know that a 0 is valid.
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::isBitSet: non-binary character detected in data at bit %d \n", $bit+$len);
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
  my $self = $_[0];  # Me
  my $bit = @_[1];
  my $len = @_[2];
  my $looper =0;
  my $count =0;

  if ($len) {
  } else {
    $len =1;
  }
	
  if (($bit+$len-1) >= $self->getBitLength()) {
    printf("**** ERROR : ecmdDataBuffer.pm::getNumBitsSet: bit %d + %d >= NumBits (%d)\n", $bit, $len, $self->getBitLength());
  }
  $len -=1;

  for($looper=0;$looper<=$len; $looper++) {
    if (substr($$self,$bit+$looper,1) == "1") {
      $count++;
    } elsif (substr($$self,$bit+$looper,1) == "0") {
#do nothing but know that a 0 is valid.
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::getNumBitSet: non-binary character detected in data at bit %d \n", $bit+$len);
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
  my $self = $_[0];  # Me
  my $shiftNum = @_[1];
  my $charHolder;
  my $looper =0;
  my $tmpStr;

  if($shiftNum) {
    #if it is zero, then just skip it all.

    for($looper=0;$looper<$shiftNum; $looper++) {
      $charHolder = chop($$self);
      $$self = "0" . $$self;
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
  my $self = $_[0];  # Me
  my $shiftNum = @_[1];
  my $looper =0;
  my $tmpStr;

  if($shiftNum) {
    #if it is zero, then just skip it all.

    for($looper=0;$looper<$shiftNum; $looper++) {
      $tmpStr = substr($$self,1,$self->getBitLength()-1);
      $$self = $tmpStr . "0";
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
  my $self = $_[0];  # Me
  my $shiftNum = @_[1];
  my $charHolder;
  my $looper =0;
  my $tmpStr;

  if($shiftNum) {
    #if it is zero, then just skip it all.

    for($looper=0;$looper<$shiftNum; $looper++) {
      $charHolder = chop($$self);
      $$self = $charHolder . $$self;
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
  my $self = $_[0];  # Me
  my $shiftNum = @_[1];
  my $charHolder;
  my $looper =0;
  my $tmpStr;

  if($shiftNum) {
    #if it is zero, then just skip it all.

    for($looper=0;$looper<$shiftNum; $looper++) {
      $charHolder = substr($$self,0,1);
      $tmpStr = substr($$self,1,$self->getBitLength()-1);
      $$self = $tmpStr . $charHolder;
    }
  }
}


#######################################################################################
=for functionBrief
  /**
   * @brief Clear entire buffer to 0s
   */
  void  flushTo0();
=cut

sub flushTo0() {
  my $self = $_[0];  # Me
  my $len =0;
  my $looper =0;

  $len = length($$self);
  $$self ="0";
  for($looper=0;$looper<$len-1; $looper++) {
    $$self = $$self . "0";
  }
}


#######################################################################################
=for functionBrief
  /**
   * @brief Set entire buffer to 1s
   */
  void  flushTo1();
=cut

sub flushTo1() {
  my $self = $_[0];  # Me
  my $len =0;
  my $looper =0;

  $len = length($$self);
  $$self ="1";
  for($looper=0;$looper<$len-1; $looper++) {
    $$self = $$self . "1";
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
  my $self = $_[0];  # Me
#  /* Performs bit inversion on entire DataBuffer class */
  my $len =0;
  my $looper =0;

  $len = length($$self);
  for($looper=0;$looper<$len; $looper++) {

# /* data could be an xstate so don't invert anything other than 1's or 0's */

    if(substr($$self,$looper,1) == '1') {
      substr($$self,$looper,1) = 0;
    }
    elsif (substr($$self,$looper,1) == '0') {
      substr($$self,$looper,1) = '1';
    }
  }
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
  my $self = $_[0];  # Me
  my $i_invMask    = @_[1];
  my $i_invByteLen = @_[2];
  my $len =0;
  my $looper =0;

#do the shorter of the three possible lenghts

  $len = length($i_invMask);
  if($len > $i_invByteLen * 4) { $len = $i_invByteLen * 4; }
  if(length($$self) < $len) { $len = legth($$self); }

  for($looper=0;$looper<$len; $looper++) {
    if(substr($i_invMask,$looper,1) == 1) {
# /* data could be an xstate so don't invert anything other than 1's or 0's */
      if(substr($$self,$looper,1) == '1') {
        substr($$self,$looper,1) = 0;
      }
      elsif (substr($$self,$looper,1) == '0') {
        substr($$self,$looper,1) = '1';
      }
    }
  }
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
  my $self = $_[0];  # Me
#  void  insert(ecmdDataBuffer & i_bufferIn, int i_start, int i_len);
}


#######################################################################################
=for functionBrief
  /**
   * @brief Insert a right aligned (decimal) uint32_t array into this DataBuffer
   * @param i_datain uint32_t array to copy into this DataBuffer - data is taken right aligned
   * @param i_start Start bit to insert into
   * @param i_len Length of bits to insert
   * @post Data is copied from datain into this DataBuffer at specified location
   */
  void  insertFromRight(uint32_t * i_datain, int i_start, int i_len);
=cut
sub insertFromRight() {
  my $self = $_[0];  # Me

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
  my $self = $_[0];  # Me
#	my $o_bufferOut = @_[1];
  my $i_start   = @_[2];
  my $i_len     = @_[3];
  my $len =0;

  $len = length($$self);
  if(($i_start + $i_len) > $len) {
    printf( "**** ERROR : ecmdDataBuffer.pm::extract: start + len %d > NumBits (%d)\n", $i_start + $i_len, $len);
  }
  else {
#		$o_bufferOut = substr($iv_DataStr,$i_start,$i_len); doesn't work this way
    @_[1] = substr($$self,$i_start,$i_len);
  }
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
  my $self = $_[0];  # Me
  my $i_bufferIn = @_[1];
  my $i_startbit = @_[2];
  my $i_len      = @_[3];
  my $len =0;
  my $looper =0;

  $len = length($$self);
  if(($i_startbit + $i_len) > $len) {
    printf( "**** ERROR : ecmdDataBuffer.pm::setOr: startbit + len %d > NumBits (%d)\n", $i_startbit + $i_len, $len);
  }
  else {
    for($looper=0; $looper< $i_len; $looper++) {
      if(substr($i_bufferIn,$looper,1) == '1') {
        substr($$self,$i_startbit + $looper,1) = '1';
      }
    }
  }
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
  my $self = $_[0];  # Me
  my $i_bufferIn = @_[1];
  my $looper =0;

  if(length($i_bufferIn) != length($$self)) {
    printf( "**** ERROR : ecmdDataBuffer.pm::merge: NumBits in (%d) do not match NumBits (%d)\n", length($i_bufferIn), length($$self));
  }
  else {
    for($looper=0; $looper < length($$self); $looper++) {
      if(substr($i_bufferIn,$looper,1) == '1') {
        substr($$self,$looper,1) = '1';
      }
    }
  }
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
  my $self = $_[0];  # Me
  my $i_bufferIn = @_[1];
  my $i_startbit = @_[2];
  my $i_len      = @_[3];
  my $len =0;
  my $looper =0;

  $len = length($$self);
  if(($i_startbit + $i_len) > $len) {
    printf( "**** ERROR : ecmdDataBuffer.pm::setAnd: startbit + len %d > NumBits (%d)\n", $i_startbit + $i_len, $len);
  }
  else {
    for($looper=0; $looper< $i_len; $looper++) {
      if(substr($i_bufferIn,$looper,1) == '0') {
        substr($$self,$i_startbit + $looper,1) = '0';
      }
    }
  }
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
  my $self = $_[0];  # Me
  @_[1] = $$self;
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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
#  std::string genHexLeftStr(int i_start, int i_bitlen);
#  std::string genHexLeftStr();
  my $i_start   = @_[1];
  my $i_bitlen  = @_[2];
  my $len       = 0;
  my $hexStr    = "";
  my $remainder = 0;
  my $nibbles   = 0;
  my $looper    = 0;
  my $thisNible = 0;
  my $hexCnt    = 0;

  my @theBin = ("0000","0001","0010","0011","0100","0101","0110","0111",
                "1000","1001","1010","1011","1100","1101","1110","1111");
  my @theHex = ("0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F");


  $len = length($$self);
  if(($i_start + $i_bitlen) > $len) {
    printf( "**** ERROR : ecmdDataBuffer.pm::genHexLeftStr: start + len %d > NumBits (%d)\n", $i_start + $i_bitlen, $len);
  }
	
  if ($self->hasXstate($i_start, $i_bitlen)) {
    printf("**** ERROR : ecmdDataBuffer.pm::genHexLeftStr: Cannot extract when non-binary (X-State) character present\n");
    return "-1";
  }

  $remainder = $i_bitlen %4;
  $nibbles   = ($i_bitlen-$remainder) /4;

  for($looper=0; $looper < $nibbles; $looper++) {
    $thisNible = substr($$self,$i_start + $looper*4,4);
    for($hexCnt=0; $hexCnt <16; $hexCnt++) {
      if( $thisNible eq $theBin[$hexCnt]) {
        $hexStr = $hexStr . $theHex[$hexCnt];
      }
    }
  }
	#hex string should now have all of the nibbles, we just need the remaining bits 
	#easy and cheesey way is to do a big if+else method.
  if($remainder == 1) {
		#looper is alread at the next nibble count.
    $thisNible = substr($$self,$i_start + $looper*4,1);
    if($thisNible eq "0") {
      $hexStr = $hexStr . "0";
    } elsif ($thisNible eq "1") {
      $hexStr = $hexStr . "8";
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::genHexLeftStr: Data is non binary data at offset %d\n", $looper*4);
      return -1;
    }
  } elsif ($remainder == 2) {
    #looper is alread at the next nibble count.
    $thisNible = substr($$self,$i_start + $looper*4,2);
    if($thisNible eq "00") {
      $hexStr = $hexStr . "0";
    } elsif ($thisNible eq "01") {
      $hexStr = $hexStr . "4";
    } elsif ($thisNible eq "10") {
      $hexStr = $hexStr . "8";
    } elsif ($thisNible eq "11") {
      $hexStr = $hexStr . "C";
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::genHexLeftStr: Data is non binary data at offset %d\n", $looper*4);
      return -1;
    }
  } elsif ($remainder ==3) {
		#looper is alread at the next nibble count.
    $thisNible = substr($$self,$i_start + $looper*4,3);
    if($thisNible eq "000") {
      $hexStr = $hexStr . "0";
    } elsif ($thisNible eq "001") {
      $hexStr = $hexStr . "2";
    } elsif ($thisNible eq "010") {
      $hexStr = $hexStr . "4";
    } elsif ($thisNible eq "011") {
      $hexStr = $hexStr . "6";
    } elsif ($thisNible eq "100") {
      $hexStr = $hexStr . "8";
    } elsif ($thisNible eq "101") {
      $hexStr = $hexStr . "A";
    } elsif ($thisNible eq "110") {
      $hexStr = $hexStr . "C";
    } elsif ($thisNible eq "111") {
      $hexStr = $hexStr . "E";
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::genHexLeftStr: Data is non binary data at offset %d\n", $looper*4);
      return -1;
    }
  } else {
    #must be 0 so do nothing extra.
  }

  return $hexStr;
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
  my $self = $_[0];  # Me
#  std::string genHexRightStr(int i_start, int i_bitlen); 
#  std::string genHexRightStr(); 
  my $i_start   = @_[1];
  my $i_bitlen  = @_[2];
  my $len       = 0;
  my $hexStr    = "";
  my $remainder = 0;
  my $nibbles   = 0;
  my $looper    = 0;
  my $thisNible = 0;
  my $hexCnt    = 0;

  my @theBin = ("0000","0001","0010","0011","0100","0101","0110","0111",
                  "1000","1001","1010","1011","1100","1101","1110","1111");
  my @theHex = ("0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F");


  $len = length($$self);
  if(($i_start + $i_bitlen) > $len) {
    printf( "**** ERROR : ecmdDataBuffer.pm::genHexrightStr: start + len %d > NumBits (%d)\n", $i_start + $i_bitlen, $len);
  }
	
  if ($self->hasXstate($i_start, $i_bitlen)) {
    printf("**** ERROR : ecmdDataBuffer.pm::genHexRightStr: Cannot extract when non-binary (X-State) character present\n");
    return "-1";
  }

  $remainder = $i_bitlen %4;
  $nibbles   = ($i_bitlen-$remainder) /4;

#	for($looper=0; $looper < $nibbles; $looper++) {
  while ($nibbles) {
    $thisNible = substr($$self,$i_start + $nibbles*4 + $remainder -4,4);
    for($hexCnt=0; $hexCnt <16; $hexCnt++) {
      if( $thisNible eq $theBin[$hexCnt]) {
        $hexStr = $theHex[$hexCnt] . $hexStr;
      }
    }
    $nibbles--;
  }
  #hex string should now have all of the nibbles, we just need the remaining bits 
  #easy and cheesey way is to do a big if+else method.
  if($remainder == 1) {
    #looper is alread at the next nibble count.
    $thisNible = substr($$self,$i_start,1);
    if($thisNible eq "0") {
      $hexStr = "0" . $hexStr;
    } elsif ($thisNible eq "1") {
      $hexStr = "1" . $hexStr;
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::genHexRightStr: Data is non binary data near offset %d\n", $i_start);
      return -1;
    }
  } elsif ($remainder == 2) {
    #looper is alread at the next nibble count.
    $thisNible = substr($$self,$i_start,2);
    if($thisNible eq "00") {
      $hexStr =  "0" . $hexStr;
    } elsif ($thisNible eq "01") {
      $hexStr =  "1" . $hexStr;
    } elsif ($thisNible eq "10") {
      $hexStr =  "2" . $hexStr;
    } elsif ($thisNible eq "11") {
      $hexStr =  "3" . $hexStr;
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::genHexRightStr: Data is non binary data near offset %d\n", $i_start);
      return -1;
    }
  } elsif ($remainder ==3) {
    #looper is alread at the next nibble count.
    $thisNible = substr($$self,$i_start,3);
    if($thisNible eq "000") {
      $hexStr =  "0" . $hexStr;
    } elsif ($thisNible eq "001") {
      $hexStr =  "1" . $hexStr;
    } elsif ($thisNible eq "010") {
      $hexStr =  "2" . $hexStr;
    } elsif ($thisNible eq "011") {
      $hexStr =  "3" . $hexStr;
    } elsif ($thisNible eq "100") {
      $hexStr =  "4" . $hexStr;
    } elsif ($thisNible eq "101") {
      $hexStr =  "5" . $hexStr;
    } elsif ($thisNible eq "110") {
      $hexStr =  "6" . $hexStr;
    } elsif ($thisNible eq "111") {
      $hexStr =  "7" . $hexStr;
    } else {
      printf("**** ERROR : ecmdDataBuffer.pm::genHexRightStr: Data is non binary data near offset %d\n", $i_start);
      return -1;
    }
  } else {
    #must be 0 so do nothing extra.
  }

  $hexStr = "0x" . $hexStr;
  return $hexStr;
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
  my $self = $_[0];  # Me
#  std::string genBinStr(int i_start, int i_bitlen); 
#  std::string genBinStr(); 
  my $i_start  = @_[1];
  my $i_bitlen = @_[2];
  my $len      = 0;

  if ($self->hasXstate($i_start, $i_bitlen)) {
    printf("**** WARNING : ecmdDataBuffer.pm::genBinStr: Cannot extract when non-binary (X-State) character present\n");
  }

  if($i_bitlen) {
#we have a start & bitlen so do the calculation.
    my $ext;
    $len = length($$self);
    if(($i_start + $i_bitlen) > $len) {
      printf( "**** ERROR : ecmdDataBuffer.pm::genBinStr: start + len %d > NumBits (%d)\n", $i_start + $i_bitlen, $len);
    }
    else {
      $ext = substr($$self,$i_start,$i_bitlen);
    }
    return $ext;
  } 
  else {
#we don't have any parameters, so return the whole string.
    return $$self;
  }
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
  my $self = $_[0];  # Me
#  std::string genXstateStr(int i_start, int i_bitlen);
#  std::string genXstateStr();
  my $i_start  = @_[1];
  my $i_bitlen = @_[2];
  my $len      = 0;

  if($i_bitlen) {
#we have a start & bitlen so do the calculation.
    my $ext;
    $len = length($$self);
    if(($i_start + $i_bitlen) > $len) {
      printf( "**** ERROR : ecmdDataBuffer.pm::genXstateStr: start + len %d > NumBits (%d)\n", $i_start + $i_bitlen, $len);
    }
    else {
      $ext = substr($$self,$i_start,$i_bitlen);
    }
    return $ext;
  } 
  else {
#we don't have any parameters, so return the whole string.
    return $$self;
  }
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
  my $self = $_[0];  # Me
#  int insertFromHexLeft (const char * i_hexChars, int i_start = 0, int i_length = 0);
  my $i_hexChars = @_[1];
  my $i_start    = @_[2];
  my $i_bitlen   = @_[3];
  my $len        = 0;
  my $remainder  = 0;
  my $nibbles    = 0;
  my $looper     = 0;
  my $thisNible  = 0;
  my $hexCnt     = 0;
  my $newBin     = "";
  my $tmpNewBin  = "";

  my @theBin = ("0000","0001","0010","0011","0100","0101","0110","0111",
                  "1000","1001","1010","1011","1100","1101","1110","1111");
  my @theHex = ("0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F");


  $len = length($$self);
  if(($i_start + $i_bitlen) > $len) {
    printf( "**** ERROR : ecmdDataBuffer.pm::insertFromHexLeft: start + len %d > NumBits (%d)\n", $i_start + $i_bitlen, $len);
    return -1;
  }

  $remainder = $i_bitlen %4;
  $nibbles   = ($i_bitlen-$remainder) /4;

  if($remainder) {
    # this will handle remainder
    $nibbles++;
  }
	
  $len = length($i_hexChars);
  if($len < $nibbles) {
    printf( "**** ERROR : ecmdDataBuffer.pm::insertFromHexLeft: Not enough data provided according to desired length\n");
    return -1;
  }


  for($looper=0; $looper < $nibbles; $looper++) {
    $thisNible = substr($i_hexChars, $looper,1);
    for($hexCnt=0; $hexCnt <16; $hexCnt++) {
      if( $thisNible eq $theHex[$hexCnt]) {
        $newBin = "$newBin" . "$theBin[$hexCnt]";
      }
    }
  }

  #remember newBin may be to big due to remainder bits, so use i_bitlen
  $newBin = substr($newBin, 0, $i_bitlen);
  substr($$self, $i_start, $i_bitlen) = $newBin;
  return 0;
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
  my $self = $_[0];  # Me
#  int insertFromHexRight (const char * i_hexChars, int i_start = 0, int i_expectedLength = 0);
  my $i_hexChars = @_[1];
  my $i_start    = @_[2];
  my $i_bitlen   = @_[3];
  my $len        = 0;
  my $remainder  = 0;
  my $nibbles    = 0;
  my $looper     = 0;
  my $thisNible  = 0;
  my $hexCnt     = 0;
  my $newBin     = "";
  my $tmpNewBin  = "";
  my $offset     = 0;

  my @theBin = ("0000","0001","0010","0011","0100","0101","0110","0111",
                "1000","1001","1010","1011","1100","1101","1110","1111");
  my @theHex = ("0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F");


  $len = length($$self);
  if(($i_start + $i_bitlen) > $len) {
    printf( "**** ERROR : ecmdDataBuffer.pm::insertFromHexRight: start + len %d > NumBits (%d)\n", $i_start + $i_bitlen, $len);
    return -1;
  }

  $remainder = $i_bitlen %4;
  $nibbles   = ($i_bitlen-$remainder) /4;

  if($remainder) {
    # this will handle remainder
    $nibbles++;
  }
	
  $len = length($i_hexChars);
  if($len < $nibbles) {
    printf( "**** ERROR : ecmdDataBuffer.pm::insertFromHexRight: Not enough data provided according to desired length\n");
    return -1;
  }

  $offset = length($i_hexChars);

  for($looper=0; $looper < $nibbles; $looper++) {
    $thisNible = substr($i_hexChars, $offset-$looper,1);
    for($hexCnt=0; $hexCnt <16; $hexCnt++) {
      if( $thisNible eq $theHex[$hexCnt]) {
        $newBin = "$theBin[$hexCnt]" . "$newBin";
      }
    }
  }

  #remember newBin may be to big due to remainder bits, so use i_bitlen
  if($remainder == 1) {
    $offset =3;
  } elsif ($remainder == 2) {
    $offset =2;
  } elsif ($remainder == 3) {
    $offset =1;
  } else {
    #this should never happen
  }

  $newBin = substr($newBin, $offset, $i_bitlen);
  substr($$self, $i_start, $i_bitlen) = $newBin;
  return 0;

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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
#  int   hasXstate(int i_start, int i_length); /* check subset */
#  int   hasXstate(); 

  my $i_start  = @_[0];
  my $i_length = @_[1];
  my $len    =0;
  my $looper =0;
  my $val =0;
  my $rc =0;

  $len = length($$self);

  if($i_length) {
# we have both parameters
    if($len < ($i_start + $i_length)) {
      printf( "**** ERROR : ecmdDataBuffer.pm::hasXstate: start + len %d > NumBits (%d)\n", $i_start + $i_length, $len);
    } else {
      for($looper=0; $looper < $i_length; $looper++) {
        $val = substr($$self,$looper,1);
        if( !($val eq "0") && !($val eq "1") ) {
          $rc = 1;
          return $rc;
        }
      }
      return 0;
    }
  } 
  else {
    #no parameters so check the whole thing.
    for($looper=0;$looper < $len; $looper++) {
      $val = substr($$self,$looper,1);
      if( !($val eq "0") && !($val eq "1") ) {
        $rc = 1;
        return $rc;
      }
    }
    return 0;
  }
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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
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
  my $self = $_[0];  # Me
#  void  memCopyOutXstate(char * o_buf, int i_bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */
}



sub getstr {
  # Error Checking
  if ($#_ > 0) {
    # error
  }

  # Get Args
  my ($self) = $_[0];

  # Do work
  return $$self;
}

sub setstr {
  # Error Checking
  if ($#_ > 1) {
    # error
  }

  # Get Args
  my ($self) = $_[0];  # Me
  $$self = $_[1]; # The data the passed
}


1;

