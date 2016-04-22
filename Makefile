CC=gcc5
AS=$(CC) -c
AR?=ar

CFLAGS?=-g
CFLAGS+=-Wall -Wextra -Wno-unused-parameter
ifeq ($(VEC), avx512)
	SSE=no
	AVX2=no
	AVX512=yes
	CFLAGS+=-mavx512bw -mavx512f -mavx512vl
else ifeq ($(VEC), avx2)
	SSE=no
	AVX2=yes
	AVX512=no
	CFLAGS+=-mavx2
else ifeq ($(VEC), sse)
	SSE=yes
	AVX2=no
	AVX512=no
	CFLAGS+=-mssse3
else ifeq ($(VEC), none)
	SSE=no
	AVX2=no
	AVX512=no
else
	$(error Set environment variable VEC to one of {none,sse,avx2,avx512})=
endif

# use -march=native if we're compiling for x86
BENCH_ARCH_OPTION=
MACHINE=$(shell uname -m | sed 's/i.86/i386/g')
# ifneq ($(VEC), none)
#     ifeq ($(MACHINE), i386)
#         BENCH_ARCH_OPTION=-march=native
#     endif
#     ifeq ($(MACHINE), x86_64)
#         BENCH_ARCH_OPTION=-march=native
#     endif
# endif

ifeq ($(USE_AES_NI), yes)
	CFLAGS += -maes -DUSE_AES_NI
endif

OPTFLAGS=-O2
bench: OPTFLAGS=-O3 $(BENCH_ARCH_OPTION)
CFLAGS+=$(OPTFLAGS)

LIBS+=-lrt
SRCDIR=src
TESTDIR=tests
LIB_OBJS=bitstring.o encparams.o hash.o idxgen.o key.o mgf.o ntru.o poly.o rand.o arith.o sha1.o sha2.o nist_ctr_drbg.o rijndael.o rijndael-alg-fst.o
ifneq ($(VEC), none)
    ifeq ($(MACHINE), x86_64)
        LIB_OBJS+=sha1-mb-x86_64.o sha256-mb-x86_64.o
    endif
endif
TEST_OBJS=test_bitstring.o test_hash.o test_idxgen.o test_key.o test_ntru.o test.o test_poly.o test_util.o
VERSION=0.4
INST_PFX=/usr
INST_LIBDIR=$(INST_PFX)/lib
INST_INCLUDE=$(INST_PFX)/include/libntru
INST_DOCDIR=$(INST_PFX)/share/doc/libntru-$(VERSION)
INST_HEADERS=ntru.h types.h key.h encparams.h hash.h rand.h err.h
PERL=/usr/bin/perl
PERLASM_SCHEME=elf

LIB_OBJS_PATHS=$(patsubst %,$(SRCDIR)/%,$(LIB_OBJS))
TEST_OBJS_PATHS=$(patsubst %,$(TESTDIR)/%,$(TEST_OBJS))
DIST_NAME=libntru-$(VERSION)
MAKEFILENAME=$(lastword $(MAKEFILE_LIST))

.PHONY: all lib install uninstall dist test clean distclean

all: lib

lib: libntru.so

static-lib: libntru.a

libntru.so: $(LIB_OBJS_PATHS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -shared -Wl,-soname,libntru.so -o libntru.so $(LIB_OBJS_PATHS) $(LDFLAGS) $(LIBS)

libntru.a: $(LIB_OBJS_PATHS)
	$(AR) cru libntru.a $(LIB_OBJS_PATHS) aes/ecb.o aes/key_expansion.o aes/vaes128_key_expansion.o aes/vaes192_key_expansion.o aes/vaes256_key_expansion.o 


test:
	$(MAKE) -f $(MAKEFILENAME) testnoham
	@echo
	@echo Testing patent-reduced build
	LD_LIBRARY_PATH=. ./testnoham
	$(MAKE) -f $(MAKEFILENAME) testham
	@echo
	@echo Testing full build
	LD_LIBRARY_PATH=. ./testham

testham: clean lib $(TEST_OBJS_PATHS)
	@echo CFLAGS=$(CFLAGS)
	$(CC) $(CFLAGS) -o testham $(TEST_OBJS_PATHS) -L. -lntru -lm

testnoham: CFLAGS += -DNTRU_AVOID_HAMMING_WT_PATENT
testnoham: clean static-lib $(TEST_OBJS_PATHS)
	@echo CFLAGS=$(CFLAGS)
	$(CC) $(CFLAGS) -o testnoham $(TEST_OBJS_PATHS) -L. -lntru -lm

bench: static-lib
	$(CC) $(CFLAGS) $(CPPFLAGS) -o bench $(SRCDIR)/bench.c $(LDFLAGS) $(LIBS) -L. -lntru

hybrid: static-lib
	$(CC) $(CFLAGS) $(CPPFLAGS) -o hybrid $(SRCDIR)/hybrid.c $(LDFLAGS) $(LIBS) -L. -lntru -lcrypto

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -fPIC $< -o $@

$(SRCDIR)/sha1-mb-x86_64.s: $(SRCDIR)/sha1-mb-x86_64.pl; $(PERL) $(SRCDIR)/sha1-mb-x86_64.pl $(PERLASM_SCHEME) > $@
$(SRCDIR)/sha1-mb-x86_64.o: $(SRCDIR)/sha1-mb-x86_64.s
	$(AS) $(SRCDIR)/sha1-mb-x86_64.s -o $@
$(SRCDIR)/sha256-mb-x86_64.s: $(SRCDIR)/sha256-mb-x86_64.pl; $(PERL) $(SRCDIR)/sha256-mb-x86_64.pl $(PERLASM_SCHEME) > $@
$(SRCDIR)/sha256-mb-x86_64.o: $(SRCDIR)/sha256-mb-x86_64.s
	$(AS) $(SRCDIR)/sha256-mb-x86_64.s -o $@

tests/%.o: tests/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -I$(SRCDIR) -c $< -o $@

clean:
	@# also clean files generated on other OSes
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*.s $(TESTDIR)/*.o libntru.so libntru.a libntru.dylib libntru.dll testham testnoham testham.exe testnoham.exe bench bench.exe hybrid hybrid.exe

