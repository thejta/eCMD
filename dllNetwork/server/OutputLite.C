//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <string>
#include <OutputLite.H>

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

OutputLite::OutputLite() {
}

OutputLite::~OutputLite() {
}

void OutputLite::print(const char* printMsg, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, printMsg);

  vprintf(printMsg, arg_ptr);

  va_end(arg_ptr);
}

uint32_t OutputLite::error(uint32_t rc, std::string functionName, const char* errMsg, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, errMsg);

  error(rc, functionName.c_str(), errMsg, arg_ptr);

  va_end(arg_ptr);
  return rc;
}

void OutputLite::error(std::string functionName, const char* errMsg, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, errMsg);

  error(0, functionName.c_str(), errMsg, arg_ptr);

  va_end(arg_ptr);
}

uint32_t OutputLite::error(uint32_t rc, const char* functionName, const char* errMsg, va_list &arg_ptr) {
  std::string errString;
  /* Build the string */
  errString = "ERROR: (";
  errString += functionName;
  errString += "): ";
  errString += errMsg;

  /* Now print it */
  vprintf(errString.c_str(), arg_ptr);

  return rc;
}

void OutputLite::warning(std::string functionName, const char* warnMsg, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, warnMsg);

  warning(functionName.c_str(), warnMsg, arg_ptr);

  va_end(arg_ptr);
}

void OutputLite::warning(const char* functionName, const char* warnMsg, va_list &arg_ptr) {
  std::string warnString;
  /* Build the string */
  warnString = "WARNING: (";
  warnString += functionName;
  warnString += "): ";
  warnString += warnMsg;

  /* Now print it */
  vprintf(warnString.c_str(), arg_ptr);
}

void OutputLite::note(std::string functionName, const char* noteMsg, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, noteMsg);

  note(functionName.c_str(), noteMsg, arg_ptr);

  va_end(arg_ptr);
}

void OutputLite::note(const char* functionName, const char* noteMsg, va_list &arg_ptr) {
  std::string noteString;
  /* Build the string */
  noteString = "NOTE: (";
  noteString += functionName;
  noteString += "): ";
  noteString += noteMsg;

  /* Now print it */
  vprintf(noteString.c_str(), arg_ptr);
}
