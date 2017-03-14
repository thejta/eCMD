#ifndef OutputLite_h
#define OutputLite_h
//IBM_PROLOG_BEGIN_TAG
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

