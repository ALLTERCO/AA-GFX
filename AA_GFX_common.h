#ifndef AA_GFX_COMMON_H_INCLUDED_

#define AA_GFX_COMMON_DI_INCLUDES \
	int16_t cursor_x; \
	int16_t cursor_y; \
	uint8_t wrap; 

#define AA_GFX_COMMON_DI_INITS(di)\
	di->cursor_x=10;\
	di->cursor_y=10;\
	di->wrap=1; 
	
#define GFX_DCF_ABSOLUTE 1
#define GFX_DCF_SETBG 2
#endif
