/***************************************************
 * This is part of LGPL AA-GFX library 
 * 
 * This library is Copyright (c) 2016, Allterco, Inc.  All rights reserved
 * 
 ****************************************************/

/**********
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 **********/


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
typedef unsigned (*rle_read_t) (rle_decoding_t *dec, void *buf, unsigned bytes); //return number fo bytes actually red. less than requested signals error in reading
struct rle_decoding_tag {
	void *userdata;
	rle_read_t rle_read;
	uint16_t len;
	uint8_t hb;
	uint8_t lb;
	bool is_compressing;
};

void rle_decoding_init(rle_decoding_t *dec, rle_read_t rle_read,void *userdata);

//return actual number of words written
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

typedef void (*flushio_t) (rle_encoding_t *enc, void *buf, unsigned bytes); //writeout given bytes


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

