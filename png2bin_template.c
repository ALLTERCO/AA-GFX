#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <png.h>
#include <zlib.h>

#include COLORMAPH

#define USAGE "Usage!\n"
const char * ct2string(int ct){
	switch (ct) {
		case PNG_COLOR_TYPE_GRAY       :return "PNG_COLOR_TYPE_GRAY";
		case PNG_COLOR_TYPE_GRAY_ALPHA :return "PNG_COLOR_TYPE_GRAY_ALPHA";
		case PNG_COLOR_TYPE_PALETTE    :return "PNG_COLOR_TYPE_PALETTE";
		case PNG_COLOR_TYPE_RGB        :return "PNG_COLOR_TYPE_RGB";
		case PNG_COLOR_TYPE_RGB_ALPHA  :return "PNG_COLOR_TYPE_RGB_ALPHA";
		case PNG_COLOR_MASK_PALETTE    :return "PNG_COLOR_MASK_PALETTE";
// 		case PNG_COLOR_MASK_COLOR      :return "PNG_COLOR_MASK_COLOR";
// 		case PNG_COLOR_MASK_ALPHA      :return "PNG_COLOR_MASK_ALPHA";
		default: return "unknown";
	}
}
int main(int argc, char * argv[]){
	//printf("white->0x%04x red->0x%04x green->0x%04x blue->0x%04x\n",(unsigned)COLORMAP(0xff,0xff,0xff),(unsigned)COLORMAP(0xff,0,0),(unsigned)COLORMAP(0,0xff,0),(unsigned)COLORMAP(0,0,0xff));
	if (argc<3) {
		printf (USAGE);
		return -1;
	}
	FILE *in=fopen(argv[1],"rb");
	if (!in) {
		perror("fopen");
		printf("Failed to open %s as input file!\n" USAGE,argv[1]);
		return -2 ;
	}
	uint64_t header;
	if (fread(&header,sizeof(header),1,in)!=1) {
		printf("Failed to read %d bytes from  file %s\n" USAGE, (int)sizeof(header),argv[1]);
		return -2 ;
	}
	if (png_sig_cmp((const void *)&header, 0, sizeof(header))!=0){
		printf("Failed to match %d bytes  to png magic bytes from  file %s\n" USAGE, (int)sizeof(header),argv[1]);
		return -3;
	}
	
	
	png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL,NULL, NULL);
	
	if (!png_ptr){
		printf("Failed to init a read png struct!\n");
		return -4;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	
	if (!info_ptr)	{
		png_destroy_read_struct(&png_ptr,(png_infopp)NULL, (png_infopp)NULL);
		printf("Failed to init a info png struct!\n");
		return -5;
	}
	png_init_io(png_ptr, in);
	png_set_sig_bytes(png_ptr, sizeof(header));
	
	png_set_gamma(png_ptr, PNG_DEFAULT_sRGB, PNG_DEFAULT_sRGB);
	#if PNG_LIBPNG_VER >= 10504
	png_set_alpha_mode(png_ptr, PNG_ALPHA_PNG, PNG_DEFAULT_sRGB);
	#else
	png_set_gamma(png_ptr, PNG_DEFAULT_sRGB, 1.0/PNG_DEFAULT_sRGB);
	#endif

	//png_read_png(png_ptr, info_ptr,PNG_TRANSFORM_IDENTITY ,NULL);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_GRAY_TO_RGB|PNG_TRANSFORM_EXPAND|PNG_TRANSFORM_SCALE_16|PNG_TRANSFORM_STRIP_ALPHA , NULL);
	png_uint_32 width, height;
	int bit_depth,  color_type;
	if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL)) {
		printf("Failed to png_get_IHDR for %s\n",argv[1]);
		return -6;
	};
	if (bit_depth!=8 || color_type!=PNG_COLOR_TYPE_RGB) {
		printf("Failed to transform %s to 8bit RGB representation!\n",argv[1]);
		return -7;
	}
	png_bytepp row_pointers=png_get_rows(png_ptr, info_ptr);
	printf ("file %s is w:%u h:%u \n",argv[1],(unsigned) width,(unsigned) height) ;
	fclose (in);
	FILE *out=fopen (argv[2],"wb");
	if (!out) {
		printf ("failed to open %s for writing!\n",argv[2]);
		return -8;
	}
	FILE *rleout=NULL;
	uint8_t *rledata=NULL;
	unsigned rledata_sz=0;
	if (argc>=4) {
		rleout=fopen (argv[3],"wb");
		if (!rleout) {
			printf ("failed to open %s for rle writing!\n",argv[3]);
			return -9;
		}
		rledata_sz=width*2;
		rledata=malloc(rledata_sz);
		if (!rledata) {
			printf ("failed to allocate memorey for rle encoding!\n");
			return -10;
		}
	}
	
	png_uint_32 ri=0;
	for (;ri<height; ri++){
		png_uint_32 pi=0;
		int is_rleencoded=(width>=8);
		unsigned rle_pout=0;
		uint16_t lastrle_color=COLORMAP(row_pointers[ri][0],row_pointers[ri][1],row_pointers[ri][2]);
		uint8_t lastrle_cnt=0;
		uint8_t *rlebpos=rledata;
		for (;pi<width;pi++) {
			uint16_t clr=COLORMAP(row_pointers[ri][pi*3],row_pointers[ri][pi*3+1],row_pointers[ri][pi*3+2]);
			row_pointers[ri][pi*2]=clr>>8;
			row_pointers[ri][pi*2+1]=clr&0xff;
			if (rledata_sz && is_rleencoded){
				int doout=0;
				if (lastrle_color==clr) {
					lastrle_cnt++;
					if (lastrle_cnt==0xff) doout=1;
				} else {
					doout=1;
				}
				if (doout){
					rlebpos[0]=lastrle_cnt;
					rlebpos[1]=lastrle_color>>8;
					rlebpos[1]=lastrle_color&0xff;
					rlebpos+=3;
					rle_pout++;
					if (rledata_sz -(rledata-rlebpos)<=5){
						//inifficent for rle row!
						is_rleencoded=0;
					} else {
						lastrle_color=clr;
						lastrle_cnt=1;
					}
				}
				
			}
			
		}
		if (rledata_sz && is_rleencoded) { //final out 
			rlebpos[0]=lastrle_cnt;
			rlebpos[1]=lastrle_color>>8;
			rlebpos[1]=lastrle_color&0xff;
			rlebpos+=3;
			rle_pout++;
			if (rledata_sz -(rledata-rlebpos)<=5){
				//inifficent for rle row!
				is_rleencoded=0;
			}
		}
		if (is_rleencoded==0) {
			if (width<=0x3f){
				uint8_t lineh=width; 
				if (fwrite(&lineh,1,1,rleout)!=1){
					printf ("Failed to complete write to %s\n",argv[3]);
					return -11;
				}
			} else {
				uint8_t lineh[2];
				lineh[0]=0x40|(((width&0x3fff)>>8)); 
				lineh[1]=width&0xff; 
				if (fwrite(&lineh,2,1,rleout)!=1){
					printf ("Failed to complete write to %s\n",argv[3]);
					return -11;
				}
			}
			if (fwrite(row_pointers[ri],2,width,rleout)!=width){
				printf ("Failed to complete write to %s\n",argv[3]);
				return -11;
			}
		} else { //is rle encoded
			if (rle_pout<=0x3f){
				uint8_t lineh=0x80|rle_pout; 
				if (fwrite(&lineh,1,1,rleout)!=1){
					printf ("Failed to complete write to %s\n",argv[3]);
					return -11;
				}
			} else {
				uint8_t lineh[2];
				lineh[0]=0xC0|(((rle_pout&0x3fff)>>8)); 
				lineh[1]=rle_pout&0xff; 
				if (fwrite(&lineh,2,1,rleout)!=1){
					printf ("Failed to complete write to %s\n",argv[3]);
					return -11;
				}
			}
			if (fwrite(rledata,3,rle_pout,rleout)!=rle_pout){
				printf ("Failed to complete write to %s\n",argv[3]);
				return -11;
			}
		}
		if (fwrite(row_pointers[ri],2,width,out)!=width){
			printf ("Failed to complete write to %s\n",argv[2]);
			return -12;
		}
	}
	fclose(out);
	printf ("%s filled with color mapped RGB raw data!\n",argv[2]);
	return 0;
}
