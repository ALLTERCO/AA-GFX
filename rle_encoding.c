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

#include <stdio.h>
#include <stdlib.h>

void rle_encoding_init(rle_encoding_t *enc, void* buf, unsigned bufsz,flushio_t flushio,void *userdata){
	memset(enc,0,sizeof(*enc));
	enc->userdata=userdata;
	enc->flushio=flushio;
	enc->iobuf=buf;
	enc->iobuf_end=enc->iobuf+bufsz;
	enc->cseq.start=enc->iobuf;
	enc->cseq.end=enc->iobuf;
	enc->cseq.is_compressing=false;
	enc->cseq.len=0;
}
bool rle_encodeing_make_room(rle_encoding_t *enc,unsigned size){
	if (enc->iobuf_end-enc->cseq.end>size) return true;
	
	unsigned bytestoflush=enc->cseq.start-enc->iobuf;
	if ((enc->iobuf_end-enc->cseq.end)+bytestoflush<size){
		//printf ("rle_encodeing_make_room failes with bytestoflush: %u size needed: %u free space atm: %ld\n",bytestoflush,size,(enc->iobuf_end-enc->cseq.end));
		return false;
	}
	if (bytestoflush) enc->flushio(enc,enc->iobuf,bytestoflush);
	
	unsigned bytestomove=enc->cseq.end-enc->cseq.start;
	if (bytestomove) memmove(enc->iobuf,enc->cseq.start,bytestomove);
	enc->cseq.start-=bytestoflush;
	enc->cseq.end-=bytestoflush;
	return true;
}

//true if successfuly stored; no changes made if false returned
bool seq_store(rle_encoding_t *enc){
	rleseq_t *s=&enc->cseq;
	if (s->start==s->end) { //create 
		if (!rle_encodeing_make_room(enc,3)) return false;
		s->end[0]=(s->is_compressing?RLE_IS_COMPRESSED:0)|(s->len&RLE_LENSTRIP); //short len of 1 or 2 in some cases 
		s->end[1]=s->lastvalue>>8; 
		s->end[2]=s->lastvalue&0xff; 
		s->end+=3;
		s->shortstored=true;
		return true;
	}
	//already stored .. we now have to fix it (if we can) 
	if (!s->shortstored){ //long stored we just need to update..
		if (s->len>RLE_MAXLONGLEN) return false; //we can't store this!
		s->start[0]=(s->is_compressing?RLE_IS_COMPRESSED:0) |  RLE_IS_LONG | ((s->len>>8)&RLE_LENSTRIP);
		s->start[1]=s->len&0xff;
		return true;
	}
	//short stored 
	if (s->len<=RLE_MAXSHORTLEN) { //we can keep it short and just update:
		s->start[0]=(s->is_compressing?RLE_IS_COMPRESSED:0) | (s->len&RLE_LENSTRIP); 
		return true;
	}
	//we need to expand .. if we can..
	if (!rle_encodeing_make_room(enc,1)) return false;
	s->start[0]=RLE_IS_LONG | (s->is_compressing?RLE_IS_COMPRESSED:0) | ((s->len>>8)&RLE_LENSTRIP);
	//move some data:
	if (s->is_compressing) { 
		s->start[2]=s->lastvalue>>8; 
		s->start[3]=s->lastvalue&0xff; 
	} else {//not compressing
		memmove(s->start+1,s->start,s->end-s->start);
	}
	s->end++; //as we have grown 
	//finish new len write
	s->start[1]=s->len&0xff;
	s->shortstored=false;
	return true;
}

void rle_encoding_add(rle_encoding_t *enc, uint16_t data, bool flushblock) {
	rleseq_t *s=&enc->cseq;
	if (s->len==0) {
		s->len=1;
		s->is_compressing=true;
		s->lastvalue=data;
		if (!seq_store(enc)) exit(1); //this should always suceed
	} else {
		if (s->lastvalue==data){ //we got a rpt
			if (s->is_compressing){
				s->len++;
				if (!seq_store(enc)) { //can't store atm
					rle_encoding_flush(enc); 
					s->len=1;
					if (!seq_store(enc)) exit(2); //this should always suceed
				}
			} else { //non compressing seq with a rpt
				if (s->len==1){ //just switch to compressing
					s->is_compressing=true;
					s->len++;
					if (!seq_store(enc)) exit(3); //this should always suceed
				} else {
					//trim current seq
					s->end-=2; //we output words so we rewind with 2 bytes to undo las output
					s->len--;
					if (!seq_store(enc)) exit(4); //this should always suceed
					//create new seq
					s->start=s->end;
					s->is_compressing=1;
					s->len=2;
					if (!seq_store(enc)) exit(5); //this should always suceed
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
					if (!seq_store(enc)) exit(6); //this should always suceed
				} else { //switch  to non compressing seq
					//this will still be short stored so we need space just for the data
					if (!rle_encodeing_make_room(enc,2)) exit(7); //this should always suceed
					s->is_compressing=false;
					s->len++;
					if (!seq_store(enc)) exit(8); //this should always suceed
					//store non rpt value
					s->lastvalue=data;
					s->end[0]=data>>8; 
					s->end[1]=data&0xff; 
					s->end+=2;
				}
			} else { //non repeating seq
				//now we will grow but how much:
				bool cangrow;
				if (s->len==RLE_MAXSHORTLEN) cangrow=rle_encodeing_make_room(enc,3); //we will switch to long len now
				else cangrow=rle_encodeing_make_room(enc,2); //len mode will not change
				if (!cangrow){ //we must flush and create new seq
					rle_encoding_flush(enc); 
					s->len=1;
					s->lastvalue=data;
					if (!seq_store(enc)) exit(9); //this should always suceed
				} else {
					s->len++;
					if (!seq_store(enc)) exit(10); //this should always suceed
					//store non rpt value
					s->lastvalue=data;
					s->end[0]=data>>8; 
					s->end[1]=data&0xff; 
					s->end+=2;
				}
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
