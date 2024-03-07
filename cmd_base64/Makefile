CFLAGS += -std=c99 -O3 -Wall -Wextra -pedantic


OBJS = \
  src/libbase64.o

.PHONY: all clean

all: bin/base64

base64: bin/base64

bin/base64: cmd_src/base64.o src/libbase64.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): CFLAGS += -Isrc


# The user should supply compiler flags for the codecs they want to build.
# Check which codecs we're going to include:
ifdef AVX512_CFLAGS
  BOPTIONS += $(AVX512_CFLAGS)
endif
ifdef AVX2_CFLAGS
  BOPTIONS += $(AVX2_CFLAGS)
endif
ifdef NEON32_CFLAGS
  BOPTIONS += $(NEON32_CFLAGS)
endif
ifdef NEON64_CFLAGS
  BOPTIONS += $(NEON64_CFLAGS)
endif
ifdef SSSE3_CFLAGS
  BOPTIONS += $(SSSE3_CFLAGS)
endif
ifdef SSE41_CFLAGS
  BOPTIONS += $(SSE41_CFLAGS)
endif
ifdef SSE42_CFLAGS
  BOPTIONS += $(SSE42_CFLAGS)
endif
ifdef AVX_CFLAGS
  BOPTIONS += $(AVX_CFLAGS)
endif
ifdef OPENMP
  BOPTIONS += -fopenmp
endif


src/libbase64.o:    CFLAGS += $(BOPTIONS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -f bin/base64 cmd_src/base64.o  $(OBJS) 
