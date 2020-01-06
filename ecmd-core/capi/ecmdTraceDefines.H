//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
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

#ifndef ecmdTraceDefines_H 
#define ecmdTraceDefines_H

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <stdarg.h>
#include <string>

/**
 * @file ecmdTraceDefines.H
 * @brief Provides a defined set of trace functions for ecmdDataBuffer
 *
 * This header defines the default set ecmdDataBuffer trace functions
 * It can be overridden by any plugin to fit their environment
*/

// The __PRETTY_FUNCTION__ macro isn't all that pretty
// This inline function cleans it up into the class::function desired format
// If not called from a class, it also handles that and gives just the function
//std::string methodName(const std::string & prettyFunction) {
//  size_t begin, end;
//  size_t colons = prettyFunction.find("::");
//  if (colons == std::string::npos) {
//    begin = prettyFunction.find(" ") + 1;
//    end = prettyFunction.find("(") - begin;
//  } else {
//    begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
//    end = prettyFunction.rfind("(") - begin;
//  }
//
//  return prettyFunction.substr(begin,end);
//}

//#define FUNCNAME methodName(__PRETTY_FUNCTION__).c_str()

//#define ETRAC(...) printf(__VA_ARGS__);
//#define ETRAC(errmsg, ...) printf((std::string("ERROR: (%s): ") + errmsg).c_str(), __FUNCTION__, ##__VA_ARGS__);
#define ETRAC(errmsg, ...) printf("ERROR: (%s): " errmsg, __PRETTY_FUNCTION__, ##__VA_ARGS__);

//#define ETRAC0(fmt) printf( "%s> ETRC: " fmt "\n", __FUNCTION__);

#endif
