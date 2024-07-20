#include "lz.h"

#ifdef DEBUG_MSG
#include <stdio.h>

#define pr_info(fmt, ...) fprintf(stdout, fmt "\n", ##__VA_ARGS__)
#define pr_err(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define pr_info(...)
#define pr_err(...)
#endif /* DEBUG_MSG */

// the output data struct: 4B raw size + 1B wid + K*(2B dist|len + 1B symbol)
uint32_t encode(const uint8_t *in, uint32_t szIn, uint8_t *out, uint8_t wid) {
  const uint16_t maxDistMatch = 1 << ((8 * sizeof(uint16_t)) - wid);
  const uint16_t maxLenMatch = 1 << (wid);

  uint16_t dist_len;
  uint32_t outSize, ofsSym;

  // put raw data size and @len width in the output header
  *((uint32_t *)out) = szIn;
  *(out + 4) = wid;

  // real compressed data begin
  outSize = 5;
  for (uint32_t ofsIn = 0; ofsIn < szIn; ++ofsIn) {
    uint16_t distMatch = 0, lenMatch = 0;

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

    // write tuple(dist,len,symbol)
    *(out + outSize) = dist_len & 0xFF;
    *(out + outSize + 1) = (dist_len >> 8) & 0xFF;
    *(out + outSize + 2) = *(in + ofsSym);
    outSize += 3;
  }
  return outSize;
}

uint32_t decode(const uint8_t *in, uint8_t *out) {
  uint8_t wid;
  uint16_t dist_len, lenMatch, distMatch, dist_len_mask;
  uint32_t ofsIn, ofsOut, ofsMatch, szOut;

  szOut = *((uint32_t *)in);
  wid = *(in + 4);
  ofsIn = 5;

  dist_len_mask = (1 << wid) - 1;

  for (ofsOut = 0; ofsOut < szOut; ++ofsOut) {
    *((uint8_t *)&dist_len) = *(in + ofsIn);
    *((uint8_t *)&dist_len + 1) = *(in + ofsIn + 1);

    ofsIn += 2;
    distMatch = dist_len >> wid;
    lenMatch = distMatch ? ((dist_len & dist_len_mask) + 1) : 0;
    if (distMatch)
      for (ofsMatch = ofsOut - distMatch; lenMatch > 0; --lenMatch)
        out[ofsOut++] = out[ofsMatch++];
    *(out + ofsOut) = *(in + ofsIn++);
  }

  return ofsOut;
}
