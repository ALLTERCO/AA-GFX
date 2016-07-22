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


#define RLE_H_EXPORT_ENCODING
#include "rle.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void rle_encoding_init(rle_encoding_t *enc, void* buf, unsigned bufsz,flushio_t flushio,void *userdata){
	memset(enc,0,sizeof(*enc));
	enc->userdata=userdata;
	enc->flushio=flushio;
	enc->iobuf=buf;
	enc->iobuf_end=enc->iobuf+bufsz;
	enc->cseq.start=enc->iobuf;
	enc->cseq.end=enc->iobuf;
	enc->cseq.is_compressing=0;
	enc->cseq.len=0;
}
void rle_encodeing_make_room(rle_encoding_t *enc,unsigned size){
	if (enc->iobuf_end-enc->cseq.end>size) return;
	
	unsigned bytestoflush=enc->cseq.start-enc->iobuf;
	if (bytestoflush) enc->flushio(enc,enc->iobuf,bytestoflush);
	
	unsigned bytestomove=enc->cseq.end-enc->cseq.start;
	if (bytestomove) memmove(enc->iobuf,enc->cseq.start,bytestomove);
	enc->cseq.start-=bytestoflush;
	enc->cseq.end-=bytestoflush;
	
}

void seq_store(rle_encoding_t *enc){
	rleseq_t *s=&enc->cseq;
	if (s->start==s->end) { //create 
		rle_encodeing_make_room(enc,3);
		s->end[0]=(s->is_compressing?RLE_IS_COMPRESSED:0)|(s->len&RLE_LENSTRIP); //short len of 1 or 2 in some cases 
		s->end[1]=s->lastvalue>>8; 
		s->end[2]=s->lastvalue&0xff; 
		s->end+=3;
		s->shortstored=true;
		return;
	}
	//already stored .. we now have to fix it
	if (s->is_compressing) {
		if (s->shortstored) {
			if (s->len<=RLE_MAXSHORTLEN) { //we can keep it short and just update:
				s->start[0]=RLE_IS_COMPRESSED | (s->len&RLE_LENSTRIP); 
			} else { //expand:
				rle_encodeing_make_room(enc,1);
				s->end++;
				s->start[0]=RLE_IS_COMPRESSED | RLE_IS_LONG | ((s->len>>8)&RLE_LENSTRIP); //long len
				s->start[1]=s->len&0xff;
				s->start[2]=s->lastvalue>>8; 
				s->start[3]=s->lastvalue&0xff; 
				s->shortstored=false;
			}
		} else { //long stored just update:
			s->start[0]=RLE_IS_COMPRESSED | RLE_IS_LONG | ((s->len>>8)&RLE_LENSTRIP); //long len
			s->start[1]=s->len&0xff;
		}
	} else {//not compressing
		if (s->shortstored) {
			if (s->len<=RLE_MAXSHORTLEN) { //we can keep it short and just update:
				s->start[0]= (s->len&RLE_LENSTRIP); 
			} else { //expand:
				rle_encodeing_make_room(enc,1);
				//move non repeating bytes one down..
				memmove(s->start+1,s->start,s->end-s->start);
				s->end++;
				s->start[0]= RLE_IS_LONG | ((s->len>>8)&RLE_LENSTRIP); //long len
				s->start[1]=s->len&0xff;
				s->shortstored=false;
			}
		} else { //long stored just update:
			s->start[0]=RLE_IS_LONG | ((s->len>>8)&RLE_LENSTRIP); //long len
			s->start[1]=s->len&0xff;
		}
	}
}

void rle_encoding_add(rle_encoding_t *enc, uint16_t data, bool flushblock) {
	rleseq_t *s=&enc->cseq;
	if (s->len==0) {
		s->len=1;
		s->is_compressing=true;
		s->lastvalue=data;
		seq_store(enc);
	} else {
		if (s->lastvalue==data){ //we got a rpt
			if (s->is_compressing){
				s->len++;
				seq_store(enc);
			} else { //non compressing seq with a rpt
				if (s->len==1){ //just switch to compressing
					s->is_compressing=true;
					s->len++;
					seq_store(enc);
				} else {
					//trim current seq
					s->end-=2; //we output words so we rewind with 2 bytes to undo las output
					s->len--;
					seq_store(enc);
					//create new seq
					s->start=s->end;
					s->is_compressing=1;
					s->len=2;
					seq_store(enc);
				}
			}
		} else { //we got a NON rpt
			if (s->is_compressing){
				if (s->len>1) {
					//create new seq
					s->lastvalue=data;
					s->start=s->end;
					s->is_compressing=1;
					s->len=1;
					seq_store(enc);
				} else { //switch  to non compressing seq
					//this will still be short stored so we need space just for the data
					rle_encodeing_make_room(enc,2);
					s->is_compressing=false;
					s->len++;
					seq_store(enc);
					//store non rpt value
					s->lastvalue=data;
					s->end[0]=data>>8; 
					s->end[1]=data&0xff; 
					s->end+=2;
				}
			} else { //non repeating seq
				//now we will grow but how much:
				if (s->len==RLE_MAXSHORTLEN) rle_encodeing_make_room(enc,3); //we will switch to long len now
				else rle_encodeing_make_room(enc,2); //len mode will not change
				s->len++;
				seq_store(enc); //probably move some data around 
				//store non rpt value
				s->lastvalue=data;
				s->end[0]=data>>8; 
				s->end[1]=data&0xff; 
				s->end+=2;
				
			}
		}
	}
	
	if (flushblock) {
		enc->flushio(enc,enc->iobuf,s->end-enc->iobuf);
		s->len=0;
		s->start=enc->iobuf;
		s->end=enc->iobuf;
	} else {
		if (s->len==RLE_MAXLONGLEN) { //start len due to super long rpt!
			s->len=0;
			s->start=s->end+1;
			s->end++;
		}
	}
	

}
void rle_encoding_flush(rle_encoding_t *enc){
		if (enc->cseq.end==enc->iobuf) return; //nothing to flush 

		enc->flushio(enc,enc->iobuf,enc->cseq.end-enc->iobuf);
		enc->cseq.len=0;
		enc->cseq.start=enc->iobuf;
		enc->cseq.end=enc->iobuf;
}
