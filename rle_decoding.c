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



#define RLE_H_EXPORT_DECODING
#include "rle.h"
#include <string.h>

void rle_decoding_init(rle_decoding_t *dec, rle_read_t rle_read,void *userdata){
	memset(dec,0,sizeof(*dec));
	dec->userdata=userdata;
	dec->rle_read=rle_read;
}

bool rle_decoding_load(rle_decoding_t *dec){
	uint8_t b[3];
	if (dec->rle_read(dec,b,1)!=1) return false;
	dec->is_compressing=(b[0]&RLE_IS_COMPRESSED?true:false);
	if (dec->is_compressing){
		if (b[0]&RLE_IS_LONG){
			dec->len=((b[0]&RLE_LENSTRIP)<<8);//store hi part
			//load 3 more
			if (dec->rle_read(dec,b,3)!=3) return false;
			dec->len|= b[0]; //add low part
			//copy rpt values
			dec->hb=b[1];
			dec->lb=b[2];
		} else {
			dec->len=b[0]&RLE_LENSTRIP;
			//load rpt values
			if (dec->rle_read(dec,b,2)!=2) return false;
			//store
			dec->hb=b[0];
			dec->lb=b[1];
		}
	} else { //non compressing
		if (b[0]&RLE_IS_LONG){
			dec->len=((b[0]&RLE_LENSTRIP)<<8);//store hi part
			//load 1 more
			if (dec->rle_read(dec,b,1)!=1) return false;
			dec->len|= b[0]; //add low part
		} else { //short we're done
			dec->len=b[0]&RLE_LENSTRIP;
		}
		
		
	}
	return true;
}

//return actual number of words written
unsigned rle_decoding_read(rle_decoding_t *dec, void *buf, unsigned words){
	unsigned written=0;
	uint8_t *b=(uint8_t *)buf;
	while (words) {
		if (dec->len==0) { //load
			if (!rle_decoding_load(dec)) return written;
		} 
		
		//we got some seq loaded atm
		if (dec->is_compressing){
			while (words && dec->len) {
				b[0]=dec->hb; b[1]=dec->lb; words--; dec->len--; b+=2; written++;
			}
		} else {
			unsigned toread;
			if (dec->len<words) toread=dec->len;
			else toread=words;
			if (dec->rle_read(dec,b,toread*2)!=toread*2) return written;
			written+=toread;
			b+=2*toread; 
			words-=toread;
			dec->len-=toread;
		}
	}
	return written;
}

