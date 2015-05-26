/*
 * bitstream.h: generic bitstream parsing
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: bitstream.h,v 1.3 2009-02-14 20:44:15 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_BITSTREAM_H_
#define _XINELIBOUTPUT_BITSTREAM_H_


# ifdef NOCACHE

typedef struct {
  const uint8_t *data;
  int            count; /* in bits */
  int            index; /* in bits */
} br_state;

#define BR_INIT(data,bytes) { (data), 8*(bytes), 0 }

#define BR_EOF(br) ((br)->index >= (br)->count)

static inline void br_init(br_state *br, const uint8_t *data, int bytes)
{
  br->data  = data;
  br->count = 8*bytes;
  br->index = 0;
}

static inline int br_get_bit(br_state *br)
{
  if(br->index >= br->count)
    return 1; /* -> no infinite colomb's ... */

  int r = (br->data[br->index>>3] >> (7 - (br->index&7))) & 1;
  br->index++;
  return r;
}

static inline uint32_t br_get_bits(br_state *br, uint32_t n)
{
  uint32_t r = 0;
  while(n--)
    r = r | (br_get_bit(br) << n);
  return r;
}

#define br_skip_bit(br) br_skip_bits(br,1)

static inline void br_skip_bits(br_state *br, int n)
{
  br->index += n;
}


# else /* NOCACHE */


typedef struct {
  uint8_t *data;
  uint8_t *data_end;
  uint32_t cache; 
  uint32_t cache_bits;
} br_state;

#define BR_INIT(data,bytes) { (data), (data)+(bytes), 0, 0 }

static inline void br_init(br_state *br, const uint8_t *data, int bytes)
{
  br->data       = data;
  br->data_end   = data + bytes;
  br->cache      = 0;
  br->cache_bits = 0;
}

#define BR_GET_BYTE(br) \
   (br->data < br->data_end ? *br->data++ : 0xff)

#define BR_EOF(br) ((br)->data >= (br)->data_end)

static inline uint32_t br_get_bits(br_state *br, uint32_t n)
{
  if(n > 24)
    return (br_get_bits(br, 16) << 16) | br_get_bits(br, n-16);

  while (br->cache_bits < 24) {
    br->cache = (br->cache<<8) | BR_GET_BYTE(br);
    br->cache_bits += 8;
  }

  br->cache_bits -= n;
  return (br->cache >> br->cache_bits) & ((1<<n) - 1);
}

static inline int br_get_bit(br_state *br)
{
  if(!br->cache_bits) {
    br->cache = BR_GET_BYTE(br);
    br->cache_bits = 7;
  } else {
    br->cache_bits--;
  }
  return (br->cache >> br->cache_bits) & 1;
}

static inline void br_skip_bit(br_state *br)
{
  if(!br->cache_bits) {
    br->cache = BR_GET_BYTE(br);
    br->cache_bits = 7;
  } else {
    br->cache_bits--;
  }
}

static inline void br_skip_bits(br_state *br, int n)
{
  if(br->cache_bits >= n) {
    br->cache_bits -= n;
  } else {
    /* drop cached bits */
    n -= br->cache_bits;
  
    /* drop full bytes */
    br->data += (n >> 3);
    n &= 7;

    /* update cache */
    if(n) {
      br->cache = BR_GET_BYTE(br);
      br->cache_bits = 8 - n;
    } else {
      br->cache_bits = 0;
    }
  }
}
 

# endif /* NOCACHE */


#define br_get_u8(br)         br_get_bits(br, 8)
#define br_get_u16(br)        ((br_get_bits(br, 8)<<8) | br_get_bits(br, 8))

static inline uint32_t br_get_ue_golomb(br_state *br)
{
  int n = 0;
  while (!br_get_bit(br) && n < 32)
    n++;
  return n ? ((1<<n) - 1) + br_get_bits(br, n) : 0;
}

static inline int32_t br_get_se_golomb(br_state *br)
{
  uint32_t r = br_get_ue_golomb(br) + 1;
  return (r&1) ? -(r>>1) : (r>>1);
}

static inline void br_skip_golomb(br_state *br)
{
  int n = 0;
  while (!br_get_bit(br) && n < 32)
    n++;
  br_skip_bits(br, n);
}

#define br_skip_ue_golomb(br) br_skip_golomb(br)
#define br_skip_se_golomb(br) br_skip_golomb(br)


#endif /* _XINELIBOUTPUT_BITSTREAM_H_ */
