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

/**
 * @file ecmdTraceDefines.H
 * @brief Provides a defined set of trace functions for ecmdDataBuffer
 *
 * This header defines the default set ecmdDataBuffer trace functions
 * It can be overridden by any plugin to fit their environment
*/

// This define sticks an ERROR and function name at the start, then uses the
// errmsg and varargs passed in to print the message
#define ETRAC(errmsg, ...) printf("ERROR: (%s): " errmsg"\n", __FUNCTION__, ##__VA_ARGS__);

#endif
