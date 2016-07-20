#define RLE_H_EXPORT_DECODING
#include "rle.h"
#include <string.h>

void rle_decoding_init(rle_decoding_t *dec, rle_read_t rle_read,void *userdata){
	memset(dec,0,sizeof(*dec));
	dec->userdata=userdata;
	dec->rle_read=rle_read;
}

bool rle_decoding_load(rle_decoding_t *dec){
	
}

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
			dec->len=toread;
		}
	}
	return written;
}

