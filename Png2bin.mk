CFLAGS=$(shell pkg-config --cflags --libs   libpng) $(shell pkg-config --cflags --libs   zlib) -lz -Wall $(if $(RLEDECTEST),-O0 -g -DRLEDECTEST)
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
$(info === Buildig of binary encoder $(BINNAME) requested);
$(BINNAME):png2bin_template.c rle_encoding.c $(if $(RLEDECTEST),rle_decoding.c)
	@$(CC) -o $(BINNAME) $(^) $(CFLAGS) $(EXTRACFLAGS) $(DEFINES)
ifeq "$(RLEDECTEST)" "" 
	@echo === stripping the binary endcoder $(BINNAME) ...
	@strip $(BINNAME)
else
	@echo === binary endcoder $(BINNAME) is redy to be debugged!
endif

clean:
	@rm $(BINNAME)
