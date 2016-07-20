#ifndef GFX_RLE_H_INCLUDED
#define GFX_RLE_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#define RLE_IS_COMPRESSED 0x80
#define RLE_IS_LONG 0x40
#define RLE_LENSTRIP (~(RLE_IS_COMPRESSED|RLE_IS_LONG))
#define RLE_MAXSHORTLEN ((uint8_t) RLE_LENSTRIP )
#define RLE_MAXLONGLEN  (RLE_MAXSHORTLEN<<8|0xff)

#ifdef RLE_H_EXPORT_DECODING
typedef struct rle_decoding_tag rle_decoding_t;
typedef unsigned (*rle_read_t) (rle_decoding_t *dec, void *buf, unsigned bytes);
struct rle_decoding_tag {
	void *userdata;
	rle_read_t rle_read;
	uint16_t len;
	uint8_t hb;
	uint8_t lb;
	bool is_compressing;
};

void rle_decoding_init(rle_decoding_t *dec, rle_read_t rle_read,void *userdata);
unsigned rle_decoding_read(rle_decoding_t *dec, void *buf, unsigned words);

#endif //RLE_H_EXPORT_DECODING

#ifdef RLE_H_EXPORT_ENCODING

typedef struct  {
	uint8_t *start;
	uint8_t *end;
	uint16_t len;
	uint16_t lastvalue;
	bool shortstored;
	bool is_compressing;
} rleseq_t;

typedef struct rle_encoding_tag rle_encoding_t;

typedef void (*flushio_t) (rle_encoding_t *enc, void *buf, unsigned bytes);


struct rle_encoding_tag {
	void *userdata;
	uint8_t *iobuf;
	uint8_t *iobuf_end;
	flushio_t flushio;
	rleseq_t cseq;
};

void rle_encoding_init(rle_encoding_t *enc, void* buf, unsigned bufsz,flushio_t flushio,void *userdata);
void rle_encoding_add(rle_encoding_t *enc, uint16_t data, bool flushblock);
void rle_encoding_flush(rle_encoding_t *enc);
#endif

#endif//GFX_RLE_H_INCLUDED

