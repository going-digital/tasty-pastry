OBJS=odyssey.o
BIN=odyssey.bin
SDKSTAGE=

CFLAGS=-O1
CFLAGS+=-DSTANDALONE
CFLAGS+=-D__STDC_CONSTANT_MACROS
CFLAGS+=-D__STDC_LIMIT_MACROS
CFLAGS+=-DTARGET_POSIX
CFLAGS+=-D_LINUX
CFLAGS+=-Wall
CFLAGS+=-g
CFLAGS+=-pipe
CFLAGS+=-Wno-psabi
CFLAGS+=-DNDEBUG

LDFLAGS+=-L/opt/vc/lib/
LDFLAGS+=-lGLESv2

# Strip debug, and sections not needed for execution
STRIPFLAGS=-s
STRIPFLAGS+=-R .note.ABI-tag
STRIPFLAGS+=-R .note.gnu.build-id
STRIPFLAGS+=-R .hash
STRIPFLAGS+=-R .gnu.hash
STRIPFLAGS+=-R .gnu.version
STRIPFLAGS+=-R .comment

INCLUDES+=-I/opt/vc/include/
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads
INCLUDES+=-I./
INCLUDES+=-I./libs/ilclient
#INCLUDES+=-I./libs/vgfont

all: odyssey.final $(OBJS) $(BIN) $(LIB)

%.o: %.c
	@rm -f $@ 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.bin: $(OBJS)
	$(CC) -o $@ -Wl,--whole-archive $(OBJS) $(LDFLAGS) -Wl,--no-whole-archive -s -O1 -rdynamic

%.final: %.bin
	cp $< $@
	wc -c $@
	strip $(STRIPFLAGS) $@
	wc -c $@
	sstrip/sstrip $@
	wc -c $@
	./pack.sh $@
	rm $@
	mv $@.pack $@
	wc -c $@

%.a: $(OBJS)
	$(AR) r $@ $^

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)


