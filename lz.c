#include "lz.h"

#ifdef DEBUG_MSG
#include <stdio.h>

#define pr_info(fmt, ...) fprintf(stdout, fmt "\n", ##__VA_ARGS__)
#define pr_err(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define pr_info(...)
#define pr_err(...)
#endif /* DEBUG_MSG */

#pragma pack(push, 1) // there is a clang warn here, seems clangd bug
typedef struct Header {
  uint32_t szIn;
  uint8_t width;
} Header_t;
#pragma pack(pop)
_Static_assert(5 == sizeof(Header_t), "Wrong alignment");

#pragma pack(push, 1)
typedef struct Body {
  struct {
    uint8_t lo;
    uint8_t hi;
  } distlen; // distance and length to match, lower @width bits it len
  uint8_t symbol;
} Body_t;
#pragma pack(pop)
_Static_assert(3 == sizeof(Body_t), "Wrong alignment");

typedef struct EncodedData {
  Header_t header;
  Body_t body[0];
} EncodedData_t;

uint32_t encode(const uint8_t *in, uint32_t szIn, uint8_t *out, uint8_t wid) {
  EncodedData_t *outdat = (EncodedData_t *)out;

  const uint16_t maxDistMatch = 1 << ((8 * sizeof(uint16_t)) - wid);
  const uint16_t maxLenMatch = 1 << (wid);

  // put raw data size and @len width in the output header
  outdat->header.szIn = szIn;
  outdat->header.width = wid;

  // real compressed data begin
  uint32_t ofsSym, ofsOut = 0;
  for (uint32_t ofsIn = 0; ofsIn < szIn; ++ofsIn, ++ofsOut) {
    uint16_t distMatch = 0, lenMatch = 0, dist_len;

    // check search buffer
    for (uint16_t dm = 1, lm; (dm < maxDistMatch) && (dm <= ofsIn); ++dm) {
      uint32_t ofsInNew = ofsIn;
      uint32_t ofsMatch = ofsIn - dm;

      // check match length
      for (lm = 0; ofsInNew < szIn && in[ofsInNew++] == in[ofsMatch++]; ++lm) {
        if (lm == maxLenMatch)
          break;
      }

      // may have multiple repeat substring, choose better match
      if (lm > lenMatch) {
        distMatch = dm;
        lenMatch = lm;

        // best case found
        if (lenMatch == maxLenMatch)
          break;
      }
    }

    // construct dist|len, the lower @wid bits is len
    ofsIn += lenMatch;
    if ((ofsIn == szIn) && lenMatch) {
      dist_len = (lenMatch == 1) ? 0 : ((distMatch << wid) | (lenMatch - 2));
      ofsSym = ofsIn - 1;
    } else {
      dist_len = (distMatch << wid) | (lenMatch ? (lenMatch - 1) : 0);
      ofsSym = ofsIn;
    }

    // write data body
    outdat->body[ofsOut].distlen.lo = dist_len & 0xFF;
    outdat->body[ofsOut].distlen.hi = (dist_len >> 8) & 0xFF;
    outdat->body[ofsOut].symbol = *(in + ofsSym);
  }
  return sizeof(Header_t) + sizeof(Body_t) * ofsOut;
}

uint32_t decode(const uint8_t *in, uint8_t *out) {
  const EncodedData_t *indat = (EncodedData_t *)in;

  uint8_t wid = indat->header.width;
  uint32_t szOut = indat->header.szIn;
  uint16_t dist_len_mask = (1 << wid) - 1;

  // start decoding body
  uint32_t ofsOut = 0;
  for (uint32_t ofsIn = 0; ofsOut < szOut; ++ofsIn, ++ofsOut) {
    uint16_t lenMatch, distMatch, dist_len;

    *((uint8_t *)&dist_len) = indat->body[ofsIn].distlen.lo;
    *((uint8_t *)&dist_len + 1) = indat->body[ofsIn].distlen.hi;

    // copy repeated data from window
    distMatch = dist_len >> wid;
    lenMatch = distMatch ? ((dist_len & dist_len_mask) + 1) : 0;
    if (distMatch)
      for (uint32_t ofsMatch = ofsOut - distMatch; lenMatch > 0; --lenMatch)
        out[ofsOut++] = out[ofsMatch++];
    *(out + ofsOut) = indat->body[ofsIn].symbol;
  }

  return ofsOut;
}
