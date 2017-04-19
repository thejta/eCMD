#ifndef OutputLite_h
#define OutputLite_h
//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2017 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------

#include <cstdlib>
#include <cstdarg>
#include <inttypes.h>
#include <string>

#ifdef _LP64
#define UINT64_HEX_VARIN_FORMAT(_PRESC_) #_PRESC_ "lX"
#define UINT64_HEX_VARIN_FORMAT_LC(_PRESC_) #_PRESC_ "lx"
#define UINT64_DEC_VARIN_FORMAT(_PRESC_) #_PRESC_ "ld"
#else
#define UINT64_HEX_VARIN_FORMAT(_PRESC_) #_PRESC_ "llX"
#define UINT64_HEX_VARIN_FORMAT_LC(_PRESC_) #_PRESC_ "llx"
#define UINT64_DEC_VARIN_FORMAT(_PRESC_) #_PRESC_ "lld"
#endif

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

class OutputLite
{
public:
  OutputLite();
  ~OutputLite();

  /* PRINT */
  void print(const char* printMsg, ...);

  /* ERROR */
  uint32_t error(uint32_t rc, std::string functionName, const char* errMsg, ...);
  void error(std::string functionName, const char* errMsg, ...);

  /* WARNING */
  void warning(std::string functionName, const char* warnMsg, ...);

  /* NOTE */
  void note(std::string functionName, const char* noteMsg, ...);

private:
  uint32_t error(uint32_t rc, const char* functionName, const char* errMsg, va_list &arg_ptr);
  void warning(const char* functionName, const char* warnMsg, va_list &arg_ptr);
  void note(const char* functionName, const char* noteMsg, va_list &arg_ptr);
};

#endif /* OutputLite_h */

