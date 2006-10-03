#ifndef ecmdBit64_H
#define ecmdBit64_H

// Copyright **********************************************************
//                                                                      
// File ecmdBit64.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************

/* $Header$ */

/**
 * @file ecmdBit64.H
 * @brief Provides a means to handle 64 bit numbers with the eCMD PerlApi
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <inttypes.h>

#include <ecmdDefines.H>


#ifdef ENABLE_MPATROL
 #include <mpatrol.h>
#endif


/**
@brief Provides a means to handle 64 bit numbers with the eCMD PerlApi.
 All numbers treated as 64 bit hex right aligned values.
*/
class ecmdBit64 {

public:

  /**
     @brief Default Constructor, initial value is 0
  */
  ecmdBit64();
  /**
     @brief Constructor, init to string value
     @param i_strData String value to init, ie ('0xFFEE', '1234')
  */
  ecmdBit64(const char* i_strData);
  /**
     @brief Constructor, init with another ecmdBit64
     @param i_newValue Value to initialze to
  */
  ecmdBit64(const ecmdBit64 & i_newValue);
  /**
     @brief Constructor, init with integer value (only works for values < 32bits long
     @param i_newValue 32 bit value to initialize to
  */
  ecmdBit64(uint32_t i_newValue);
  /**
     @brief Constructor, init high and low word with integer values
     @param i_hiWord Integer value to place in high(left) word (bits 0-31)
     @param i_loWord Integer value to place in low(right) word (bits 32-63)
  */
  ecmdBit64(uint32_t i_hiValue, uint32_t i_loValue);

  /**
     @brief Copy the contents of this ecmdBit64 into another
     @param o_copy Value copied into this class
  */
  void copy(ecmdBit64 & o_copy) { o_copy.iv_value = iv_value; }

  /**
     @brief Set with string value
     @param i_strData String value to init, ie ('0xFFEE', '1234')
  */
  void setValue(const char* i_strData);
  /**
     @brief Set with another ecmdBit64
     @param i_newValue Value to initialize to
  */
  void setValue(const ecmdBit64 & i_newValue);
  /**
     @brief Set with integer value
     @param i_newValue Value to initialize to
  */
  void setValue(uint32_t i_newValue);
  /**
     @brief Set high and low word with integer values
     @param i_hiWord Integer value to place in high(left) word (bits 0-31)
     @param i_loWord Integer value to place in low(right) word (bits 32-63)
  */
  void setValue(uint32_t i_hiValue, uint32_t i_loValue);

  /**
     @brief Retrieve Hex string value '0xFEEDBEEFB0B0BEEF'
     @param i_alignLen Display width (ex 3 = '0x001' 10 = '0x0000000001')
     @retval 64 bit value in a string
  */
  std::string getHexValue(uint32_t i_alignLen = 1);
  /**
     @brief Retrieve Decimal string value '1234576'
     @retval 64 bit value in a string
  */
  std::string getDecimalValue();
  /**
     @brief Retrive the hi and low word in integer form
     @param o_hiValue High(Left) word, bits (0-31)
     @param o_loWord Low(Right) word, bits (32-63)

     NOTE : To use this interface preset your input variables to a number and pass as a reference<p>
     my $hi = 0;<p>
     my $lo = 0;<p>
     $x->getValue(\$hi, \$lo);<p>
  */
  void getValue(uint32_t & o_hiValue, uint32_t & o_loValue);


#ifndef ECMD_PERLAPI
  /**
     @brief Return the actual uint64_t value, only for swig to work, not available to user
  */
  uint64_t getRawValue() { return iv_value; }
  /**
     @brief Set the actual uint64_t value, only for swig to work
  */
  void setRawValue(uint64_t i_newValue) { iv_value = i_newValue; }
#endif




