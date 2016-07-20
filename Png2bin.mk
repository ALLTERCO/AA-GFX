CFLAGS=$(shell pkg-config --cflags --libs   libpng) $(shell pkg-config --cflags --libs   zlib) -lz -Wall
ifeq "$(BINNAME)" "" 
$(error BINNAME variable not set!)
endif
ifeq "$(COLORMAPH)" "" 
$(error COLORMAPH variable not set!)
endif
DEFINES+="-DCOLORMAPH=\"$(COLORMAPH)\""
ifeq "$(COLORMAP)" "" 
$(error COLORMAP variable not set!)
endif
DEFINES+="-DCOLORMAP(r,g,b)=$(COLORMAP)(r,g,b)"

$(BINNAME):png2bin_template.c rle_encoding.c
	$(CC) -o $(BINNAME) $(^) $(CFLAGS)  $(EXTRACFLAGS) $(DEFINES)
clean:
	@rm pngtest
