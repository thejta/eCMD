//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

#ifndef ecmdDataBufferTrace_H 
#define ecmdDataBufferTrace_H

/**
 * @file ecmdDataBufferTrace.H
 * @brief Provides a defined set of trace functions for ecmdDataBuffer
 *
 * This header defines the default set ecmdDataBuffer trace functions
 * It can be overridden by any plugin to fit their environment
*/

#define ETRAC0(fmt) printf( "%s> ETRC: " fmt "\n", __FUNCTION__);
#define ETRAC1(fmt, arg1) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1);
#define ETRAC2(fmt, arg1, arg2) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2);
#define ETRAC3(fmt, arg1, arg2, arg3) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3);
#define ETRAC4(fmt, arg1, arg2, arg3, arg4) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4);
#define ETRAC5(fmt, arg1, arg2, arg3, arg4, arg5) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5);
#define ETRAC6(fmt, arg1, arg2, arg3, arg4, arg5, arg6) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6);
#define ETRAC7(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
#define ETRAC8(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
#define ETRAC9(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

#define PTRAC0(fmt) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__); \
  ecmdOutput(buffer);}
#define PTRAC1(fmt, arg1) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1); \
  ecmdOutput(buffer);}
#define PTRAC2(fmt, arg1, arg2) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2); \
  ecmdOutput(buffer);}
#define PTRAC3(fmt, arg1, arg2, arg3) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2, arg3); \
  ecmdOutput(buffer);}
#define PTRAC4(fmt, arg1, arg2, arg3, arg4) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2, arg3, \
  arg4); \
  ecmdOutput(buffer);}
#define PTRAC5(fmt, arg1, arg2, arg3, arg4, arg5) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5); \
  ecmdOutput(buffer);}
#define PTRAC6(fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6); \
  ecmdOutput(buffer);}
#define PTRAC7(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6, arg7); \
  ecmdOutput(buffer);}
#define PTRAC8(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6, arg7, arg8); \
  ecmdOutput(buffer);}
#define PTRAC9(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: " fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6, arg7, arg8, arg9); \
  ecmdOutput(buffer);}


#endif
