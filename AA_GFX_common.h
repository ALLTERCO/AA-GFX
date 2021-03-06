#ifndef AA_GFX_COMMON_H_INCLUDED_

#ifndef COLOR_T
#define COLOR_T uint16_t
#endif
#ifndef PUTS_DEF_COLOR
#define PUTS_DEF_COLOR 0xFFFF
#endif

#ifdef GFX_puts
#define AA_GFX_COMMON_DI_INCLUDES_PUTS\
	const GFXfont* puts_font;\
	uint8_t puts_fontSize;\
	uint8_t puts_flags;\
	COLOR_T puts_color;\
	COLOR_T puts_colorbg;
	
#define AA_GFX_COMMON_DI_INCLUDES_PUTS_INIT(di) \
	di->puts_font=NULL;\
	di->puts_fontSize=1;\
	di->puts_flags=0;\
	di->puts_color=PUTS_DEF_COLOR;\
	di->puts_colorbg=0;
	
#else 
#define AA_GFX_COMMON_DI_INCLUDES_PUTS
#define AA_GFX_COMMON_DI_INCLUDES_PUTS_INIT(di)
#endif

#define AA_GFX_COMMON_DI_INCLUDES \
	int16_t cursor_x; \
	int16_t cursor_y; \
	uint8_t wrap; \
	AA_GFX_COMMON_DI_INCLUDES_PUTS

#define AA_GFX_COMMON_DI_INITS(di)\
	di->cursor_x=10;\
	di->cursor_y=10;\
	di->wrap=1; \
	AA_GFX_COMMON_DI_INCLUDES_PUTS_INIT(di)

//used in fonts data
#ifndef ARDUINO
#ifndef PROGMEM
#define PROGMEM
#endif
#endif
	
#define GFX_DCF_ABSOLUTE 1
#define GFX_DCF_SETBG 2
#endif