  /* NOTE: To add operators to this class you also need to update ecmdClientPerlApi.i
     and ecmdClientPerlApi.pl to get them working in swig */






  
  /** @name Operator overloads */
  //@{
#ifndef ECMD_PERLAPI
  /**
     @brief Copy Constructor
     @param i_master Class to copy from
  */
  ecmdBit64& operator=(const ecmdBit64 & i_master);
#endif
  /**
     @brief Operator ==
     @param i_rhs Right-Hand-Side
  */
  int operator == (const ecmdBit64& i_rhs) const;
  /**
     @brief Operator ==
     @param i_rhs Right-Hand-Side
  */
  int operator == (uint32_t i_rhs) const;
  /**
     @brief Operator !=
     @param i_rhs Right-Hand-Side
  */
  int operator != (const ecmdBit64& i_rhs) const;
  /**
     @brief Operator !=
     @param i_rhs Right-Hand-Side
  */
  int operator != (uint32_t i_rhs) const;

  /**
     @brief Operator <
     @param i_rhs Right-Hand-Side
  */
  int operator < (const ecmdBit64 & i_rhs) const;

  /**
     @brief Operator <
     @param i_rhs Right-Hand-Side
  */
  int operator < (uint32_t i_rhs) const;

  /**
     @brief Operator <=
     @param i_rhs Right-Hand-Side
  */
  int operator <= (const ecmdBit64 & i_rhs) const;

  /**
     @brief Operator <=
     @param i_rhs Right-Hand-Side
  */
  int operator <= (uint32_t i_rhs) const;

  /**
     @brief Operator >
     @param i_rhs Right-Hand-Side
  */
  int operator > (const ecmdBit64 & i_rhs) const;

  /**
     @brief Operator >
     @param i_rhs Right-Hand-Side
  */
  int operator > (uint32_t i_rhs) const;

  /**
     @brief Operator >=
     @param i_rhs Right-Hand-Side
  */
  int operator >= (const ecmdBit64 & i_rhs) const;

  /**
     @brief Operator >=
     @param i_rhs Right-Hand-Side
  */
  int operator >= (uint32_t i_rhs) const;




  /**
     @brief Operator +
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator + (const ecmdBit64 & i_rhs) const;
  /**
     @brief Operator +
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator + (uint32_t i_rhs) const;
  /**
     @brief Operator -
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator - (const ecmdBit64 & i_rhs) const;
  /**
     @brief Operator -
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator - (uint32_t i_rhs) const;
  /**
     @brief Operator *
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator * (const ecmdBit64 & i_rhs) const;
  /**
     @brief Operator *
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator * (uint32_t i_rhs) const;
  /**
     @brief Operator /
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator / (const ecmdBit64 & i_rhs) const;
  /**
     @brief Operator /
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator / (uint32_t i_rhs) const;

  /**
     @brief Operator %
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator % (const ecmdBit64 & i_rhs) const;
  /**
     @brief Operator %
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator % (uint32_t i_rhs) const;

  /**
     @brief Operator !
  */
  ecmdBit64 operator ! () const;

  /**
     @brief Operator ~
  */
  ecmdBit64 operator ~ () const;

  /**
     @brief Operator <<
     @param i_shift shift amount
  */
  ecmdBit64 operator << (int i_shift) const;

  /**
     @brief Operator >>
     @param i_shift shift amount
  */
  ecmdBit64 operator >> (int i_shift) const;


  /**
     @brief Operator &
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator & (const ecmdBit64& i_rhs) const;    

  /**
     @brief Operator |
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator | (const ecmdBit64& i_rhs) const;    

  /**
     @brief Operator ^
     @param i_rhs Right-Hand-Side
  */
  ecmdBit64 operator ^ (const ecmdBit64& i_rhs) const;    


  /**
     @brief Operator ++
     @param i_num arg to match typedef
  */
  void operator ++ (int i_num);

  /**
     @brief Operator --
     @param i_num arg to match typedef
  */
  void operator -- (int i_num);

private:
  uint64_t iv_value;
	    
};


#endif /* ecmdBit64_H */
