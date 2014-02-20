/**
 *  @file fapiPlatUtil.H
 *
 *  @brief Defines platform implemented utility functions.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 */

#ifndef FAPIPLATUTIL_H_
#define FAPIPLATUTIL_H_

#ifdef AIX
  #include <sys/machine.h>
  #define bswap_16(x) ( (((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8) )
  #define bswap_32(x) ( (((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24) )
  #define bswap_64(x) ( (((x) & 0xff00000000000000ull) >> 56) | (((x) & 0x00ff000000000000ull) >> 40) | (((x) & 0x0000ff0000000000ull) >> 24) | (((x) & 0x000000ff00000000ull) >> 8) | \
                        (((x) & 0x00000000ff000000ull) << 8) | (((x) & 0x0000000000ff0000ull) << 24) | (((x) & 0x000000000000ff00ull) << 40) | (((x) & 0x00000000000000ffull) << 56) )
#else
  #include <endian.h>
  #include <byteswap.h>
#endif

#ifndef BYTE_ORDER
#error "byte order is not defined"
#endif
#ifndef BIG_ENDIAN
#error "big endian is not defined"
#endif
// Defines for endian convertion 
#if BYTE_ORDER == BIG_ENDIAN
  #define FAPI_PLAT_BE16TOH(x) (x)
  #define FAPI_PLAT_LE16TOH(x) bswap_16(x)
  #define FAPI_PLAT_HTOBE16(x) (x)
  #define FAPI_PLAT_HTOLE16(x) bswap_16(x)

  #define FAPI_PLAT_BE32TOH(x) (x)
  #define FAPI_PLAT_LE32TOH(x) bswap_32(x)
  #define FAPI_PLAT_HTOBE32(x) (x)
  #define FAPI_PLAT_HTOLE32(x) bswap_32(x)

  #define FAPI_PLAT_BE64TOH(x) (x)
  #define FAPI_PLAT_LE64TOH(x) bswap_64(x)
  #define FAPI_PLAT_HTOBE64(x) (x)
  #define FAPI_PLAT_HTOLE64(x) bswap_64(x)
#else
  #define FAPI_PLAT_BE16TOH(x) bswap_16(x)
  #define FAPI_PLAT_LE16TOH(x) (x)
  #define FAPI_PLAT_HTOBE16(x) bswap_16(x)
  #define FAPI_PLAT_HTOLE16(x) (x)

  #define FAPI_PLAT_BE32TOH(x) bswap_32(x)
  #define FAPI_PLAT_LE32TOH(x) (x)
  #define FAPI_PLAT_HTOBE32(x) bswap_32(x)
  #define FAPI_PLAT_HTOLE32(x) (x)

  #define FAPI_PLAT_BE64TOH(x) bswap_64(x)
  #define FAPI_PLAT_LE64TOH(x) (x)
  #define FAPI_PLAT_HTOBE64(x) bswap_64(x)
  #define FAPI_PLAT_HTOLE64(x) (x)
#endif

#include <stdlib.h>
#define fapiPlatMalloc(s) malloc(s)
#define fapiPlatFree(p) free(p)


#endif // FAPIPLATUTIL_H_
