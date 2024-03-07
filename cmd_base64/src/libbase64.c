#include <stdint.h>
#include <stddef.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "libbase64.h"



//#include "env.h"
//#include "tables/tables.h"
//#include "codecs.h"




////-------------> config.h  begine <-------------------------
#define HAVE_AVX512 0
#define HAVE_AVX2   0
#define HAVE_NEON32 0
#define HAVE_NEON64 0
#define HAVE_SSSE3  0
#define HAVE_SSE41  0
#define HAVE_SSE42  0
#define HAVE_AVX    0
////-------------> config.h  end <-------------------------




////-------------> env.h  begine <-------------------------
#ifndef BASE64_ENV_H
#define BASE64_ENV_H

// This header file contains macro definitions that describe certain aspects of
// the compile-time environment. Compatibility and portability macros go here.

// Define machine endianness. This is for GCC:
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#  define BASE64_LITTLE_ENDIAN 1
#else
#  define BASE64_LITTLE_ENDIAN 0
#endif

// This is for Clang:
#ifdef __LITTLE_ENDIAN__
#  define BASE64_LITTLE_ENDIAN 1
#endif

#ifdef __BIG_ENDIAN__
#  define BASE64_LITTLE_ENDIAN 0
#endif

// MSVC++ needs intrin.h for _byteswap_uint64 (issue #68):
#if BASE64_LITTLE_ENDIAN && defined(_MSC_VER)
#  include <intrin.h>
#endif

// Endian conversion functions:
#if BASE64_LITTLE_ENDIAN
#  ifdef _MSC_VER
//   Microsoft Visual C++:
#    define BASE64_HTOBE32(x)	_byteswap_ulong(x)
#    define BASE64_HTOBE64(x)	_byteswap_uint64(x)
#  else
//   GCC and Clang:
#    define BASE64_HTOBE32(x)	__builtin_bswap32(x)
#    define BASE64_HTOBE64(x)	__builtin_bswap64(x)
#  endif
#else
// No conversion needed:
#  define BASE64_HTOBE32(x)	(x)
#  define BASE64_HTOBE64(x)	(x)
#endif

// Detect word size:
#if defined (__x86_64__)
// This also works for the x32 ABI, which has a 64-bit word size.
#  define BASE64_WORDSIZE 64
#elif defined (_INTEGRAL_MAX_BITS)
#  define BASE64_WORDSIZE _INTEGRAL_MAX_BITS
#elif defined (__WORDSIZE)
#  define BASE64_WORDSIZE __WORDSIZE
#elif defined (__SIZE_WIDTH__)
#  define BASE64_WORDSIZE __SIZE_WIDTH__
#else
#  error BASE64_WORDSIZE_NOT_DEFINED
#endif

// End-of-file definitions.
// Almost end-of-file when waiting for the last '=' character:
#define BASE64_AEOF 1
// End-of-file when stream end has been reached or invalid input provided:
#define BASE64_EOF 2

// GCC 7 defaults to issuing a warning for fallthrough in switch statements,
// unless the fallthrough cases are marked with an attribute. As we use
// fallthrough deliberately, define an alias for the attribute:
#if __GNUC__ >= 7
#  define BASE64_FALLTHROUGH  __attribute__((fallthrough));
#else
#  define BASE64_FALLTHROUGH
#endif

#endif	// BASE64_ENV_H
////-------------> env.h  end <-------------------------




//////-------------> tables/tables.h  begine <-------------------------
//#ifndef BASE64_TABLES_H
//#define BASE64_TABLES_H
//
////#include <stdint.h>
//
////#include "../env.h"
//
//// These tables are used by all codecs for fallback plain encoding/decoding:
//extern const uint8_t base64_table_enc_6bit[];
//extern const uint8_t base64_table_dec_8bit[];
//
//// These tables are used for the 32-bit and 64-bit generic decoders:
//#if BASE64_WORDSIZE >= 32
//extern const uint32_t base64_table_dec_32bit_d0[];
//extern const uint32_t base64_table_dec_32bit_d1[];
//extern const uint32_t base64_table_dec_32bit_d2[];
//extern const uint32_t base64_table_dec_32bit_d3[];
//
//// This table is used by the 32 and 64-bit generic encoders:
//extern const uint16_t base64_table_enc_12bit[];
//#endif
//
//#endif	// BASE64_TABLES_H
//////-------------> tables/tables.h  end <-------------------------




////-------------> codecs.h  begine <-------------------------
//#include "config.h"

// Function parameters for encoding functions:
#define BASE64_ENC_PARAMS			\
	( struct base64_state	*state		\
	, const char		*src		\
	, size_t		 srclen		\
	, char			*out		\
	, size_t		*outlen		\
	)

// Function parameters for decoding functions:
#define BASE64_DEC_PARAMS			\
	( struct base64_state	*state		\
	, const char		*src		\
	, size_t		 srclen		\
	, char			*out		\
	, size_t		*outlen		\
	)

// Function signature for encoding functions:
#define BASE64_ENC_FUNCTION(arch)		\
	void					\
	base64_stream_encode_ ## arch		\
	BASE64_ENC_PARAMS

// Function signature for decoding functions:
#define BASE64_DEC_FUNCTION(arch)		\
	int					\
	base64_stream_decode_ ## arch		\
	BASE64_DEC_PARAMS

// Cast away unused variable, silence compiler:
#define UNUSED(x)		((void)(x))

// Stub function when encoder arch unsupported:
#define BASE64_ENC_STUB				\
	UNUSED(state);				\
	UNUSED(src);				\
	UNUSED(srclen);				\
	UNUSED(out);				\
						\
	*outlen = 0;

// Stub function when decoder arch unsupported:
#define BASE64_DEC_STUB				\
	UNUSED(state);				\
	UNUSED(src);				\
	UNUSED(srclen);				\
	UNUSED(out);				\
	UNUSED(outlen);				\
						\
	return -1;

struct codec
{
	void (* enc) BASE64_ENC_PARAMS;
	int  (* dec) BASE64_DEC_PARAMS;
};

extern void codec_choose (struct codec *, int flags);

////-------------> codecs.h  end <-------------------------







////=============> tables/tables.c  end <=========================
//#include "tables.h"

const uint8_t
base64_table_enc_6bit[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789"
	"+/";

// In the lookup table below, note that the value for '=' (character 61) is
// 254, not 255. This character is used for in-band signaling of the end of
// the datastream, and we will use that later. The characters A-Z, a-z, 0-9
// and + / are mapped to their "decoded" values. The other bytes all map to
// the value 255, which flags them as "invalid input".

const uint8_t
base64_table_dec_8bit[] =
{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		//   0..15
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		//  16..31
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,		//  32..47
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 254, 255, 255,		//  48..63
	255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,		//  64..79
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,		//  80..95
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,		//  96..111
	 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,		// 112..127
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// 128..143
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};

#if BASE64_WORDSIZE >= 32
//#  include "table_dec_32bit.h"
//#  include "table_enc_12bit.h"
////------ tables/table_dec_32bit.h  begine <-----
//#include <stdint.h>
#define CHAR62 '+'
#define CHAR63 '/'
#define CHARPAD '='


#if BASE64_LITTLE_ENDIAN


/* SPECIAL DECODE TABLES FOR LITTLE ENDIAN (INTEL) CPUS */

const uint32_t base64_table_dec_32bit_d0[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x000000f8, 0xffffffff, 0xffffffff, 0xffffffff, 0x000000fc,
0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
0x00000064, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
0x000000c4, 0x000000c8, 0x000000cc, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


const uint32_t base64_table_dec_32bit_d1[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x0000e003, 0xffffffff, 0xffffffff, 0xffffffff, 0x0000f003,
0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
0x00009001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
0x00001003, 0x00002003, 0x00003003, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


const uint32_t base64_table_dec_32bit_d2[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x00800f00, 0xffffffff, 0xffffffff, 0xffffffff, 0x00c00f00,
0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
0x00400600, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
0x00400c00, 0x00800c00, 0x00c00c00, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


const uint32_t base64_table_dec_32bit_d3[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x003e0000, 0xffffffff, 0xffffffff, 0xffffffff, 0x003f0000,
0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
0x00190000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
0x00310000, 0x00320000, 0x00330000, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


#else


/* SPECIAL DECODE TABLES FOR BIG ENDIAN (IBM/MOTOROLA/SUN) CPUS */

const uint32_t base64_table_dec_32bit_d0[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xf8000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xfc000000,
0xd0000000, 0xd4000000, 0xd8000000, 0xdc000000, 0xe0000000, 0xe4000000,
0xe8000000, 0xec000000, 0xf0000000, 0xf4000000, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x04000000, 0x08000000, 0x0c000000, 0x10000000, 0x14000000, 0x18000000,
0x1c000000, 0x20000000, 0x24000000, 0x28000000, 0x2c000000, 0x30000000,
0x34000000, 0x38000000, 0x3c000000, 0x40000000, 0x44000000, 0x48000000,
0x4c000000, 0x50000000, 0x54000000, 0x58000000, 0x5c000000, 0x60000000,
0x64000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x68000000, 0x6c000000, 0x70000000, 0x74000000, 0x78000000,
0x7c000000, 0x80000000, 0x84000000, 0x88000000, 0x8c000000, 0x90000000,
0x94000000, 0x98000000, 0x9c000000, 0xa0000000, 0xa4000000, 0xa8000000,
0xac000000, 0xb0000000, 0xb4000000, 0xb8000000, 0xbc000000, 0xc0000000,
0xc4000000, 0xc8000000, 0xcc000000, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


const uint32_t base64_table_dec_32bit_d1[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x03e00000, 0xffffffff, 0xffffffff, 0xffffffff, 0x03f00000,
0x03400000, 0x03500000, 0x03600000, 0x03700000, 0x03800000, 0x03900000,
0x03a00000, 0x03b00000, 0x03c00000, 0x03d00000, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x00100000, 0x00200000, 0x00300000, 0x00400000, 0x00500000, 0x00600000,
0x00700000, 0x00800000, 0x00900000, 0x00a00000, 0x00b00000, 0x00c00000,
0x00d00000, 0x00e00000, 0x00f00000, 0x01000000, 0x01100000, 0x01200000,
0x01300000, 0x01400000, 0x01500000, 0x01600000, 0x01700000, 0x01800000,
0x01900000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x01a00000, 0x01b00000, 0x01c00000, 0x01d00000, 0x01e00000,
0x01f00000, 0x02000000, 0x02100000, 0x02200000, 0x02300000, 0x02400000,
0x02500000, 0x02600000, 0x02700000, 0x02800000, 0x02900000, 0x02a00000,
0x02b00000, 0x02c00000, 0x02d00000, 0x02e00000, 0x02f00000, 0x03000000,
0x03100000, 0x03200000, 0x03300000, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


const uint32_t base64_table_dec_32bit_d2[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x000f8000, 0xffffffff, 0xffffffff, 0xffffffff, 0x000fc000,
0x000d0000, 0x000d4000, 0x000d8000, 0x000dc000, 0x000e0000, 0x000e4000,
0x000e8000, 0x000ec000, 0x000f0000, 0x000f4000, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x00004000, 0x00008000, 0x0000c000, 0x00010000, 0x00014000, 0x00018000,
0x0001c000, 0x00020000, 0x00024000, 0x00028000, 0x0002c000, 0x00030000,
0x00034000, 0x00038000, 0x0003c000, 0x00040000, 0x00044000, 0x00048000,
0x0004c000, 0x00050000, 0x00054000, 0x00058000, 0x0005c000, 0x00060000,
0x00064000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x00068000, 0x0006c000, 0x00070000, 0x00074000, 0x00078000,
0x0007c000, 0x00080000, 0x00084000, 0x00088000, 0x0008c000, 0x00090000,
0x00094000, 0x00098000, 0x0009c000, 0x000a0000, 0x000a4000, 0x000a8000,
0x000ac000, 0x000b0000, 0x000b4000, 0x000b8000, 0x000bc000, 0x000c0000,
0x000c4000, 0x000c8000, 0x000cc000, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


const uint32_t base64_table_dec_32bit_d3[256] = {
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x00003e00, 0xffffffff, 0xffffffff, 0xffffffff, 0x00003f00,
0x00003400, 0x00003500, 0x00003600, 0x00003700, 0x00003800, 0x00003900,
0x00003a00, 0x00003b00, 0x00003c00, 0x00003d00, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
0x00000100, 0x00000200, 0x00000300, 0x00000400, 0x00000500, 0x00000600,
0x00000700, 0x00000800, 0x00000900, 0x00000a00, 0x00000b00, 0x00000c00,
0x00000d00, 0x00000e00, 0x00000f00, 0x00001000, 0x00001100, 0x00001200,
0x00001300, 0x00001400, 0x00001500, 0x00001600, 0x00001700, 0x00001800,
0x00001900, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0x00001a00, 0x00001b00, 0x00001c00, 0x00001d00, 0x00001e00,
0x00001f00, 0x00002000, 0x00002100, 0x00002200, 0x00002300, 0x00002400,
0x00002500, 0x00002600, 0x00002700, 0x00002800, 0x00002900, 0x00002a00,
0x00002b00, 0x00002c00, 0x00002d00, 0x00002e00, 0x00002f00, 0x00003000,
0x00003100, 0x00003200, 0x00003300, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};


#endif
////------ tables/table_dec_32bit.h  end <-----

////------ tables/table_enc_12bit.h  begine <-----
//#include <stdint.h>

const uint16_t base64_table_enc_12bit[] = {
#if BASE64_LITTLE_ENDIAN
	0x4141U, 0x4241U, 0x4341U, 0x4441U, 0x4541U, 0x4641U, 0x4741U, 0x4841U,
	0x4941U, 0x4A41U, 0x4B41U, 0x4C41U, 0x4D41U, 0x4E41U, 0x4F41U, 0x5041U,
	0x5141U, 0x5241U, 0x5341U, 0x5441U, 0x5541U, 0x5641U, 0x5741U, 0x5841U,
	0x5941U, 0x5A41U, 0x6141U, 0x6241U, 0x6341U, 0x6441U, 0x6541U, 0x6641U,
	0x6741U, 0x6841U, 0x6941U, 0x6A41U, 0x6B41U, 0x6C41U, 0x6D41U, 0x6E41U,
	0x6F41U, 0x7041U, 0x7141U, 0x7241U, 0x7341U, 0x7441U, 0x7541U, 0x7641U,
	0x7741U, 0x7841U, 0x7941U, 0x7A41U, 0x3041U, 0x3141U, 0x3241U, 0x3341U,
	0x3441U, 0x3541U, 0x3641U, 0x3741U, 0x3841U, 0x3941U, 0x2B41U, 0x2F41U,
	0x4142U, 0x4242U, 0x4342U, 0x4442U, 0x4542U, 0x4642U, 0x4742U, 0x4842U,
	0x4942U, 0x4A42U, 0x4B42U, 0x4C42U, 0x4D42U, 0x4E42U, 0x4F42U, 0x5042U,
	0x5142U, 0x5242U, 0x5342U, 0x5442U, 0x5542U, 0x5642U, 0x5742U, 0x5842U,
	0x5942U, 0x5A42U, 0x6142U, 0x6242U, 0x6342U, 0x6442U, 0x6542U, 0x6642U,
	0x6742U, 0x6842U, 0x6942U, 0x6A42U, 0x6B42U, 0x6C42U, 0x6D42U, 0x6E42U,
	0x6F42U, 0x7042U, 0x7142U, 0x7242U, 0x7342U, 0x7442U, 0x7542U, 0x7642U,
	0x7742U, 0x7842U, 0x7942U, 0x7A42U, 0x3042U, 0x3142U, 0x3242U, 0x3342U,
	0x3442U, 0x3542U, 0x3642U, 0x3742U, 0x3842U, 0x3942U, 0x2B42U, 0x2F42U,
	0x4143U, 0x4243U, 0x4343U, 0x4443U, 0x4543U, 0x4643U, 0x4743U, 0x4843U,
	0x4943U, 0x4A43U, 0x4B43U, 0x4C43U, 0x4D43U, 0x4E43U, 0x4F43U, 0x5043U,
	0x5143U, 0x5243U, 0x5343U, 0x5443U, 0x5543U, 0x5643U, 0x5743U, 0x5843U,
	0x5943U, 0x5A43U, 0x6143U, 0x6243U, 0x6343U, 0x6443U, 0x6543U, 0x6643U,
	0x6743U, 0x6843U, 0x6943U, 0x6A43U, 0x6B43U, 0x6C43U, 0x6D43U, 0x6E43U,
	0x6F43U, 0x7043U, 0x7143U, 0x7243U, 0x7343U, 0x7443U, 0x7543U, 0x7643U,
	0x7743U, 0x7843U, 0x7943U, 0x7A43U, 0x3043U, 0x3143U, 0x3243U, 0x3343U,
	0x3443U, 0x3543U, 0x3643U, 0x3743U, 0x3843U, 0x3943U, 0x2B43U, 0x2F43U,
	0x4144U, 0x4244U, 0x4344U, 0x4444U, 0x4544U, 0x4644U, 0x4744U, 0x4844U,
	0x4944U, 0x4A44U, 0x4B44U, 0x4C44U, 0x4D44U, 0x4E44U, 0x4F44U, 0x5044U,
	0x5144U, 0x5244U, 0x5344U, 0x5444U, 0x5544U, 0x5644U, 0x5744U, 0x5844U,
	0x5944U, 0x5A44U, 0x6144U, 0x6244U, 0x6344U, 0x6444U, 0x6544U, 0x6644U,
	0x6744U, 0x6844U, 0x6944U, 0x6A44U, 0x6B44U, 0x6C44U, 0x6D44U, 0x6E44U,
	0x6F44U, 0x7044U, 0x7144U, 0x7244U, 0x7344U, 0x7444U, 0x7544U, 0x7644U,
	0x7744U, 0x7844U, 0x7944U, 0x7A44U, 0x3044U, 0x3144U, 0x3244U, 0x3344U,
	0x3444U, 0x3544U, 0x3644U, 0x3744U, 0x3844U, 0x3944U, 0x2B44U, 0x2F44U,
	0x4145U, 0x4245U, 0x4345U, 0x4445U, 0x4545U, 0x4645U, 0x4745U, 0x4845U,
	0x4945U, 0x4A45U, 0x4B45U, 0x4C45U, 0x4D45U, 0x4E45U, 0x4F45U, 0x5045U,
	0x5145U, 0x5245U, 0x5345U, 0x5445U, 0x5545U, 0x5645U, 0x5745U, 0x5845U,
	0x5945U, 0x5A45U, 0x6145U, 0x6245U, 0x6345U, 0x6445U, 0x6545U, 0x6645U,
	0x6745U, 0x6845U, 0x6945U, 0x6A45U, 0x6B45U, 0x6C45U, 0x6D45U, 0x6E45U,
	0x6F45U, 0x7045U, 0x7145U, 0x7245U, 0x7345U, 0x7445U, 0x7545U, 0x7645U,
	0x7745U, 0x7845U, 0x7945U, 0x7A45U, 0x3045U, 0x3145U, 0x3245U, 0x3345U,
	0x3445U, 0x3545U, 0x3645U, 0x3745U, 0x3845U, 0x3945U, 0x2B45U, 0x2F45U,
	0x4146U, 0x4246U, 0x4346U, 0x4446U, 0x4546U, 0x4646U, 0x4746U, 0x4846U,
	0x4946U, 0x4A46U, 0x4B46U, 0x4C46U, 0x4D46U, 0x4E46U, 0x4F46U, 0x5046U,
	0x5146U, 0x5246U, 0x5346U, 0x5446U, 0x5546U, 0x5646U, 0x5746U, 0x5846U,
	0x5946U, 0x5A46U, 0x6146U, 0x6246U, 0x6346U, 0x6446U, 0x6546U, 0x6646U,
	0x6746U, 0x6846U, 0x6946U, 0x6A46U, 0x6B46U, 0x6C46U, 0x6D46U, 0x6E46U,
	0x6F46U, 0x7046U, 0x7146U, 0x7246U, 0x7346U, 0x7446U, 0x7546U, 0x7646U,
	0x7746U, 0x7846U, 0x7946U, 0x7A46U, 0x3046U, 0x3146U, 0x3246U, 0x3346U,
	0x3446U, 0x3546U, 0x3646U, 0x3746U, 0x3846U, 0x3946U, 0x2B46U, 0x2F46U,
	0x4147U, 0x4247U, 0x4347U, 0x4447U, 0x4547U, 0x4647U, 0x4747U, 0x4847U,
	0x4947U, 0x4A47U, 0x4B47U, 0x4C47U, 0x4D47U, 0x4E47U, 0x4F47U, 0x5047U,
	0x5147U, 0x5247U, 0x5347U, 0x5447U, 0x5547U, 0x5647U, 0x5747U, 0x5847U,
	0x5947U, 0x5A47U, 0x6147U, 0x6247U, 0x6347U, 0x6447U, 0x6547U, 0x6647U,
	0x6747U, 0x6847U, 0x6947U, 0x6A47U, 0x6B47U, 0x6C47U, 0x6D47U, 0x6E47U,
	0x6F47U, 0x7047U, 0x7147U, 0x7247U, 0x7347U, 0x7447U, 0x7547U, 0x7647U,
	0x7747U, 0x7847U, 0x7947U, 0x7A47U, 0x3047U, 0x3147U, 0x3247U, 0x3347U,
	0x3447U, 0x3547U, 0x3647U, 0x3747U, 0x3847U, 0x3947U, 0x2B47U, 0x2F47U,
	0x4148U, 0x4248U, 0x4348U, 0x4448U, 0x4548U, 0x4648U, 0x4748U, 0x4848U,
	0x4948U, 0x4A48U, 0x4B48U, 0x4C48U, 0x4D48U, 0x4E48U, 0x4F48U, 0x5048U,
	0x5148U, 0x5248U, 0x5348U, 0x5448U, 0x5548U, 0x5648U, 0x5748U, 0x5848U,
	0x5948U, 0x5A48U, 0x6148U, 0x6248U, 0x6348U, 0x6448U, 0x6548U, 0x6648U,
	0x6748U, 0x6848U, 0x6948U, 0x6A48U, 0x6B48U, 0x6C48U, 0x6D48U, 0x6E48U,
	0x6F48U, 0x7048U, 0x7148U, 0x7248U, 0x7348U, 0x7448U, 0x7548U, 0x7648U,
	0x7748U, 0x7848U, 0x7948U, 0x7A48U, 0x3048U, 0x3148U, 0x3248U, 0x3348U,
	0x3448U, 0x3548U, 0x3648U, 0x3748U, 0x3848U, 0x3948U, 0x2B48U, 0x2F48U,
	0x4149U, 0x4249U, 0x4349U, 0x4449U, 0x4549U, 0x4649U, 0x4749U, 0x4849U,
	0x4949U, 0x4A49U, 0x4B49U, 0x4C49U, 0x4D49U, 0x4E49U, 0x4F49U, 0x5049U,
	0x5149U, 0x5249U, 0x5349U, 0x5449U, 0x5549U, 0x5649U, 0x5749U, 0x5849U,
	0x5949U, 0x5A49U, 0x6149U, 0x6249U, 0x6349U, 0x6449U, 0x6549U, 0x6649U,
	0x6749U, 0x6849U, 0x6949U, 0x6A49U, 0x6B49U, 0x6C49U, 0x6D49U, 0x6E49U,
	0x6F49U, 0x7049U, 0x7149U, 0x7249U, 0x7349U, 0x7449U, 0x7549U, 0x7649U,
	0x7749U, 0x7849U, 0x7949U, 0x7A49U, 0x3049U, 0x3149U, 0x3249U, 0x3349U,
	0x3449U, 0x3549U, 0x3649U, 0x3749U, 0x3849U, 0x3949U, 0x2B49U, 0x2F49U,
	0x414AU, 0x424AU, 0x434AU, 0x444AU, 0x454AU, 0x464AU, 0x474AU, 0x484AU,
	0x494AU, 0x4A4AU, 0x4B4AU, 0x4C4AU, 0x4D4AU, 0x4E4AU, 0x4F4AU, 0x504AU,
	0x514AU, 0x524AU, 0x534AU, 0x544AU, 0x554AU, 0x564AU, 0x574AU, 0x584AU,
	0x594AU, 0x5A4AU, 0x614AU, 0x624AU, 0x634AU, 0x644AU, 0x654AU, 0x664AU,
	0x674AU, 0x684AU, 0x694AU, 0x6A4AU, 0x6B4AU, 0x6C4AU, 0x6D4AU, 0x6E4AU,
	0x6F4AU, 0x704AU, 0x714AU, 0x724AU, 0x734AU, 0x744AU, 0x754AU, 0x764AU,
	0x774AU, 0x784AU, 0x794AU, 0x7A4AU, 0x304AU, 0x314AU, 0x324AU, 0x334AU,
	0x344AU, 0x354AU, 0x364AU, 0x374AU, 0x384AU, 0x394AU, 0x2B4AU, 0x2F4AU,
	0x414BU, 0x424BU, 0x434BU, 0x444BU, 0x454BU, 0x464BU, 0x474BU, 0x484BU,
	0x494BU, 0x4A4BU, 0x4B4BU, 0x4C4BU, 0x4D4BU, 0x4E4BU, 0x4F4BU, 0x504BU,
	0x514BU, 0x524BU, 0x534BU, 0x544BU, 0x554BU, 0x564BU, 0x574BU, 0x584BU,
	0x594BU, 0x5A4BU, 0x614BU, 0x624BU, 0x634BU, 0x644BU, 0x654BU, 0x664BU,
	0x674BU, 0x684BU, 0x694BU, 0x6A4BU, 0x6B4BU, 0x6C4BU, 0x6D4BU, 0x6E4BU,
	0x6F4BU, 0x704BU, 0x714BU, 0x724BU, 0x734BU, 0x744BU, 0x754BU, 0x764BU,
	0x774BU, 0x784BU, 0x794BU, 0x7A4BU, 0x304BU, 0x314BU, 0x324BU, 0x334BU,
	0x344BU, 0x354BU, 0x364BU, 0x374BU, 0x384BU, 0x394BU, 0x2B4BU, 0x2F4BU,
	0x414CU, 0x424CU, 0x434CU, 0x444CU, 0x454CU, 0x464CU, 0x474CU, 0x484CU,
	0x494CU, 0x4A4CU, 0x4B4CU, 0x4C4CU, 0x4D4CU, 0x4E4CU, 0x4F4CU, 0x504CU,
	0x514CU, 0x524CU, 0x534CU, 0x544CU, 0x554CU, 0x564CU, 0x574CU, 0x584CU,
	0x594CU, 0x5A4CU, 0x614CU, 0x624CU, 0x634CU, 0x644CU, 0x654CU, 0x664CU,
	0x674CU, 0x684CU, 0x694CU, 0x6A4CU, 0x6B4CU, 0x6C4CU, 0x6D4CU, 0x6E4CU,
	0x6F4CU, 0x704CU, 0x714CU, 0x724CU, 0x734CU, 0x744CU, 0x754CU, 0x764CU,
	0x774CU, 0x784CU, 0x794CU, 0x7A4CU, 0x304CU, 0x314CU, 0x324CU, 0x334CU,
	0x344CU, 0x354CU, 0x364CU, 0x374CU, 0x384CU, 0x394CU, 0x2B4CU, 0x2F4CU,
	0x414DU, 0x424DU, 0x434DU, 0x444DU, 0x454DU, 0x464DU, 0x474DU, 0x484DU,
	0x494DU, 0x4A4DU, 0x4B4DU, 0x4C4DU, 0x4D4DU, 0x4E4DU, 0x4F4DU, 0x504DU,
	0x514DU, 0x524DU, 0x534DU, 0x544DU, 0x554DU, 0x564DU, 0x574DU, 0x584DU,
	0x594DU, 0x5A4DU, 0x614DU, 0x624DU, 0x634DU, 0x644DU, 0x654DU, 0x664DU,
	0x674DU, 0x684DU, 0x694DU, 0x6A4DU, 0x6B4DU, 0x6C4DU, 0x6D4DU, 0x6E4DU,
	0x6F4DU, 0x704DU, 0x714DU, 0x724DU, 0x734DU, 0x744DU, 0x754DU, 0x764DU,
	0x774DU, 0x784DU, 0x794DU, 0x7A4DU, 0x304DU, 0x314DU, 0x324DU, 0x334DU,
	0x344DU, 0x354DU, 0x364DU, 0x374DU, 0x384DU, 0x394DU, 0x2B4DU, 0x2F4DU,
	0x414EU, 0x424EU, 0x434EU, 0x444EU, 0x454EU, 0x464EU, 0x474EU, 0x484EU,
	0x494EU, 0x4A4EU, 0x4B4EU, 0x4C4EU, 0x4D4EU, 0x4E4EU, 0x4F4EU, 0x504EU,
	0x514EU, 0x524EU, 0x534EU, 0x544EU, 0x554EU, 0x564EU, 0x574EU, 0x584EU,
	0x594EU, 0x5A4EU, 0x614EU, 0x624EU, 0x634EU, 0x644EU, 0x654EU, 0x664EU,
	0x674EU, 0x684EU, 0x694EU, 0x6A4EU, 0x6B4EU, 0x6C4EU, 0x6D4EU, 0x6E4EU,
	0x6F4EU, 0x704EU, 0x714EU, 0x724EU, 0x734EU, 0x744EU, 0x754EU, 0x764EU,
	0x774EU, 0x784EU, 0x794EU, 0x7A4EU, 0x304EU, 0x314EU, 0x324EU, 0x334EU,
	0x344EU, 0x354EU, 0x364EU, 0x374EU, 0x384EU, 0x394EU, 0x2B4EU, 0x2F4EU,
	0x414FU, 0x424FU, 0x434FU, 0x444FU, 0x454FU, 0x464FU, 0x474FU, 0x484FU,
	0x494FU, 0x4A4FU, 0x4B4FU, 0x4C4FU, 0x4D4FU, 0x4E4FU, 0x4F4FU, 0x504FU,
	0x514FU, 0x524FU, 0x534FU, 0x544FU, 0x554FU, 0x564FU, 0x574FU, 0x584FU,
	0x594FU, 0x5A4FU, 0x614FU, 0x624FU, 0x634FU, 0x644FU, 0x654FU, 0x664FU,
	0x674FU, 0x684FU, 0x694FU, 0x6A4FU, 0x6B4FU, 0x6C4FU, 0x6D4FU, 0x6E4FU,
	0x6F4FU, 0x704FU, 0x714FU, 0x724FU, 0x734FU, 0x744FU, 0x754FU, 0x764FU,
	0x774FU, 0x784FU, 0x794FU, 0x7A4FU, 0x304FU, 0x314FU, 0x324FU, 0x334FU,
	0x344FU, 0x354FU, 0x364FU, 0x374FU, 0x384FU, 0x394FU, 0x2B4FU, 0x2F4FU,
	0x4150U, 0x4250U, 0x4350U, 0x4450U, 0x4550U, 0x4650U, 0x4750U, 0x4850U,
	0x4950U, 0x4A50U, 0x4B50U, 0x4C50U, 0x4D50U, 0x4E50U, 0x4F50U, 0x5050U,
	0x5150U, 0x5250U, 0x5350U, 0x5450U, 0x5550U, 0x5650U, 0x5750U, 0x5850U,
	0x5950U, 0x5A50U, 0x6150U, 0x6250U, 0x6350U, 0x6450U, 0x6550U, 0x6650U,
	0x6750U, 0x6850U, 0x6950U, 0x6A50U, 0x6B50U, 0x6C50U, 0x6D50U, 0x6E50U,
	0x6F50U, 0x7050U, 0x7150U, 0x7250U, 0x7350U, 0x7450U, 0x7550U, 0x7650U,
	0x7750U, 0x7850U, 0x7950U, 0x7A50U, 0x3050U, 0x3150U, 0x3250U, 0x3350U,
	0x3450U, 0x3550U, 0x3650U, 0x3750U, 0x3850U, 0x3950U, 0x2B50U, 0x2F50U,
	0x4151U, 0x4251U, 0x4351U, 0x4451U, 0x4551U, 0x4651U, 0x4751U, 0x4851U,
	0x4951U, 0x4A51U, 0x4B51U, 0x4C51U, 0x4D51U, 0x4E51U, 0x4F51U, 0x5051U,
	0x5151U, 0x5251U, 0x5351U, 0x5451U, 0x5551U, 0x5651U, 0x5751U, 0x5851U,
	0x5951U, 0x5A51U, 0x6151U, 0x6251U, 0x6351U, 0x6451U, 0x6551U, 0x6651U,
	0x6751U, 0x6851U, 0x6951U, 0x6A51U, 0x6B51U, 0x6C51U, 0x6D51U, 0x6E51U,
	0x6F51U, 0x7051U, 0x7151U, 0x7251U, 0x7351U, 0x7451U, 0x7551U, 0x7651U,
	0x7751U, 0x7851U, 0x7951U, 0x7A51U, 0x3051U, 0x3151U, 0x3251U, 0x3351U,
	0x3451U, 0x3551U, 0x3651U, 0x3751U, 0x3851U, 0x3951U, 0x2B51U, 0x2F51U,
	0x4152U, 0x4252U, 0x4352U, 0x4452U, 0x4552U, 0x4652U, 0x4752U, 0x4852U,
	0x4952U, 0x4A52U, 0x4B52U, 0x4C52U, 0x4D52U, 0x4E52U, 0x4F52U, 0x5052U,
	0x5152U, 0x5252U, 0x5352U, 0x5452U, 0x5552U, 0x5652U, 0x5752U, 0x5852U,
	0x5952U, 0x5A52U, 0x6152U, 0x6252U, 0x6352U, 0x6452U, 0x6552U, 0x6652U,
	0x6752U, 0x6852U, 0x6952U, 0x6A52U, 0x6B52U, 0x6C52U, 0x6D52U, 0x6E52U,
	0x6F52U, 0x7052U, 0x7152U, 0x7252U, 0x7352U, 0x7452U, 0x7552U, 0x7652U,
	0x7752U, 0x7852U, 0x7952U, 0x7A52U, 0x3052U, 0x3152U, 0x3252U, 0x3352U,
	0x3452U, 0x3552U, 0x3652U, 0x3752U, 0x3852U, 0x3952U, 0x2B52U, 0x2F52U,
	0x4153U, 0x4253U, 0x4353U, 0x4453U, 0x4553U, 0x4653U, 0x4753U, 0x4853U,
	0x4953U, 0x4A53U, 0x4B53U, 0x4C53U, 0x4D53U, 0x4E53U, 0x4F53U, 0x5053U,
	0x5153U, 0x5253U, 0x5353U, 0x5453U, 0x5553U, 0x5653U, 0x5753U, 0x5853U,
	0x5953U, 0x5A53U, 0x6153U, 0x6253U, 0x6353U, 0x6453U, 0x6553U, 0x6653U,
	0x6753U, 0x6853U, 0x6953U, 0x6A53U, 0x6B53U, 0x6C53U, 0x6D53U, 0x6E53U,
	0x6F53U, 0x7053U, 0x7153U, 0x7253U, 0x7353U, 0x7453U, 0x7553U, 0x7653U,
	0x7753U, 0x7853U, 0x7953U, 0x7A53U, 0x3053U, 0x3153U, 0x3253U, 0x3353U,
	0x3453U, 0x3553U, 0x3653U, 0x3753U, 0x3853U, 0x3953U, 0x2B53U, 0x2F53U,
	0x4154U, 0x4254U, 0x4354U, 0x4454U, 0x4554U, 0x4654U, 0x4754U, 0x4854U,
	0x4954U, 0x4A54U, 0x4B54U, 0x4C54U, 0x4D54U, 0x4E54U, 0x4F54U, 0x5054U,
	0x5154U, 0x5254U, 0x5354U, 0x5454U, 0x5554U, 0x5654U, 0x5754U, 0x5854U,
	0x5954U, 0x5A54U, 0x6154U, 0x6254U, 0x6354U, 0x6454U, 0x6554U, 0x6654U,
	0x6754U, 0x6854U, 0x6954U, 0x6A54U, 0x6B54U, 0x6C54U, 0x6D54U, 0x6E54U,
	0x6F54U, 0x7054U, 0x7154U, 0x7254U, 0x7354U, 0x7454U, 0x7554U, 0x7654U,
	0x7754U, 0x7854U, 0x7954U, 0x7A54U, 0x3054U, 0x3154U, 0x3254U, 0x3354U,
	0x3454U, 0x3554U, 0x3654U, 0x3754U, 0x3854U, 0x3954U, 0x2B54U, 0x2F54U,
	0x4155U, 0x4255U, 0x4355U, 0x4455U, 0x4555U, 0x4655U, 0x4755U, 0x4855U,
	0x4955U, 0x4A55U, 0x4B55U, 0x4C55U, 0x4D55U, 0x4E55U, 0x4F55U, 0x5055U,
	0x5155U, 0x5255U, 0x5355U, 0x5455U, 0x5555U, 0x5655U, 0x5755U, 0x5855U,
	0x5955U, 0x5A55U, 0x6155U, 0x6255U, 0x6355U, 0x6455U, 0x6555U, 0x6655U,
	0x6755U, 0x6855U, 0x6955U, 0x6A55U, 0x6B55U, 0x6C55U, 0x6D55U, 0x6E55U,
	0x6F55U, 0x7055U, 0x7155U, 0x7255U, 0x7355U, 0x7455U, 0x7555U, 0x7655U,
	0x7755U, 0x7855U, 0x7955U, 0x7A55U, 0x3055U, 0x3155U, 0x3255U, 0x3355U,
	0x3455U, 0x3555U, 0x3655U, 0x3755U, 0x3855U, 0x3955U, 0x2B55U, 0x2F55U,
	0x4156U, 0x4256U, 0x4356U, 0x4456U, 0x4556U, 0x4656U, 0x4756U, 0x4856U,
	0x4956U, 0x4A56U, 0x4B56U, 0x4C56U, 0x4D56U, 0x4E56U, 0x4F56U, 0x5056U,
	0x5156U, 0x5256U, 0x5356U, 0x5456U, 0x5556U, 0x5656U, 0x5756U, 0x5856U,
	0x5956U, 0x5A56U, 0x6156U, 0x6256U, 0x6356U, 0x6456U, 0x6556U, 0x6656U,
	0x6756U, 0x6856U, 0x6956U, 0x6A56U, 0x6B56U, 0x6C56U, 0x6D56U, 0x6E56U,
	0x6F56U, 0x7056U, 0x7156U, 0x7256U, 0x7356U, 0x7456U, 0x7556U, 0x7656U,
	0x7756U, 0x7856U, 0x7956U, 0x7A56U, 0x3056U, 0x3156U, 0x3256U, 0x3356U,
	0x3456U, 0x3556U, 0x3656U, 0x3756U, 0x3856U, 0x3956U, 0x2B56U, 0x2F56U,
	0x4157U, 0x4257U, 0x4357U, 0x4457U, 0x4557U, 0x4657U, 0x4757U, 0x4857U,
	0x4957U, 0x4A57U, 0x4B57U, 0x4C57U, 0x4D57U, 0x4E57U, 0x4F57U, 0x5057U,
	0x5157U, 0x5257U, 0x5357U, 0x5457U, 0x5557U, 0x5657U, 0x5757U, 0x5857U,
	0x5957U, 0x5A57U, 0x6157U, 0x6257U, 0x6357U, 0x6457U, 0x6557U, 0x6657U,
	0x6757U, 0x6857U, 0x6957U, 0x6A57U, 0x6B57U, 0x6C57U, 0x6D57U, 0x6E57U,
	0x6F57U, 0x7057U, 0x7157U, 0x7257U, 0x7357U, 0x7457U, 0x7557U, 0x7657U,
	0x7757U, 0x7857U, 0x7957U, 0x7A57U, 0x3057U, 0x3157U, 0x3257U, 0x3357U,
	0x3457U, 0x3557U, 0x3657U, 0x3757U, 0x3857U, 0x3957U, 0x2B57U, 0x2F57U,
	0x4158U, 0x4258U, 0x4358U, 0x4458U, 0x4558U, 0x4658U, 0x4758U, 0x4858U,
	0x4958U, 0x4A58U, 0x4B58U, 0x4C58U, 0x4D58U, 0x4E58U, 0x4F58U, 0x5058U,
	0x5158U, 0x5258U, 0x5358U, 0x5458U, 0x5558U, 0x5658U, 0x5758U, 0x5858U,
	0x5958U, 0x5A58U, 0x6158U, 0x6258U, 0x6358U, 0x6458U, 0x6558U, 0x6658U,
	0x6758U, 0x6858U, 0x6958U, 0x6A58U, 0x6B58U, 0x6C58U, 0x6D58U, 0x6E58U,
	0x6F58U, 0x7058U, 0x7158U, 0x7258U, 0x7358U, 0x7458U, 0x7558U, 0x7658U,
	0x7758U, 0x7858U, 0x7958U, 0x7A58U, 0x3058U, 0x3158U, 0x3258U, 0x3358U,
	0x3458U, 0x3558U, 0x3658U, 0x3758U, 0x3858U, 0x3958U, 0x2B58U, 0x2F58U,
	0x4159U, 0x4259U, 0x4359U, 0x4459U, 0x4559U, 0x4659U, 0x4759U, 0x4859U,
	0x4959U, 0x4A59U, 0x4B59U, 0x4C59U, 0x4D59U, 0x4E59U, 0x4F59U, 0x5059U,
	0x5159U, 0x5259U, 0x5359U, 0x5459U, 0x5559U, 0x5659U, 0x5759U, 0x5859U,
	0x5959U, 0x5A59U, 0x6159U, 0x6259U, 0x6359U, 0x6459U, 0x6559U, 0x6659U,
	0x6759U, 0x6859U, 0x6959U, 0x6A59U, 0x6B59U, 0x6C59U, 0x6D59U, 0x6E59U,
	0x6F59U, 0x7059U, 0x7159U, 0x7259U, 0x7359U, 0x7459U, 0x7559U, 0x7659U,
	0x7759U, 0x7859U, 0x7959U, 0x7A59U, 0x3059U, 0x3159U, 0x3259U, 0x3359U,
	0x3459U, 0x3559U, 0x3659U, 0x3759U, 0x3859U, 0x3959U, 0x2B59U, 0x2F59U,
	0x415AU, 0x425AU, 0x435AU, 0x445AU, 0x455AU, 0x465AU, 0x475AU, 0x485AU,
	0x495AU, 0x4A5AU, 0x4B5AU, 0x4C5AU, 0x4D5AU, 0x4E5AU, 0x4F5AU, 0x505AU,
	0x515AU, 0x525AU, 0x535AU, 0x545AU, 0x555AU, 0x565AU, 0x575AU, 0x585AU,
	0x595AU, 0x5A5AU, 0x615AU, 0x625AU, 0x635AU, 0x645AU, 0x655AU, 0x665AU,
	0x675AU, 0x685AU, 0x695AU, 0x6A5AU, 0x6B5AU, 0x6C5AU, 0x6D5AU, 0x6E5AU,
	0x6F5AU, 0x705AU, 0x715AU, 0x725AU, 0x735AU, 0x745AU, 0x755AU, 0x765AU,
	0x775AU, 0x785AU, 0x795AU, 0x7A5AU, 0x305AU, 0x315AU, 0x325AU, 0x335AU,
	0x345AU, 0x355AU, 0x365AU, 0x375AU, 0x385AU, 0x395AU, 0x2B5AU, 0x2F5AU,
	0x4161U, 0x4261U, 0x4361U, 0x4461U, 0x4561U, 0x4661U, 0x4761U, 0x4861U,
	0x4961U, 0x4A61U, 0x4B61U, 0x4C61U, 0x4D61U, 0x4E61U, 0x4F61U, 0x5061U,
	0x5161U, 0x5261U, 0x5361U, 0x5461U, 0x5561U, 0x5661U, 0x5761U, 0x5861U,
	0x5961U, 0x5A61U, 0x6161U, 0x6261U, 0x6361U, 0x6461U, 0x6561U, 0x6661U,
	0x6761U, 0x6861U, 0x6961U, 0x6A61U, 0x6B61U, 0x6C61U, 0x6D61U, 0x6E61U,
	0x6F61U, 0x7061U, 0x7161U, 0x7261U, 0x7361U, 0x7461U, 0x7561U, 0x7661U,
	0x7761U, 0x7861U, 0x7961U, 0x7A61U, 0x3061U, 0x3161U, 0x3261U, 0x3361U,
	0x3461U, 0x3561U, 0x3661U, 0x3761U, 0x3861U, 0x3961U, 0x2B61U, 0x2F61U,
	0x4162U, 0x4262U, 0x4362U, 0x4462U, 0x4562U, 0x4662U, 0x4762U, 0x4862U,
	0x4962U, 0x4A62U, 0x4B62U, 0x4C62U, 0x4D62U, 0x4E62U, 0x4F62U, 0x5062U,
	0x5162U, 0x5262U, 0x5362U, 0x5462U, 0x5562U, 0x5662U, 0x5762U, 0x5862U,
	0x5962U, 0x5A62U, 0x6162U, 0x6262U, 0x6362U, 0x6462U, 0x6562U, 0x6662U,
	0x6762U, 0x6862U, 0x6962U, 0x6A62U, 0x6B62U, 0x6C62U, 0x6D62U, 0x6E62U,
	0x6F62U, 0x7062U, 0x7162U, 0x7262U, 0x7362U, 0x7462U, 0x7562U, 0x7662U,
	0x7762U, 0x7862U, 0x7962U, 0x7A62U, 0x3062U, 0x3162U, 0x3262U, 0x3362U,
	0x3462U, 0x3562U, 0x3662U, 0x3762U, 0x3862U, 0x3962U, 0x2B62U, 0x2F62U,
	0x4163U, 0x4263U, 0x4363U, 0x4463U, 0x4563U, 0x4663U, 0x4763U, 0x4863U,
	0x4963U, 0x4A63U, 0x4B63U, 0x4C63U, 0x4D63U, 0x4E63U, 0x4F63U, 0x5063U,
	0x5163U, 0x5263U, 0x5363U, 0x5463U, 0x5563U, 0x5663U, 0x5763U, 0x5863U,
	0x5963U, 0x5A63U, 0x6163U, 0x6263U, 0x6363U, 0x6463U, 0x6563U, 0x6663U,
	0x6763U, 0x6863U, 0x6963U, 0x6A63U, 0x6B63U, 0x6C63U, 0x6D63U, 0x6E63U,
	0x6F63U, 0x7063U, 0x7163U, 0x7263U, 0x7363U, 0x7463U, 0x7563U, 0x7663U,
	0x7763U, 0x7863U, 0x7963U, 0x7A63U, 0x3063U, 0x3163U, 0x3263U, 0x3363U,
	0x3463U, 0x3563U, 0x3663U, 0x3763U, 0x3863U, 0x3963U, 0x2B63U, 0x2F63U,
	0x4164U, 0x4264U, 0x4364U, 0x4464U, 0x4564U, 0x4664U, 0x4764U, 0x4864U,
	0x4964U, 0x4A64U, 0x4B64U, 0x4C64U, 0x4D64U, 0x4E64U, 0x4F64U, 0x5064U,
	0x5164U, 0x5264U, 0x5364U, 0x5464U, 0x5564U, 0x5664U, 0x5764U, 0x5864U,
	0x5964U, 0x5A64U, 0x6164U, 0x6264U, 0x6364U, 0x6464U, 0x6564U, 0x6664U,
	0x6764U, 0x6864U, 0x6964U, 0x6A64U, 0x6B64U, 0x6C64U, 0x6D64U, 0x6E64U,
	0x6F64U, 0x7064U, 0x7164U, 0x7264U, 0x7364U, 0x7464U, 0x7564U, 0x7664U,
	0x7764U, 0x7864U, 0x7964U, 0x7A64U, 0x3064U, 0x3164U, 0x3264U, 0x3364U,
	0x3464U, 0x3564U, 0x3664U, 0x3764U, 0x3864U, 0x3964U, 0x2B64U, 0x2F64U,
	0x4165U, 0x4265U, 0x4365U, 0x4465U, 0x4565U, 0x4665U, 0x4765U, 0x4865U,
	0x4965U, 0x4A65U, 0x4B65U, 0x4C65U, 0x4D65U, 0x4E65U, 0x4F65U, 0x5065U,
	0x5165U, 0x5265U, 0x5365U, 0x5465U, 0x5565U, 0x5665U, 0x5765U, 0x5865U,
	0x5965U, 0x5A65U, 0x6165U, 0x6265U, 0x6365U, 0x6465U, 0x6565U, 0x6665U,
	0x6765U, 0x6865U, 0x6965U, 0x6A65U, 0x6B65U, 0x6C65U, 0x6D65U, 0x6E65U,
	0x6F65U, 0x7065U, 0x7165U, 0x7265U, 0x7365U, 0x7465U, 0x7565U, 0x7665U,
	0x7765U, 0x7865U, 0x7965U, 0x7A65U, 0x3065U, 0x3165U, 0x3265U, 0x3365U,
	0x3465U, 0x3565U, 0x3665U, 0x3765U, 0x3865U, 0x3965U, 0x2B65U, 0x2F65U,
	0x4166U, 0x4266U, 0x4366U, 0x4466U, 0x4566U, 0x4666U, 0x4766U, 0x4866U,
	0x4966U, 0x4A66U, 0x4B66U, 0x4C66U, 0x4D66U, 0x4E66U, 0x4F66U, 0x5066U,
	0x5166U, 0x5266U, 0x5366U, 0x5466U, 0x5566U, 0x5666U, 0x5766U, 0x5866U,
	0x5966U, 0x5A66U, 0x6166U, 0x6266U, 0x6366U, 0x6466U, 0x6566U, 0x6666U,
	0x6766U, 0x6866U, 0x6966U, 0x6A66U, 0x6B66U, 0x6C66U, 0x6D66U, 0x6E66U,
	0x6F66U, 0x7066U, 0x7166U, 0x7266U, 0x7366U, 0x7466U, 0x7566U, 0x7666U,
	0x7766U, 0x7866U, 0x7966U, 0x7A66U, 0x3066U, 0x3166U, 0x3266U, 0x3366U,
	0x3466U, 0x3566U, 0x3666U, 0x3766U, 0x3866U, 0x3966U, 0x2B66U, 0x2F66U,
	0x4167U, 0x4267U, 0x4367U, 0x4467U, 0x4567U, 0x4667U, 0x4767U, 0x4867U,
	0x4967U, 0x4A67U, 0x4B67U, 0x4C67U, 0x4D67U, 0x4E67U, 0x4F67U, 0x5067U,
	0x5167U, 0x5267U, 0x5367U, 0x5467U, 0x5567U, 0x5667U, 0x5767U, 0x5867U,
	0x5967U, 0x5A67U, 0x6167U, 0x6267U, 0x6367U, 0x6467U, 0x6567U, 0x6667U,
	0x6767U, 0x6867U, 0x6967U, 0x6A67U, 0x6B67U, 0x6C67U, 0x6D67U, 0x6E67U,
	0x6F67U, 0x7067U, 0x7167U, 0x7267U, 0x7367U, 0x7467U, 0x7567U, 0x7667U,
	0x7767U, 0x7867U, 0x7967U, 0x7A67U, 0x3067U, 0x3167U, 0x3267U, 0x3367U,
	0x3467U, 0x3567U, 0x3667U, 0x3767U, 0x3867U, 0x3967U, 0x2B67U, 0x2F67U,
	0x4168U, 0x4268U, 0x4368U, 0x4468U, 0x4568U, 0x4668U, 0x4768U, 0x4868U,
	0x4968U, 0x4A68U, 0x4B68U, 0x4C68U, 0x4D68U, 0x4E68U, 0x4F68U, 0x5068U,
	0x5168U, 0x5268U, 0x5368U, 0x5468U, 0x5568U, 0x5668U, 0x5768U, 0x5868U,
	0x5968U, 0x5A68U, 0x6168U, 0x6268U, 0x6368U, 0x6468U, 0x6568U, 0x6668U,
	0x6768U, 0x6868U, 0x6968U, 0x6A68U, 0x6B68U, 0x6C68U, 0x6D68U, 0x6E68U,
	0x6F68U, 0x7068U, 0x7168U, 0x7268U, 0x7368U, 0x7468U, 0x7568U, 0x7668U,
	0x7768U, 0x7868U, 0x7968U, 0x7A68U, 0x3068U, 0x3168U, 0x3268U, 0x3368U,
	0x3468U, 0x3568U, 0x3668U, 0x3768U, 0x3868U, 0x3968U, 0x2B68U, 0x2F68U,
	0x4169U, 0x4269U, 0x4369U, 0x4469U, 0x4569U, 0x4669U, 0x4769U, 0x4869U,
	0x4969U, 0x4A69U, 0x4B69U, 0x4C69U, 0x4D69U, 0x4E69U, 0x4F69U, 0x5069U,
	0x5169U, 0x5269U, 0x5369U, 0x5469U, 0x5569U, 0x5669U, 0x5769U, 0x5869U,
	0x5969U, 0x5A69U, 0x6169U, 0x6269U, 0x6369U, 0x6469U, 0x6569U, 0x6669U,
	0x6769U, 0x6869U, 0x6969U, 0x6A69U, 0x6B69U, 0x6C69U, 0x6D69U, 0x6E69U,
	0x6F69U, 0x7069U, 0x7169U, 0x7269U, 0x7369U, 0x7469U, 0x7569U, 0x7669U,
	0x7769U, 0x7869U, 0x7969U, 0x7A69U, 0x3069U, 0x3169U, 0x3269U, 0x3369U,
	0x3469U, 0x3569U, 0x3669U, 0x3769U, 0x3869U, 0x3969U, 0x2B69U, 0x2F69U,
	0x416AU, 0x426AU, 0x436AU, 0x446AU, 0x456AU, 0x466AU, 0x476AU, 0x486AU,
	0x496AU, 0x4A6AU, 0x4B6AU, 0x4C6AU, 0x4D6AU, 0x4E6AU, 0x4F6AU, 0x506AU,
	0x516AU, 0x526AU, 0x536AU, 0x546AU, 0x556AU, 0x566AU, 0x576AU, 0x586AU,
	0x596AU, 0x5A6AU, 0x616AU, 0x626AU, 0x636AU, 0x646AU, 0x656AU, 0x666AU,
	0x676AU, 0x686AU, 0x696AU, 0x6A6AU, 0x6B6AU, 0x6C6AU, 0x6D6AU, 0x6E6AU,
	0x6F6AU, 0x706AU, 0x716AU, 0x726AU, 0x736AU, 0x746AU, 0x756AU, 0x766AU,
	0x776AU, 0x786AU, 0x796AU, 0x7A6AU, 0x306AU, 0x316AU, 0x326AU, 0x336AU,
	0x346AU, 0x356AU, 0x366AU, 0x376AU, 0x386AU, 0x396AU, 0x2B6AU, 0x2F6AU,
	0x416BU, 0x426BU, 0x436BU, 0x446BU, 0x456BU, 0x466BU, 0x476BU, 0x486BU,
	0x496BU, 0x4A6BU, 0x4B6BU, 0x4C6BU, 0x4D6BU, 0x4E6BU, 0x4F6BU, 0x506BU,
	0x516BU, 0x526BU, 0x536BU, 0x546BU, 0x556BU, 0x566BU, 0x576BU, 0x586BU,
	0x596BU, 0x5A6BU, 0x616BU, 0x626BU, 0x636BU, 0x646BU, 0x656BU, 0x666BU,
	0x676BU, 0x686BU, 0x696BU, 0x6A6BU, 0x6B6BU, 0x6C6BU, 0x6D6BU, 0x6E6BU,
	0x6F6BU, 0x706BU, 0x716BU, 0x726BU, 0x736BU, 0x746BU, 0x756BU, 0x766BU,
	0x776BU, 0x786BU, 0x796BU, 0x7A6BU, 0x306BU, 0x316BU, 0x326BU, 0x336BU,
	0x346BU, 0x356BU, 0x366BU, 0x376BU, 0x386BU, 0x396BU, 0x2B6BU, 0x2F6BU,
	0x416CU, 0x426CU, 0x436CU, 0x446CU, 0x456CU, 0x466CU, 0x476CU, 0x486CU,
	0x496CU, 0x4A6CU, 0x4B6CU, 0x4C6CU, 0x4D6CU, 0x4E6CU, 0x4F6CU, 0x506CU,
	0x516CU, 0x526CU, 0x536CU, 0x546CU, 0x556CU, 0x566CU, 0x576CU, 0x586CU,
	0x596CU, 0x5A6CU, 0x616CU, 0x626CU, 0x636CU, 0x646CU, 0x656CU, 0x666CU,
	0x676CU, 0x686CU, 0x696CU, 0x6A6CU, 0x6B6CU, 0x6C6CU, 0x6D6CU, 0x6E6CU,
	0x6F6CU, 0x706CU, 0x716CU, 0x726CU, 0x736CU, 0x746CU, 0x756CU, 0x766CU,
	0x776CU, 0x786CU, 0x796CU, 0x7A6CU, 0x306CU, 0x316CU, 0x326CU, 0x336CU,
	0x346CU, 0x356CU, 0x366CU, 0x376CU, 0x386CU, 0x396CU, 0x2B6CU, 0x2F6CU,
	0x416DU, 0x426DU, 0x436DU, 0x446DU, 0x456DU, 0x466DU, 0x476DU, 0x486DU,
	0x496DU, 0x4A6DU, 0x4B6DU, 0x4C6DU, 0x4D6DU, 0x4E6DU, 0x4F6DU, 0x506DU,
	0x516DU, 0x526DU, 0x536DU, 0x546DU, 0x556DU, 0x566DU, 0x576DU, 0x586DU,
	0x596DU, 0x5A6DU, 0x616DU, 0x626DU, 0x636DU, 0x646DU, 0x656DU, 0x666DU,
	0x676DU, 0x686DU, 0x696DU, 0x6A6DU, 0x6B6DU, 0x6C6DU, 0x6D6DU, 0x6E6DU,
	0x6F6DU, 0x706DU, 0x716DU, 0x726DU, 0x736DU, 0x746DU, 0x756DU, 0x766DU,
	0x776DU, 0x786DU, 0x796DU, 0x7A6DU, 0x306DU, 0x316DU, 0x326DU, 0x336DU,
	0x346DU, 0x356DU, 0x366DU, 0x376DU, 0x386DU, 0x396DU, 0x2B6DU, 0x2F6DU,
	0x416EU, 0x426EU, 0x436EU, 0x446EU, 0x456EU, 0x466EU, 0x476EU, 0x486EU,
	0x496EU, 0x4A6EU, 0x4B6EU, 0x4C6EU, 0x4D6EU, 0x4E6EU, 0x4F6EU, 0x506EU,
	0x516EU, 0x526EU, 0x536EU, 0x546EU, 0x556EU, 0x566EU, 0x576EU, 0x586EU,
	0x596EU, 0x5A6EU, 0x616EU, 0x626EU, 0x636EU, 0x646EU, 0x656EU, 0x666EU,
	0x676EU, 0x686EU, 0x696EU, 0x6A6EU, 0x6B6EU, 0x6C6EU, 0x6D6EU, 0x6E6EU,
	0x6F6EU, 0x706EU, 0x716EU, 0x726EU, 0x736EU, 0x746EU, 0x756EU, 0x766EU,
	0x776EU, 0x786EU, 0x796EU, 0x7A6EU, 0x306EU, 0x316EU, 0x326EU, 0x336EU,
	0x346EU, 0x356EU, 0x366EU, 0x376EU, 0x386EU, 0x396EU, 0x2B6EU, 0x2F6EU,
	0x416FU, 0x426FU, 0x436FU, 0x446FU, 0x456FU, 0x466FU, 0x476FU, 0x486FU,
	0x496FU, 0x4A6FU, 0x4B6FU, 0x4C6FU, 0x4D6FU, 0x4E6FU, 0x4F6FU, 0x506FU,
	0x516FU, 0x526FU, 0x536FU, 0x546FU, 0x556FU, 0x566FU, 0x576FU, 0x586FU,
	0x596FU, 0x5A6FU, 0x616FU, 0x626FU, 0x636FU, 0x646FU, 0x656FU, 0x666FU,
	0x676FU, 0x686FU, 0x696FU, 0x6A6FU, 0x6B6FU, 0x6C6FU, 0x6D6FU, 0x6E6FU,
	0x6F6FU, 0x706FU, 0x716FU, 0x726FU, 0x736FU, 0x746FU, 0x756FU, 0x766FU,
	0x776FU, 0x786FU, 0x796FU, 0x7A6FU, 0x306FU, 0x316FU, 0x326FU, 0x336FU,
	0x346FU, 0x356FU, 0x366FU, 0x376FU, 0x386FU, 0x396FU, 0x2B6FU, 0x2F6FU,
	0x4170U, 0x4270U, 0x4370U, 0x4470U, 0x4570U, 0x4670U, 0x4770U, 0x4870U,
	0x4970U, 0x4A70U, 0x4B70U, 0x4C70U, 0x4D70U, 0x4E70U, 0x4F70U, 0x5070U,
	0x5170U, 0x5270U, 0x5370U, 0x5470U, 0x5570U, 0x5670U, 0x5770U, 0x5870U,
	0x5970U, 0x5A70U, 0x6170U, 0x6270U, 0x6370U, 0x6470U, 0x6570U, 0x6670U,
	0x6770U, 0x6870U, 0x6970U, 0x6A70U, 0x6B70U, 0x6C70U, 0x6D70U, 0x6E70U,
	0x6F70U, 0x7070U, 0x7170U, 0x7270U, 0x7370U, 0x7470U, 0x7570U, 0x7670U,
	0x7770U, 0x7870U, 0x7970U, 0x7A70U, 0x3070U, 0x3170U, 0x3270U, 0x3370U,
	0x3470U, 0x3570U, 0x3670U, 0x3770U, 0x3870U, 0x3970U, 0x2B70U, 0x2F70U,
	0x4171U, 0x4271U, 0x4371U, 0x4471U, 0x4571U, 0x4671U, 0x4771U, 0x4871U,
	0x4971U, 0x4A71U, 0x4B71U, 0x4C71U, 0x4D71U, 0x4E71U, 0x4F71U, 0x5071U,
	0x5171U, 0x5271U, 0x5371U, 0x5471U, 0x5571U, 0x5671U, 0x5771U, 0x5871U,
	0x5971U, 0x5A71U, 0x6171U, 0x6271U, 0x6371U, 0x6471U, 0x6571U, 0x6671U,
	0x6771U, 0x6871U, 0x6971U, 0x6A71U, 0x6B71U, 0x6C71U, 0x6D71U, 0x6E71U,
	0x6F71U, 0x7071U, 0x7171U, 0x7271U, 0x7371U, 0x7471U, 0x7571U, 0x7671U,
	0x7771U, 0x7871U, 0x7971U, 0x7A71U, 0x3071U, 0x3171U, 0x3271U, 0x3371U,
	0x3471U, 0x3571U, 0x3671U, 0x3771U, 0x3871U, 0x3971U, 0x2B71U, 0x2F71U,
	0x4172U, 0x4272U, 0x4372U, 0x4472U, 0x4572U, 0x4672U, 0x4772U, 0x4872U,
	0x4972U, 0x4A72U, 0x4B72U, 0x4C72U, 0x4D72U, 0x4E72U, 0x4F72U, 0x5072U,
	0x5172U, 0x5272U, 0x5372U, 0x5472U, 0x5572U, 0x5672U, 0x5772U, 0x5872U,
	0x5972U, 0x5A72U, 0x6172U, 0x6272U, 0x6372U, 0x6472U, 0x6572U, 0x6672U,
	0x6772U, 0x6872U, 0x6972U, 0x6A72U, 0x6B72U, 0x6C72U, 0x6D72U, 0x6E72U,
	0x6F72U, 0x7072U, 0x7172U, 0x7272U, 0x7372U, 0x7472U, 0x7572U, 0x7672U,
	0x7772U, 0x7872U, 0x7972U, 0x7A72U, 0x3072U, 0x3172U, 0x3272U, 0x3372U,
	0x3472U, 0x3572U, 0x3672U, 0x3772U, 0x3872U, 0x3972U, 0x2B72U, 0x2F72U,
	0x4173U, 0x4273U, 0x4373U, 0x4473U, 0x4573U, 0x4673U, 0x4773U, 0x4873U,
	0x4973U, 0x4A73U, 0x4B73U, 0x4C73U, 0x4D73U, 0x4E73U, 0x4F73U, 0x5073U,
	0x5173U, 0x5273U, 0x5373U, 0x5473U, 0x5573U, 0x5673U, 0x5773U, 0x5873U,
	0x5973U, 0x5A73U, 0x6173U, 0x6273U, 0x6373U, 0x6473U, 0x6573U, 0x6673U,
	0x6773U, 0x6873U, 0x6973U, 0x6A73U, 0x6B73U, 0x6C73U, 0x6D73U, 0x6E73U,
	0x6F73U, 0x7073U, 0x7173U, 0x7273U, 0x7373U, 0x7473U, 0x7573U, 0x7673U,
	0x7773U, 0x7873U, 0x7973U, 0x7A73U, 0x3073U, 0x3173U, 0x3273U, 0x3373U,
	0x3473U, 0x3573U, 0x3673U, 0x3773U, 0x3873U, 0x3973U, 0x2B73U, 0x2F73U,
	0x4174U, 0x4274U, 0x4374U, 0x4474U, 0x4574U, 0x4674U, 0x4774U, 0x4874U,
	0x4974U, 0x4A74U, 0x4B74U, 0x4C74U, 0x4D74U, 0x4E74U, 0x4F74U, 0x5074U,
	0x5174U, 0x5274U, 0x5374U, 0x5474U, 0x5574U, 0x5674U, 0x5774U, 0x5874U,
	0x5974U, 0x5A74U, 0x6174U, 0x6274U, 0x6374U, 0x6474U, 0x6574U, 0x6674U,
	0x6774U, 0x6874U, 0x6974U, 0x6A74U, 0x6B74U, 0x6C74U, 0x6D74U, 0x6E74U,
	0x6F74U, 0x7074U, 0x7174U, 0x7274U, 0x7374U, 0x7474U, 0x7574U, 0x7674U,
	0x7774U, 0x7874U, 0x7974U, 0x7A74U, 0x3074U, 0x3174U, 0x3274U, 0x3374U,
	0x3474U, 0x3574U, 0x3674U, 0x3774U, 0x3874U, 0x3974U, 0x2B74U, 0x2F74U,
	0x4175U, 0x4275U, 0x4375U, 0x4475U, 0x4575U, 0x4675U, 0x4775U, 0x4875U,
	0x4975U, 0x4A75U, 0x4B75U, 0x4C75U, 0x4D75U, 0x4E75U, 0x4F75U, 0x5075U,
	0x5175U, 0x5275U, 0x5375U, 0x5475U, 0x5575U, 0x5675U, 0x5775U, 0x5875U,
	0x5975U, 0x5A75U, 0x6175U, 0x6275U, 0x6375U, 0x6475U, 0x6575U, 0x6675U,
	0x6775U, 0x6875U, 0x6975U, 0x6A75U, 0x6B75U, 0x6C75U, 0x6D75U, 0x6E75U,
	0x6F75U, 0x7075U, 0x7175U, 0x7275U, 0x7375U, 0x7475U, 0x7575U, 0x7675U,
	0x7775U, 0x7875U, 0x7975U, 0x7A75U, 0x3075U, 0x3175U, 0x3275U, 0x3375U,
	0x3475U, 0x3575U, 0x3675U, 0x3775U, 0x3875U, 0x3975U, 0x2B75U, 0x2F75U,
	0x4176U, 0x4276U, 0x4376U, 0x4476U, 0x4576U, 0x4676U, 0x4776U, 0x4876U,
	0x4976U, 0x4A76U, 0x4B76U, 0x4C76U, 0x4D76U, 0x4E76U, 0x4F76U, 0x5076U,
	0x5176U, 0x5276U, 0x5376U, 0x5476U, 0x5576U, 0x5676U, 0x5776U, 0x5876U,
	0x5976U, 0x5A76U, 0x6176U, 0x6276U, 0x6376U, 0x6476U, 0x6576U, 0x6676U,
	0x6776U, 0x6876U, 0x6976U, 0x6A76U, 0x6B76U, 0x6C76U, 0x6D76U, 0x6E76U,
	0x6F76U, 0x7076U, 0x7176U, 0x7276U, 0x7376U, 0x7476U, 0x7576U, 0x7676U,
	0x7776U, 0x7876U, 0x7976U, 0x7A76U, 0x3076U, 0x3176U, 0x3276U, 0x3376U,
	0x3476U, 0x3576U, 0x3676U, 0x3776U, 0x3876U, 0x3976U, 0x2B76U, 0x2F76U,
	0x4177U, 0x4277U, 0x4377U, 0x4477U, 0x4577U, 0x4677U, 0x4777U, 0x4877U,
	0x4977U, 0x4A77U, 0x4B77U, 0x4C77U, 0x4D77U, 0x4E77U, 0x4F77U, 0x5077U,
	0x5177U, 0x5277U, 0x5377U, 0x5477U, 0x5577U, 0x5677U, 0x5777U, 0x5877U,
	0x5977U, 0x5A77U, 0x6177U, 0x6277U, 0x6377U, 0x6477U, 0x6577U, 0x6677U,
	0x6777U, 0x6877U, 0x6977U, 0x6A77U, 0x6B77U, 0x6C77U, 0x6D77U, 0x6E77U,
	0x6F77U, 0x7077U, 0x7177U, 0x7277U, 0x7377U, 0x7477U, 0x7577U, 0x7677U,
	0x7777U, 0x7877U, 0x7977U, 0x7A77U, 0x3077U, 0x3177U, 0x3277U, 0x3377U,
	0x3477U, 0x3577U, 0x3677U, 0x3777U, 0x3877U, 0x3977U, 0x2B77U, 0x2F77U,
	0x4178U, 0x4278U, 0x4378U, 0x4478U, 0x4578U, 0x4678U, 0x4778U, 0x4878U,
	0x4978U, 0x4A78U, 0x4B78U, 0x4C78U, 0x4D78U, 0x4E78U, 0x4F78U, 0x5078U,
	0x5178U, 0x5278U, 0x5378U, 0x5478U, 0x5578U, 0x5678U, 0x5778U, 0x5878U,
	0x5978U, 0x5A78U, 0x6178U, 0x6278U, 0x6378U, 0x6478U, 0x6578U, 0x6678U,
	0x6778U, 0x6878U, 0x6978U, 0x6A78U, 0x6B78U, 0x6C78U, 0x6D78U, 0x6E78U,
	0x6F78U, 0x7078U, 0x7178U, 0x7278U, 0x7378U, 0x7478U, 0x7578U, 0x7678U,
	0x7778U, 0x7878U, 0x7978U, 0x7A78U, 0x3078U, 0x3178U, 0x3278U, 0x3378U,
	0x3478U, 0x3578U, 0x3678U, 0x3778U, 0x3878U, 0x3978U, 0x2B78U, 0x2F78U,
	0x4179U, 0x4279U, 0x4379U, 0x4479U, 0x4579U, 0x4679U, 0x4779U, 0x4879U,
	0x4979U, 0x4A79U, 0x4B79U, 0x4C79U, 0x4D79U, 0x4E79U, 0x4F79U, 0x5079U,
	0x5179U, 0x5279U, 0x5379U, 0x5479U, 0x5579U, 0x5679U, 0x5779U, 0x5879U,
	0x5979U, 0x5A79U, 0x6179U, 0x6279U, 0x6379U, 0x6479U, 0x6579U, 0x6679U,
	0x6779U, 0x6879U, 0x6979U, 0x6A79U, 0x6B79U, 0x6C79U, 0x6D79U, 0x6E79U,
	0x6F79U, 0x7079U, 0x7179U, 0x7279U, 0x7379U, 0x7479U, 0x7579U, 0x7679U,
	0x7779U, 0x7879U, 0x7979U, 0x7A79U, 0x3079U, 0x3179U, 0x3279U, 0x3379U,
	0x3479U, 0x3579U, 0x3679U, 0x3779U, 0x3879U, 0x3979U, 0x2B79U, 0x2F79U,
	0x417AU, 0x427AU, 0x437AU, 0x447AU, 0x457AU, 0x467AU, 0x477AU, 0x487AU,
	0x497AU, 0x4A7AU, 0x4B7AU, 0x4C7AU, 0x4D7AU, 0x4E7AU, 0x4F7AU, 0x507AU,
	0x517AU, 0x527AU, 0x537AU, 0x547AU, 0x557AU, 0x567AU, 0x577AU, 0x587AU,
	0x597AU, 0x5A7AU, 0x617AU, 0x627AU, 0x637AU, 0x647AU, 0x657AU, 0x667AU,
	0x677AU, 0x687AU, 0x697AU, 0x6A7AU, 0x6B7AU, 0x6C7AU, 0x6D7AU, 0x6E7AU,
	0x6F7AU, 0x707AU, 0x717AU, 0x727AU, 0x737AU, 0x747AU, 0x757AU, 0x767AU,
	0x777AU, 0x787AU, 0x797AU, 0x7A7AU, 0x307AU, 0x317AU, 0x327AU, 0x337AU,
	0x347AU, 0x357AU, 0x367AU, 0x377AU, 0x387AU, 0x397AU, 0x2B7AU, 0x2F7AU,
	0x4130U, 0x4230U, 0x4330U, 0x4430U, 0x4530U, 0x4630U, 0x4730U, 0x4830U,
	0x4930U, 0x4A30U, 0x4B30U, 0x4C30U, 0x4D30U, 0x4E30U, 0x4F30U, 0x5030U,
	0x5130U, 0x5230U, 0x5330U, 0x5430U, 0x5530U, 0x5630U, 0x5730U, 0x5830U,
	0x5930U, 0x5A30U, 0x6130U, 0x6230U, 0x6330U, 0x6430U, 0x6530U, 0x6630U,
	0x6730U, 0x6830U, 0x6930U, 0x6A30U, 0x6B30U, 0x6C30U, 0x6D30U, 0x6E30U,
	0x6F30U, 0x7030U, 0x7130U, 0x7230U, 0x7330U, 0x7430U, 0x7530U, 0x7630U,
	0x7730U, 0x7830U, 0x7930U, 0x7A30U, 0x3030U, 0x3130U, 0x3230U, 0x3330U,
	0x3430U, 0x3530U, 0x3630U, 0x3730U, 0x3830U, 0x3930U, 0x2B30U, 0x2F30U,
	0x4131U, 0x4231U, 0x4331U, 0x4431U, 0x4531U, 0x4631U, 0x4731U, 0x4831U,
	0x4931U, 0x4A31U, 0x4B31U, 0x4C31U, 0x4D31U, 0x4E31U, 0x4F31U, 0x5031U,
	0x5131U, 0x5231U, 0x5331U, 0x5431U, 0x5531U, 0x5631U, 0x5731U, 0x5831U,
	0x5931U, 0x5A31U, 0x6131U, 0x6231U, 0x6331U, 0x6431U, 0x6531U, 0x6631U,
	0x6731U, 0x6831U, 0x6931U, 0x6A31U, 0x6B31U, 0x6C31U, 0x6D31U, 0x6E31U,
	0x6F31U, 0x7031U, 0x7131U, 0x7231U, 0x7331U, 0x7431U, 0x7531U, 0x7631U,
	0x7731U, 0x7831U, 0x7931U, 0x7A31U, 0x3031U, 0x3131U, 0x3231U, 0x3331U,
	0x3431U, 0x3531U, 0x3631U, 0x3731U, 0x3831U, 0x3931U, 0x2B31U, 0x2F31U,
	0x4132U, 0x4232U, 0x4332U, 0x4432U, 0x4532U, 0x4632U, 0x4732U, 0x4832U,
	0x4932U, 0x4A32U, 0x4B32U, 0x4C32U, 0x4D32U, 0x4E32U, 0x4F32U, 0x5032U,
	0x5132U, 0x5232U, 0x5332U, 0x5432U, 0x5532U, 0x5632U, 0x5732U, 0x5832U,
	0x5932U, 0x5A32U, 0x6132U, 0x6232U, 0x6332U, 0x6432U, 0x6532U, 0x6632U,
	0x6732U, 0x6832U, 0x6932U, 0x6A32U, 0x6B32U, 0x6C32U, 0x6D32U, 0x6E32U,
	0x6F32U, 0x7032U, 0x7132U, 0x7232U, 0x7332U, 0x7432U, 0x7532U, 0x7632U,
	0x7732U, 0x7832U, 0x7932U, 0x7A32U, 0x3032U, 0x3132U, 0x3232U, 0x3332U,
	0x3432U, 0x3532U, 0x3632U, 0x3732U, 0x3832U, 0x3932U, 0x2B32U, 0x2F32U,
	0x4133U, 0x4233U, 0x4333U, 0x4433U, 0x4533U, 0x4633U, 0x4733U, 0x4833U,
	0x4933U, 0x4A33U, 0x4B33U, 0x4C33U, 0x4D33U, 0x4E33U, 0x4F33U, 0x5033U,
	0x5133U, 0x5233U, 0x5333U, 0x5433U, 0x5533U, 0x5633U, 0x5733U, 0x5833U,
	0x5933U, 0x5A33U, 0x6133U, 0x6233U, 0x6333U, 0x6433U, 0x6533U, 0x6633U,
	0x6733U, 0x6833U, 0x6933U, 0x6A33U, 0x6B33U, 0x6C33U, 0x6D33U, 0x6E33U,
	0x6F33U, 0x7033U, 0x7133U, 0x7233U, 0x7333U, 0x7433U, 0x7533U, 0x7633U,
	0x7733U, 0x7833U, 0x7933U, 0x7A33U, 0x3033U, 0x3133U, 0x3233U, 0x3333U,
	0x3433U, 0x3533U, 0x3633U, 0x3733U, 0x3833U, 0x3933U, 0x2B33U, 0x2F33U,
	0x4134U, 0x4234U, 0x4334U, 0x4434U, 0x4534U, 0x4634U, 0x4734U, 0x4834U,
	0x4934U, 0x4A34U, 0x4B34U, 0x4C34U, 0x4D34U, 0x4E34U, 0x4F34U, 0x5034U,
	0x5134U, 0x5234U, 0x5334U, 0x5434U, 0x5534U, 0x5634U, 0x5734U, 0x5834U,
	0x5934U, 0x5A34U, 0x6134U, 0x6234U, 0x6334U, 0x6434U, 0x6534U, 0x6634U,
	0x6734U, 0x6834U, 0x6934U, 0x6A34U, 0x6B34U, 0x6C34U, 0x6D34U, 0x6E34U,
	0x6F34U, 0x7034U, 0x7134U, 0x7234U, 0x7334U, 0x7434U, 0x7534U, 0x7634U,
	0x7734U, 0x7834U, 0x7934U, 0x7A34U, 0x3034U, 0x3134U, 0x3234U, 0x3334U,
	0x3434U, 0x3534U, 0x3634U, 0x3734U, 0x3834U, 0x3934U, 0x2B34U, 0x2F34U,
	0x4135U, 0x4235U, 0x4335U, 0x4435U, 0x4535U, 0x4635U, 0x4735U, 0x4835U,
	0x4935U, 0x4A35U, 0x4B35U, 0x4C35U, 0x4D35U, 0x4E35U, 0x4F35U, 0x5035U,
	0x5135U, 0x5235U, 0x5335U, 0x5435U, 0x5535U, 0x5635U, 0x5735U, 0x5835U,
	0x5935U, 0x5A35U, 0x6135U, 0x6235U, 0x6335U, 0x6435U, 0x6535U, 0x6635U,
	0x6735U, 0x6835U, 0x6935U, 0x6A35U, 0x6B35U, 0x6C35U, 0x6D35U, 0x6E35U,
	0x6F35U, 0x7035U, 0x7135U, 0x7235U, 0x7335U, 0x7435U, 0x7535U, 0x7635U,
	0x7735U, 0x7835U, 0x7935U, 0x7A35U, 0x3035U, 0x3135U, 0x3235U, 0x3335U,
	0x3435U, 0x3535U, 0x3635U, 0x3735U, 0x3835U, 0x3935U, 0x2B35U, 0x2F35U,
	0x4136U, 0x4236U, 0x4336U, 0x4436U, 0x4536U, 0x4636U, 0x4736U, 0x4836U,
	0x4936U, 0x4A36U, 0x4B36U, 0x4C36U, 0x4D36U, 0x4E36U, 0x4F36U, 0x5036U,
	0x5136U, 0x5236U, 0x5336U, 0x5436U, 0x5536U, 0x5636U, 0x5736U, 0x5836U,
	0x5936U, 0x5A36U, 0x6136U, 0x6236U, 0x6336U, 0x6436U, 0x6536U, 0x6636U,
	0x6736U, 0x6836U, 0x6936U, 0x6A36U, 0x6B36U, 0x6C36U, 0x6D36U, 0x6E36U,
	0x6F36U, 0x7036U, 0x7136U, 0x7236U, 0x7336U, 0x7436U, 0x7536U, 0x7636U,
	0x7736U, 0x7836U, 0x7936U, 0x7A36U, 0x3036U, 0x3136U, 0x3236U, 0x3336U,
	0x3436U, 0x3536U, 0x3636U, 0x3736U, 0x3836U, 0x3936U, 0x2B36U, 0x2F36U,
	0x4137U, 0x4237U, 0x4337U, 0x4437U, 0x4537U, 0x4637U, 0x4737U, 0x4837U,
	0x4937U, 0x4A37U, 0x4B37U, 0x4C37U, 0x4D37U, 0x4E37U, 0x4F37U, 0x5037U,
	0x5137U, 0x5237U, 0x5337U, 0x5437U, 0x5537U, 0x5637U, 0x5737U, 0x5837U,
	0x5937U, 0x5A37U, 0x6137U, 0x6237U, 0x6337U, 0x6437U, 0x6537U, 0x6637U,
	0x6737U, 0x6837U, 0x6937U, 0x6A37U, 0x6B37U, 0x6C37U, 0x6D37U, 0x6E37U,
	0x6F37U, 0x7037U, 0x7137U, 0x7237U, 0x7337U, 0x7437U, 0x7537U, 0x7637U,
	0x7737U, 0x7837U, 0x7937U, 0x7A37U, 0x3037U, 0x3137U, 0x3237U, 0x3337U,
	0x3437U, 0x3537U, 0x3637U, 0x3737U, 0x3837U, 0x3937U, 0x2B37U, 0x2F37U,
	0x4138U, 0x4238U, 0x4338U, 0x4438U, 0x4538U, 0x4638U, 0x4738U, 0x4838U,
	0x4938U, 0x4A38U, 0x4B38U, 0x4C38U, 0x4D38U, 0x4E38U, 0x4F38U, 0x5038U,
	0x5138U, 0x5238U, 0x5338U, 0x5438U, 0x5538U, 0x5638U, 0x5738U, 0x5838U,
	0x5938U, 0x5A38U, 0x6138U, 0x6238U, 0x6338U, 0x6438U, 0x6538U, 0x6638U,
	0x6738U, 0x6838U, 0x6938U, 0x6A38U, 0x6B38U, 0x6C38U, 0x6D38U, 0x6E38U,
	0x6F38U, 0x7038U, 0x7138U, 0x7238U, 0x7338U, 0x7438U, 0x7538U, 0x7638U,
	0x7738U, 0x7838U, 0x7938U, 0x7A38U, 0x3038U, 0x3138U, 0x3238U, 0x3338U,
	0x3438U, 0x3538U, 0x3638U, 0x3738U, 0x3838U, 0x3938U, 0x2B38U, 0x2F38U,
	0x4139U, 0x4239U, 0x4339U, 0x4439U, 0x4539U, 0x4639U, 0x4739U, 0x4839U,
	0x4939U, 0x4A39U, 0x4B39U, 0x4C39U, 0x4D39U, 0x4E39U, 0x4F39U, 0x5039U,
	0x5139U, 0x5239U, 0x5339U, 0x5439U, 0x5539U, 0x5639U, 0x5739U, 0x5839U,
	0x5939U, 0x5A39U, 0x6139U, 0x6239U, 0x6339U, 0x6439U, 0x6539U, 0x6639U,
	0x6739U, 0x6839U, 0x6939U, 0x6A39U, 0x6B39U, 0x6C39U, 0x6D39U, 0x6E39U,
	0x6F39U, 0x7039U, 0x7139U, 0x7239U, 0x7339U, 0x7439U, 0x7539U, 0x7639U,
	0x7739U, 0x7839U, 0x7939U, 0x7A39U, 0x3039U, 0x3139U, 0x3239U, 0x3339U,
	0x3439U, 0x3539U, 0x3639U, 0x3739U, 0x3839U, 0x3939U, 0x2B39U, 0x2F39U,
	0x412BU, 0x422BU, 0x432BU, 0x442BU, 0x452BU, 0x462BU, 0x472BU, 0x482BU,
	0x492BU, 0x4A2BU, 0x4B2BU, 0x4C2BU, 0x4D2BU, 0x4E2BU, 0x4F2BU, 0x502BU,
	0x512BU, 0x522BU, 0x532BU, 0x542BU, 0x552BU, 0x562BU, 0x572BU, 0x582BU,
	0x592BU, 0x5A2BU, 0x612BU, 0x622BU, 0x632BU, 0x642BU, 0x652BU, 0x662BU,
	0x672BU, 0x682BU, 0x692BU, 0x6A2BU, 0x6B2BU, 0x6C2BU, 0x6D2BU, 0x6E2BU,
	0x6F2BU, 0x702BU, 0x712BU, 0x722BU, 0x732BU, 0x742BU, 0x752BU, 0x762BU,
	0x772BU, 0x782BU, 0x792BU, 0x7A2BU, 0x302BU, 0x312BU, 0x322BU, 0x332BU,
	0x342BU, 0x352BU, 0x362BU, 0x372BU, 0x382BU, 0x392BU, 0x2B2BU, 0x2F2BU,
	0x412FU, 0x422FU, 0x432FU, 0x442FU, 0x452FU, 0x462FU, 0x472FU, 0x482FU,
	0x492FU, 0x4A2FU, 0x4B2FU, 0x4C2FU, 0x4D2FU, 0x4E2FU, 0x4F2FU, 0x502FU,
	0x512FU, 0x522FU, 0x532FU, 0x542FU, 0x552FU, 0x562FU, 0x572FU, 0x582FU,
	0x592FU, 0x5A2FU, 0x612FU, 0x622FU, 0x632FU, 0x642FU, 0x652FU, 0x662FU,
	0x672FU, 0x682FU, 0x692FU, 0x6A2FU, 0x6B2FU, 0x6C2FU, 0x6D2FU, 0x6E2FU,
	0x6F2FU, 0x702FU, 0x712FU, 0x722FU, 0x732FU, 0x742FU, 0x752FU, 0x762FU,
	0x772FU, 0x782FU, 0x792FU, 0x7A2FU, 0x302FU, 0x312FU, 0x322FU, 0x332FU,
	0x342FU, 0x352FU, 0x362FU, 0x372FU, 0x382FU, 0x392FU, 0x2B2FU, 0x2F2FU,
#else
	0x4141U, 0x4142U, 0x4143U, 0x4144U, 0x4145U, 0x4146U, 0x4147U, 0x4148U,
	0x4149U, 0x414AU, 0x414BU, 0x414CU, 0x414DU, 0x414EU, 0x414FU, 0x4150U,
	0x4151U, 0x4152U, 0x4153U, 0x4154U, 0x4155U, 0x4156U, 0x4157U, 0x4158U,
	0x4159U, 0x415AU, 0x4161U, 0x4162U, 0x4163U, 0x4164U, 0x4165U, 0x4166U,
	0x4167U, 0x4168U, 0x4169U, 0x416AU, 0x416BU, 0x416CU, 0x416DU, 0x416EU,
	0x416FU, 0x4170U, 0x4171U, 0x4172U, 0x4173U, 0x4174U, 0x4175U, 0x4176U,
	0x4177U, 0x4178U, 0x4179U, 0x417AU, 0x4130U, 0x4131U, 0x4132U, 0x4133U,
	0x4134U, 0x4135U, 0x4136U, 0x4137U, 0x4138U, 0x4139U, 0x412BU, 0x412FU,
	0x4241U, 0x4242U, 0x4243U, 0x4244U, 0x4245U, 0x4246U, 0x4247U, 0x4248U,
	0x4249U, 0x424AU, 0x424BU, 0x424CU, 0x424DU, 0x424EU, 0x424FU, 0x4250U,
	0x4251U, 0x4252U, 0x4253U, 0x4254U, 0x4255U, 0x4256U, 0x4257U, 0x4258U,
	0x4259U, 0x425AU, 0x4261U, 0x4262U, 0x4263U, 0x4264U, 0x4265U, 0x4266U,
	0x4267U, 0x4268U, 0x4269U, 0x426AU, 0x426BU, 0x426CU, 0x426DU, 0x426EU,
	0x426FU, 0x4270U, 0x4271U, 0x4272U, 0x4273U, 0x4274U, 0x4275U, 0x4276U,
	0x4277U, 0x4278U, 0x4279U, 0x427AU, 0x4230U, 0x4231U, 0x4232U, 0x4233U,
	0x4234U, 0x4235U, 0x4236U, 0x4237U, 0x4238U, 0x4239U, 0x422BU, 0x422FU,
	0x4341U, 0x4342U, 0x4343U, 0x4344U, 0x4345U, 0x4346U, 0x4347U, 0x4348U,
	0x4349U, 0x434AU, 0x434BU, 0x434CU, 0x434DU, 0x434EU, 0x434FU, 0x4350U,
	0x4351U, 0x4352U, 0x4353U, 0x4354U, 0x4355U, 0x4356U, 0x4357U, 0x4358U,
	0x4359U, 0x435AU, 0x4361U, 0x4362U, 0x4363U, 0x4364U, 0x4365U, 0x4366U,
	0x4367U, 0x4368U, 0x4369U, 0x436AU, 0x436BU, 0x436CU, 0x436DU, 0x436EU,
	0x436FU, 0x4370U, 0x4371U, 0x4372U, 0x4373U, 0x4374U, 0x4375U, 0x4376U,
	0x4377U, 0x4378U, 0x4379U, 0x437AU, 0x4330U, 0x4331U, 0x4332U, 0x4333U,
	0x4334U, 0x4335U, 0x4336U, 0x4337U, 0x4338U, 0x4339U, 0x432BU, 0x432FU,
	0x4441U, 0x4442U, 0x4443U, 0x4444U, 0x4445U, 0x4446U, 0x4447U, 0x4448U,
	0x4449U, 0x444AU, 0x444BU, 0x444CU, 0x444DU, 0x444EU, 0x444FU, 0x4450U,
	0x4451U, 0x4452U, 0x4453U, 0x4454U, 0x4455U, 0x4456U, 0x4457U, 0x4458U,
	0x4459U, 0x445AU, 0x4461U, 0x4462U, 0x4463U, 0x4464U, 0x4465U, 0x4466U,
	0x4467U, 0x4468U, 0x4469U, 0x446AU, 0x446BU, 0x446CU, 0x446DU, 0x446EU,
	0x446FU, 0x4470U, 0x4471U, 0x4472U, 0x4473U, 0x4474U, 0x4475U, 0x4476U,
	0x4477U, 0x4478U, 0x4479U, 0x447AU, 0x4430U, 0x4431U, 0x4432U, 0x4433U,
	0x4434U, 0x4435U, 0x4436U, 0x4437U, 0x4438U, 0x4439U, 0x442BU, 0x442FU,
	0x4541U, 0x4542U, 0x4543U, 0x4544U, 0x4545U, 0x4546U, 0x4547U, 0x4548U,
	0x4549U, 0x454AU, 0x454BU, 0x454CU, 0x454DU, 0x454EU, 0x454FU, 0x4550U,
	0x4551U, 0x4552U, 0x4553U, 0x4554U, 0x4555U, 0x4556U, 0x4557U, 0x4558U,
	0x4559U, 0x455AU, 0x4561U, 0x4562U, 0x4563U, 0x4564U, 0x4565U, 0x4566U,
	0x4567U, 0x4568U, 0x4569U, 0x456AU, 0x456BU, 0x456CU, 0x456DU, 0x456EU,
	0x456FU, 0x4570U, 0x4571U, 0x4572U, 0x4573U, 0x4574U, 0x4575U, 0x4576U,
	0x4577U, 0x4578U, 0x4579U, 0x457AU, 0x4530U, 0x4531U, 0x4532U, 0x4533U,
	0x4534U, 0x4535U, 0x4536U, 0x4537U, 0x4538U, 0x4539U, 0x452BU, 0x452FU,
	0x4641U, 0x4642U, 0x4643U, 0x4644U, 0x4645U, 0x4646U, 0x4647U, 0x4648U,
	0x4649U, 0x464AU, 0x464BU, 0x464CU, 0x464DU, 0x464EU, 0x464FU, 0x4650U,
	0x4651U, 0x4652U, 0x4653U, 0x4654U, 0x4655U, 0x4656U, 0x4657U, 0x4658U,
	0x4659U, 0x465AU, 0x4661U, 0x4662U, 0x4663U, 0x4664U, 0x4665U, 0x4666U,
	0x4667U, 0x4668U, 0x4669U, 0x466AU, 0x466BU, 0x466CU, 0x466DU, 0x466EU,
	0x466FU, 0x4670U, 0x4671U, 0x4672U, 0x4673U, 0x4674U, 0x4675U, 0x4676U,
	0x4677U, 0x4678U, 0x4679U, 0x467AU, 0x4630U, 0x4631U, 0x4632U, 0x4633U,
	0x4634U, 0x4635U, 0x4636U, 0x4637U, 0x4638U, 0x4639U, 0x462BU, 0x462FU,
	0x4741U, 0x4742U, 0x4743U, 0x4744U, 0x4745U, 0x4746U, 0x4747U, 0x4748U,
	0x4749U, 0x474AU, 0x474BU, 0x474CU, 0x474DU, 0x474EU, 0x474FU, 0x4750U,
	0x4751U, 0x4752U, 0x4753U, 0x4754U, 0x4755U, 0x4756U, 0x4757U, 0x4758U,
	0x4759U, 0x475AU, 0x4761U, 0x4762U, 0x4763U, 0x4764U, 0x4765U, 0x4766U,
	0x4767U, 0x4768U, 0x4769U, 0x476AU, 0x476BU, 0x476CU, 0x476DU, 0x476EU,
	0x476FU, 0x4770U, 0x4771U, 0x4772U, 0x4773U, 0x4774U, 0x4775U, 0x4776U,
	0x4777U, 0x4778U, 0x4779U, 0x477AU, 0x4730U, 0x4731U, 0x4732U, 0x4733U,
	0x4734U, 0x4735U, 0x4736U, 0x4737U, 0x4738U, 0x4739U, 0x472BU, 0x472FU,
	0x4841U, 0x4842U, 0x4843U, 0x4844U, 0x4845U, 0x4846U, 0x4847U, 0x4848U,
	0x4849U, 0x484AU, 0x484BU, 0x484CU, 0x484DU, 0x484EU, 0x484FU, 0x4850U,
	0x4851U, 0x4852U, 0x4853U, 0x4854U, 0x4855U, 0x4856U, 0x4857U, 0x4858U,
	0x4859U, 0x485AU, 0x4861U, 0x4862U, 0x4863U, 0x4864U, 0x4865U, 0x4866U,
	0x4867U, 0x4868U, 0x4869U, 0x486AU, 0x486BU, 0x486CU, 0x486DU, 0x486EU,
	0x486FU, 0x4870U, 0x4871U, 0x4872U, 0x4873U, 0x4874U, 0x4875U, 0x4876U,
	0x4877U, 0x4878U, 0x4879U, 0x487AU, 0x4830U, 0x4831U, 0x4832U, 0x4833U,
	0x4834U, 0x4835U, 0x4836U, 0x4837U, 0x4838U, 0x4839U, 0x482BU, 0x482FU,
	0x4941U, 0x4942U, 0x4943U, 0x4944U, 0x4945U, 0x4946U, 0x4947U, 0x4948U,
	0x4949U, 0x494AU, 0x494BU, 0x494CU, 0x494DU, 0x494EU, 0x494FU, 0x4950U,
	0x4951U, 0x4952U, 0x4953U, 0x4954U, 0x4955U, 0x4956U, 0x4957U, 0x4958U,
	0x4959U, 0x495AU, 0x4961U, 0x4962U, 0x4963U, 0x4964U, 0x4965U, 0x4966U,
	0x4967U, 0x4968U, 0x4969U, 0x496AU, 0x496BU, 0x496CU, 0x496DU, 0x496EU,
	0x496FU, 0x4970U, 0x4971U, 0x4972U, 0x4973U, 0x4974U, 0x4975U, 0x4976U,
	0x4977U, 0x4978U, 0x4979U, 0x497AU, 0x4930U, 0x4931U, 0x4932U, 0x4933U,
	0x4934U, 0x4935U, 0x4936U, 0x4937U, 0x4938U, 0x4939U, 0x492BU, 0x492FU,
	0x4A41U, 0x4A42U, 0x4A43U, 0x4A44U, 0x4A45U, 0x4A46U, 0x4A47U, 0x4A48U,
	0x4A49U, 0x4A4AU, 0x4A4BU, 0x4A4CU, 0x4A4DU, 0x4A4EU, 0x4A4FU, 0x4A50U,
	0x4A51U, 0x4A52U, 0x4A53U, 0x4A54U, 0x4A55U, 0x4A56U, 0x4A57U, 0x4A58U,
	0x4A59U, 0x4A5AU, 0x4A61U, 0x4A62U, 0x4A63U, 0x4A64U, 0x4A65U, 0x4A66U,
	0x4A67U, 0x4A68U, 0x4A69U, 0x4A6AU, 0x4A6BU, 0x4A6CU, 0x4A6DU, 0x4A6EU,
	0x4A6FU, 0x4A70U, 0x4A71U, 0x4A72U, 0x4A73U, 0x4A74U, 0x4A75U, 0x4A76U,
	0x4A77U, 0x4A78U, 0x4A79U, 0x4A7AU, 0x4A30U, 0x4A31U, 0x4A32U, 0x4A33U,
	0x4A34U, 0x4A35U, 0x4A36U, 0x4A37U, 0x4A38U, 0x4A39U, 0x4A2BU, 0x4A2FU,
	0x4B41U, 0x4B42U, 0x4B43U, 0x4B44U, 0x4B45U, 0x4B46U, 0x4B47U, 0x4B48U,
	0x4B49U, 0x4B4AU, 0x4B4BU, 0x4B4CU, 0x4B4DU, 0x4B4EU, 0x4B4FU, 0x4B50U,
	0x4B51U, 0x4B52U, 0x4B53U, 0x4B54U, 0x4B55U, 0x4B56U, 0x4B57U, 0x4B58U,
	0x4B59U, 0x4B5AU, 0x4B61U, 0x4B62U, 0x4B63U, 0x4B64U, 0x4B65U, 0x4B66U,
	0x4B67U, 0x4B68U, 0x4B69U, 0x4B6AU, 0x4B6BU, 0x4B6CU, 0x4B6DU, 0x4B6EU,
	0x4B6FU, 0x4B70U, 0x4B71U, 0x4B72U, 0x4B73U, 0x4B74U, 0x4B75U, 0x4B76U,
	0x4B77U, 0x4B78U, 0x4B79U, 0x4B7AU, 0x4B30U, 0x4B31U, 0x4B32U, 0x4B33U,
	0x4B34U, 0x4B35U, 0x4B36U, 0x4B37U, 0x4B38U, 0x4B39U, 0x4B2BU, 0x4B2FU,
	0x4C41U, 0x4C42U, 0x4C43U, 0x4C44U, 0x4C45U, 0x4C46U, 0x4C47U, 0x4C48U,
	0x4C49U, 0x4C4AU, 0x4C4BU, 0x4C4CU, 0x4C4DU, 0x4C4EU, 0x4C4FU, 0x4C50U,
	0x4C51U, 0x4C52U, 0x4C53U, 0x4C54U, 0x4C55U, 0x4C56U, 0x4C57U, 0x4C58U,
	0x4C59U, 0x4C5AU, 0x4C61U, 0x4C62U, 0x4C63U, 0x4C64U, 0x4C65U, 0x4C66U,
	0x4C67U, 0x4C68U, 0x4C69U, 0x4C6AU, 0x4C6BU, 0x4C6CU, 0x4C6DU, 0x4C6EU,
	0x4C6FU, 0x4C70U, 0x4C71U, 0x4C72U, 0x4C73U, 0x4C74U, 0x4C75U, 0x4C76U,
	0x4C77U, 0x4C78U, 0x4C79U, 0x4C7AU, 0x4C30U, 0x4C31U, 0x4C32U, 0x4C33U,
	0x4C34U, 0x4C35U, 0x4C36U, 0x4C37U, 0x4C38U, 0x4C39U, 0x4C2BU, 0x4C2FU,
	0x4D41U, 0x4D42U, 0x4D43U, 0x4D44U, 0x4D45U, 0x4D46U, 0x4D47U, 0x4D48U,
	0x4D49U, 0x4D4AU, 0x4D4BU, 0x4D4CU, 0x4D4DU, 0x4D4EU, 0x4D4FU, 0x4D50U,
	0x4D51U, 0x4D52U, 0x4D53U, 0x4D54U, 0x4D55U, 0x4D56U, 0x4D57U, 0x4D58U,
	0x4D59U, 0x4D5AU, 0x4D61U, 0x4D62U, 0x4D63U, 0x4D64U, 0x4D65U, 0x4D66U,
	0x4D67U, 0x4D68U, 0x4D69U, 0x4D6AU, 0x4D6BU, 0x4D6CU, 0x4D6DU, 0x4D6EU,
	0x4D6FU, 0x4D70U, 0x4D71U, 0x4D72U, 0x4D73U, 0x4D74U, 0x4D75U, 0x4D76U,
	0x4D77U, 0x4D78U, 0x4D79U, 0x4D7AU, 0x4D30U, 0x4D31U, 0x4D32U, 0x4D33U,
	0x4D34U, 0x4D35U, 0x4D36U, 0x4D37U, 0x4D38U, 0x4D39U, 0x4D2BU, 0x4D2FU,
	0x4E41U, 0x4E42U, 0x4E43U, 0x4E44U, 0x4E45U, 0x4E46U, 0x4E47U, 0x4E48U,
	0x4E49U, 0x4E4AU, 0x4E4BU, 0x4E4CU, 0x4E4DU, 0x4E4EU, 0x4E4FU, 0x4E50U,
	0x4E51U, 0x4E52U, 0x4E53U, 0x4E54U, 0x4E55U, 0x4E56U, 0x4E57U, 0x4E58U,
	0x4E59U, 0x4E5AU, 0x4E61U, 0x4E62U, 0x4E63U, 0x4E64U, 0x4E65U, 0x4E66U,
	0x4E67U, 0x4E68U, 0x4E69U, 0x4E6AU, 0x4E6BU, 0x4E6CU, 0x4E6DU, 0x4E6EU,
	0x4E6FU, 0x4E70U, 0x4E71U, 0x4E72U, 0x4E73U, 0x4E74U, 0x4E75U, 0x4E76U,
	0x4E77U, 0x4E78U, 0x4E79U, 0x4E7AU, 0x4E30U, 0x4E31U, 0x4E32U, 0x4E33U,
	0x4E34U, 0x4E35U, 0x4E36U, 0x4E37U, 0x4E38U, 0x4E39U, 0x4E2BU, 0x4E2FU,
	0x4F41U, 0x4F42U, 0x4F43U, 0x4F44U, 0x4F45U, 0x4F46U, 0x4F47U, 0x4F48U,
	0x4F49U, 0x4F4AU, 0x4F4BU, 0x4F4CU, 0x4F4DU, 0x4F4EU, 0x4F4FU, 0x4F50U,
	0x4F51U, 0x4F52U, 0x4F53U, 0x4F54U, 0x4F55U, 0x4F56U, 0x4F57U, 0x4F58U,
	0x4F59U, 0x4F5AU, 0x4F61U, 0x4F62U, 0x4F63U, 0x4F64U, 0x4F65U, 0x4F66U,
	0x4F67U, 0x4F68U, 0x4F69U, 0x4F6AU, 0x4F6BU, 0x4F6CU, 0x4F6DU, 0x4F6EU,
	0x4F6FU, 0x4F70U, 0x4F71U, 0x4F72U, 0x4F73U, 0x4F74U, 0x4F75U, 0x4F76U,
	0x4F77U, 0x4F78U, 0x4F79U, 0x4F7AU, 0x4F30U, 0x4F31U, 0x4F32U, 0x4F33U,
	0x4F34U, 0x4F35U, 0x4F36U, 0x4F37U, 0x4F38U, 0x4F39U, 0x4F2BU, 0x4F2FU,
	0x5041U, 0x5042U, 0x5043U, 0x5044U, 0x5045U, 0x5046U, 0x5047U, 0x5048U,
	0x5049U, 0x504AU, 0x504BU, 0x504CU, 0x504DU, 0x504EU, 0x504FU, 0x5050U,
	0x5051U, 0x5052U, 0x5053U, 0x5054U, 0x5055U, 0x5056U, 0x5057U, 0x5058U,
	0x5059U, 0x505AU, 0x5061U, 0x5062U, 0x5063U, 0x5064U, 0x5065U, 0x5066U,
	0x5067U, 0x5068U, 0x5069U, 0x506AU, 0x506BU, 0x506CU, 0x506DU, 0x506EU,
	0x506FU, 0x5070U, 0x5071U, 0x5072U, 0x5073U, 0x5074U, 0x5075U, 0x5076U,
	0x5077U, 0x5078U, 0x5079U, 0x507AU, 0x5030U, 0x5031U, 0x5032U, 0x5033U,
	0x5034U, 0x5035U, 0x5036U, 0x5037U, 0x5038U, 0x5039U, 0x502BU, 0x502FU,
	0x5141U, 0x5142U, 0x5143U, 0x5144U, 0x5145U, 0x5146U, 0x5147U, 0x5148U,
	0x5149U, 0x514AU, 0x514BU, 0x514CU, 0x514DU, 0x514EU, 0x514FU, 0x5150U,
	0x5151U, 0x5152U, 0x5153U, 0x5154U, 0x5155U, 0x5156U, 0x5157U, 0x5158U,
	0x5159U, 0x515AU, 0x5161U, 0x5162U, 0x5163U, 0x5164U, 0x5165U, 0x5166U,
	0x5167U, 0x5168U, 0x5169U, 0x516AU, 0x516BU, 0x516CU, 0x516DU, 0x516EU,
	0x516FU, 0x5170U, 0x5171U, 0x5172U, 0x5173U, 0x5174U, 0x5175U, 0x5176U,
	0x5177U, 0x5178U, 0x5179U, 0x517AU, 0x5130U, 0x5131U, 0x5132U, 0x5133U,
	0x5134U, 0x5135U, 0x5136U, 0x5137U, 0x5138U, 0x5139U, 0x512BU, 0x512FU,
	0x5241U, 0x5242U, 0x5243U, 0x5244U, 0x5245U, 0x5246U, 0x5247U, 0x5248U,
	0x5249U, 0x524AU, 0x524BU, 0x524CU, 0x524DU, 0x524EU, 0x524FU, 0x5250U,
	0x5251U, 0x5252U, 0x5253U, 0x5254U, 0x5255U, 0x5256U, 0x5257U, 0x5258U,
	0x5259U, 0x525AU, 0x5261U, 0x5262U, 0x5263U, 0x5264U, 0x5265U, 0x5266U,
	0x5267U, 0x5268U, 0x5269U, 0x526AU, 0x526BU, 0x526CU, 0x526DU, 0x526EU,
	0x526FU, 0x5270U, 0x5271U, 0x5272U, 0x5273U, 0x5274U, 0x5275U, 0x5276U,
	0x5277U, 0x5278U, 0x5279U, 0x527AU, 0x5230U, 0x5231U, 0x5232U, 0x5233U,
	0x5234U, 0x5235U, 0x5236U, 0x5237U, 0x5238U, 0x5239U, 0x522BU, 0x522FU,
	0x5341U, 0x5342U, 0x5343U, 0x5344U, 0x5345U, 0x5346U, 0x5347U, 0x5348U,
	0x5349U, 0x534AU, 0x534BU, 0x534CU, 0x534DU, 0x534EU, 0x534FU, 0x5350U,
	0x5351U, 0x5352U, 0x5353U, 0x5354U, 0x5355U, 0x5356U, 0x5357U, 0x5358U,
	0x5359U, 0x535AU, 0x5361U, 0x5362U, 0x5363U, 0x5364U, 0x5365U, 0x5366U,
	0x5367U, 0x5368U, 0x5369U, 0x536AU, 0x536BU, 0x536CU, 0x536DU, 0x536EU,
	0x536FU, 0x5370U, 0x5371U, 0x5372U, 0x5373U, 0x5374U, 0x5375U, 0x5376U,
	0x5377U, 0x5378U, 0x5379U, 0x537AU, 0x5330U, 0x5331U, 0x5332U, 0x5333U,
	0x5334U, 0x5335U, 0x5336U, 0x5337U, 0x5338U, 0x5339U, 0x532BU, 0x532FU,
	0x5441U, 0x5442U, 0x5443U, 0x5444U, 0x5445U, 0x5446U, 0x5447U, 0x5448U,
	0x5449U, 0x544AU, 0x544BU, 0x544CU, 0x544DU, 0x544EU, 0x544FU, 0x5450U,
	0x5451U, 0x5452U, 0x5453U, 0x5454U, 0x5455U, 0x5456U, 0x5457U, 0x5458U,
	0x5459U, 0x545AU, 0x5461U, 0x5462U, 0x5463U, 0x5464U, 0x5465U, 0x5466U,
	0x5467U, 0x5468U, 0x5469U, 0x546AU, 0x546BU, 0x546CU, 0x546DU, 0x546EU,
	0x546FU, 0x5470U, 0x5471U, 0x5472U, 0x5473U, 0x5474U, 0x5475U, 0x5476U,
	0x5477U, 0x5478U, 0x5479U, 0x547AU, 0x5430U, 0x5431U, 0x5432U, 0x5433U,
	0x5434U, 0x5435U, 0x5436U, 0x5437U, 0x5438U, 0x5439U, 0x542BU, 0x542FU,
	0x5541U, 0x5542U, 0x5543U, 0x5544U, 0x5545U, 0x5546U, 0x5547U, 0x5548U,
	0x5549U, 0x554AU, 0x554BU, 0x554CU, 0x554DU, 0x554EU, 0x554FU, 0x5550U,
	0x5551U, 0x5552U, 0x5553U, 0x5554U, 0x5555U, 0x5556U, 0x5557U, 0x5558U,
	0x5559U, 0x555AU, 0x5561U, 0x5562U, 0x5563U, 0x5564U, 0x5565U, 0x5566U,
	0x5567U, 0x5568U, 0x5569U, 0x556AU, 0x556BU, 0x556CU, 0x556DU, 0x556EU,
	0x556FU, 0x5570U, 0x5571U, 0x5572U, 0x5573U, 0x5574U, 0x5575U, 0x5576U,
	0x5577U, 0x5578U, 0x5579U, 0x557AU, 0x5530U, 0x5531U, 0x5532U, 0x5533U,
	0x5534U, 0x5535U, 0x5536U, 0x5537U, 0x5538U, 0x5539U, 0x552BU, 0x552FU,
	0x5641U, 0x5642U, 0x5643U, 0x5644U, 0x5645U, 0x5646U, 0x5647U, 0x5648U,
	0x5649U, 0x564AU, 0x564BU, 0x564CU, 0x564DU, 0x564EU, 0x564FU, 0x5650U,
	0x5651U, 0x5652U, 0x5653U, 0x5654U, 0x5655U, 0x5656U, 0x5657U, 0x5658U,
	0x5659U, 0x565AU, 0x5661U, 0x5662U, 0x5663U, 0x5664U, 0x5665U, 0x5666U,
	0x5667U, 0x5668U, 0x5669U, 0x566AU, 0x566BU, 0x566CU, 0x566DU, 0x566EU,
	0x566FU, 0x5670U, 0x5671U, 0x5672U, 0x5673U, 0x5674U, 0x5675U, 0x5676U,
	0x5677U, 0x5678U, 0x5679U, 0x567AU, 0x5630U, 0x5631U, 0x5632U, 0x5633U,
	0x5634U, 0x5635U, 0x5636U, 0x5637U, 0x5638U, 0x5639U, 0x562BU, 0x562FU,
	0x5741U, 0x5742U, 0x5743U, 0x5744U, 0x5745U, 0x5746U, 0x5747U, 0x5748U,
	0x5749U, 0x574AU, 0x574BU, 0x574CU, 0x574DU, 0x574EU, 0x574FU, 0x5750U,
	0x5751U, 0x5752U, 0x5753U, 0x5754U, 0x5755U, 0x5756U, 0x5757U, 0x5758U,
	0x5759U, 0x575AU, 0x5761U, 0x5762U, 0x5763U, 0x5764U, 0x5765U, 0x5766U,
	0x5767U, 0x5768U, 0x5769U, 0x576AU, 0x576BU, 0x576CU, 0x576DU, 0x576EU,
	0x576FU, 0x5770U, 0x5771U, 0x5772U, 0x5773U, 0x5774U, 0x5775U, 0x5776U,
	0x5777U, 0x5778U, 0x5779U, 0x577AU, 0x5730U, 0x5731U, 0x5732U, 0x5733U,
	0x5734U, 0x5735U, 0x5736U, 0x5737U, 0x5738U, 0x5739U, 0x572BU, 0x572FU,
	0x5841U, 0x5842U, 0x5843U, 0x5844U, 0x5845U, 0x5846U, 0x5847U, 0x5848U,
	0x5849U, 0x584AU, 0x584BU, 0x584CU, 0x584DU, 0x584EU, 0x584FU, 0x5850U,
	0x5851U, 0x5852U, 0x5853U, 0x5854U, 0x5855U, 0x5856U, 0x5857U, 0x5858U,
	0x5859U, 0x585AU, 0x5861U, 0x5862U, 0x5863U, 0x5864U, 0x5865U, 0x5866U,
	0x5867U, 0x5868U, 0x5869U, 0x586AU, 0x586BU, 0x586CU, 0x586DU, 0x586EU,
	0x586FU, 0x5870U, 0x5871U, 0x5872U, 0x5873U, 0x5874U, 0x5875U, 0x5876U,
	0x5877U, 0x5878U, 0x5879U, 0x587AU, 0x5830U, 0x5831U, 0x5832U, 0x5833U,
	0x5834U, 0x5835U, 0x5836U, 0x5837U, 0x5838U, 0x5839U, 0x582BU, 0x582FU,
	0x5941U, 0x5942U, 0x5943U, 0x5944U, 0x5945U, 0x5946U, 0x5947U, 0x5948U,
	0x5949U, 0x594AU, 0x594BU, 0x594CU, 0x594DU, 0x594EU, 0x594FU, 0x5950U,
	0x5951U, 0x5952U, 0x5953U, 0x5954U, 0x5955U, 0x5956U, 0x5957U, 0x5958U,
	0x5959U, 0x595AU, 0x5961U, 0x5962U, 0x5963U, 0x5964U, 0x5965U, 0x5966U,
	0x5967U, 0x5968U, 0x5969U, 0x596AU, 0x596BU, 0x596CU, 0x596DU, 0x596EU,
	0x596FU, 0x5970U, 0x5971U, 0x5972U, 0x5973U, 0x5974U, 0x5975U, 0x5976U,
	0x5977U, 0x5978U, 0x5979U, 0x597AU, 0x5930U, 0x5931U, 0x5932U, 0x5933U,
	0x5934U, 0x5935U, 0x5936U, 0x5937U, 0x5938U, 0x5939U, 0x592BU, 0x592FU,
	0x5A41U, 0x5A42U, 0x5A43U, 0x5A44U, 0x5A45U, 0x5A46U, 0x5A47U, 0x5A48U,
	0x5A49U, 0x5A4AU, 0x5A4BU, 0x5A4CU, 0x5A4DU, 0x5A4EU, 0x5A4FU, 0x5A50U,
	0x5A51U, 0x5A52U, 0x5A53U, 0x5A54U, 0x5A55U, 0x5A56U, 0x5A57U, 0x5A58U,
	0x5A59U, 0x5A5AU, 0x5A61U, 0x5A62U, 0x5A63U, 0x5A64U, 0x5A65U, 0x5A66U,
	0x5A67U, 0x5A68U, 0x5A69U, 0x5A6AU, 0x5A6BU, 0x5A6CU, 0x5A6DU, 0x5A6EU,
	0x5A6FU, 0x5A70U, 0x5A71U, 0x5A72U, 0x5A73U, 0x5A74U, 0x5A75U, 0x5A76U,
	0x5A77U, 0x5A78U, 0x5A79U, 0x5A7AU, 0x5A30U, 0x5A31U, 0x5A32U, 0x5A33U,
	0x5A34U, 0x5A35U, 0x5A36U, 0x5A37U, 0x5A38U, 0x5A39U, 0x5A2BU, 0x5A2FU,
	0x6141U, 0x6142U, 0x6143U, 0x6144U, 0x6145U, 0x6146U, 0x6147U, 0x6148U,
	0x6149U, 0x614AU, 0x614BU, 0x614CU, 0x614DU, 0x614EU, 0x614FU, 0x6150U,
	0x6151U, 0x6152U, 0x6153U, 0x6154U, 0x6155U, 0x6156U, 0x6157U, 0x6158U,
	0x6159U, 0x615AU, 0x6161U, 0x6162U, 0x6163U, 0x6164U, 0x6165U, 0x6166U,
	0x6167U, 0x6168U, 0x6169U, 0x616AU, 0x616BU, 0x616CU, 0x616DU, 0x616EU,
	0x616FU, 0x6170U, 0x6171U, 0x6172U, 0x6173U, 0x6174U, 0x6175U, 0x6176U,
	0x6177U, 0x6178U, 0x6179U, 0x617AU, 0x6130U, 0x6131U, 0x6132U, 0x6133U,
	0x6134U, 0x6135U, 0x6136U, 0x6137U, 0x6138U, 0x6139U, 0x612BU, 0x612FU,
	0x6241U, 0x6242U, 0x6243U, 0x6244U, 0x6245U, 0x6246U, 0x6247U, 0x6248U,
	0x6249U, 0x624AU, 0x624BU, 0x624CU, 0x624DU, 0x624EU, 0x624FU, 0x6250U,
	0x6251U, 0x6252U, 0x6253U, 0x6254U, 0x6255U, 0x6256U, 0x6257U, 0x6258U,
	0x6259U, 0x625AU, 0x6261U, 0x6262U, 0x6263U, 0x6264U, 0x6265U, 0x6266U,
	0x6267U, 0x6268U, 0x6269U, 0x626AU, 0x626BU, 0x626CU, 0x626DU, 0x626EU,
	0x626FU, 0x6270U, 0x6271U, 0x6272U, 0x6273U, 0x6274U, 0x6275U, 0x6276U,
	0x6277U, 0x6278U, 0x6279U, 0x627AU, 0x6230U, 0x6231U, 0x6232U, 0x6233U,
	0x6234U, 0x6235U, 0x6236U, 0x6237U, 0x6238U, 0x6239U, 0x622BU, 0x622FU,
	0x6341U, 0x6342U, 0x6343U, 0x6344U, 0x6345U, 0x6346U, 0x6347U, 0x6348U,
	0x6349U, 0x634AU, 0x634BU, 0x634CU, 0x634DU, 0x634EU, 0x634FU, 0x6350U,
	0x6351U, 0x6352U, 0x6353U, 0x6354U, 0x6355U, 0x6356U, 0x6357U, 0x6358U,
	0x6359U, 0x635AU, 0x6361U, 0x6362U, 0x6363U, 0x6364U, 0x6365U, 0x6366U,
	0x6367U, 0x6368U, 0x6369U, 0x636AU, 0x636BU, 0x636CU, 0x636DU, 0x636EU,
	0x636FU, 0x6370U, 0x6371U, 0x6372U, 0x6373U, 0x6374U, 0x6375U, 0x6376U,
	0x6377U, 0x6378U, 0x6379U, 0x637AU, 0x6330U, 0x6331U, 0x6332U, 0x6333U,
	0x6334U, 0x6335U, 0x6336U, 0x6337U, 0x6338U, 0x6339U, 0x632BU, 0x632FU,
	0x6441U, 0x6442U, 0x6443U, 0x6444U, 0x6445U, 0x6446U, 0x6447U, 0x6448U,
	0x6449U, 0x644AU, 0x644BU, 0x644CU, 0x644DU, 0x644EU, 0x644FU, 0x6450U,
	0x6451U, 0x6452U, 0x6453U, 0x6454U, 0x6455U, 0x6456U, 0x6457U, 0x6458U,
	0x6459U, 0x645AU, 0x6461U, 0x6462U, 0x6463U, 0x6464U, 0x6465U, 0x6466U,
	0x6467U, 0x6468U, 0x6469U, 0x646AU, 0x646BU, 0x646CU, 0x646DU, 0x646EU,
	0x646FU, 0x6470U, 0x6471U, 0x6472U, 0x6473U, 0x6474U, 0x6475U, 0x6476U,
	0x6477U, 0x6478U, 0x6479U, 0x647AU, 0x6430U, 0x6431U, 0x6432U, 0x6433U,
	0x6434U, 0x6435U, 0x6436U, 0x6437U, 0x6438U, 0x6439U, 0x642BU, 0x642FU,
	0x6541U, 0x6542U, 0x6543U, 0x6544U, 0x6545U, 0x6546U, 0x6547U, 0x6548U,
	0x6549U, 0x654AU, 0x654BU, 0x654CU, 0x654DU, 0x654EU, 0x654FU, 0x6550U,
	0x6551U, 0x6552U, 0x6553U, 0x6554U, 0x6555U, 0x6556U, 0x6557U, 0x6558U,
	0x6559U, 0x655AU, 0x6561U, 0x6562U, 0x6563U, 0x6564U, 0x6565U, 0x6566U,
	0x6567U, 0x6568U, 0x6569U, 0x656AU, 0x656BU, 0x656CU, 0x656DU, 0x656EU,
	0x656FU, 0x6570U, 0x6571U, 0x6572U, 0x6573U, 0x6574U, 0x6575U, 0x6576U,
	0x6577U, 0x6578U, 0x6579U, 0x657AU, 0x6530U, 0x6531U, 0x6532U, 0x6533U,
	0x6534U, 0x6535U, 0x6536U, 0x6537U, 0x6538U, 0x6539U, 0x652BU, 0x652FU,
	0x6641U, 0x6642U, 0x6643U, 0x6644U, 0x6645U, 0x6646U, 0x6647U, 0x6648U,
	0x6649U, 0x664AU, 0x664BU, 0x664CU, 0x664DU, 0x664EU, 0x664FU, 0x6650U,
	0x6651U, 0x6652U, 0x6653U, 0x6654U, 0x6655U, 0x6656U, 0x6657U, 0x6658U,
	0x6659U, 0x665AU, 0x6661U, 0x6662U, 0x6663U, 0x6664U, 0x6665U, 0x6666U,
	0x6667U, 0x6668U, 0x6669U, 0x666AU, 0x666BU, 0x666CU, 0x666DU, 0x666EU,
	0x666FU, 0x6670U, 0x6671U, 0x6672U, 0x6673U, 0x6674U, 0x6675U, 0x6676U,
	0x6677U, 0x6678U, 0x6679U, 0x667AU, 0x6630U, 0x6631U, 0x6632U, 0x6633U,
	0x6634U, 0x6635U, 0x6636U, 0x6637U, 0x6638U, 0x6639U, 0x662BU, 0x662FU,
	0x6741U, 0x6742U, 0x6743U, 0x6744U, 0x6745U, 0x6746U, 0x6747U, 0x6748U,
	0x6749U, 0x674AU, 0x674BU, 0x674CU, 0x674DU, 0x674EU, 0x674FU, 0x6750U,
	0x6751U, 0x6752U, 0x6753U, 0x6754U, 0x6755U, 0x6756U, 0x6757U, 0x6758U,
	0x6759U, 0x675AU, 0x6761U, 0x6762U, 0x6763U, 0x6764U, 0x6765U, 0x6766U,
	0x6767U, 0x6768U, 0x6769U, 0x676AU, 0x676BU, 0x676CU, 0x676DU, 0x676EU,
	0x676FU, 0x6770U, 0x6771U, 0x6772U, 0x6773U, 0x6774U, 0x6775U, 0x6776U,
	0x6777U, 0x6778U, 0x6779U, 0x677AU, 0x6730U, 0x6731U, 0x6732U, 0x6733U,
	0x6734U, 0x6735U, 0x6736U, 0x6737U, 0x6738U, 0x6739U, 0x672BU, 0x672FU,
	0x6841U, 0x6842U, 0x6843U, 0x6844U, 0x6845U, 0x6846U, 0x6847U, 0x6848U,
	0x6849U, 0x684AU, 0x684BU, 0x684CU, 0x684DU, 0x684EU, 0x684FU, 0x6850U,
	0x6851U, 0x6852U, 0x6853U, 0x6854U, 0x6855U, 0x6856U, 0x6857U, 0x6858U,
	0x6859U, 0x685AU, 0x6861U, 0x6862U, 0x6863U, 0x6864U, 0x6865U, 0x6866U,
	0x6867U, 0x6868U, 0x6869U, 0x686AU, 0x686BU, 0x686CU, 0x686DU, 0x686EU,
	0x686FU, 0x6870U, 0x6871U, 0x6872U, 0x6873U, 0x6874U, 0x6875U, 0x6876U,
	0x6877U, 0x6878U, 0x6879U, 0x687AU, 0x6830U, 0x6831U, 0x6832U, 0x6833U,
	0x6834U, 0x6835U, 0x6836U, 0x6837U, 0x6838U, 0x6839U, 0x682BU, 0x682FU,
	0x6941U, 0x6942U, 0x6943U, 0x6944U, 0x6945U, 0x6946U, 0x6947U, 0x6948U,
	0x6949U, 0x694AU, 0x694BU, 0x694CU, 0x694DU, 0x694EU, 0x694FU, 0x6950U,
	0x6951U, 0x6952U, 0x6953U, 0x6954U, 0x6955U, 0x6956U, 0x6957U, 0x6958U,
	0x6959U, 0x695AU, 0x6961U, 0x6962U, 0x6963U, 0x6964U, 0x6965U, 0x6966U,
	0x6967U, 0x6968U, 0x6969U, 0x696AU, 0x696BU, 0x696CU, 0x696DU, 0x696EU,
	0x696FU, 0x6970U, 0x6971U, 0x6972U, 0x6973U, 0x6974U, 0x6975U, 0x6976U,
	0x6977U, 0x6978U, 0x6979U, 0x697AU, 0x6930U, 0x6931U, 0x6932U, 0x6933U,
	0x6934U, 0x6935U, 0x6936U, 0x6937U, 0x6938U, 0x6939U, 0x692BU, 0x692FU,
	0x6A41U, 0x6A42U, 0x6A43U, 0x6A44U, 0x6A45U, 0x6A46U, 0x6A47U, 0x6A48U,
	0x6A49U, 0x6A4AU, 0x6A4BU, 0x6A4CU, 0x6A4DU, 0x6A4EU, 0x6A4FU, 0x6A50U,
	0x6A51U, 0x6A52U, 0x6A53U, 0x6A54U, 0x6A55U, 0x6A56U, 0x6A57U, 0x6A58U,
	0x6A59U, 0x6A5AU, 0x6A61U, 0x6A62U, 0x6A63U, 0x6A64U, 0x6A65U, 0x6A66U,
	0x6A67U, 0x6A68U, 0x6A69U, 0x6A6AU, 0x6A6BU, 0x6A6CU, 0x6A6DU, 0x6A6EU,
	0x6A6FU, 0x6A70U, 0x6A71U, 0x6A72U, 0x6A73U, 0x6A74U, 0x6A75U, 0x6A76U,
	0x6A77U, 0x6A78U, 0x6A79U, 0x6A7AU, 0x6A30U, 0x6A31U, 0x6A32U, 0x6A33U,
	0x6A34U, 0x6A35U, 0x6A36U, 0x6A37U, 0x6A38U, 0x6A39U, 0x6A2BU, 0x6A2FU,
	0x6B41U, 0x6B42U, 0x6B43U, 0x6B44U, 0x6B45U, 0x6B46U, 0x6B47U, 0x6B48U,
	0x6B49U, 0x6B4AU, 0x6B4BU, 0x6B4CU, 0x6B4DU, 0x6B4EU, 0x6B4FU, 0x6B50U,
	0x6B51U, 0x6B52U, 0x6B53U, 0x6B54U, 0x6B55U, 0x6B56U, 0x6B57U, 0x6B58U,
	0x6B59U, 0x6B5AU, 0x6B61U, 0x6B62U, 0x6B63U, 0x6B64U, 0x6B65U, 0x6B66U,
	0x6B67U, 0x6B68U, 0x6B69U, 0x6B6AU, 0x6B6BU, 0x6B6CU, 0x6B6DU, 0x6B6EU,
	0x6B6FU, 0x6B70U, 0x6B71U, 0x6B72U, 0x6B73U, 0x6B74U, 0x6B75U, 0x6B76U,
	0x6B77U, 0x6B78U, 0x6B79U, 0x6B7AU, 0x6B30U, 0x6B31U, 0x6B32U, 0x6B33U,
	0x6B34U, 0x6B35U, 0x6B36U, 0x6B37U, 0x6B38U, 0x6B39U, 0x6B2BU, 0x6B2FU,
	0x6C41U, 0x6C42U, 0x6C43U, 0x6C44U, 0x6C45U, 0x6C46U, 0x6C47U, 0x6C48U,
	0x6C49U, 0x6C4AU, 0x6C4BU, 0x6C4CU, 0x6C4DU, 0x6C4EU, 0x6C4FU, 0x6C50U,
	0x6C51U, 0x6C52U, 0x6C53U, 0x6C54U, 0x6C55U, 0x6C56U, 0x6C57U, 0x6C58U,
	0x6C59U, 0x6C5AU, 0x6C61U, 0x6C62U, 0x6C63U, 0x6C64U, 0x6C65U, 0x6C66U,
	0x6C67U, 0x6C68U, 0x6C69U, 0x6C6AU, 0x6C6BU, 0x6C6CU, 0x6C6DU, 0x6C6EU,
	0x6C6FU, 0x6C70U, 0x6C71U, 0x6C72U, 0x6C73U, 0x6C74U, 0x6C75U, 0x6C76U,
	0x6C77U, 0x6C78U, 0x6C79U, 0x6C7AU, 0x6C30U, 0x6C31U, 0x6C32U, 0x6C33U,
	0x6C34U, 0x6C35U, 0x6C36U, 0x6C37U, 0x6C38U, 0x6C39U, 0x6C2BU, 0x6C2FU,
	0x6D41U, 0x6D42U, 0x6D43U, 0x6D44U, 0x6D45U, 0x6D46U, 0x6D47U, 0x6D48U,
	0x6D49U, 0x6D4AU, 0x6D4BU, 0x6D4CU, 0x6D4DU, 0x6D4EU, 0x6D4FU, 0x6D50U,
	0x6D51U, 0x6D52U, 0x6D53U, 0x6D54U, 0x6D55U, 0x6D56U, 0x6D57U, 0x6D58U,
	0x6D59U, 0x6D5AU, 0x6D61U, 0x6D62U, 0x6D63U, 0x6D64U, 0x6D65U, 0x6D66U,
	0x6D67U, 0x6D68U, 0x6D69U, 0x6D6AU, 0x6D6BU, 0x6D6CU, 0x6D6DU, 0x6D6EU,
	0x6D6FU, 0x6D70U, 0x6D71U, 0x6D72U, 0x6D73U, 0x6D74U, 0x6D75U, 0x6D76U,
	0x6D77U, 0x6D78U, 0x6D79U, 0x6D7AU, 0x6D30U, 0x6D31U, 0x6D32U, 0x6D33U,
	0x6D34U, 0x6D35U, 0x6D36U, 0x6D37U, 0x6D38U, 0x6D39U, 0x6D2BU, 0x6D2FU,
	0x6E41U, 0x6E42U, 0x6E43U, 0x6E44U, 0x6E45U, 0x6E46U, 0x6E47U, 0x6E48U,
	0x6E49U, 0x6E4AU, 0x6E4BU, 0x6E4CU, 0x6E4DU, 0x6E4EU, 0x6E4FU, 0x6E50U,
	0x6E51U, 0x6E52U, 0x6E53U, 0x6E54U, 0x6E55U, 0x6E56U, 0x6E57U, 0x6E58U,
	0x6E59U, 0x6E5AU, 0x6E61U, 0x6E62U, 0x6E63U, 0x6E64U, 0x6E65U, 0x6E66U,
	0x6E67U, 0x6E68U, 0x6E69U, 0x6E6AU, 0x6E6BU, 0x6E6CU, 0x6E6DU, 0x6E6EU,
	0x6E6FU, 0x6E70U, 0x6E71U, 0x6E72U, 0x6E73U, 0x6E74U, 0x6E75U, 0x6E76U,
	0x6E77U, 0x6E78U, 0x6E79U, 0x6E7AU, 0x6E30U, 0x6E31U, 0x6E32U, 0x6E33U,
	0x6E34U, 0x6E35U, 0x6E36U, 0x6E37U, 0x6E38U, 0x6E39U, 0x6E2BU, 0x6E2FU,
	0x6F41U, 0x6F42U, 0x6F43U, 0x6F44U, 0x6F45U, 0x6F46U, 0x6F47U, 0x6F48U,
	0x6F49U, 0x6F4AU, 0x6F4BU, 0x6F4CU, 0x6F4DU, 0x6F4EU, 0x6F4FU, 0x6F50U,
	0x6F51U, 0x6F52U, 0x6F53U, 0x6F54U, 0x6F55U, 0x6F56U, 0x6F57U, 0x6F58U,
	0x6F59U, 0x6F5AU, 0x6F61U, 0x6F62U, 0x6F63U, 0x6F64U, 0x6F65U, 0x6F66U,
	0x6F67U, 0x6F68U, 0x6F69U, 0x6F6AU, 0x6F6BU, 0x6F6CU, 0x6F6DU, 0x6F6EU,
	0x6F6FU, 0x6F70U, 0x6F71U, 0x6F72U, 0x6F73U, 0x6F74U, 0x6F75U, 0x6F76U,
	0x6F77U, 0x6F78U, 0x6F79U, 0x6F7AU, 0x6F30U, 0x6F31U, 0x6F32U, 0x6F33U,
	0x6F34U, 0x6F35U, 0x6F36U, 0x6F37U, 0x6F38U, 0x6F39U, 0x6F2BU, 0x6F2FU,
	0x7041U, 0x7042U, 0x7043U, 0x7044U, 0x7045U, 0x7046U, 0x7047U, 0x7048U,
	0x7049U, 0x704AU, 0x704BU, 0x704CU, 0x704DU, 0x704EU, 0x704FU, 0x7050U,
	0x7051U, 0x7052U, 0x7053U, 0x7054U, 0x7055U, 0x7056U, 0x7057U, 0x7058U,
	0x7059U, 0x705AU, 0x7061U, 0x7062U, 0x7063U, 0x7064U, 0x7065U, 0x7066U,
	0x7067U, 0x7068U, 0x7069U, 0x706AU, 0x706BU, 0x706CU, 0x706DU, 0x706EU,
	0x706FU, 0x7070U, 0x7071U, 0x7072U, 0x7073U, 0x7074U, 0x7075U, 0x7076U,
	0x7077U, 0x7078U, 0x7079U, 0x707AU, 0x7030U, 0x7031U, 0x7032U, 0x7033U,
	0x7034U, 0x7035U, 0x7036U, 0x7037U, 0x7038U, 0x7039U, 0x702BU, 0x702FU,
	0x7141U, 0x7142U, 0x7143U, 0x7144U, 0x7145U, 0x7146U, 0x7147U, 0x7148U,
	0x7149U, 0x714AU, 0x714BU, 0x714CU, 0x714DU, 0x714EU, 0x714FU, 0x7150U,
	0x7151U, 0x7152U, 0x7153U, 0x7154U, 0x7155U, 0x7156U, 0x7157U, 0x7158U,
	0x7159U, 0x715AU, 0x7161U, 0x7162U, 0x7163U, 0x7164U, 0x7165U, 0x7166U,
	0x7167U, 0x7168U, 0x7169U, 0x716AU, 0x716BU, 0x716CU, 0x716DU, 0x716EU,
	0x716FU, 0x7170U, 0x7171U, 0x7172U, 0x7173U, 0x7174U, 0x7175U, 0x7176U,
	0x7177U, 0x7178U, 0x7179U, 0x717AU, 0x7130U, 0x7131U, 0x7132U, 0x7133U,
	0x7134U, 0x7135U, 0x7136U, 0x7137U, 0x7138U, 0x7139U, 0x712BU, 0x712FU,
	0x7241U, 0x7242U, 0x7243U, 0x7244U, 0x7245U, 0x7246U, 0x7247U, 0x7248U,
	0x7249U, 0x724AU, 0x724BU, 0x724CU, 0x724DU, 0x724EU, 0x724FU, 0x7250U,
	0x7251U, 0x7252U, 0x7253U, 0x7254U, 0x7255U, 0x7256U, 0x7257U, 0x7258U,
	0x7259U, 0x725AU, 0x7261U, 0x7262U, 0x7263U, 0x7264U, 0x7265U, 0x7266U,
	0x7267U, 0x7268U, 0x7269U, 0x726AU, 0x726BU, 0x726CU, 0x726DU, 0x726EU,
	0x726FU, 0x7270U, 0x7271U, 0x7272U, 0x7273U, 0x7274U, 0x7275U, 0x7276U,
	0x7277U, 0x7278U, 0x7279U, 0x727AU, 0x7230U, 0x7231U, 0x7232U, 0x7233U,
	0x7234U, 0x7235U, 0x7236U, 0x7237U, 0x7238U, 0x7239U, 0x722BU, 0x722FU,
	0x7341U, 0x7342U, 0x7343U, 0x7344U, 0x7345U, 0x7346U, 0x7347U, 0x7348U,
	0x7349U, 0x734AU, 0x734BU, 0x734CU, 0x734DU, 0x734EU, 0x734FU, 0x7350U,
	0x7351U, 0x7352U, 0x7353U, 0x7354U, 0x7355U, 0x7356U, 0x7357U, 0x7358U,
	0x7359U, 0x735AU, 0x7361U, 0x7362U, 0x7363U, 0x7364U, 0x7365U, 0x7366U,
	0x7367U, 0x7368U, 0x7369U, 0x736AU, 0x736BU, 0x736CU, 0x736DU, 0x736EU,
	0x736FU, 0x7370U, 0x7371U, 0x7372U, 0x7373U, 0x7374U, 0x7375U, 0x7376U,
	0x7377U, 0x7378U, 0x7379U, 0x737AU, 0x7330U, 0x7331U, 0x7332U, 0x7333U,
	0x7334U, 0x7335U, 0x7336U, 0x7337U, 0x7338U, 0x7339U, 0x732BU, 0x732FU,
	0x7441U, 0x7442U, 0x7443U, 0x7444U, 0x7445U, 0x7446U, 0x7447U, 0x7448U,
	0x7449U, 0x744AU, 0x744BU, 0x744CU, 0x744DU, 0x744EU, 0x744FU, 0x7450U,
	0x7451U, 0x7452U, 0x7453U, 0x7454U, 0x7455U, 0x7456U, 0x7457U, 0x7458U,
	0x7459U, 0x745AU, 0x7461U, 0x7462U, 0x7463U, 0x7464U, 0x7465U, 0x7466U,
	0x7467U, 0x7468U, 0x7469U, 0x746AU, 0x746BU, 0x746CU, 0x746DU, 0x746EU,
	0x746FU, 0x7470U, 0x7471U, 0x7472U, 0x7473U, 0x7474U, 0x7475U, 0x7476U,
	0x7477U, 0x7478U, 0x7479U, 0x747AU, 0x7430U, 0x7431U, 0x7432U, 0x7433U,
	0x7434U, 0x7435U, 0x7436U, 0x7437U, 0x7438U, 0x7439U, 0x742BU, 0x742FU,
	0x7541U, 0x7542U, 0x7543U, 0x7544U, 0x7545U, 0x7546U, 0x7547U, 0x7548U,
	0x7549U, 0x754AU, 0x754BU, 0x754CU, 0x754DU, 0x754EU, 0x754FU, 0x7550U,
	0x7551U, 0x7552U, 0x7553U, 0x7554U, 0x7555U, 0x7556U, 0x7557U, 0x7558U,
	0x7559U, 0x755AU, 0x7561U, 0x7562U, 0x7563U, 0x7564U, 0x7565U, 0x7566U,
	0x7567U, 0x7568U, 0x7569U, 0x756AU, 0x756BU, 0x756CU, 0x756DU, 0x756EU,
	0x756FU, 0x7570U, 0x7571U, 0x7572U, 0x7573U, 0x7574U, 0x7575U, 0x7576U,
	0x7577U, 0x7578U, 0x7579U, 0x757AU, 0x7530U, 0x7531U, 0x7532U, 0x7533U,
	0x7534U, 0x7535U, 0x7536U, 0x7537U, 0x7538U, 0x7539U, 0x752BU, 0x752FU,
	0x7641U, 0x7642U, 0x7643U, 0x7644U, 0x7645U, 0x7646U, 0x7647U, 0x7648U,
	0x7649U, 0x764AU, 0x764BU, 0x764CU, 0x764DU, 0x764EU, 0x764FU, 0x7650U,
	0x7651U, 0x7652U, 0x7653U, 0x7654U, 0x7655U, 0x7656U, 0x7657U, 0x7658U,
	0x7659U, 0x765AU, 0x7661U, 0x7662U, 0x7663U, 0x7664U, 0x7665U, 0x7666U,
	0x7667U, 0x7668U, 0x7669U, 0x766AU, 0x766BU, 0x766CU, 0x766DU, 0x766EU,
	0x766FU, 0x7670U, 0x7671U, 0x7672U, 0x7673U, 0x7674U, 0x7675U, 0x7676U,
	0x7677U, 0x7678U, 0x7679U, 0x767AU, 0x7630U, 0x7631U, 0x7632U, 0x7633U,
	0x7634U, 0x7635U, 0x7636U, 0x7637U, 0x7638U, 0x7639U, 0x762BU, 0x762FU,
	0x7741U, 0x7742U, 0x7743U, 0x7744U, 0x7745U, 0x7746U, 0x7747U, 0x7748U,
	0x7749U, 0x774AU, 0x774BU, 0x774CU, 0x774DU, 0x774EU, 0x774FU, 0x7750U,
	0x7751U, 0x7752U, 0x7753U, 0x7754U, 0x7755U, 0x7756U, 0x7757U, 0x7758U,
	0x7759U, 0x775AU, 0x7761U, 0x7762U, 0x7763U, 0x7764U, 0x7765U, 0x7766U,
	0x7767U, 0x7768U, 0x7769U, 0x776AU, 0x776BU, 0x776CU, 0x776DU, 0x776EU,
	0x776FU, 0x7770U, 0x7771U, 0x7772U, 0x7773U, 0x7774U, 0x7775U, 0x7776U,
	0x7777U, 0x7778U, 0x7779U, 0x777AU, 0x7730U, 0x7731U, 0x7732U, 0x7733U,
	0x7734U, 0x7735U, 0x7736U, 0x7737U, 0x7738U, 0x7739U, 0x772BU, 0x772FU,
	0x7841U, 0x7842U, 0x7843U, 0x7844U, 0x7845U, 0x7846U, 0x7847U, 0x7848U,
	0x7849U, 0x784AU, 0x784BU, 0x784CU, 0x784DU, 0x784EU, 0x784FU, 0x7850U,
	0x7851U, 0x7852U, 0x7853U, 0x7854U, 0x7855U, 0x7856U, 0x7857U, 0x7858U,
	0x7859U, 0x785AU, 0x7861U, 0x7862U, 0x7863U, 0x7864U, 0x7865U, 0x7866U,
	0x7867U, 0x7868U, 0x7869U, 0x786AU, 0x786BU, 0x786CU, 0x786DU, 0x786EU,
	0x786FU, 0x7870U, 0x7871U, 0x7872U, 0x7873U, 0x7874U, 0x7875U, 0x7876U,
	0x7877U, 0x7878U, 0x7879U, 0x787AU, 0x7830U, 0x7831U, 0x7832U, 0x7833U,
	0x7834U, 0x7835U, 0x7836U, 0x7837U, 0x7838U, 0x7839U, 0x782BU, 0x782FU,
	0x7941U, 0x7942U, 0x7943U, 0x7944U, 0x7945U, 0x7946U, 0x7947U, 0x7948U,
	0x7949U, 0x794AU, 0x794BU, 0x794CU, 0x794DU, 0x794EU, 0x794FU, 0x7950U,
	0x7951U, 0x7952U, 0x7953U, 0x7954U, 0x7955U, 0x7956U, 0x7957U, 0x7958U,
	0x7959U, 0x795AU, 0x7961U, 0x7962U, 0x7963U, 0x7964U, 0x7965U, 0x7966U,
	0x7967U, 0x7968U, 0x7969U, 0x796AU, 0x796BU, 0x796CU, 0x796DU, 0x796EU,
	0x796FU, 0x7970U, 0x7971U, 0x7972U, 0x7973U, 0x7974U, 0x7975U, 0x7976U,
	0x7977U, 0x7978U, 0x7979U, 0x797AU, 0x7930U, 0x7931U, 0x7932U, 0x7933U,
	0x7934U, 0x7935U, 0x7936U, 0x7937U, 0x7938U, 0x7939U, 0x792BU, 0x792FU,
	0x7A41U, 0x7A42U, 0x7A43U, 0x7A44U, 0x7A45U, 0x7A46U, 0x7A47U, 0x7A48U,
	0x7A49U, 0x7A4AU, 0x7A4BU, 0x7A4CU, 0x7A4DU, 0x7A4EU, 0x7A4FU, 0x7A50U,
	0x7A51U, 0x7A52U, 0x7A53U, 0x7A54U, 0x7A55U, 0x7A56U, 0x7A57U, 0x7A58U,
	0x7A59U, 0x7A5AU, 0x7A61U, 0x7A62U, 0x7A63U, 0x7A64U, 0x7A65U, 0x7A66U,
	0x7A67U, 0x7A68U, 0x7A69U, 0x7A6AU, 0x7A6BU, 0x7A6CU, 0x7A6DU, 0x7A6EU,
	0x7A6FU, 0x7A70U, 0x7A71U, 0x7A72U, 0x7A73U, 0x7A74U, 0x7A75U, 0x7A76U,
	0x7A77U, 0x7A78U, 0x7A79U, 0x7A7AU, 0x7A30U, 0x7A31U, 0x7A32U, 0x7A33U,
	0x7A34U, 0x7A35U, 0x7A36U, 0x7A37U, 0x7A38U, 0x7A39U, 0x7A2BU, 0x7A2FU,
	0x3041U, 0x3042U, 0x3043U, 0x3044U, 0x3045U, 0x3046U, 0x3047U, 0x3048U,
	0x3049U, 0x304AU, 0x304BU, 0x304CU, 0x304DU, 0x304EU, 0x304FU, 0x3050U,
	0x3051U, 0x3052U, 0x3053U, 0x3054U, 0x3055U, 0x3056U, 0x3057U, 0x3058U,
	0x3059U, 0x305AU, 0x3061U, 0x3062U, 0x3063U, 0x3064U, 0x3065U, 0x3066U,
	0x3067U, 0x3068U, 0x3069U, 0x306AU, 0x306BU, 0x306CU, 0x306DU, 0x306EU,
	0x306FU, 0x3070U, 0x3071U, 0x3072U, 0x3073U, 0x3074U, 0x3075U, 0x3076U,
	0x3077U, 0x3078U, 0x3079U, 0x307AU, 0x3030U, 0x3031U, 0x3032U, 0x3033U,
	0x3034U, 0x3035U, 0x3036U, 0x3037U, 0x3038U, 0x3039U, 0x302BU, 0x302FU,
	0x3141U, 0x3142U, 0x3143U, 0x3144U, 0x3145U, 0x3146U, 0x3147U, 0x3148U,
	0x3149U, 0x314AU, 0x314BU, 0x314CU, 0x314DU, 0x314EU, 0x314FU, 0x3150U,
	0x3151U, 0x3152U, 0x3153U, 0x3154U, 0x3155U, 0x3156U, 0x3157U, 0x3158U,
	0x3159U, 0x315AU, 0x3161U, 0x3162U, 0x3163U, 0x3164U, 0x3165U, 0x3166U,
	0x3167U, 0x3168U, 0x3169U, 0x316AU, 0x316BU, 0x316CU, 0x316DU, 0x316EU,
	0x316FU, 0x3170U, 0x3171U, 0x3172U, 0x3173U, 0x3174U, 0x3175U, 0x3176U,
	0x3177U, 0x3178U, 0x3179U, 0x317AU, 0x3130U, 0x3131U, 0x3132U, 0x3133U,
	0x3134U, 0x3135U, 0x3136U, 0x3137U, 0x3138U, 0x3139U, 0x312BU, 0x312FU,
	0x3241U, 0x3242U, 0x3243U, 0x3244U, 0x3245U, 0x3246U, 0x3247U, 0x3248U,
	0x3249U, 0x324AU, 0x324BU, 0x324CU, 0x324DU, 0x324EU, 0x324FU, 0x3250U,
	0x3251U, 0x3252U, 0x3253U, 0x3254U, 0x3255U, 0x3256U, 0x3257U, 0x3258U,
	0x3259U, 0x325AU, 0x3261U, 0x3262U, 0x3263U, 0x3264U, 0x3265U, 0x3266U,
	0x3267U, 0x3268U, 0x3269U, 0x326AU, 0x326BU, 0x326CU, 0x326DU, 0x326EU,
	0x326FU, 0x3270U, 0x3271U, 0x3272U, 0x3273U, 0x3274U, 0x3275U, 0x3276U,
	0x3277U, 0x3278U, 0x3279U, 0x327AU, 0x3230U, 0x3231U, 0x3232U, 0x3233U,
	0x3234U, 0x3235U, 0x3236U, 0x3237U, 0x3238U, 0x3239U, 0x322BU, 0x322FU,
	0x3341U, 0x3342U, 0x3343U, 0x3344U, 0x3345U, 0x3346U, 0x3347U, 0x3348U,
	0x3349U, 0x334AU, 0x334BU, 0x334CU, 0x334DU, 0x334EU, 0x334FU, 0x3350U,
	0x3351U, 0x3352U, 0x3353U, 0x3354U, 0x3355U, 0x3356U, 0x3357U, 0x3358U,
	0x3359U, 0x335AU, 0x3361U, 0x3362U, 0x3363U, 0x3364U, 0x3365U, 0x3366U,
	0x3367U, 0x3368U, 0x3369U, 0x336AU, 0x336BU, 0x336CU, 0x336DU, 0x336EU,
	0x336FU, 0x3370U, 0x3371U, 0x3372U, 0x3373U, 0x3374U, 0x3375U, 0x3376U,
	0x3377U, 0x3378U, 0x3379U, 0x337AU, 0x3330U, 0x3331U, 0x3332U, 0x3333U,
	0x3334U, 0x3335U, 0x3336U, 0x3337U, 0x3338U, 0x3339U, 0x332BU, 0x332FU,
	0x3441U, 0x3442U, 0x3443U, 0x3444U, 0x3445U, 0x3446U, 0x3447U, 0x3448U,
	0x3449U, 0x344AU, 0x344BU, 0x344CU, 0x344DU, 0x344EU, 0x344FU, 0x3450U,
	0x3451U, 0x3452U, 0x3453U, 0x3454U, 0x3455U, 0x3456U, 0x3457U, 0x3458U,
	0x3459U, 0x345AU, 0x3461U, 0x3462U, 0x3463U, 0x3464U, 0x3465U, 0x3466U,
	0x3467U, 0x3468U, 0x3469U, 0x346AU, 0x346BU, 0x346CU, 0x346DU, 0x346EU,
	0x346FU, 0x3470U, 0x3471U, 0x3472U, 0x3473U, 0x3474U, 0x3475U, 0x3476U,
	0x3477U, 0x3478U, 0x3479U, 0x347AU, 0x3430U, 0x3431U, 0x3432U, 0x3433U,
	0x3434U, 0x3435U, 0x3436U, 0x3437U, 0x3438U, 0x3439U, 0x342BU, 0x342FU,
	0x3541U, 0x3542U, 0x3543U, 0x3544U, 0x3545U, 0x3546U, 0x3547U, 0x3548U,
	0x3549U, 0x354AU, 0x354BU, 0x354CU, 0x354DU, 0x354EU, 0x354FU, 0x3550U,
	0x3551U, 0x3552U, 0x3553U, 0x3554U, 0x3555U, 0x3556U, 0x3557U, 0x3558U,
	0x3559U, 0x355AU, 0x3561U, 0x3562U, 0x3563U, 0x3564U, 0x3565U, 0x3566U,
	0x3567U, 0x3568U, 0x3569U, 0x356AU, 0x356BU, 0x356CU, 0x356DU, 0x356EU,
	0x356FU, 0x3570U, 0x3571U, 0x3572U, 0x3573U, 0x3574U, 0x3575U, 0x3576U,
	0x3577U, 0x3578U, 0x3579U, 0x357AU, 0x3530U, 0x3531U, 0x3532U, 0x3533U,
	0x3534U, 0x3535U, 0x3536U, 0x3537U, 0x3538U, 0x3539U, 0x352BU, 0x352FU,
	0x3641U, 0x3642U, 0x3643U, 0x3644U, 0x3645U, 0x3646U, 0x3647U, 0x3648U,
	0x3649U, 0x364AU, 0x364BU, 0x364CU, 0x364DU, 0x364EU, 0x364FU, 0x3650U,
	0x3651U, 0x3652U, 0x3653U, 0x3654U, 0x3655U, 0x3656U, 0x3657U, 0x3658U,
	0x3659U, 0x365AU, 0x3661U, 0x3662U, 0x3663U, 0x3664U, 0x3665U, 0x3666U,
	0x3667U, 0x3668U, 0x3669U, 0x366AU, 0x366BU, 0x366CU, 0x366DU, 0x366EU,
	0x366FU, 0x3670U, 0x3671U, 0x3672U, 0x3673U, 0x3674U, 0x3675U, 0x3676U,
	0x3677U, 0x3678U, 0x3679U, 0x367AU, 0x3630U, 0x3631U, 0x3632U, 0x3633U,
	0x3634U, 0x3635U, 0x3636U, 0x3637U, 0x3638U, 0x3639U, 0x362BU, 0x362FU,
	0x3741U, 0x3742U, 0x3743U, 0x3744U, 0x3745U, 0x3746U, 0x3747U, 0x3748U,
	0x3749U, 0x374AU, 0x374BU, 0x374CU, 0x374DU, 0x374EU, 0x374FU, 0x3750U,
	0x3751U, 0x3752U, 0x3753U, 0x3754U, 0x3755U, 0x3756U, 0x3757U, 0x3758U,
	0x3759U, 0x375AU, 0x3761U, 0x3762U, 0x3763U, 0x3764U, 0x3765U, 0x3766U,
	0x3767U, 0x3768U, 0x3769U, 0x376AU, 0x376BU, 0x376CU, 0x376DU, 0x376EU,
	0x376FU, 0x3770U, 0x3771U, 0x3772U, 0x3773U, 0x3774U, 0x3775U, 0x3776U,
	0x3777U, 0x3778U, 0x3779U, 0x377AU, 0x3730U, 0x3731U, 0x3732U, 0x3733U,
	0x3734U, 0x3735U, 0x3736U, 0x3737U, 0x3738U, 0x3739U, 0x372BU, 0x372FU,
	0x3841U, 0x3842U, 0x3843U, 0x3844U, 0x3845U, 0x3846U, 0x3847U, 0x3848U,
	0x3849U, 0x384AU, 0x384BU, 0x384CU, 0x384DU, 0x384EU, 0x384FU, 0x3850U,
	0x3851U, 0x3852U, 0x3853U, 0x3854U, 0x3855U, 0x3856U, 0x3857U, 0x3858U,
	0x3859U, 0x385AU, 0x3861U, 0x3862U, 0x3863U, 0x3864U, 0x3865U, 0x3866U,
	0x3867U, 0x3868U, 0x3869U, 0x386AU, 0x386BU, 0x386CU, 0x386DU, 0x386EU,
	0x386FU, 0x3870U, 0x3871U, 0x3872U, 0x3873U, 0x3874U, 0x3875U, 0x3876U,
	0x3877U, 0x3878U, 0x3879U, 0x387AU, 0x3830U, 0x3831U, 0x3832U, 0x3833U,
	0x3834U, 0x3835U, 0x3836U, 0x3837U, 0x3838U, 0x3839U, 0x382BU, 0x382FU,
	0x3941U, 0x3942U, 0x3943U, 0x3944U, 0x3945U, 0x3946U, 0x3947U, 0x3948U,
	0x3949U, 0x394AU, 0x394BU, 0x394CU, 0x394DU, 0x394EU, 0x394FU, 0x3950U,
	0x3951U, 0x3952U, 0x3953U, 0x3954U, 0x3955U, 0x3956U, 0x3957U, 0x3958U,
	0x3959U, 0x395AU, 0x3961U, 0x3962U, 0x3963U, 0x3964U, 0x3965U, 0x3966U,
	0x3967U, 0x3968U, 0x3969U, 0x396AU, 0x396BU, 0x396CU, 0x396DU, 0x396EU,
	0x396FU, 0x3970U, 0x3971U, 0x3972U, 0x3973U, 0x3974U, 0x3975U, 0x3976U,
	0x3977U, 0x3978U, 0x3979U, 0x397AU, 0x3930U, 0x3931U, 0x3932U, 0x3933U,
	0x3934U, 0x3935U, 0x3936U, 0x3937U, 0x3938U, 0x3939U, 0x392BU, 0x392FU,
	0x2B41U, 0x2B42U, 0x2B43U, 0x2B44U, 0x2B45U, 0x2B46U, 0x2B47U, 0x2B48U,
	0x2B49U, 0x2B4AU, 0x2B4BU, 0x2B4CU, 0x2B4DU, 0x2B4EU, 0x2B4FU, 0x2B50U,
	0x2B51U, 0x2B52U, 0x2B53U, 0x2B54U, 0x2B55U, 0x2B56U, 0x2B57U, 0x2B58U,
	0x2B59U, 0x2B5AU, 0x2B61U, 0x2B62U, 0x2B63U, 0x2B64U, 0x2B65U, 0x2B66U,
	0x2B67U, 0x2B68U, 0x2B69U, 0x2B6AU, 0x2B6BU, 0x2B6CU, 0x2B6DU, 0x2B6EU,
	0x2B6FU, 0x2B70U, 0x2B71U, 0x2B72U, 0x2B73U, 0x2B74U, 0x2B75U, 0x2B76U,
	0x2B77U, 0x2B78U, 0x2B79U, 0x2B7AU, 0x2B30U, 0x2B31U, 0x2B32U, 0x2B33U,
	0x2B34U, 0x2B35U, 0x2B36U, 0x2B37U, 0x2B38U, 0x2B39U, 0x2B2BU, 0x2B2FU,
	0x2F41U, 0x2F42U, 0x2F43U, 0x2F44U, 0x2F45U, 0x2F46U, 0x2F47U, 0x2F48U,
	0x2F49U, 0x2F4AU, 0x2F4BU, 0x2F4CU, 0x2F4DU, 0x2F4EU, 0x2F4FU, 0x2F50U,
	0x2F51U, 0x2F52U, 0x2F53U, 0x2F54U, 0x2F55U, 0x2F56U, 0x2F57U, 0x2F58U,
	0x2F59U, 0x2F5AU, 0x2F61U, 0x2F62U, 0x2F63U, 0x2F64U, 0x2F65U, 0x2F66U,
	0x2F67U, 0x2F68U, 0x2F69U, 0x2F6AU, 0x2F6BU, 0x2F6CU, 0x2F6DU, 0x2F6EU,
	0x2F6FU, 0x2F70U, 0x2F71U, 0x2F72U, 0x2F73U, 0x2F74U, 0x2F75U, 0x2F76U,
	0x2F77U, 0x2F78U, 0x2F79U, 0x2F7AU, 0x2F30U, 0x2F31U, 0x2F32U, 0x2F33U,
	0x2F34U, 0x2F35U, 0x2F36U, 0x2F37U, 0x2F38U, 0x2F39U, 0x2F2BU, 0x2F2FU,
#endif
};
////------ tables/table_enc_12bit.h  end <-----
#endif
////=============> tables/tables.c  end <=========================






////-:--:--:--:--:--:--:-- arch begine --:--:--:--:--:--:--:--:--:--:--:--:--:-
////=============> arch/avx512/codec.c  begine <=========================
//#include <stdint.h>
//#include <stddef.h>
#include <stdlib.h>

//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#if HAVE_AVX512
#include <immintrin.h>

//#include "../avx2/dec_reshuffle.c"
//#include "../avx2/dec_loop.c"
//#include "enc_reshuffle_translate.c"
//#include "enc_loop.c"

//=============> avx2/dec_reshuffle.c  begine <===
static inline __m256i
dec_reshuffle (const __m256i in)
{
	// in, lower lane, bits, upper case are most significant bits, lower
	// case are least significant bits:
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA

	const __m256i merge_ab_and_bc = _mm256_maddubs_epi16(in, _mm256_set1_epi32(0x01400140));
	// 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	// 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	// 0000eeee FFffffff 0000DDDD DDddEEEE
	// 0000bbbb CCcccccc 0000AAAA AAaaBBBB

	__m256i out = _mm256_madd_epi16(merge_ab_and_bc, _mm256_set1_epi32(0x00011000));
	// 00000000 JJJJJJjj KKKKkkkk LLllllll
	// 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	// 00000000 DDDDDDdd EEEEeeee FFffffff
	// 00000000 AAAAAAaa BBBBbbbb CCcccccc

	// Pack bytes together in each lane:
	out = _mm256_shuffle_epi8(out, _mm256_setr_epi8(
		2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1,
		2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1));
	// 00000000 00000000 00000000 00000000
	// LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	// HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	// DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa

	// Pack lanes:
	return _mm256_permutevar8x32_epi32(out, _mm256_setr_epi32(0, 1, 2, 4, 5, 6, -1, -1));
}
//=============> avx2/dec_reshuffle.c  end <===

//=============> avx2/dec_loop.c  begine <===
static inline int
dec_loop_avx2_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const __m256i lut_lo = _mm256_setr_epi8(
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A,
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);

	const __m256i lut_hi = _mm256_setr_epi8(
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);

	const __m256i lut_roll = _mm256_setr_epi8(
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0,
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0);

	const __m256i mask_2F = _mm256_set1_epi8(0x2F);

	// Load input:
	__m256i str = _mm256_loadu_si256((__m256i *) *s);

	// See the SSSE3 decoder for an explanation of the algorithm.
	const __m256i hi_nibbles = _mm256_and_si256(_mm256_srli_epi32(str, 4), mask_2F);
	const __m256i lo_nibbles = _mm256_and_si256(str, mask_2F);
	const __m256i hi         = _mm256_shuffle_epi8(lut_hi, hi_nibbles);
	const __m256i lo         = _mm256_shuffle_epi8(lut_lo, lo_nibbles);

	if (!_mm256_testz_si256(lo, hi)) {
		return 0;
	}

	const __m256i eq_2F = _mm256_cmpeq_epi8(str, mask_2F);
	const __m256i roll  = _mm256_shuffle_epi8(lut_roll, _mm256_add_epi8(eq_2F, hi_nibbles));

	// Now simply add the delta values to the input:
	str = _mm256_add_epi8(str, roll);

	// Reshuffle the input to packed 12-byte output format:
	str = dec_reshuffle(str);

	// Store the output:
	_mm256_storeu_si256((__m256i *) *o, str);

	*s += 32;
	*o += 24;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_avx2 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 45) {
		return;
	}

	// Process blocks of 32 bytes per round. Because 8 extra zero bytes are
	// written after the output, ensure that there will be at least 13
	// bytes of input data left to cover the gap. (11 data bytes and up to
	// two end-of-string markers.)
	size_t rounds = (*slen - 13) / 32;

	*slen -= rounds * 32;	// 32 bytes consumed per round
	*olen += rounds * 24;	// 24 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_avx2_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 32;
	*olen -= rounds * 24;
}
//=============> avx2/dec_loop.c  end <===

//=============> avx512/enc_reshuffle_translate.c  begine <===
// AVX512 algorithm is based on permutevar and multishift. The code is based on
// https://github.com/WojciechMula/base64simd which is under BSD-2 license.

static inline __m512i
enc_reshuffle_translate (const __m512i input)
{
	// 32-bit input
	// [ 0  0  0  0  0  0  0  0|c1 c0 d5 d4 d3 d2 d1 d0|
	//  b3 b2 b1 b0 c5 c4 c3 c2|a5 a4 a3 a2 a1 a0 b5 b4]
	// output order  [1, 2, 0, 1]
	// [b3 b2 b1 b0 c5 c4 c3 c2|c1 c0 d5 d4 d3 d2 d1 d0|
	//  a5 a4 a3 a2 a1 a0 b5 b4|b3 b2 b1 b0 c3 c2 c1 c0]

	const __m512i shuffle_input = _mm512_setr_epi32(0x01020001,
	                                                0x04050304,
	                                                0x07080607,
	                                                0x0a0b090a,
	                                                0x0d0e0c0d,
	                                                0x10110f10,
	                                                0x13141213,
	                                                0x16171516,
	                                                0x191a1819,
	                                                0x1c1d1b1c,
	                                                0x1f201e1f,
	                                                0x22232122,
	                                                0x25262425,
	                                                0x28292728,
	                                                0x2b2c2a2b,
	                                                0x2e2f2d2e);

	// Reorder bytes
	// [b3 b2 b1 b0 c5 c4 c3 c2|c1 c0 d5 d4 d3 d2 d1 d0|
	//  a5 a4 a3 a2 a1 a0 b5 b4|b3 b2 b1 b0 c3 c2 c1 c0]
	const __m512i in = _mm512_permutexvar_epi8(shuffle_input, input);

	// After multishift a single 32-bit lane has following layout
	// [c1 c0 d5 d4 d3 d2 d1 d0|b1 b0 c5 c4 c3 c2 c1 c0|
	//  a1 a0 b5 b4 b3 b2 b1 b0|d1 d0 a5 a4 a3 a2 a1 a0]
	// (a = [10:17], b = [4:11], c = [22:27], d = [16:21])

	// 48, 54, 36, 42, 16, 22, 4, 10
	const __m512i shifts = _mm512_set1_epi64(0x3036242a1016040alu);
	__m512i shuffled_in = _mm512_multishift_epi64_epi8(shifts, in);

	// Translate immediatedly after reshuffled.
	const __m512i lookup = _mm512_loadu_si512(base64_table_enc_6bit);

	// Translation 6-bit values to ASCII.
	return _mm512_permutexvar_epi8(shuffled_in, lookup);
}
//=============> avx512/enc_reshuffle_translate.c  end <===

//=============> avx512/enc_loop.c  begine <===
static inline void
enc_loop_avx512_inner (const uint8_t **s, uint8_t **o)
{
	// Load input.
	__m512i src = _mm512_loadu_si512((__m512i *) *s);

	// Reshuffle, translate, store.
	src = enc_reshuffle_translate(src);
	_mm512_storeu_si512((__m512i *) *o, src);

	*s += 48;
	*o += 64;
}

static inline void
enc_loop_avx512 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 64) {
		return;
	}

	// Process blocks of 48 bytes at a time. Because blocks are loaded 64
	// bytes at a time, ensure that there will be at least 24 remaining
	// bytes after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer.
	size_t rounds = (*slen - 24) / 48;

	*slen -= rounds * 48;   // 48 bytes consumed per round
	*olen += rounds * 64;   // 64 bytes produced per round

	while (rounds > 0) {
		if (rounds >= 8) {
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_avx512_inner(s, o);
			enc_loop_avx512_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_avx512_inner(s, o);
		break;
	}
}
//=============> avx512/enc_loop.c  end <===

#endif	// HAVE_AVX512

BASE64_ENC_FUNCTION(avx512)
{
#if HAVE_AVX512
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
	enc_loop_avx512(&s, &slen, &o, &olen);
	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

// Reuse AVX2 decoding. Not supporting AVX512 at present
BASE64_DEC_FUNCTION(avx512)
{
#if HAVE_AVX512
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_avx2(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/avx512/codec.c  end <=========================




////=============> arch/avx2/codec.c  begine <=========================
//#include <stdint.h>
//#include <stddef.h>
#include <stdlib.h>

//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#if HAVE_AVX2
#include <immintrin.h>

// Only enable inline assembly on supported compilers and on 64-bit CPUs.
#ifndef BASE64_AVX2_USE_ASM
# if (defined(__GNUC__) || defined(__clang__)) && BASE64_WORDSIZE == 64
#  define BASE64_AVX2_USE_ASM 1
# else
#  define BASE64_AVX2_USE_ASM 0
# endif
#endif

//#include "dec_reshuffle.c"
//#include "dec_loop.c"
//=============> avx2/dec_reshuffle.c  begine <===
static inline __m256i
dec_reshuffle (const __m256i in)
{
	// in, lower lane, bits, upper case are most significant bits, lower
	// case are least significant bits:
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA

	const __m256i merge_ab_and_bc = _mm256_maddubs_epi16(in, _mm256_set1_epi32(0x01400140));
	// 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	// 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	// 0000eeee FFffffff 0000DDDD DDddEEEE
	// 0000bbbb CCcccccc 0000AAAA AAaaBBBB

	__m256i out = _mm256_madd_epi16(merge_ab_and_bc, _mm256_set1_epi32(0x00011000));
	// 00000000 JJJJJJjj KKKKkkkk LLllllll
	// 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	// 00000000 DDDDDDdd EEEEeeee FFffffff
	// 00000000 AAAAAAaa BBBBbbbb CCcccccc

	// Pack bytes together in each lane:
	out = _mm256_shuffle_epi8(out, _mm256_setr_epi8(
		2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1,
		2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1));
	// 00000000 00000000 00000000 00000000
	// LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	// HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	// DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa

	// Pack lanes:
	return _mm256_permutevar8x32_epi32(out, _mm256_setr_epi32(0, 1, 2, 4, 5, 6, -1, -1));
}
//=============> avx2/dec_reshuffle.c  end <===

//=============> avx2/dec_loop.c  begine <===
static inline int
dec_loop_avx2_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const __m256i lut_lo = _mm256_setr_epi8(
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A,
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);

	const __m256i lut_hi = _mm256_setr_epi8(
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);

	const __m256i lut_roll = _mm256_setr_epi8(
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0,
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0);

	const __m256i mask_2F = _mm256_set1_epi8(0x2F);

	// Load input:
	__m256i str = _mm256_loadu_si256((__m256i *) *s);

	// See the SSSE3 decoder for an explanation of the algorithm.
	const __m256i hi_nibbles = _mm256_and_si256(_mm256_srli_epi32(str, 4), mask_2F);
	const __m256i lo_nibbles = _mm256_and_si256(str, mask_2F);
	const __m256i hi         = _mm256_shuffle_epi8(lut_hi, hi_nibbles);
	const __m256i lo         = _mm256_shuffle_epi8(lut_lo, lo_nibbles);

	if (!_mm256_testz_si256(lo, hi)) {
		return 0;
	}

	const __m256i eq_2F = _mm256_cmpeq_epi8(str, mask_2F);
	const __m256i roll  = _mm256_shuffle_epi8(lut_roll, _mm256_add_epi8(eq_2F, hi_nibbles));

	// Now simply add the delta values to the input:
	str = _mm256_add_epi8(str, roll);

	// Reshuffle the input to packed 12-byte output format:
	str = dec_reshuffle(str);

	// Store the output:
	_mm256_storeu_si256((__m256i *) *o, str);

	*s += 32;
	*o += 24;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_avx2 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 45) {
		return;
	}

	// Process blocks of 32 bytes per round. Because 8 extra zero bytes are
	// written after the output, ensure that there will be at least 13
	// bytes of input data left to cover the gap. (11 data bytes and up to
	// two end-of-string markers.)
	size_t rounds = (*slen - 13) / 32;

	*slen -= rounds * 32;	// 32 bytes consumed per round
	*olen += rounds * 24;	// 24 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_avx2_inner(s, o, &rounds) &&
			    dec_loop_avx2_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_avx2_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 32;
	*olen -= rounds * 24;
}
//=============> avx2/dec_loop.c  end <===

#if BASE64_AVX2_USE_ASM
//# include "enc_loop_asm.c"
//=============> avx2/enc_loop_asm.c  begine <===
// Apologies in advance for combining the preprocessor with inline assembly,
// two notoriously gnarly parts of C, but it was necessary to avoid a lot of
// code repetition. The preprocessor is used to template large sections of
// inline assembly that differ only in the registers used. If the code was
// written out by hand, it would become very large and hard to audit.

// Generate a block of inline assembly that loads register R0 from memory. The
// offset at which the register is loaded is set by the given round and a
// constant offset.
#define LOAD(R0, ROUND, OFFSET) \
	"vlddqu ("#ROUND" * 24 + "#OFFSET")(%[src]), %["R0"] \n\t"

// Generate a block of inline assembly that deinterleaves and shuffles register
// R0 using preloaded constants. Outputs in R0 and R1.
#define SHUF(R0, R1, R2) \
	"vpshufb  %[lut0], %["R0"], %["R1"] \n\t" \
	"vpand    %["R1"], %[msk0], %["R2"] \n\t" \
	"vpand    %["R1"], %[msk2], %["R1"] \n\t" \
	"vpmulhuw %["R2"], %[msk1], %["R2"] \n\t" \
	"vpmullw  %["R1"], %[msk3], %["R1"] \n\t" \
	"vpor     %["R1"], %["R2"], %["R1"] \n\t"

// Generate a block of inline assembly that takes R0 and R1 and translates
// their contents to the base64 alphabet, using preloaded constants.
#define TRAN(R0, R1, R2) \
	"vpsubusb %[n51],  %["R1"], %["R0"] \n\t" \
	"vpcmpgtb %[n25],  %["R1"], %["R2"] \n\t" \
	"vpsubb   %["R2"], %["R0"], %["R0"] \n\t" \
	"vpshufb  %["R0"], %[lut1], %["R2"] \n\t" \
	"vpaddb   %["R1"], %["R2"], %["R0"] \n\t"

// Generate a block of inline assembly that stores the given register R0 at an
// offset set by the given round.
#define STOR(R0, ROUND) \
	"vmovdqu %["R0"], ("#ROUND" * 32)(%[dst]) \n\t"

// Generate a block of inline assembly that generates a single self-contained
// encoder round: fetch the data, process it, and store the result. Then update
// the source and destination pointers.
#define ROUND() \
	LOAD("a", 0, -4) \
	SHUF("a", "b", "c") \
	TRAN("a", "b", "c") \
	STOR("a", 0) \
	"add $24, %[src] \n\t" \
	"add $32, %[dst] \n\t"

// Define a macro that initiates a three-way interleaved encoding round by
// preloading registers a, b and c from memory.
// The register graph shows which registers are in use during each step, and
// is a visual aid for choosing registers for that step. Symbol index:
//
//  +  indicates that a register is loaded by that step.
//  |  indicates that a register is in use and must not be touched.
//  -  indicates that a register is decommissioned by that step.
//  x  indicates that a register is used as a temporary by that step.
//  V  indicates that a register is an input or output to the macro.
//
#define ROUND_3_INIT() 			/*  a b c d e f  */ \
	LOAD("a",   0,  -4)		/*  +            */ \
	SHUF("a", "d", "e")		/*  |     + x    */ \
	LOAD("b",   1,  -4)		/*  | +   |      */ \
	TRAN("a", "d", "e")		/*  | |   - x    */ \
	LOAD("c",   2,  -4)		/*  V V V        */

// Define a macro that translates, shuffles and stores the input registers A, B
// and C, and preloads registers D, E and F for the next round.
// This macro can be arbitrarily daisy-chained by feeding output registers D, E
// and F back into the next round as input registers A, B and C. The macro
// carefully interleaves memory operations with data operations for optimal
// pipelined performance.

#define ROUND_3(ROUND, A,B,C,D,E,F) 	/*  A B C D E F  */ \
	LOAD(D, (ROUND + 3), -4)	/*  V V V +      */ \
	SHUF(B, E, F)			/*  | | | | + x  */ \
	STOR(A, (ROUND + 0))		/*  - | | | |    */ \
	TRAN(B, E, F)			/*    | | | - x  */ \
	LOAD(E, (ROUND + 4), -4)	/*    | | | +    */ \
	SHUF(C, A, F)			/*  + | | | | x  */ \
	STOR(B, (ROUND + 1))		/*  | - | | |    */ \
	TRAN(C, A, F)			/*  -   | | | x  */ \
	LOAD(F, (ROUND + 5), -4)	/*      | | | +  */ \
	SHUF(D, A, B)			/*  + x | | | |  */ \
	STOR(C, (ROUND + 2))		/*  |   - | | |  */ \
	TRAN(D, A, B)			/*  - x   V V V  */

// Define a macro that terminates a ROUND_3 macro by taking pre-loaded
// registers D, E and F, and translating, shuffling and storing them.
#define ROUND_3_END(ROUND, A,B,C,D,E,F)	/*  A B C D E F  */ \
	SHUF(E, A, B)			/*  + x   V V V  */ \
	STOR(D, (ROUND + 3))		/*  |     - | |  */ \
	TRAN(E, A, B)			/*  - x     | |  */ \
	SHUF(F, C, D)			/*      + x | |  */ \
	STOR(E, (ROUND + 4))		/*      |   - |  */ \
	TRAN(F, C, D)			/*      - x   |  */ \
	STOR(F, (ROUND + 5))		/*            -  */

// Define a type A round. Inputs are a, b, and c, outputs are d, e, and f.
#define ROUND_3_A(ROUND) \
	ROUND_3(ROUND, "a", "b", "c", "d", "e", "f")

// Define a type B round. Inputs and outputs are swapped with regard to type A.
#define ROUND_3_B(ROUND) \
	ROUND_3(ROUND, "d", "e", "f", "a", "b", "c")

// Terminating macro for a type A round.
#define ROUND_3_A_LAST(ROUND) \
	ROUND_3_A(ROUND) \
	ROUND_3_END(ROUND, "a", "b", "c", "d", "e", "f")

// Terminating macro for a type B round.
#define ROUND_3_B_LAST(ROUND) \
	ROUND_3_B(ROUND) \
	ROUND_3_END(ROUND, "d", "e", "f", "a", "b", "c")

// Suppress clang's warning that the literal string in the asm statement is
// overlong (longer than the ISO-mandated minimum size of 4095 bytes for C99
// compilers). It may be true, but the goal here is not C99 portability.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverlength-strings"

static inline void
enc_loop_avx2 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	// For a clearer explanation of the algorithm used by this function,
	// please refer to the plain (not inline assembly) implementation. This
	// function follows the same basic logic.

	if (*slen < 32) {
		return;
	}

	// Process blocks of 24 bytes at a time. Because blocks are loaded 32
	// bytes at a time an offset of -4, ensure that there will be at least
	// 4 remaining bytes after the last round, so that the final read will
	// not pass beyond the bounds of the input buffer.
	size_t rounds = (*slen - 4) / 24;

	*slen -= rounds * 24;   // 24 bytes consumed per round
	*olen += rounds * 32;   // 32 bytes produced per round

	// Pre-decrement the number of rounds to get the number of rounds
	// *after* the first round, which is handled as a special case.
	rounds--;

	// Number of times to go through the 36x loop.
	size_t loops = rounds / 36;

	// Number of rounds remaining after the 36x loop.
	rounds %= 36;

	// Lookup tables.
	const __m256i lut0 = _mm256_set_epi8(
		10, 11,  9, 10,  7,  8,  6,  7,  4,  5,  3,  4,  1,  2,  0,  1,
		14, 15, 13, 14, 11, 12, 10, 11,  8,  9,  7,  8,  5,  6,  4,  5);

	const __m256i lut1 = _mm256_setr_epi8(
		65, 71, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 0, 0,
		65, 71, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 0, 0);

	// Temporary registers.
	__m256i a, b, c, d, e;

	// Temporary register f doubles as the shift mask for the first round.
	__m256i f = _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6);

	__asm__ volatile (

		// The first loop iteration requires special handling to ensure
		// that the read, which is normally done at an offset of -4,
		// does not underflow the buffer. Load the buffer at an offset
		// of 0 and permute the input to achieve the same effect.
		LOAD("a", 0, 0)
		"vpermd %[a], %[f], %[a] \n\t"

		// Perform the standard shuffling and translation steps.
		SHUF("a", "b", "c")
		TRAN("a", "b", "c")

		// Store the result and increment the source and dest pointers.
		"vmovdqu %[a], (%[dst]) \n\t"
		"add     $24,  %[src]   \n\t"
		"add     $32,  %[dst]   \n\t"

		// If there are 36 rounds or more, enter a 36x unrolled loop of
		// interleaved encoding rounds. The rounds interleave memory
		// operations (load/store) with data operations (table lookups,
		// etc) to maximize pipeline throughput.
		"    test %[loops], %[loops] \n\t"
		"    jz   18f                \n\t"
		"    jmp  36f                \n\t"
		"                            \n\t"
		".balign 64                  \n\t"
		"36: " ROUND_3_INIT()
		"    " ROUND_3_A( 0)
		"    " ROUND_3_B( 3)
		"    " ROUND_3_A( 6)
		"    " ROUND_3_B( 9)
		"    " ROUND_3_A(12)
		"    " ROUND_3_B(15)
		"    " ROUND_3_A(18)
		"    " ROUND_3_B(21)
		"    " ROUND_3_A(24)
		"    " ROUND_3_B(27)
		"    " ROUND_3_A_LAST(30)
		"    add $(24 * 36), %[src] \n\t"
		"    add $(32 * 36), %[dst] \n\t"
		"    dec %[loops]           \n\t"
		"    jnz 36b                \n\t"

		// Enter an 18x unrolled loop for rounds of 18 or more.
		"18: cmp $18, %[rounds] \n\t"
		"    jl  9f             \n\t"
		"    " ROUND_3_INIT()
		"    " ROUND_3_A(0)
		"    " ROUND_3_B(3)
		"    " ROUND_3_A(6)
		"    " ROUND_3_B(9)
		"    " ROUND_3_A_LAST(12)
		"    sub $18,        %[rounds] \n\t"
		"    add $(24 * 18), %[src]    \n\t"
		"    add $(32 * 18), %[dst]    \n\t"

		// Enter a 9x unrolled loop for rounds of 9 or more.
		"9:  cmp $9, %[rounds] \n\t"
		"    jl  6f            \n\t"
		"    " ROUND_3_INIT()
		"    " ROUND_3_A(0)
		"    " ROUND_3_B_LAST(3)
		"    sub $9,        %[rounds] \n\t"
		"    add $(24 * 9), %[src]    \n\t"
		"    add $(32 * 9), %[dst]    \n\t"

		// Enter a 6x unrolled loop for rounds of 6 or more.
		"6:  cmp $6, %[rounds] \n\t"
		"    jl  55f           \n\t"
		"    " ROUND_3_INIT()
		"    " ROUND_3_A_LAST(0)
		"    sub $6,        %[rounds] \n\t"
		"    add $(24 * 6), %[src]    \n\t"
		"    add $(32 * 6), %[dst]    \n\t"

		// Dispatch the remaining rounds 0..5.
		"55: cmp $3, %[rounds] \n\t"
		"    jg  45f           \n\t"
		"    je  3f            \n\t"
		"    cmp $1, %[rounds] \n\t"
		"    jg  2f            \n\t"
		"    je  1f            \n\t"
		"    jmp 0f            \n\t"

		"45: cmp $4, %[rounds] \n\t"
		"    je  4f            \n\t"

		// Block of non-interlaced encoding rounds, which can each
		// individually be jumped to. Rounds fall through to the next.
		"5: " ROUND()
		"4: " ROUND()
		"3: " ROUND()
		"2: " ROUND()
		"1: " ROUND()
		"0: \n\t"

		// Outputs (modified).
		: [rounds] "+r"  (rounds),
		  [loops]  "+r"  (loops),
		  [src]    "+r"  (*s),
		  [dst]    "+r"  (*o),
		  [a]      "=&x" (a),
		  [b]      "=&x" (b),
		  [c]      "=&x" (c),
		  [d]      "=&x" (d),
		  [e]      "=&x" (e),
		  [f]      "+x"  (f)

		// Inputs (not modified).
		: [lut0] "x" (lut0),
		  [lut1] "x" (lut1),
		  [msk0] "x" (_mm256_set1_epi32(0x0FC0FC00)),
		  [msk1] "x" (_mm256_set1_epi32(0x04000040)),
		  [msk2] "x" (_mm256_set1_epi32(0x003F03F0)),
		  [msk3] "x" (_mm256_set1_epi32(0x01000010)),
		  [n51]  "x" (_mm256_set1_epi8(51)),
		  [n25]  "x" (_mm256_set1_epi8(25))

		// Clobbers.
		: "cc", "memory"
	);
}

#pragma GCC diagnostic pop
//=============> avx2/enc_loop_asm.c  end <===

#else

//# include "enc_translate.c"
//# include "enc_reshuffle.c"
//# include "enc_loop.c"
//=============> avx2/enc_translate.c  begine <===
static inline __m256i
enc_translate (const __m256i in)
{
	// A lookup table containing the absolute offsets for all ranges:
	const __m256i lut = _mm256_setr_epi8(
		65, 71, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 0, 0,
		65, 71, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 0, 0);

	// Translate values 0..63 to the Base64 alphabet. There are five sets:
	// #  From      To         Abs    Index  Characters
	// 0  [0..25]   [65..90]   +65        0  ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// 1  [26..51]  [97..122]  +71        1  abcdefghijklmnopqrstuvwxyz
	// 2  [52..61]  [48..57]    -4  [2..11]  0123456789
	// 3  [62]      [43]       -19       12  +
	// 4  [63]      [47]       -16       13  /

	// Create LUT indices from the input. The index for range #0 is right,
	// others are 1 less than expected:
	__m256i indices = _mm256_subs_epu8(in, _mm256_set1_epi8(51));

	// mask is 0xFF (-1) for range #[1..4] and 0x00 for range #0:
	const __m256i mask = _mm256_cmpgt_epi8(in, _mm256_set1_epi8(25));

	// Subtract -1, so add 1 to indices for range #[1..4]. All indices are
	// now correct:
	indices = _mm256_sub_epi8(indices, mask);

	// Add offsets to input values:
	return _mm256_add_epi8(in, _mm256_shuffle_epi8(lut, indices));
}
//=============> avx2/enc_translate.c  end <===

//=============> avx2/enc_reshuffle.c  begine <===
static inline __m256i
enc_reshuffle (const __m256i input)
{
	// Translation of the SSSE3 reshuffling algorithm to AVX2. This one
	// works with shifted (4 bytes) input in order to be able to work
	// efficiently in the two 128-bit lanes.

	// Input, bytes MSB to LSB:
	// 0 0 0 0 x w v u t s r q p o n m
	// l k j i h g f e d c b a 0 0 0 0

	const __m256i in = _mm256_shuffle_epi8(input, _mm256_set_epi8(
		10, 11,  9, 10,
		 7,  8,  6,  7,
		 4,  5,  3,  4,
		 1,  2,  0,  1,

		14, 15, 13, 14,
		11, 12, 10, 11,
		 8,  9,  7,  8,
		 5,  6,  4,  5));
	// in, bytes MSB to LSB:
	// w x v w
	// t u s t
	// q r p q
	// n o m n
	// k l j k
	// h i g h
	// e f d e
	// b c a b

	const __m256i t0 = _mm256_and_si256(in, _mm256_set1_epi32(0x0FC0FC00));
	// bits, upper case are most significant bits, lower case are least
	// significant bits.
	// 0000wwww XX000000 VVVVVV00 00000000
	// 0000tttt UU000000 SSSSSS00 00000000
	// 0000qqqq RR000000 PPPPPP00 00000000
	// 0000nnnn OO000000 MMMMMM00 00000000
	// 0000kkkk LL000000 JJJJJJ00 00000000
	// 0000hhhh II000000 GGGGGG00 00000000
	// 0000eeee FF000000 DDDDDD00 00000000
	// 0000bbbb CC000000 AAAAAA00 00000000

	const __m256i t1 = _mm256_mulhi_epu16(t0, _mm256_set1_epi32(0x04000040));
	// 00000000 00wwwwXX 00000000 00VVVVVV
	// 00000000 00ttttUU 00000000 00SSSSSS
	// 00000000 00qqqqRR 00000000 00PPPPPP
	// 00000000 00nnnnOO 00000000 00MMMMMM
	// 00000000 00kkkkLL 00000000 00JJJJJJ
	// 00000000 00hhhhII 00000000 00GGGGGG
	// 00000000 00eeeeFF 00000000 00DDDDDD
	// 00000000 00bbbbCC 00000000 00AAAAAA

	const __m256i t2 = _mm256_and_si256(in, _mm256_set1_epi32(0x003F03F0));
	// 00000000 00xxxxxx 000000vv WWWW0000
	// 00000000 00uuuuuu 000000ss TTTT0000
	// 00000000 00rrrrrr 000000pp QQQQ0000
	// 00000000 00oooooo 000000mm NNNN0000
	// 00000000 00llllll 000000jj KKKK0000
	// 00000000 00iiiiii 000000gg HHHH0000
	// 00000000 00ffffff 000000dd EEEE0000
	// 00000000 00cccccc 000000aa BBBB0000

	const __m256i t3 = _mm256_mullo_epi16(t2, _mm256_set1_epi32(0x01000010));
	// 00xxxxxx 00000000 00vvWWWW 00000000
	// 00uuuuuu 00000000 00ssTTTT 00000000
	// 00rrrrrr 00000000 00ppQQQQ 00000000
	// 00oooooo 00000000 00mmNNNN 00000000
	// 00llllll 00000000 00jjKKKK 00000000
	// 00iiiiii 00000000 00ggHHHH 00000000
	// 00ffffff 00000000 00ddEEEE 00000000
	// 00cccccc 00000000 00aaBBBB 00000000

	return _mm256_or_si256(t1, t3);
	// 00xxxxxx 00wwwwXX 00vvWWWW 00VVVVVV
	// 00uuuuuu 00ttttUU 00ssTTTT 00SSSSSS
	// 00rrrrrr 00qqqqRR 00ppQQQQ 00PPPPPP
	// 00oooooo 00nnnnOO 00mmNNNN 00MMMMMM
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA
}
//=============> avx2/enc_reshuffle.c  end <===

//=============> avx2/enc_loop.c  begine <===
static inline void
enc_loop_avx2_inner_first (const uint8_t **s, uint8_t **o)
{
	// First load is done at s - 0 to not get a segfault:
	__m256i src = _mm256_loadu_si256((__m256i *) *s);

	// Shift by 4 bytes, as required by enc_reshuffle:
	src = _mm256_permutevar8x32_epi32(src, _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6));

	// Reshuffle, translate, store:
	src = enc_reshuffle(src);
	src = enc_translate(src);
	_mm256_storeu_si256((__m256i *) *o, src);

	// Subsequent loads will be done at s - 4, set pointer for next round:
	*s += 20;
	*o += 32;
}

static inline void
enc_loop_avx2_inner (const uint8_t **s, uint8_t **o)
{
	// Load input:
	__m256i src = _mm256_loadu_si256((__m256i *) *s);

	// Reshuffle, translate, store:
	src = enc_reshuffle(src);
	src = enc_translate(src);
	_mm256_storeu_si256((__m256i *) *o, src);

	*s += 24;
	*o += 32;
}

static inline void
enc_loop_avx2 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 32) {
		return;
	}

	// Process blocks of 24 bytes at a time. Because blocks are loaded 32
	// bytes at a time an offset of -4, ensure that there will be at least
	// 4 remaining bytes after the last round, so that the final read will
	// not pass beyond the bounds of the input buffer:
	size_t rounds = (*slen - 4) / 24;

	*slen -= rounds * 24;   // 24 bytes consumed per round
	*olen += rounds * 32;   // 32 bytes produced per round

	// The first loop iteration requires special handling to ensure that
	// the read, which is done at an offset, does not underflow the buffer:
	enc_loop_avx2_inner_first(s, o);
	rounds--;

	while (rounds > 0) {
		if (rounds >= 8) {
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_avx2_inner(s, o);
			enc_loop_avx2_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_avx2_inner(s, o);
		break;
	}

	// Add the offset back:
	*s += 4;
}
//=============> avx2/enc_loop.c  end <===

#endif

#endif	// HAVE_AVX2

BASE64_ENC_FUNCTION(avx2)
{
#if HAVE_AVX2
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
	enc_loop_avx2(&s, &slen, &o, &olen);
	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

BASE64_DEC_FUNCTION(avx2)
{
#if HAVE_AVX2
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_avx2(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/avx2/codec.c  end <=========================




////=============> arch/generic/codec.c  begine <=========================
//#include <stdint.h>
//#include <stddef.h>
#include <string.h>

//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#if BASE64_WORDSIZE == 32

//#  include "32/enc_loop.c"
//=============> generic/32/enc_loop.c  begine <===
static inline void
enc_loop_generic_32_inner (const uint8_t **s, uint8_t **o)
{
	uint32_t src;

	// Load input:
	memcpy(&src, *s, sizeof (src));

	// Reorder to 32-bit big-endian, if not already in that format. The
	// workset must be in big-endian, otherwise the shifted bits do not
	// carry over properly among adjacent bytes:
	src = BASE64_HTOBE32(src);

	// Two indices for the 12-bit lookup table:
	const size_t index0 = (src >> 20) & 0xFFFU;
	const size_t index1 = (src >>  8) & 0xFFFU;

	// Table lookup and store:
	memcpy(*o + 0, base64_table_enc_12bit + index0, 2);
	memcpy(*o + 2, base64_table_enc_12bit + index1, 2);

	*s += 3;
	*o += 4;
}

static inline void
enc_loop_generic_32 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 4) {
		return;
	}

	// Process blocks of 3 bytes at a time. Because blocks are loaded 4
	// bytes at a time, ensure that there will be at least one remaining
	// byte after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 1) / 3;

	*slen -= rounds * 3;	// 3 bytes consumed per round
	*olen += rounds * 4;	// 4 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_generic_32_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> generic/32/enc_loop.c  end <===

#elif BASE64_WORDSIZE == 64
//#  include "64/enc_loop.c"
//=============> generic/64/enc_loop.c  end <===
static inline void
enc_loop_generic_64_inner (const uint8_t **s, uint8_t **o)
{
	uint64_t src;

	// Load input:
	memcpy(&src, *s, sizeof (src));

	// Reorder to 64-bit big-endian, if not already in that format. The
	// workset must be in big-endian, otherwise the shifted bits do not
	// carry over properly among adjacent bytes:
	src = BASE64_HTOBE64(src);

	// Four indices for the 12-bit lookup table:
	const size_t index0 = (src >> 52) & 0xFFFU;
	const size_t index1 = (src >> 40) & 0xFFFU;
	const size_t index2 = (src >> 28) & 0xFFFU;
	const size_t index3 = (src >> 16) & 0xFFFU;

	// Table lookup and store:
	memcpy(*o + 0, base64_table_enc_12bit + index0, 2);
	memcpy(*o + 2, base64_table_enc_12bit + index1, 2);
	memcpy(*o + 4, base64_table_enc_12bit + index2, 2);
	memcpy(*o + 6, base64_table_enc_12bit + index3, 2);

	*s += 6;
	*o += 8;
}

static inline void
enc_loop_generic_64 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 8) {
		return;
	}

	// Process blocks of 6 bytes at a time. Because blocks are loaded 8
	// bytes at a time, ensure that there will be at least 2 remaining
	// bytes after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 2) / 6;

	*slen -= rounds * 6;	// 6 bytes consumed per round
	*olen += rounds * 8;	// 8 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_generic_64_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> generic/64/enc_loop.c  end <===
#endif

#if BASE64_WORDSIZE >= 32
//#  include "32/dec_loop.c"
//=============> generic/32/dec_loop.c  begine <===
static inline int
dec_loop_generic_32_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const uint32_t str
		= base64_table_dec_32bit_d0[(*s)[0]]
		| base64_table_dec_32bit_d1[(*s)[1]]
		| base64_table_dec_32bit_d2[(*s)[2]]
		| base64_table_dec_32bit_d3[(*s)[3]];

#if BASE64_LITTLE_ENDIAN

	// LUTs for little-endian set MSB in case of invalid character:
	if (str & UINT32_C(0x80000000)) {
		return 0;
	}
#else
	// LUTs for big-endian set LSB in case of invalid character:
	if (str & UINT32_C(1)) {
		return 0;
	}
#endif
	// Store the output:
	memcpy(*o, &str, sizeof (str));

	*s += 4;
	*o += 3;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_generic_32 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 8) {
		return;
	}

	// Process blocks of 4 bytes per round. Because one extra zero byte is
	// written after the output, ensure that there will be at least 4 bytes
	// of input data left to cover the gap. (Two data bytes and up to two
	// end-of-string markers.)
	size_t rounds = (*slen - 4) / 4;

	*slen -= rounds * 4;	// 4 bytes consumed per round
	*olen += rounds * 3;	// 3 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_generic_32_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 4;
	*olen -= rounds * 3;
}
//=============> generic/32/dec_loop.c  end <===
#endif

BASE64_ENC_FUNCTION(plain)
{
	//#include "enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
#if BASE64_WORDSIZE == 32
	enc_loop_generic_32(&s, &slen, &o, &olen);
#elif BASE64_WORDSIZE == 64
	enc_loop_generic_64(&s, &slen, &o, &olen);
#endif
	//#include "enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
}

BASE64_DEC_FUNCTION(plain)
{
	//#include "dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
#if BASE64_WORDSIZE >= 32
	dec_loop_generic_32(&s, &slen, &o, &olen);
#endif
	//#include "dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
}

////=============> arch/generic/codec.c  end <=========================

////=============> arch/neon32/codec.c   begine <=========================
//#include <stdint.h>
//#include <stddef.h>
//#include <string.h>
//
//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#ifdef __arm__
#  if (defined(__ARM_NEON__) || defined(__ARM_NEON)) && HAVE_NEON32
#    define BASE64_USE_NEON32
#  endif
#endif

#ifdef BASE64_USE_NEON32
#include <arm_neon.h>

// Only enable inline assembly on supported compilers.
#if defined(__GNUC__) || defined(__clang__)
#define BASE64_NEON32_USE_ASM
#endif

static inline uint8x16_t
vqtbl1q_u8 (const uint8x16_t lut, const uint8x16_t indices)
{
	// NEON32 only supports 64-bit wide lookups in 128-bit tables. Emulate
	// the NEON64 `vqtbl1q_u8` intrinsic to do 128-bit wide lookups.
	uint8x8x2_t lut2;
	uint8x8x2_t result;

	lut2.val[0] = vget_low_u8(lut);
	lut2.val[1] = vget_high_u8(lut);

	result.val[0] = vtbl2_u8(lut2, vget_low_u8(indices));
	result.val[1] = vtbl2_u8(lut2, vget_high_u8(indices));

	return vcombine_u8(result.val[0], result.val[1]);
}

//#include "../generic/32/dec_loop.c"
//#include "../generic/32/enc_loop.c"
//#include "dec_loop.c"
//#include "enc_reshuffle.c"
//#include "enc_translate.c"
//#include "enc_loop.c"

//=============> generic/32/dec_loop.c  begine <===
static inline int
dec_loop_generic_32_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const uint32_t str
		= base64_table_dec_32bit_d0[(*s)[0]]
		| base64_table_dec_32bit_d1[(*s)[1]]
		| base64_table_dec_32bit_d2[(*s)[2]]
		| base64_table_dec_32bit_d3[(*s)[3]];

#if BASE64_LITTLE_ENDIAN

	// LUTs for little-endian set MSB in case of invalid character:
	if (str & UINT32_C(0x80000000)) {
		return 0;
	}
#else
	// LUTs for big-endian set LSB in case of invalid character:
	if (str & UINT32_C(1)) {
		return 0;
	}
#endif
	// Store the output:
	memcpy(*o, &str, sizeof (str));

	*s += 4;
	*o += 3;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_generic_32 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 8) {
		return;
	}

	// Process blocks of 4 bytes per round. Because one extra zero byte is
	// written after the output, ensure that there will be at least 4 bytes
	// of input data left to cover the gap. (Two data bytes and up to two
	// end-of-string markers.)
	size_t rounds = (*slen - 4) / 4;

	*slen -= rounds * 4;	// 4 bytes consumed per round
	*olen += rounds * 3;	// 3 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_generic_32_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 4;
	*olen -= rounds * 3;
}
//=============> generic/32/dec_loop.c  end <===

//=============> generic/32/enc_loop.c  begine <===
static inline void
enc_loop_generic_32_inner (const uint8_t **s, uint8_t **o)
{
	uint32_t src;

	// Load input:
	memcpy(&src, *s, sizeof (src));

	// Reorder to 32-bit big-endian, if not already in that format. The
	// workset must be in big-endian, otherwise the shifted bits do not
	// carry over properly among adjacent bytes:
	src = BASE64_HTOBE32(src);

	// Two indices for the 12-bit lookup table:
	const size_t index0 = (src >> 20) & 0xFFFU;
	const size_t index1 = (src >>  8) & 0xFFFU;

	// Table lookup and store:
	memcpy(*o + 0, base64_table_enc_12bit + index0, 2);
	memcpy(*o + 2, base64_table_enc_12bit + index1, 2);

	*s += 3;
	*o += 4;
}

static inline void
enc_loop_generic_32 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 4) {
		return;
	}

	// Process blocks of 3 bytes at a time. Because blocks are loaded 4
	// bytes at a time, ensure that there will be at least one remaining
	// byte after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 1) / 3;

	*slen -= rounds * 3;	// 3 bytes consumed per round
	*olen += rounds * 4;	// 4 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_generic_32_inner(s, o);
			enc_loop_generic_32_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_generic_32_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> generic/32/enc_loop.c  end <===

//=============> neon32/dec_loop.c  begine <===
static inline int
is_nonzero (const uint8x16_t v)
{
	uint64_t u64;
	const uint64x2_t v64 = vreinterpretq_u64_u8(v);
	const uint32x2_t v32 = vqmovn_u64(v64);

	vst1_u64(&u64, vreinterpret_u64_u32(v32));
	return u64 != 0;
}

static inline uint8x16_t
delta_lookup (const uint8x16_t v)
{
	const uint8x8_t lut = {
		0, 16, 19, 4, (uint8_t) -65, (uint8_t) -65, (uint8_t) -71, (uint8_t) -71,
	};

	return vcombine_u8(
		vtbl1_u8(lut, vget_low_u8(v)),
		vtbl1_u8(lut, vget_high_u8(v)));
}

static inline uint8x16_t
dec_loop_neon32_lane (uint8x16_t *lane)
{
	// See the SSSE3 decoder for an explanation of the algorithm.
	const uint8x16_t lut_lo = {
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A
	};

	const uint8x16_t lut_hi = {
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10
	};

	const uint8x16_t mask_0F = vdupq_n_u8(0x0F);
	const uint8x16_t mask_2F = vdupq_n_u8(0x2F);

	const uint8x16_t hi_nibbles = vshrq_n_u8(*lane, 4);
	const uint8x16_t lo_nibbles = vandq_u8(*lane, mask_0F);
	const uint8x16_t eq_2F      = vceqq_u8(*lane, mask_2F);

	const uint8x16_t hi = vqtbl1q_u8(lut_hi, hi_nibbles);
	const uint8x16_t lo = vqtbl1q_u8(lut_lo, lo_nibbles);

	// Now simply add the delta values to the input:
	*lane = vaddq_u8(*lane, delta_lookup(vaddq_u8(eq_2F, hi_nibbles)));

	// Return the validity mask:
	return vandq_u8(lo, hi);
}

static inline void
dec_loop_neon32 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 64) {
		return;
	}

	// Process blocks of 64 bytes per round. Unlike the SSE codecs, no
	// extra trailing zero bytes are written, so it is not necessary to
	// reserve extra input bytes:
	size_t rounds = *slen / 64;

	*slen -= rounds * 64;	// 64 bytes consumed per round
	*olen += rounds * 48;	// 48 bytes produced per round

	do {
		uint8x16x3_t dec;

		// Load 64 bytes and deinterleave:
		uint8x16x4_t str = vld4q_u8(*s);

		// Decode each lane, collect a mask of invalid inputs:
		const uint8x16_t classified
			= dec_loop_neon32_lane(&str.val[0])
			| dec_loop_neon32_lane(&str.val[1])
			| dec_loop_neon32_lane(&str.val[2])
			| dec_loop_neon32_lane(&str.val[3]);

		// Check for invalid input: if any of the delta values are
		// zero, fall back on bytewise code to do error checking and
		// reporting:
		if (is_nonzero(classified)) {
			break;
		}

		// Compress four bytes into three:
		dec.val[0] = vorrq_u8(vshlq_n_u8(str.val[0], 2), vshrq_n_u8(str.val[1], 4));
		dec.val[1] = vorrq_u8(vshlq_n_u8(str.val[1], 4), vshrq_n_u8(str.val[2], 2));
		dec.val[2] = vorrq_u8(vshlq_n_u8(str.val[2], 6), str.val[3]);

		// Interleave and store decoded result:
		vst3q_u8(*o, dec);

		*s += 64;
		*o += 48;

	} while (--rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 64;
	*olen -= rounds * 48;
}
//=============> neon32/dec_loop.c  end <===

//=============> neon32/enc_reshuffle.c  begine <===
static inline uint8x16x4_t
enc_reshuffle (uint8x16x3_t in)
{
	uint8x16x4_t out;

	// Input:
	// in[0]  = a7 a6 a5 a4 a3 a2 a1 a0
	// in[1]  = b7 b6 b5 b4 b3 b2 b1 b0
	// in[2]  = c7 c6 c5 c4 c3 c2 c1 c0

	// Output:
	// out[0] = 00 00 a7 a6 a5 a4 a3 a2
	// out[1] = 00 00 a1 a0 b7 b6 b5 b4
	// out[2] = 00 00 b3 b2 b1 b0 c7 c6
	// out[3] = 00 00 c5 c4 c3 c2 c1 c0

	// Move the input bits to where they need to be in the outputs. Except
	// for the first output, the high two bits are not cleared.
	out.val[0] = vshrq_n_u8(in.val[0], 2);
	out.val[1] = vshrq_n_u8(in.val[1], 4);
	out.val[2] = vshrq_n_u8(in.val[2], 6);
	out.val[1] = vsliq_n_u8(out.val[1], in.val[0], 4);
	out.val[2] = vsliq_n_u8(out.val[2], in.val[1], 2);

	// Clear the high two bits in the second, third and fourth output.
	out.val[1] = vandq_u8(out.val[1], vdupq_n_u8(0x3F));
	out.val[2] = vandq_u8(out.val[2], vdupq_n_u8(0x3F));
	out.val[3] = vandq_u8(in.val[2],  vdupq_n_u8(0x3F));

	return out;
}
//=============> neon32/enc_reshuffle.c  end <===

//=============> neon32/enc_translate.c  begine <===
static inline uint8x16x4_t
enc_translate (const uint8x16x4_t in)
{
	// A lookup table containing the absolute offsets for all ranges:
	const uint8x16_t lut = {
		 65U,  71U, 252U, 252U,
		252U, 252U, 252U, 252U,
		252U, 252U, 252U, 252U,
		237U, 240U,   0U,   0U
	};

	const uint8x16_t offset = vdupq_n_u8(51);

	uint8x16x4_t indices, mask, delta, out;

	// Translate values 0..63 to the Base64 alphabet. There are five sets:
	// #  From      To         Abs    Index  Characters
	// 0  [0..25]   [65..90]   +65        0  ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// 1  [26..51]  [97..122]  +71        1  abcdefghijklmnopqrstuvwxyz
	// 2  [52..61]  [48..57]    -4  [2..11]  0123456789
	// 3  [62]      [43]       -19       12  +
	// 4  [63]      [47]       -16       13  /

	// Create LUT indices from input:
	// the index for range #0 is right, others are 1 less than expected:
	indices.val[0] = vqsubq_u8(in.val[0], offset);
	indices.val[1] = vqsubq_u8(in.val[1], offset);
	indices.val[2] = vqsubq_u8(in.val[2], offset);
	indices.val[3] = vqsubq_u8(in.val[3], offset);

	// mask is 0xFF (-1) for range #[1..4] and 0x00 for range #0:
	mask.val[0] = vcgtq_u8(in.val[0], vdupq_n_u8(25));
	mask.val[1] = vcgtq_u8(in.val[1], vdupq_n_u8(25));
	mask.val[2] = vcgtq_u8(in.val[2], vdupq_n_u8(25));
	mask.val[3] = vcgtq_u8(in.val[3], vdupq_n_u8(25));

	// Subtract -1, so add 1 to indices for range #[1..4], All indices are
	// now correct:
	indices.val[0] = vsubq_u8(indices.val[0], mask.val[0]);
	indices.val[1] = vsubq_u8(indices.val[1], mask.val[1]);
	indices.val[2] = vsubq_u8(indices.val[2], mask.val[2]);
	indices.val[3] = vsubq_u8(indices.val[3], mask.val[3]);

	// Lookup delta values:
	delta.val[0] = vqtbl1q_u8(lut, indices.val[0]);
	delta.val[1] = vqtbl1q_u8(lut, indices.val[1]);
	delta.val[2] = vqtbl1q_u8(lut, indices.val[2]);
	delta.val[3] = vqtbl1q_u8(lut, indices.val[3]);

	// Add delta values:
	out.val[0] = vaddq_u8(in.val[0], delta.val[0]);
	out.val[1] = vaddq_u8(in.val[1], delta.val[1]);
	out.val[2] = vaddq_u8(in.val[2], delta.val[2]);
	out.val[3] = vaddq_u8(in.val[3], delta.val[3]);

	return out;
}
//=============> neon32/enc_translate.c  end <===

//=============> neon32/enc_loop.c  begine <===
#ifdef BASE64_NEON32_USE_ASM
static inline void
enc_loop_neon32_inner_asm (const uint8_t **s, uint8_t **o)
{
	// This function duplicates the functionality of enc_loop_neon32_inner,
	// but entirely with inline assembly. This gives a significant speedup
	// over using NEON intrinsics, which do not always generate very good
	// code. The logic of the assembly is directly lifted from the
	// intrinsics version, so it can be used as a guide to this code.

	// Temporary registers, used as scratch space.
	uint8x16_t tmp0, tmp1, tmp2, tmp3;
	uint8x16_t mask0, mask1, mask2, mask3;

	// A lookup table containing the absolute offsets for all ranges.
	const uint8x16_t lut = {
		  65U,  71U, 252U, 252U,
		 252U, 252U, 252U, 252U,
		 252U, 252U, 252U, 252U,
		 237U, 240U,   0U,   0U
	};

	// Numeric constants.
	const uint8x16_t n51 = vdupq_n_u8(51);
	const uint8x16_t n25 = vdupq_n_u8(25);
	const uint8x16_t n63 = vdupq_n_u8(63);

	__asm__ (

		// Load 48 bytes and deinterleave. The bytes are loaded to
		// hard-coded registers q12, q13 and q14, to ensure that they
		// are contiguous. Increment the source pointer.
		"vld3.8 {d24, d26, d28}, [%[src]]! \n\t"
		"vld3.8 {d25, d27, d29}, [%[src]]! \n\t"

		// Reshuffle the bytes using temporaries.
		"vshr.u8 %q[t0], q12,    #2      \n\t"
		"vshr.u8 %q[t1], q13,    #4      \n\t"
		"vshr.u8 %q[t2], q14,    #6      \n\t"
		"vsli.8  %q[t1], q12,    #4      \n\t"
		"vsli.8  %q[t2], q13,    #2      \n\t"
		"vand.u8 %q[t1], %q[t1], %q[n63] \n\t"
		"vand.u8 %q[t2], %q[t2], %q[n63] \n\t"
		"vand.u8 %q[t3], q14,    %q[n63] \n\t"

		// t0..t3 are the reshuffled inputs. Create LUT indices.
		"vqsub.u8 q12, %q[t0], %q[n51] \n\t"
		"vqsub.u8 q13, %q[t1], %q[n51] \n\t"
		"vqsub.u8 q14, %q[t2], %q[n51] \n\t"
		"vqsub.u8 q15, %q[t3], %q[n51] \n\t"

		// Create the mask for range #0.
		"vcgt.u8 %q[m0], %q[t0], %q[n25] \n\t"
		"vcgt.u8 %q[m1], %q[t1], %q[n25] \n\t"
		"vcgt.u8 %q[m2], %q[t2], %q[n25] \n\t"
		"vcgt.u8 %q[m3], %q[t3], %q[n25] \n\t"

		// Subtract -1 to correct the LUT indices.
		"vsub.u8 q12, %q[m0] \n\t"
		"vsub.u8 q13, %q[m1] \n\t"
		"vsub.u8 q14, %q[m2] \n\t"
		"vsub.u8 q15, %q[m3] \n\t"

		// Lookup the delta values.
		"vtbl.u8 d24, {%q[lut]}, d24 \n\t"
		"vtbl.u8 d25, {%q[lut]}, d25 \n\t"
		"vtbl.u8 d26, {%q[lut]}, d26 \n\t"
		"vtbl.u8 d27, {%q[lut]}, d27 \n\t"
		"vtbl.u8 d28, {%q[lut]}, d28 \n\t"
		"vtbl.u8 d29, {%q[lut]}, d29 \n\t"
		"vtbl.u8 d30, {%q[lut]}, d30 \n\t"
		"vtbl.u8 d31, {%q[lut]}, d31 \n\t"

		// Add the delta values.
		"vadd.u8 q12, %q[t0] \n\t"
		"vadd.u8 q13, %q[t1] \n\t"
		"vadd.u8 q14, %q[t2] \n\t"
		"vadd.u8 q15, %q[t3] \n\t"

		// Store 64 bytes and interleave. Increment the dest pointer.
		"vst4.8 {d24, d26, d28, d30}, [%[dst]]! \n\t"
		"vst4.8 {d25, d27, d29, d31}, [%[dst]]! \n\t"

		// Outputs (modified).
		: [src] "+r"  (*s),
		  [dst] "+r"  (*o),
		  [t0]  "=&w" (tmp0),
		  [t1]  "=&w" (tmp1),
		  [t2]  "=&w" (tmp2),
		  [t3]  "=&w" (tmp3),
		  [m0]  "=&w" (mask0),
		  [m1]  "=&w" (mask1),
		  [m2]  "=&w" (mask2),
		  [m3]  "=&w" (mask3)

		// Inputs (not modified).
		: [lut] "w" (lut),
		  [n25] "w" (n25),
		  [n51] "w" (n51),
		  [n63] "w" (n63)

		// Clobbers.
		: "d24", "d25", "d26", "d27", "d28", "d29", "d30", "d31",
		  "cc", "memory"
	);
}
#endif

static inline void
enc_loop_neon32_inner (const uint8_t **s, uint8_t **o)
{
#ifdef BASE64_NEON32_USE_ASM
	enc_loop_neon32_inner_asm(s, o);
#else
	// Load 48 bytes and deinterleave:
	uint8x16x3_t src = vld3q_u8(*s);

	// Reshuffle:
	uint8x16x4_t out = enc_reshuffle(src);

	// Translate reshuffled bytes to the Base64 alphabet:
	out = enc_translate(out);

	// Interleave and store output:
	vst4q_u8(*o, out);

	*s += 48;
	*o += 64;
#endif
}

static inline void
enc_loop_neon32 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	size_t rounds = *slen / 48;

	*slen -= rounds * 48;	// 48 bytes consumed per round
	*olen += rounds * 64;	// 64 bytes produced per round

	while (rounds > 0) {
		if (rounds >= 8) {
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_neon32_inner(s, o);
			enc_loop_neon32_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_neon32_inner(s, o);
		break;
	}
}
//=============> neon32/enc_loop.c  end <===

#endif	// BASE64_USE_NEON32

// Stride size is so large on these NEON 32-bit functions
// (48 bytes encode, 32 bytes decode) that we inline the
// uint32 codec to stay performant on smaller inputs.

BASE64_ENC_FUNCTION(neon32)
{
#ifdef BASE64_USE_NEON32
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
	enc_loop_neon32(&s, &slen, &o, &olen);
	enc_loop_generic_32(&s, &slen, &o, &olen);
	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

BASE64_DEC_FUNCTION(neon32)
{
#ifdef BASE64_USE_NEON32
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_neon32(&s, &slen, &o, &olen);
	dec_loop_generic_32(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/neon32/codec.c   end <=========================




////=============> arch/neon64/codec.c   begine <=========================
//#include <stdint.h>
//#include <stddef.h>
//#include <string.h>
//
//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#ifdef __aarch64__
#  if (defined(__ARM_NEON__) || defined(__ARM_NEON)) && HAVE_NEON64
#    define BASE64_USE_NEON64
#  endif
#endif

#ifdef BASE64_USE_NEON64
#include <arm_neon.h>

// Only enable inline assembly on supported compilers.
#if defined(__GNUC__) || defined(__clang__)
#define BASE64_NEON64_USE_ASM
#endif

static inline uint8x16x4_t
load_64byte_table (const uint8_t *p)
{
#ifdef BASE64_NEON64_USE_ASM

	// Force the table to be loaded into contiguous registers. GCC will not
	// normally allocate contiguous registers for a `uint8x16x4_t'. These
	// registers are chosen to not conflict with the ones in the enc loop.
	register uint8x16_t t0 __asm__ ("v8");
	register uint8x16_t t1 __asm__ ("v9");
	register uint8x16_t t2 __asm__ ("v10");
	register uint8x16_t t3 __asm__ ("v11");

	__asm__ (
		"ld1 {%[t0].16b, %[t1].16b, %[t2].16b, %[t3].16b}, [%[src]], #64 \n\t"
		: [src] "+r" (p),
		  [t0]  "=w" (t0),
		  [t1]  "=w" (t1),
		  [t2]  "=w" (t2),
		  [t3]  "=w" (t3)
	);

	return (uint8x16x4_t) {
		.val[0] = t0,
		.val[1] = t1,
		.val[2] = t2,
		.val[3] = t3,
	};
#else
	return vld1q_u8_x4(p);
#endif
}

//#include "../generic/32/dec_loop.c"
//#include "../generic/64/enc_loop.c"
//#include "dec_loop.c"

//=============> generic/32/dec_loop.c  begine <===
static inline int
dec_loop_generic_32_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const uint32_t str
		= base64_table_dec_32bit_d0[(*s)[0]]
		| base64_table_dec_32bit_d1[(*s)[1]]
		| base64_table_dec_32bit_d2[(*s)[2]]
		| base64_table_dec_32bit_d3[(*s)[3]];

#if BASE64_LITTLE_ENDIAN

	// LUTs for little-endian set MSB in case of invalid character:
	if (str & UINT32_C(0x80000000)) {
		return 0;
	}
#else
	// LUTs for big-endian set LSB in case of invalid character:
	if (str & UINT32_C(1)) {
		return 0;
	}
#endif
	// Store the output:
	memcpy(*o, &str, sizeof (str));

	*s += 4;
	*o += 3;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_generic_32 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 8) {
		return;
	}

	// Process blocks of 4 bytes per round. Because one extra zero byte is
	// written after the output, ensure that there will be at least 4 bytes
	// of input data left to cover the gap. (Two data bytes and up to two
	// end-of-string markers.)
	size_t rounds = (*slen - 4) / 4;

	*slen -= rounds * 4;	// 4 bytes consumed per round
	*olen += rounds * 3;	// 3 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_generic_32_inner(s, o, &rounds) &&
			    dec_loop_generic_32_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_generic_32_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 4;
	*olen -= rounds * 3;
}
//=============> generic/32/dec_loop.c  end <===

//=============> generic/64/enc_loop.c  begine <===
static inline void
enc_loop_generic_64_inner (const uint8_t **s, uint8_t **o)
{
	uint64_t src;

	// Load input:
	memcpy(&src, *s, sizeof (src));

	// Reorder to 64-bit big-endian, if not already in that format. The
	// workset must be in big-endian, otherwise the shifted bits do not
	// carry over properly among adjacent bytes:
	src = BASE64_HTOBE64(src);

	// Four indices for the 12-bit lookup table:
	const size_t index0 = (src >> 52) & 0xFFFU;
	const size_t index1 = (src >> 40) & 0xFFFU;
	const size_t index2 = (src >> 28) & 0xFFFU;
	const size_t index3 = (src >> 16) & 0xFFFU;

	// Table lookup and store:
	memcpy(*o + 0, base64_table_enc_12bit + index0, 2);
	memcpy(*o + 2, base64_table_enc_12bit + index1, 2);
	memcpy(*o + 4, base64_table_enc_12bit + index2, 2);
	memcpy(*o + 6, base64_table_enc_12bit + index3, 2);

	*s += 6;
	*o += 8;
}

static inline void
enc_loop_generic_64 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 8) {
		return;
	}

	// Process blocks of 6 bytes at a time. Because blocks are loaded 8
	// bytes at a time, ensure that there will be at least 2 remaining
	// bytes after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 2) / 6;

	*slen -= rounds * 6;	// 6 bytes consumed per round
	*olen += rounds * 8;	// 8 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_generic_64_inner(s, o);
			enc_loop_generic_64_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_generic_64_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> generic/64/enc_loop.c  end <===

//=============> neon64/dec_loop.c  begine <===
// The input consists of five valid character sets in the Base64 alphabet,
// which we need to map back to the 6-bit values they represent.
// There are three ranges, two singles, and then there's the rest.
//
//   #  From       To        LUT  Characters
//   1  [0..42]    [255]      #1  invalid input
//   2  [43]       [62]       #1  +
//   3  [44..46]   [255]      #1  invalid input
//   4  [47]       [63]       #1  /
//   5  [48..57]   [52..61]   #1  0..9
//   6  [58..63]   [255]      #1  invalid input
//   7  [64]       [255]      #2  invalid input
//   8  [65..90]   [0..25]    #2  A..Z
//   9  [91..96]   [255]      #2  invalid input
//  10  [97..122]  [26..51]   #2  a..z
//  11  [123..126] [255]      #2  invalid input
// (12) Everything else => invalid input

// The first LUT will use the VTBL instruction (out of range indices are set to
// 0 in destination).
static const uint8_t dec_lut1[] = {
	255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U,
	255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U,
	255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U,  62U, 255U, 255U, 255U,  63U,
	 52U,  53U,  54U,  55U,  56U,  57U,  58U,  59U,  60U,  61U, 255U, 255U, 255U, 255U, 255U, 255U,
};

// The second LUT will use the VTBX instruction (out of range indices will be
// unchanged in destination). Input [64..126] will be mapped to index [1..63]
// in this LUT. Index 0 means that value comes from LUT #1.
static const uint8_t dec_lut2[] = {
	  0U, 255U,   0U,   1U,   2U,   3U,   4U,   5U,   6U,   7U,   8U,   9U,  10U,  11U,  12U,  13U,
	 14U,  15U,  16U,  17U,  18U,  19U,  20U,  21U,  22U,  23U,  24U,  25U, 255U, 255U, 255U, 255U,
	255U, 255U,  26U,  27U,  28U,  29U,  30U,  31U,  32U,  33U,  34U,  35U,  36U,  37U,  38U,  39U,
	 40U,  41U,  42U,  43U,  44U,  45U,  46U,  47U,  48U,  49U,  50U,  51U, 255U, 255U, 255U, 255U,
};

// All input values in range for the first look-up will be 0U in the second
// look-up result. All input values out of range for the first look-up will be
// 0U in the first look-up result. Thus, the two results can be ORed without
// conflicts.
//
// Invalid characters that are in the valid range for either look-up will be
// set to 255U in the combined result. Other invalid characters will just be
// passed through with the second look-up result (using the VTBX instruction).
// Since the second LUT is 64 bytes, those passed-through values are guaranteed
// to have a value greater than 63U. Therefore, valid characters will be mapped
// to the valid [0..63] range and all invalid characters will be mapped to
// values greater than 63.

static inline void
dec_loop_neon64 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 64) {
		return;
	}

	// Process blocks of 64 bytes per round. Unlike the SSE codecs, no
	// extra trailing zero bytes are written, so it is not necessary to
	// reserve extra input bytes:
	size_t rounds = *slen / 64;

	*slen -= rounds * 64;	// 64 bytes consumed per round
	*olen += rounds * 48;	// 48 bytes produced per round

	const uint8x16x4_t tbl_dec1 = load_64byte_table(dec_lut1);
	const uint8x16x4_t tbl_dec2 = load_64byte_table(dec_lut2);

	do {
		const uint8x16_t offset = vdupq_n_u8(63U);
		uint8x16x4_t dec1, dec2;
		uint8x16x3_t dec;

		// Load 64 bytes and deinterleave:
		uint8x16x4_t str = vld4q_u8((uint8_t *) *s);

		// Get indices for second LUT:
		dec2.val[0] = vqsubq_u8(str.val[0], offset);
		dec2.val[1] = vqsubq_u8(str.val[1], offset);
		dec2.val[2] = vqsubq_u8(str.val[2], offset);
		dec2.val[3] = vqsubq_u8(str.val[3], offset);

		// Get values from first LUT:
		dec1.val[0] = vqtbl4q_u8(tbl_dec1, str.val[0]);
		dec1.val[1] = vqtbl4q_u8(tbl_dec1, str.val[1]);
		dec1.val[2] = vqtbl4q_u8(tbl_dec1, str.val[2]);
		dec1.val[3] = vqtbl4q_u8(tbl_dec1, str.val[3]);

		// Get values from second LUT:
		dec2.val[0] = vqtbx4q_u8(dec2.val[0], tbl_dec2, dec2.val[0]);
		dec2.val[1] = vqtbx4q_u8(dec2.val[1], tbl_dec2, dec2.val[1]);
		dec2.val[2] = vqtbx4q_u8(dec2.val[2], tbl_dec2, dec2.val[2]);
		dec2.val[3] = vqtbx4q_u8(dec2.val[3], tbl_dec2, dec2.val[3]);

		// Get final values:
		str.val[0] = vorrq_u8(dec1.val[0], dec2.val[0]);
		str.val[1] = vorrq_u8(dec1.val[1], dec2.val[1]);
		str.val[2] = vorrq_u8(dec1.val[2], dec2.val[2]);
		str.val[3] = vorrq_u8(dec1.val[3], dec2.val[3]);

		// Check for invalid input, any value larger than 63:
		const uint8x16_t classified
			= vcgtq_u8(str.val[0], vdupq_n_u8(63))
			| vcgtq_u8(str.val[1], vdupq_n_u8(63))
			| vcgtq_u8(str.val[2], vdupq_n_u8(63))
			| vcgtq_u8(str.val[3], vdupq_n_u8(63));

		// Check that all bits are zero:
		if (vmaxvq_u8(classified) != 0U) {
			break;
		}

		// Compress four bytes into three:
		dec.val[0] = vshlq_n_u8(str.val[0], 2) | vshrq_n_u8(str.val[1], 4);
		dec.val[1] = vshlq_n_u8(str.val[1], 4) | vshrq_n_u8(str.val[2], 2);
		dec.val[2] = vshlq_n_u8(str.val[2], 6) | str.val[3];

		// Interleave and store decoded result:
		vst3q_u8((uint8_t *) *o, dec);

		*s += 64;
		*o += 48;

	} while (--rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 64;
	*olen -= rounds * 48;
}
//=============> neon64/dec_loop.c  end <===

#ifdef BASE64_NEON64_USE_ASM
//# include "enc_loop_asm.c"
//=============> neon64/enc_loop_asm.c  begine <===
// Apologies in advance for combining the preprocessor with inline assembly,
// two notoriously gnarly parts of C, but it was necessary to avoid a lot of
// code repetition. The preprocessor is used to template large sections of
// inline assembly that differ only in the registers used. If the code was
// written out by hand, it would become very large and hard to audit.

// Generate a block of inline assembly that loads three user-defined registers
// A, B, C from memory and deinterleaves them, post-incrementing the src
// pointer. The register set should be sequential.
#define LOAD(A, B, C) \
	"ld3 {"A".16b, "B".16b, "C".16b}, [%[src]], #48 \n\t"

// Generate a block of inline assembly that takes three deinterleaved registers
// and shuffles the bytes. The output is in temporary registers t0..t3.
#define SHUF(A, B, C) \
	"ushr %[t0].16b, "A".16b,   #2         \n\t" \
	"ushr %[t1].16b, "B".16b,   #4         \n\t" \
	"ushr %[t2].16b, "C".16b,   #6         \n\t" \
	"sli  %[t1].16b, "A".16b,   #4         \n\t" \
	"sli  %[t2].16b, "B".16b,   #2         \n\t" \
	"and  %[t1].16b, %[t1].16b, %[n63].16b \n\t" \
	"and  %[t2].16b, %[t2].16b, %[n63].16b \n\t" \
	"and  %[t3].16b, "C".16b,   %[n63].16b \n\t"

// Generate a block of inline assembly that takes temporary registers t0..t3
// and translates them to the base64 alphabet, using a table loaded into
// v8..v11. The output is in user-defined registers A..D.
#define TRAN(A, B, C, D) \
	"tbl "A".16b, {v8.16b-v11.16b}, %[t0].16b \n\t" \
	"tbl "B".16b, {v8.16b-v11.16b}, %[t1].16b \n\t" \
	"tbl "C".16b, {v8.16b-v11.16b}, %[t2].16b \n\t" \
	"tbl "D".16b, {v8.16b-v11.16b}, %[t3].16b \n\t"

// Generate a block of inline assembly that interleaves four registers and
// stores them, post-incrementing the destination pointer.
#define STOR(A, B, C, D) \
	"st4 {"A".16b, "B".16b, "C".16b, "D".16b}, [%[dst]], #64 \n\t"

// Generate a block of inline assembly that generates a single self-contained
// encoder round: fetch the data, process it, and store the result.
#define ROUND() \
	LOAD("v12", "v13", "v14") \
	SHUF("v12", "v13", "v14") \
	TRAN("v12", "v13", "v14", "v15") \
	STOR("v12", "v13", "v14", "v15")

// Generate a block of assembly that generates a type A interleaved encoder
// round. It uses registers that were loaded by the previous type B round, and
// in turn loads registers for the next type B round.
#define ROUND_A() \
	SHUF("v2",  "v3",  "v4") \
	LOAD("v12", "v13", "v14") \
	TRAN("v2",  "v3",  "v4", "v5") \
	STOR("v2",  "v3",  "v4", "v5")

// Type B interleaved encoder round. Same as type A, but register sets swapped.
#define ROUND_B() \
	SHUF("v12", "v13", "v14") \
	LOAD("v2",  "v3",  "v4") \
	TRAN("v12", "v13", "v14", "v15") \
	STOR("v12", "v13", "v14", "v15")

// The first type A round needs to load its own registers.
#define ROUND_A_FIRST() \
	LOAD("v2", "v3", "v4") \
	ROUND_A()

// The last type B round omits the load for the next step.
#define ROUND_B_LAST() \
	SHUF("v12", "v13", "v14") \
	TRAN("v12", "v13", "v14", "v15") \
	STOR("v12", "v13", "v14", "v15")

// Suppress clang's warning that the literal string in the asm statement is
// overlong (longer than the ISO-mandated minimum size of 4095 bytes for C99
// compilers). It may be true, but the goal here is not C99 portability.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverlength-strings"

static inline void
enc_loop_neon64 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	size_t rounds = *slen / 48;

	if (rounds == 0) {
		return;
	}

	*slen -= rounds * 48;	// 48 bytes consumed per round.
	*olen += rounds * 64;	// 64 bytes produced per round.

	// Number of times to go through the 8x loop.
	size_t loops = rounds / 8;

	// Number of rounds remaining after the 8x loop.
	rounds %= 8;

	// Temporary registers, used as scratch space.
	uint8x16_t tmp0, tmp1, tmp2, tmp3;

	__asm__ volatile (

		// Load the encoding table into v8..v11.
		"    ld1 {v8.16b-v11.16b}, [%[tbl]] \n\t"

		// If there are eight rounds or more, enter an 8x unrolled loop
		// of interleaved encoding rounds. The rounds interleave memory
		// operations (load/store) with data operations to maximize
		// pipeline throughput.
		"    cbz %[loops], 4f \n\t"

		// The SIMD instructions do not touch the flags.
		"88: subs %[loops], %[loops], #1 \n\t"
		"    " ROUND_A_FIRST()
		"    " ROUND_B()
		"    " ROUND_A()
		"    " ROUND_B()
		"    " ROUND_A()
		"    " ROUND_B()
		"    " ROUND_A()
		"    " ROUND_B_LAST()
		"    b.ne 88b \n\t"

		// Enter a 4x unrolled loop for rounds of 4 or more.
		"4:  cmp  %[rounds], #4 \n\t"
		"    b.lt 30f           \n\t"
		"    " ROUND_A_FIRST()
		"    " ROUND_B()
		"    " ROUND_A()
		"    " ROUND_B_LAST()
		"    sub %[rounds], %[rounds], #4 \n\t"

		// Dispatch the remaining rounds 0..3.
		"30: cbz  %[rounds], 0f \n\t"
		"    cmp  %[rounds], #2 \n\t"
		"    b.eq 2f            \n\t"
		"    b.lt 1f            \n\t"

		// Block of non-interlaced encoding rounds, which can each
		// individually be jumped to. Rounds fall through to the next.
		"3:  " ROUND()
		"2:  " ROUND()
		"1:  " ROUND()
		"0:  \n\t"

		// Outputs (modified).
		: [loops] "+r"  (loops),
		  [src]   "+r"  (*s),
		  [dst]   "+r"  (*o),
		  [t0]    "=&w" (tmp0),
		  [t1]    "=&w" (tmp1),
		  [t2]    "=&w" (tmp2),
		  [t3]    "=&w" (tmp3)

		// Inputs (not modified).
		: [rounds] "r" (rounds),
		  [tbl]    "r" (base64_table_enc_6bit),
		  [n63]    "w" (vdupq_n_u8(63))

		// Clobbers.
		: "v2",  "v3",  "v4",  "v5",
		  "v8",  "v9",  "v10", "v11",
		  "v12", "v13", "v14", "v15",
		  "cc", "memory"
	);
}

#pragma GCC diagnostic pop
//=============> neon64/enc_loop_asm.c  end <===
#else
//# include "enc_reshuffle.c"
//# include "enc_loop.c"
//=============> neon64/enc_reshuffle.c  begine <===
static inline uint8x16x4_t
enc_reshuffle (const uint8x16x3_t in)
{
	uint8x16x4_t out;

	// Input:
	// in[0]  = a7 a6 a5 a4 a3 a2 a1 a0
	// in[1]  = b7 b6 b5 b4 b3 b2 b1 b0
	// in[2]  = c7 c6 c5 c4 c3 c2 c1 c0

	// Output:
	// out[0] = 00 00 a7 a6 a5 a4 a3 a2
	// out[1] = 00 00 a1 a0 b7 b6 b5 b4
	// out[2] = 00 00 b3 b2 b1 b0 c7 c6
	// out[3] = 00 00 c5 c4 c3 c2 c1 c0

	// Move the input bits to where they need to be in the outputs. Except
	// for the first output, the high two bits are not cleared.
	out.val[0] = vshrq_n_u8(in.val[0], 2);
	out.val[1] = vshrq_n_u8(in.val[1], 4);
	out.val[2] = vshrq_n_u8(in.val[2], 6);
	out.val[1] = vsliq_n_u8(out.val[1], in.val[0], 4);
	out.val[2] = vsliq_n_u8(out.val[2], in.val[1], 2);

	// Clear the high two bits in the second, third and fourth output.
	out.val[1] = vandq_u8(out.val[1], vdupq_n_u8(0x3F));
	out.val[2] = vandq_u8(out.val[2], vdupq_n_u8(0x3F));
	out.val[3] = vandq_u8(in.val[2],  vdupq_n_u8(0x3F));

	return out;
}
//=============> neon64/enc_reshuffle.c  end <===

//=============> neon64/enc_loop.c  begine <===
static inline void
enc_loop_neon64_inner (const uint8_t **s, uint8_t **o, const uint8x16x4_t tbl_enc)
{
	// Load 48 bytes and deinterleave:
	uint8x16x3_t src = vld3q_u8(*s);

	// Divide bits of three input bytes over four output bytes:
	uint8x16x4_t out = enc_reshuffle(src);

	// The bits have now been shifted to the right locations;
	// translate their values 0..63 to the Base64 alphabet.
	// Use a 64-byte table lookup:
	out.val[0] = vqtbl4q_u8(tbl_enc, out.val[0]);
	out.val[1] = vqtbl4q_u8(tbl_enc, out.val[1]);
	out.val[2] = vqtbl4q_u8(tbl_enc, out.val[2]);
	out.val[3] = vqtbl4q_u8(tbl_enc, out.val[3]);

	// Interleave and store output:
	vst4q_u8(*o, out);

	*s += 48;
	*o += 64;
}

static inline void
enc_loop_neon64 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	size_t rounds = *slen / 48;

	*slen -= rounds * 48;	// 48 bytes consumed per round
	*olen += rounds * 64;	// 64 bytes produced per round

	// Load the encoding table:
	const uint8x16x4_t tbl_enc = load_64byte_table(base64_table_enc_6bit);

	while (rounds > 0) {
		if (rounds >= 8) {
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_neon64_inner(s, o, tbl_enc);
			enc_loop_neon64_inner(s, o, tbl_enc);
			rounds -= 2;
			continue;
		}
		enc_loop_neon64_inner(s, o, tbl_enc);
		break;
	}
}
//=============> neon64/enc_loop.c  end <===
#endif

#endif	// BASE64_USE_NEON64

// Stride size is so large on these NEON 64-bit functions
// (48 bytes encode, 64 bytes decode) that we inline the
// uint64 codec to stay performant on smaller inputs.

BASE64_ENC_FUNCTION(neon64)
{
#ifdef BASE64_USE_NEON64
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
	enc_loop_neon64(&s, &slen, &o, &olen);
	enc_loop_generic_64(&s, &slen, &o, &olen);
	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

BASE64_DEC_FUNCTION(neon64)
{
#ifdef BASE64_USE_NEON64
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_neon64(&s, &slen, &o, &olen);
	dec_loop_generic_32(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/neon64/codec.c   end <=========================




////=============> arch/ssse3/codec.c   begine <=========================
//#include <stdint.h>
//#include <stddef.h>
//#include <stdlib.h>
//
//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#if HAVE_SSSE3
#include <tmmintrin.h>

// Only enable inline assembly on supported compilers and on 64-bit CPUs.
// 32-bit CPUs with SSSE3 support, such as low-end Atoms, only have eight XMM
// registers, which is not enough to run the inline assembly.
#ifndef BASE64_SSSE3_USE_ASM
# if (defined(__GNUC__) || defined(__clang__)) && BASE64_WORDSIZE == 64
#  define BASE64_SSSE3_USE_ASM 1
# else
#  define BASE64_SSSE3_USE_ASM 0
# endif
#endif

//#include "dec_reshuffle.c"
//#include "dec_loop.c"

//=============> ssse3/dec_reshuffle.c  begine <===
static inline __m128i
dec_reshuffle (const __m128i in)
{
	// in, bits, upper case are most significant bits, lower case are least significant bits
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA

	const __m128i merge_ab_and_bc = _mm_maddubs_epi16(in, _mm_set1_epi32(0x01400140));
	// 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	// 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	// 0000eeee FFffffff 0000DDDD DDddEEEE
	// 0000bbbb CCcccccc 0000AAAA AAaaBBBB

	const __m128i out = _mm_madd_epi16(merge_ab_and_bc, _mm_set1_epi32(0x00011000));
	// 00000000 JJJJJJjj KKKKkkkk LLllllll
	// 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	// 00000000 DDDDDDdd EEEEeeee FFffffff
	// 00000000 AAAAAAaa BBBBbbbb CCcccccc

	// Pack bytes together:
	return  _mm_shuffle_epi8(out, _mm_setr_epi8(
		 2,  1,  0,
		 6,  5,  4,
		10,  9,  8,
		14, 13, 12,
		-1, -1, -1, -1));
	// 00000000 00000000 00000000 00000000
	// LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	// HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	// DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa
}
//=============> ssse3/dec_reshuffle.c  end <===

//=============> ssse3/dec_loop.c  begine <===
// The input consists of six character sets in the Base64 alphabet, which we
// need to map back to the 6-bit values they represent. There are three ranges,
// two singles, and then there's the rest.
//
//  #  From       To        Add  Characters
//  1  [43]       [62]      +19  +
//  2  [47]       [63]      +16  /
//  3  [48..57]   [52..61]   +4  0..9
//  4  [65..90]   [0..25]   -65  A..Z
//  5  [97..122]  [26..51]  -71  a..z
// (6) Everything else => invalid input
//
// We will use lookup tables for character validation and offset computation.
// Remember that 0x2X and 0x0X are the same index for _mm_shuffle_epi8, this
// allows to mask with 0x2F instead of 0x0F and thus save one constant
// declaration (register and/or memory access).
//
// For offsets:
// Perfect hash for lut = ((src >> 4) & 0x2F) + ((src == 0x2F) ? 0xFF : 0x00)
// 0000 = garbage
// 0001 = /
// 0010 = +
// 0011 = 0-9
// 0100 = A-Z
// 0101 = A-Z
// 0110 = a-z
// 0111 = a-z
// 1000 >= garbage
//
// For validation, here's the table.
// A character is valid if and only if the AND of the 2 lookups equals 0:
//
// hi \ lo              0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
//      LUT             0x15 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x13 0x1A 0x1B 0x1B 0x1B 0x1A
//
// 0000 0x10 char        NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS   HT   LF   VT   FF   CR   SO   SI
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0001 0x10 char        DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0010 0x01 char               !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
//           andlut     0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x00 0x01 0x01 0x01 0x00
//
// 0011 0x02 char          0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x02 0x02 0x02 0x02 0x02 0x02
//
// 0100 0x04 char          @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
//
// 0101 0x08 char          P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 0110 0x04 char          `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
// 0111 0x08 char          p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 1000 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1001 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1010 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1011 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1100 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1101 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1110 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1111 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10

static inline int
dec_loop_ssse3_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const __m128i lut_lo = _mm_setr_epi8(
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);

	const __m128i lut_hi = _mm_setr_epi8(
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);

	const __m128i lut_roll = _mm_setr_epi8(
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0);

	const __m128i mask_2F = _mm_set1_epi8(0x2F);

	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Table lookups:
	const __m128i hi_nibbles = _mm_and_si128(_mm_srli_epi32(str, 4), mask_2F);
	const __m128i lo_nibbles = _mm_and_si128(str, mask_2F);
	const __m128i hi         = _mm_shuffle_epi8(lut_hi, hi_nibbles);
	const __m128i lo         = _mm_shuffle_epi8(lut_lo, lo_nibbles);

	// Check for invalid input: if any "and" values from lo and hi are not
	// zero, fall back on bytewise code to do error checking and reporting:
	if (_mm_movemask_epi8(_mm_cmpgt_epi8(_mm_and_si128(lo, hi), _mm_setzero_si128())) != 0) {
		return 0;
	}

	const __m128i eq_2F = _mm_cmpeq_epi8(str, mask_2F);
	const __m128i roll  = _mm_shuffle_epi8(lut_roll, _mm_add_epi8(eq_2F, hi_nibbles));

	// Now simply add the delta values to the input:
	str = _mm_add_epi8(str, roll);

	// Reshuffle the input to packed 12-byte output format:
	str = dec_reshuffle(str);

	// Store the output:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 16;
	*o += 12;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 24) {
		return;
	}

	// Process blocks of 16 bytes per round. Because 4 extra zero bytes are
	// written after the output, ensure that there will be at least 8 bytes
	// of input data left to cover the gap. (6 data bytes and up to two
	// end-of-string markers.)
	size_t rounds = (*slen - 8) / 16;

	*slen -= rounds * 16;	// 16 bytes consumed per round
	*olen += rounds * 12;	// 12 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_ssse3_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 16;
	*olen -= rounds * 12;
}
//=============> ssse3/dec_loop.c  end <===

#if BASE64_SSSE3_USE_ASM
//# include "enc_loop_asm.c"
//=============> ssse3/enc_loop_asm.c  begine <===
//=============> ssse3/enc_loop_asm.c  end <===
#else
//# include "enc_reshuffle.c"
//# include "enc_translate.c"
//# include "enc_loop.c"
//=============> ssse3/enc_reshuffle.c  begine <===
static inline __m128i
enc_reshuffle (__m128i in)
{
	// Input, bytes MSB to LSB:
	// 0 0 0 0 l k j i h g f e d c b a

	in = _mm_shuffle_epi8(in, _mm_set_epi8(
		10, 11,  9, 10,
		 7,  8,  6,  7,
		 4,  5,  3,  4,
		 1,  2,  0,  1));
	// in, bytes MSB to LSB:
	// k l j k
	// h i g h
	// e f d e
	// b c a b

	const __m128i t0 = _mm_and_si128(in, _mm_set1_epi32(0x0FC0FC00));
	// bits, upper case are most significant bits, lower case are least significant bits
	// 0000kkkk LL000000 JJJJJJ00 00000000
	// 0000hhhh II000000 GGGGGG00 00000000
	// 0000eeee FF000000 DDDDDD00 00000000
	// 0000bbbb CC000000 AAAAAA00 00000000

	const __m128i t1 = _mm_mulhi_epu16(t0, _mm_set1_epi32(0x04000040));
	// 00000000 00kkkkLL 00000000 00JJJJJJ
	// 00000000 00hhhhII 00000000 00GGGGGG
	// 00000000 00eeeeFF 00000000 00DDDDDD
	// 00000000 00bbbbCC 00000000 00AAAAAA

	const __m128i t2 = _mm_and_si128(in, _mm_set1_epi32(0x003F03F0));
	// 00000000 00llllll 000000jj KKKK0000
	// 00000000 00iiiiii 000000gg HHHH0000
	// 00000000 00ffffff 000000dd EEEE0000
	// 00000000 00cccccc 000000aa BBBB0000

	const __m128i t3 = _mm_mullo_epi16(t2, _mm_set1_epi32(0x01000010));
	// 00llllll 00000000 00jjKKKK 00000000
	// 00iiiiii 00000000 00ggHHHH 00000000
	// 00ffffff 00000000 00ddEEEE 00000000
	// 00cccccc 00000000 00aaBBBB 00000000

	return _mm_or_si128(t1, t3);
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA
}
//=============> ssse3/enc_reshuffle.c  end <===

//=============> ssse3/enc_translate.c  begine <===
static inline __m128i
enc_translate (const __m128i in)
{
	// A lookup table containing the absolute offsets for all ranges:
	const __m128i lut = _mm_setr_epi8(
		 65,  71, -4, -4,
		 -4,  -4, -4, -4,
		 -4,  -4, -4, -4,
		-19, -16,  0,  0
	);

	// Translate values 0..63 to the Base64 alphabet. There are five sets:
	// #  From      To         Abs    Index  Characters
	// 0  [0..25]   [65..90]   +65        0  ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// 1  [26..51]  [97..122]  +71        1  abcdefghijklmnopqrstuvwxyz
	// 2  [52..61]  [48..57]    -4  [2..11]  0123456789
	// 3  [62]      [43]       -19       12  +
	// 4  [63]      [47]       -16       13  /

	// Create LUT indices from the input. The index for range #0 is right,
	// others are 1 less than expected:
	__m128i indices = _mm_subs_epu8(in, _mm_set1_epi8(51));

	// mask is 0xFF (-1) for range #[1..4] and 0x00 for range #0:
	__m128i mask = _mm_cmpgt_epi8(in, _mm_set1_epi8(25));

	// Subtract -1, so add 1 to indices for range #[1..4]. All indices are
	// now correct:
	indices = _mm_sub_epi8(indices, mask);

	// Add offsets to input values:
	return _mm_add_epi8(in, _mm_shuffle_epi8(lut, indices));
}
//=============> ssse3/enc_translate.c  end <===

//=============> ssse3/enc_loop.c  begine <===
static inline void
enc_loop_ssse3_inner (const uint8_t **s, uint8_t **o)
{
	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Reshuffle:
	str = enc_reshuffle(str);

	// Translate reshuffled bytes to the Base64 alphabet:
	str = enc_translate(str);

	// Store:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 12;
	*o += 16;
}

static inline void
enc_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 16) {
		return;
	}

	// Process blocks of 12 bytes at a time. Because blocks are loaded 16
	// bytes at a time, ensure that there will be at least 4 remaining
	// bytes after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 4) / 12;

	*slen -= rounds * 12;	// 12 bytes consumed per round
	*olen += rounds * 16;	// 16 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_ssse3_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> ssse3/enc_loop.c  end <===

#endif

#endif	// HAVE_SSSE3

BASE64_ENC_FUNCTION(ssse3)
{
#if HAVE_SSSE3
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
	enc_loop_ssse3(&s, &slen, &o, &olen);
	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

BASE64_DEC_FUNCTION(ssse3)
{
#if HAVE_SSSE3
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_ssse3(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/ssse3/codec.c   end <=========================




////=============> arch/sse41/codec.c   begine <=========================
//#include <stdint.h>
//#include <stddef.h>
//#include <stdlib.h>
//
//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#if HAVE_SSE41
#include <smmintrin.h>

// Only enable inline assembly on supported compilers and on 64-bit CPUs.
#ifndef BASE64_SSE41_USE_ASM
# if (defined(__GNUC__) || defined(__clang__)) && BASE64_WORDSIZE == 64
#  define BASE64_SSE41_USE_ASM 1
# else
#  define BASE64_SSE41_USE_ASM 0
# endif
#endif

//#include "../ssse3/dec_reshuffle.c"
//#include "../ssse3/dec_loop.c"

//=============> ssse3/dec_reshuffle.c  begine <===
static inline __m128i
dec_reshuffle (const __m128i in)
{
	// in, bits, upper case are most significant bits, lower case are least significant bits
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA

	const __m128i merge_ab_and_bc = _mm_maddubs_epi16(in, _mm_set1_epi32(0x01400140));
	// 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	// 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	// 0000eeee FFffffff 0000DDDD DDddEEEE
	// 0000bbbb CCcccccc 0000AAAA AAaaBBBB

	const __m128i out = _mm_madd_epi16(merge_ab_and_bc, _mm_set1_epi32(0x00011000));
	// 00000000 JJJJJJjj KKKKkkkk LLllllll
	// 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	// 00000000 DDDDDDdd EEEEeeee FFffffff
	// 00000000 AAAAAAaa BBBBbbbb CCcccccc

	// Pack bytes together:
	return  _mm_shuffle_epi8(out, _mm_setr_epi8(
		 2,  1,  0,
		 6,  5,  4,
		10,  9,  8,
		14, 13, 12,
		-1, -1, -1, -1));
	// 00000000 00000000 00000000 00000000
	// LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	// HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	// DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa
}
//=============> ssse3/dec_reshuffle.c  end <===

//=============> ssse3/dec_loop.c  begine <===
// The input consists of six character sets in the Base64 alphabet, which we
// need to map back to the 6-bit values they represent. There are three ranges,
// two singles, and then there's the rest.
//
//  #  From       To        Add  Characters
//  1  [43]       [62]      +19  +
//  2  [47]       [63]      +16  /
//  3  [48..57]   [52..61]   +4  0..9
//  4  [65..90]   [0..25]   -65  A..Z
//  5  [97..122]  [26..51]  -71  a..z
// (6) Everything else => invalid input
//
// We will use lookup tables for character validation and offset computation.
// Remember that 0x2X and 0x0X are the same index for _mm_shuffle_epi8, this
// allows to mask with 0x2F instead of 0x0F and thus save one constant
// declaration (register and/or memory access).
//
// For offsets:
// Perfect hash for lut = ((src >> 4) & 0x2F) + ((src == 0x2F) ? 0xFF : 0x00)
// 0000 = garbage
// 0001 = /
// 0010 = +
// 0011 = 0-9
// 0100 = A-Z
// 0101 = A-Z
// 0110 = a-z
// 0111 = a-z
// 1000 >= garbage
//
// For validation, here's the table.
// A character is valid if and only if the AND of the 2 lookups equals 0:
//
// hi \ lo              0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
//      LUT             0x15 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x13 0x1A 0x1B 0x1B 0x1B 0x1A
//
// 0000 0x10 char        NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS   HT   LF   VT   FF   CR   SO   SI
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0001 0x10 char        DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0010 0x01 char               !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
//           andlut     0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x00 0x01 0x01 0x01 0x00
//
// 0011 0x02 char          0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x02 0x02 0x02 0x02 0x02 0x02
//
// 0100 0x04 char          @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
//
// 0101 0x08 char          P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 0110 0x04 char          `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
// 0111 0x08 char          p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 1000 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1001 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1010 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1011 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1100 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1101 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1110 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1111 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10

static inline int
dec_loop_ssse3_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const __m128i lut_lo = _mm_setr_epi8(
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);

	const __m128i lut_hi = _mm_setr_epi8(
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);

	const __m128i lut_roll = _mm_setr_epi8(
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0);

	const __m128i mask_2F = _mm_set1_epi8(0x2F);

	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Table lookups:
	const __m128i hi_nibbles = _mm_and_si128(_mm_srli_epi32(str, 4), mask_2F);
	const __m128i lo_nibbles = _mm_and_si128(str, mask_2F);
	const __m128i hi         = _mm_shuffle_epi8(lut_hi, hi_nibbles);
	const __m128i lo         = _mm_shuffle_epi8(lut_lo, lo_nibbles);

	// Check for invalid input: if any "and" values from lo and hi are not
	// zero, fall back on bytewise code to do error checking and reporting:
	if (_mm_movemask_epi8(_mm_cmpgt_epi8(_mm_and_si128(lo, hi), _mm_setzero_si128())) != 0) {
		return 0;
	}

	const __m128i eq_2F = _mm_cmpeq_epi8(str, mask_2F);
	const __m128i roll  = _mm_shuffle_epi8(lut_roll, _mm_add_epi8(eq_2F, hi_nibbles));

	// Now simply add the delta values to the input:
	str = _mm_add_epi8(str, roll);

	// Reshuffle the input to packed 12-byte output format:
	str = dec_reshuffle(str);

	// Store the output:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 16;
	*o += 12;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 24) {
		return;
	}

	// Process blocks of 16 bytes per round. Because 4 extra zero bytes are
	// written after the output, ensure that there will be at least 8 bytes
	// of input data left to cover the gap. (6 data bytes and up to two
	// end-of-string markers.)
	size_t rounds = (*slen - 8) / 16;

	*slen -= rounds * 16;	// 16 bytes consumed per round
	*olen += rounds * 12;	// 12 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_ssse3_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 16;
	*olen -= rounds * 12;
}
//=============> ssse3/dec_loop.c  end <===

#if BASE64_SSE41_USE_ASM
//# include "../ssse3/enc_loop_asm.c"
//=============> ssse3/enc_loop_asm.c  begine <===
//=============> ssse3/enc_loop_asm.c  end <===
#else
//# include "../ssse3/enc_translate.c"
//# include "../ssse3/enc_reshuffle.c"
//# include "../ssse3/enc_loop.c"
//=============> ssse3/enc_translate.c  begine <===
static inline __m128i
enc_translate (const __m128i in)
{
	// A lookup table containing the absolute offsets for all ranges:
	const __m128i lut = _mm_setr_epi8(
		 65,  71, -4, -4,
		 -4,  -4, -4, -4,
		 -4,  -4, -4, -4,
		-19, -16,  0,  0
	);

	// Translate values 0..63 to the Base64 alphabet. There are five sets:
	// #  From      To         Abs    Index  Characters
	// 0  [0..25]   [65..90]   +65        0  ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// 1  [26..51]  [97..122]  +71        1  abcdefghijklmnopqrstuvwxyz
	// 2  [52..61]  [48..57]    -4  [2..11]  0123456789
	// 3  [62]      [43]       -19       12  +
	// 4  [63]      [47]       -16       13  /

	// Create LUT indices from the input. The index for range #0 is right,
	// others are 1 less than expected:
	__m128i indices = _mm_subs_epu8(in, _mm_set1_epi8(51));

	// mask is 0xFF (-1) for range #[1..4] and 0x00 for range #0:
	__m128i mask = _mm_cmpgt_epi8(in, _mm_set1_epi8(25));

	// Subtract -1, so add 1 to indices for range #[1..4]. All indices are
	// now correct:
	indices = _mm_sub_epi8(indices, mask);

	// Add offsets to input values:
	return _mm_add_epi8(in, _mm_shuffle_epi8(lut, indices));
}
//=============> ssse3/enc_translate.c  end <===

//=============> ssse3/enc_reshuffle.c  begine <===
static inline __m128i
enc_reshuffle (__m128i in)
{
	// Input, bytes MSB to LSB:
	// 0 0 0 0 l k j i h g f e d c b a

	in = _mm_shuffle_epi8(in, _mm_set_epi8(
		10, 11,  9, 10,
		 7,  8,  6,  7,
		 4,  5,  3,  4,
		 1,  2,  0,  1));
	// in, bytes MSB to LSB:
	// k l j k
	// h i g h
	// e f d e
	// b c a b

	const __m128i t0 = _mm_and_si128(in, _mm_set1_epi32(0x0FC0FC00));
	// bits, upper case are most significant bits, lower case are least significant bits
	// 0000kkkk LL000000 JJJJJJ00 00000000
	// 0000hhhh II000000 GGGGGG00 00000000
	// 0000eeee FF000000 DDDDDD00 00000000
	// 0000bbbb CC000000 AAAAAA00 00000000

	const __m128i t1 = _mm_mulhi_epu16(t0, _mm_set1_epi32(0x04000040));
	// 00000000 00kkkkLL 00000000 00JJJJJJ
	// 00000000 00hhhhII 00000000 00GGGGGG
	// 00000000 00eeeeFF 00000000 00DDDDDD
	// 00000000 00bbbbCC 00000000 00AAAAAA

	const __m128i t2 = _mm_and_si128(in, _mm_set1_epi32(0x003F03F0));
	// 00000000 00llllll 000000jj KKKK0000
	// 00000000 00iiiiii 000000gg HHHH0000
	// 00000000 00ffffff 000000dd EEEE0000
	// 00000000 00cccccc 000000aa BBBB0000

	const __m128i t3 = _mm_mullo_epi16(t2, _mm_set1_epi32(0x01000010));
	// 00llllll 00000000 00jjKKKK 00000000
	// 00iiiiii 00000000 00ggHHHH 00000000
	// 00ffffff 00000000 00ddEEEE 00000000
	// 00cccccc 00000000 00aaBBBB 00000000

	return _mm_or_si128(t1, t3);
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA
}
//=============> ssse3/enc_reshuffle.c  end <===

//=============> ssse3/enc_loop.c  begine <===
static inline void
enc_loop_ssse3_inner (const uint8_t **s, uint8_t **o)
{
	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Reshuffle:
	str = enc_reshuffle(str);

	// Translate reshuffled bytes to the Base64 alphabet:
	str = enc_translate(str);

	// Store:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 12;
	*o += 16;
}

static inline void
enc_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 16) {
		return;
	}

	// Process blocks of 12 bytes at a time. Because blocks are loaded 16
	// bytes at a time, ensure that there will be at least 4 remaining
	// bytes after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 4) / 12;

	*slen -= rounds * 12;	// 12 bytes consumed per round
	*olen += rounds * 16;	// 16 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_ssse3_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> ssse3/enc_loop.c  end <===
#endif

#endif	// HAVE_SSE41

BASE64_ENC_FUNCTION(sse41)
{
#if HAVE_SSE41
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
	enc_loop_ssse3(&s, &slen, &o, &olen);
	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

BASE64_DEC_FUNCTION(sse41)
{
#if HAVE_SSE41
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_ssse3(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/sse41/codec.c   end <=========================




////=============> arch/sse42/codec.c   begine <=========================
//#include <stdint.h>
//#include <stddef.h>
//#include <stdlib.h>
//
//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#if HAVE_SSE42
#include <nmmintrin.h>

// Only enable inline assembly on supported compilers and on 64-bit CPUs.
#ifndef BASE64_SSE42_USE_ASM
# if (defined(__GNUC__) || defined(__clang__)) && BASE64_WORDSIZE == 64
#  define BASE64_SSE42_USE_ASM 1
# else
#  define BASE64_SSE42_USE_ASM 0
# endif
#endif

//#include "../ssse3/dec_reshuffle.c"
//#include "../ssse3/dec_loop.c"

//=============> ssse3/dec_reshuffle.c  begine <===
static inline __m128i
dec_reshuffle (const __m128i in)
{
	// in, bits, upper case are most significant bits, lower case are least significant bits
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA

	const __m128i merge_ab_and_bc = _mm_maddubs_epi16(in, _mm_set1_epi32(0x01400140));
	// 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	// 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	// 0000eeee FFffffff 0000DDDD DDddEEEE
	// 0000bbbb CCcccccc 0000AAAA AAaaBBBB

	const __m128i out = _mm_madd_epi16(merge_ab_and_bc, _mm_set1_epi32(0x00011000));
	// 00000000 JJJJJJjj KKKKkkkk LLllllll
	// 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	// 00000000 DDDDDDdd EEEEeeee FFffffff
	// 00000000 AAAAAAaa BBBBbbbb CCcccccc

	// Pack bytes together:
	return  _mm_shuffle_epi8(out, _mm_setr_epi8(
		 2,  1,  0,
		 6,  5,  4,
		10,  9,  8,
		14, 13, 12,
		-1, -1, -1, -1));
	// 00000000 00000000 00000000 00000000
	// LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	// HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	// DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa
}
//=============> ssse3/dec_reshuffle.c  end <===

//=============> ssse3/dec_loop.c  begine <===
// The input consists of six character sets in the Base64 alphabet, which we
// need to map back to the 6-bit values they represent. There are three ranges,
// two singles, and then there's the rest.
//
//  #  From       To        Add  Characters
//  1  [43]       [62]      +19  +
//  2  [47]       [63]      +16  /
//  3  [48..57]   [52..61]   +4  0..9
//  4  [65..90]   [0..25]   -65  A..Z
//  5  [97..122]  [26..51]  -71  a..z
// (6) Everything else => invalid input
//
// We will use lookup tables for character validation and offset computation.
// Remember that 0x2X and 0x0X are the same index for _mm_shuffle_epi8, this
// allows to mask with 0x2F instead of 0x0F and thus save one constant
// declaration (register and/or memory access).
//
// For offsets:
// Perfect hash for lut = ((src >> 4) & 0x2F) + ((src == 0x2F) ? 0xFF : 0x00)
// 0000 = garbage
// 0001 = /
// 0010 = +
// 0011 = 0-9
// 0100 = A-Z
// 0101 = A-Z
// 0110 = a-z
// 0111 = a-z
// 1000 >= garbage
//
// For validation, here's the table.
// A character is valid if and only if the AND of the 2 lookups equals 0:
//
// hi \ lo              0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
//      LUT             0x15 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x13 0x1A 0x1B 0x1B 0x1B 0x1A
//
// 0000 0x10 char        NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS   HT   LF   VT   FF   CR   SO   SI
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0001 0x10 char        DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0010 0x01 char               !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
//           andlut     0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x00 0x01 0x01 0x01 0x00
//
// 0011 0x02 char          0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x02 0x02 0x02 0x02 0x02 0x02
//
// 0100 0x04 char          @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
//
// 0101 0x08 char          P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 0110 0x04 char          `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
// 0111 0x08 char          p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 1000 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1001 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1010 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1011 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1100 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1101 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1110 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1111 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10

static inline int
dec_loop_ssse3_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const __m128i lut_lo = _mm_setr_epi8(
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);

	const __m128i lut_hi = _mm_setr_epi8(
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);

	const __m128i lut_roll = _mm_setr_epi8(
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0);

	const __m128i mask_2F = _mm_set1_epi8(0x2F);

	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Table lookups:
	const __m128i hi_nibbles = _mm_and_si128(_mm_srli_epi32(str, 4), mask_2F);
	const __m128i lo_nibbles = _mm_and_si128(str, mask_2F);
	const __m128i hi         = _mm_shuffle_epi8(lut_hi, hi_nibbles);
	const __m128i lo         = _mm_shuffle_epi8(lut_lo, lo_nibbles);

	// Check for invalid input: if any "and" values from lo and hi are not
	// zero, fall back on bytewise code to do error checking and reporting:
	if (_mm_movemask_epi8(_mm_cmpgt_epi8(_mm_and_si128(lo, hi), _mm_setzero_si128())) != 0) {
		return 0;
	}

	const __m128i eq_2F = _mm_cmpeq_epi8(str, mask_2F);
	const __m128i roll  = _mm_shuffle_epi8(lut_roll, _mm_add_epi8(eq_2F, hi_nibbles));

	// Now simply add the delta values to the input:
	str = _mm_add_epi8(str, roll);

	// Reshuffle the input to packed 12-byte output format:
	str = dec_reshuffle(str);

	// Store the output:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 16;
	*o += 12;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 24) {
		return;
	}

	// Process blocks of 16 bytes per round. Because 4 extra zero bytes are
	// written after the output, ensure that there will be at least 8 bytes
	// of input data left to cover the gap. (6 data bytes and up to two
	// end-of-string markers.)
	size_t rounds = (*slen - 8) / 16;

	*slen -= rounds * 16;	// 16 bytes consumed per round
	*olen += rounds * 12;	// 12 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_ssse3_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 16;
	*olen -= rounds * 12;
}
//=============> ssse3/dec_loop.c  end <===

#if BASE64_SSE42_USE_ASM
//# include "../ssse3/enc_loop_asm.c"
//=============> ssse3/enc_loop_asm.c  begine <===
//=============> ssse3/enc_loop_asm.c  end <===
#else
//# include "../ssse3/enc_translate.c"
//# include "../ssse3/enc_reshuffle.c"
//# include "../ssse3/enc_loop.c"
//=============> ssse3/enc_translate.c  begine <===
static inline __m128i
enc_translate (const __m128i in)
{
	// A lookup table containing the absolute offsets for all ranges:
	const __m128i lut = _mm_setr_epi8(
		 65,  71, -4, -4,
		 -4,  -4, -4, -4,
		 -4,  -4, -4, -4,
		-19, -16,  0,  0
	);

	// Translate values 0..63 to the Base64 alphabet. There are five sets:
	// #  From      To         Abs    Index  Characters
	// 0  [0..25]   [65..90]   +65        0  ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// 1  [26..51]  [97..122]  +71        1  abcdefghijklmnopqrstuvwxyz
	// 2  [52..61]  [48..57]    -4  [2..11]  0123456789
	// 3  [62]      [43]       -19       12  +
	// 4  [63]      [47]       -16       13  /

	// Create LUT indices from the input. The index for range #0 is right,
	// others are 1 less than expected:
	__m128i indices = _mm_subs_epu8(in, _mm_set1_epi8(51));

	// mask is 0xFF (-1) for range #[1..4] and 0x00 for range #0:
	__m128i mask = _mm_cmpgt_epi8(in, _mm_set1_epi8(25));

	// Subtract -1, so add 1 to indices for range #[1..4]. All indices are
	// now correct:
	indices = _mm_sub_epi8(indices, mask);

	// Add offsets to input values:
	return _mm_add_epi8(in, _mm_shuffle_epi8(lut, indices));
}
//=============> ssse3/enc_translate.c  end <===

//=============> ssse3/enc_reshuffle.c  begine <===
static inline __m128i
enc_reshuffle (__m128i in)
{
	// Input, bytes MSB to LSB:
	// 0 0 0 0 l k j i h g f e d c b a

	in = _mm_shuffle_epi8(in, _mm_set_epi8(
		10, 11,  9, 10,
		 7,  8,  6,  7,
		 4,  5,  3,  4,
		 1,  2,  0,  1));
	// in, bytes MSB to LSB:
	// k l j k
	// h i g h
	// e f d e
	// b c a b

	const __m128i t0 = _mm_and_si128(in, _mm_set1_epi32(0x0FC0FC00));
	// bits, upper case are most significant bits, lower case are least significant bits
	// 0000kkkk LL000000 JJJJJJ00 00000000
	// 0000hhhh II000000 GGGGGG00 00000000
	// 0000eeee FF000000 DDDDDD00 00000000
	// 0000bbbb CC000000 AAAAAA00 00000000

	const __m128i t1 = _mm_mulhi_epu16(t0, _mm_set1_epi32(0x04000040));
	// 00000000 00kkkkLL 00000000 00JJJJJJ
	// 00000000 00hhhhII 00000000 00GGGGGG
	// 00000000 00eeeeFF 00000000 00DDDDDD
	// 00000000 00bbbbCC 00000000 00AAAAAA

	const __m128i t2 = _mm_and_si128(in, _mm_set1_epi32(0x003F03F0));
	// 00000000 00llllll 000000jj KKKK0000
	// 00000000 00iiiiii 000000gg HHHH0000
	// 00000000 00ffffff 000000dd EEEE0000
	// 00000000 00cccccc 000000aa BBBB0000

	const __m128i t3 = _mm_mullo_epi16(t2, _mm_set1_epi32(0x01000010));
	// 00llllll 00000000 00jjKKKK 00000000
	// 00iiiiii 00000000 00ggHHHH 00000000
	// 00ffffff 00000000 00ddEEEE 00000000
	// 00cccccc 00000000 00aaBBBB 00000000

	return _mm_or_si128(t1, t3);
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA
}
//=============> ssse3/enc_reshuffle.c  end <===

//=============> ssse3/enc_loop.c  begine <===
static inline void
enc_loop_ssse3_inner (const uint8_t **s, uint8_t **o)
{
	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Reshuffle:
	str = enc_reshuffle(str);

	// Translate reshuffled bytes to the Base64 alphabet:
	str = enc_translate(str);

	// Store:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 12;
	*o += 16;
}

static inline void
enc_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 16) {
		return;
	}

	// Process blocks of 12 bytes at a time. Because blocks are loaded 16
	// bytes at a time, ensure that there will be at least 4 remaining
	// bytes after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 4) / 12;

	*slen -= rounds * 12;	// 12 bytes consumed per round
	*olen += rounds * 16;	// 16 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_ssse3_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> ssse3/enc_loop.c  end <===
#endif

#endif	// HAVE_SSE42

BASE64_ENC_FUNCTION(sse42)
{
#if HAVE_SSE42
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===
	enc_loop_ssse3(&s, &slen, &o, &olen);
	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

BASE64_DEC_FUNCTION(sse42)
{
#if HAVE_SSE42
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_ssse3(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/sse42/codec.c   end <=========================




////=============> arch/avx/codec.c   end <=========================
//#include <stdint.h>
//#include <stddef.h>
//#include <stdlib.h>
//
//#include "../../../include/libbase64.h"
//#include "../../tables/tables.h"
//#include "../../codecs.h"
//#include "config.h"
//#include "../../env.h"

#if HAVE_AVX
#include <immintrin.h>

// Only enable inline assembly on supported compilers and on 64-bit CPUs.
#ifndef BASE64_AVX_USE_ASM
# if (defined(__GNUC__) || defined(__clang__)) && BASE64_WORDSIZE == 64
#  define BASE64_AVX_USE_ASM 1
# else
#  define BASE64_AVX_USE_ASM 0
# endif
#endif

//#include "../ssse3/dec_reshuffle.c"
//#include "../ssse3/dec_loop.c"
//=============> ssse3/dec_reshuffle.c  begine <===
static inline __m128i
dec_reshuffle (const __m128i in)
{
	// in, bits, upper case are most significant bits, lower case are least significant bits
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA

	const __m128i merge_ab_and_bc = _mm_maddubs_epi16(in, _mm_set1_epi32(0x01400140));
	// 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	// 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	// 0000eeee FFffffff 0000DDDD DDddEEEE
	// 0000bbbb CCcccccc 0000AAAA AAaaBBBB

	const __m128i out = _mm_madd_epi16(merge_ab_and_bc, _mm_set1_epi32(0x00011000));
	// 00000000 JJJJJJjj KKKKkkkk LLllllll
	// 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	// 00000000 DDDDDDdd EEEEeeee FFffffff
	// 00000000 AAAAAAaa BBBBbbbb CCcccccc

	// Pack bytes together:
	return  _mm_shuffle_epi8(out, _mm_setr_epi8(
		 2,  1,  0,
		 6,  5,  4,
		10,  9,  8,
		14, 13, 12,
		-1, -1, -1, -1));
	// 00000000 00000000 00000000 00000000
	// LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	// HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	// DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa
}
//=============> ssse3/dec_reshuffle.c  end <===

//=============> ssse3/dec_loop.c  begine <===
// The input consists of six character sets in the Base64 alphabet, which we
// need to map back to the 6-bit values they represent. There are three ranges,
// two singles, and then there's the rest.
//
//  #  From       To        Add  Characters
//  1  [43]       [62]      +19  +
//  2  [47]       [63]      +16  /
//  3  [48..57]   [52..61]   +4  0..9
//  4  [65..90]   [0..25]   -65  A..Z
//  5  [97..122]  [26..51]  -71  a..z
// (6) Everything else => invalid input
//
// We will use lookup tables for character validation and offset computation.
// Remember that 0x2X and 0x0X are the same index for _mm_shuffle_epi8, this
// allows to mask with 0x2F instead of 0x0F and thus save one constant
// declaration (register and/or memory access).
//
// For offsets:
// Perfect hash for lut = ((src >> 4) & 0x2F) + ((src == 0x2F) ? 0xFF : 0x00)
// 0000 = garbage
// 0001 = /
// 0010 = +
// 0011 = 0-9
// 0100 = A-Z
// 0101 = A-Z
// 0110 = a-z
// 0111 = a-z
// 1000 >= garbage
//
// For validation, here's the table.
// A character is valid if and only if the AND of the 2 lookups equals 0:
//
// hi \ lo              0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
//      LUT             0x15 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x11 0x13 0x1A 0x1B 0x1B 0x1B 0x1A
//
// 0000 0x10 char        NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS   HT   LF   VT   FF   CR   SO   SI
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0001 0x10 char        DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
//           andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
//
// 0010 0x01 char               !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
//           andlut     0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x00 0x01 0x01 0x01 0x00
//
// 0011 0x02 char          0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x02 0x02 0x02 0x02 0x02 0x02
//
// 0100 0x04 char          @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
//
// 0101 0x08 char          P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 0110 0x04 char          `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
//           andlut     0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
// 0111 0x08 char          p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~
//           andlut     0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x08 0x08 0x08 0x08 0x08
//
// 1000 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1001 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1010 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1011 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1100 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1101 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1110 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10
// 1111 0x10 andlut     0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10

static inline int
dec_loop_ssse3_inner (const uint8_t **s, uint8_t **o, size_t *rounds)
{
	const __m128i lut_lo = _mm_setr_epi8(
		0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);

	const __m128i lut_hi = _mm_setr_epi8(
		0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);

	const __m128i lut_roll = _mm_setr_epi8(
		0,  16,  19,   4, -65, -65, -71, -71,
		0,   0,   0,   0,   0,   0,   0,   0);

	const __m128i mask_2F = _mm_set1_epi8(0x2F);

	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Table lookups:
	const __m128i hi_nibbles = _mm_and_si128(_mm_srli_epi32(str, 4), mask_2F);
	const __m128i lo_nibbles = _mm_and_si128(str, mask_2F);
	const __m128i hi         = _mm_shuffle_epi8(lut_hi, hi_nibbles);
	const __m128i lo         = _mm_shuffle_epi8(lut_lo, lo_nibbles);

	// Check for invalid input: if any "and" values from lo and hi are not
	// zero, fall back on bytewise code to do error checking and reporting:
	if (_mm_movemask_epi8(_mm_cmpgt_epi8(_mm_and_si128(lo, hi), _mm_setzero_si128())) != 0) {
		return 0;
	}

	const __m128i eq_2F = _mm_cmpeq_epi8(str, mask_2F);
	const __m128i roll  = _mm_shuffle_epi8(lut_roll, _mm_add_epi8(eq_2F, hi_nibbles));

	// Now simply add the delta values to the input:
	str = _mm_add_epi8(str, roll);

	// Reshuffle the input to packed 12-byte output format:
	str = dec_reshuffle(str);

	// Store the output:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 16;
	*o += 12;
	*rounds -= 1;

	return 1;
}

static inline void
dec_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 24) {
		return;
	}

	// Process blocks of 16 bytes per round. Because 4 extra zero bytes are
	// written after the output, ensure that there will be at least 8 bytes
	// of input data left to cover the gap. (6 data bytes and up to two
	// end-of-string markers.)
	size_t rounds = (*slen - 8) / 16;

	*slen -= rounds * 16;	// 16 bytes consumed per round
	*olen += rounds * 12;	// 12 bytes produced per round

	do {
		if (rounds >= 8) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 4) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		if (rounds >= 2) {
			if (dec_loop_ssse3_inner(s, o, &rounds) &&
			    dec_loop_ssse3_inner(s, o, &rounds)) {
				continue;
			}
			break;
		}
		dec_loop_ssse3_inner(s, o, &rounds);
		break;

	} while (rounds > 0);

	// Adjust for any rounds that were skipped:
	*slen += rounds * 16;
	*olen -= rounds * 12;
}
//=============> ssse3/dec_loop.c  end <===

#if BASE64_AVX_USE_ASM

//# include "enc_loop_asm.c"
//=============> avx/enc_loop_asm.c  begine <===
//=============> avx/enc_loop_asm.c  end <===

#else

//# include "../ssse3/enc_translate.c"
//# include "../ssse3/enc_reshuffle.c"
//# include "../ssse3/enc_loop.c"
//=============> ssse3/enc_translate.c  begine <===
static inline __m128i
enc_translate (const __m128i in)
{
	// A lookup table containing the absolute offsets for all ranges:
	const __m128i lut = _mm_setr_epi8(
		 65,  71, -4, -4,
		 -4,  -4, -4, -4,
		 -4,  -4, -4, -4,
		-19, -16,  0,  0
	);

	// Translate values 0..63 to the Base64 alphabet. There are five sets:
	// #  From      To         Abs    Index  Characters
	// 0  [0..25]   [65..90]   +65        0  ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// 1  [26..51]  [97..122]  +71        1  abcdefghijklmnopqrstuvwxyz
	// 2  [52..61]  [48..57]    -4  [2..11]  0123456789
	// 3  [62]      [43]       -19       12  +
	// 4  [63]      [47]       -16       13  /

	// Create LUT indices from the input. The index for range #0 is right,
	// others are 1 less than expected:
	__m128i indices = _mm_subs_epu8(in, _mm_set1_epi8(51));

	// mask is 0xFF (-1) for range #[1..4] and 0x00 for range #0:
	__m128i mask = _mm_cmpgt_epi8(in, _mm_set1_epi8(25));

	// Subtract -1, so add 1 to indices for range #[1..4]. All indices are
	// now correct:
	indices = _mm_sub_epi8(indices, mask);

	// Add offsets to input values:
	return _mm_add_epi8(in, _mm_shuffle_epi8(lut, indices));
}
//=============> ssse3/enc_translate.c  end <===

//=============> ssse3/enc_reshuffle.c  begine <===
static inline __m128i
enc_reshuffle (__m128i in)
{
	// Input, bytes MSB to LSB:
	// 0 0 0 0 l k j i h g f e d c b a

	in = _mm_shuffle_epi8(in, _mm_set_epi8(
		10, 11,  9, 10,
		 7,  8,  6,  7,
		 4,  5,  3,  4,
		 1,  2,  0,  1));
	// in, bytes MSB to LSB:
	// k l j k
	// h i g h
	// e f d e
	// b c a b

	const __m128i t0 = _mm_and_si128(in, _mm_set1_epi32(0x0FC0FC00));
	// bits, upper case are most significant bits, lower case are least significant bits
	// 0000kkkk LL000000 JJJJJJ00 00000000
	// 0000hhhh II000000 GGGGGG00 00000000
	// 0000eeee FF000000 DDDDDD00 00000000
	// 0000bbbb CC000000 AAAAAA00 00000000

	const __m128i t1 = _mm_mulhi_epu16(t0, _mm_set1_epi32(0x04000040));
	// 00000000 00kkkkLL 00000000 00JJJJJJ
	// 00000000 00hhhhII 00000000 00GGGGGG
	// 00000000 00eeeeFF 00000000 00DDDDDD
	// 00000000 00bbbbCC 00000000 00AAAAAA

	const __m128i t2 = _mm_and_si128(in, _mm_set1_epi32(0x003F03F0));
	// 00000000 00llllll 000000jj KKKK0000
	// 00000000 00iiiiii 000000gg HHHH0000
	// 00000000 00ffffff 000000dd EEEE0000
	// 00000000 00cccccc 000000aa BBBB0000

	const __m128i t3 = _mm_mullo_epi16(t2, _mm_set1_epi32(0x01000010));
	// 00llllll 00000000 00jjKKKK 00000000
	// 00iiiiii 00000000 00ggHHHH 00000000
	// 00ffffff 00000000 00ddEEEE 00000000
	// 00cccccc 00000000 00aaBBBB 00000000

	return _mm_or_si128(t1, t3);
	// 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	// 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	// 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	// 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA
}
//=============> ssse3/enc_reshuffle.c  end <===

//=============> ssse3/enc_loop.c  begine <===
static inline void
enc_loop_ssse3_inner (const uint8_t **s, uint8_t **o)
{
	// Load input:
	__m128i str = _mm_loadu_si128((__m128i *) *s);

	// Reshuffle:
	str = enc_reshuffle(str);

	// Translate reshuffled bytes to the Base64 alphabet:
	str = enc_translate(str);

	// Store:
	_mm_storeu_si128((__m128i *) *o, str);

	*s += 12;
	*o += 16;
}

static inline void
enc_loop_ssse3 (const uint8_t **s, size_t *slen, uint8_t **o, size_t *olen)
{
	if (*slen < 16) {
		return;
	}

	// Process blocks of 12 bytes at a time. Because blocks are loaded 16
	// bytes at a time, ensure that there will be at least 4 remaining
	// bytes after the last round, so that the final read will not pass
	// beyond the bounds of the input buffer:
	size_t rounds = (*slen - 4) / 12;

	*slen -= rounds * 12;	// 12 bytes consumed per round
	*olen += rounds * 16;	// 16 bytes produced per round

	do {
		if (rounds >= 8) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 8;
			continue;
		}
		if (rounds >= 4) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 4;
			continue;
		}
		if (rounds >= 2) {
			enc_loop_ssse3_inner(s, o);
			enc_loop_ssse3_inner(s, o);
			rounds -= 2;
			continue;
		}
		enc_loop_ssse3_inner(s, o);
		break;

	} while (rounds > 0);
}
//=============> ssse3/enc_loop.c  end <===

#endif

#endif	// HAVE_AVX

BASE64_ENC_FUNCTION(avx)
{
#if HAVE_AVX
	//#include "../generic/enc_head.c"
    //=============> generic/enc_head.c  begine <===
    // Assume that *out is large enough to contain the output.
    // Theoretically it should be 4/3 the length of src.
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // Turn three bytes into four 6-bit numbers:
    // in[0] = 00111111
    // in[1] = 00112222
    // in[2] = 00222233
    // in[3] = 00333333

    // Duff's device, a for() loop inside a switch() statement. Legal!
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/enc_head.c  end <===

	// For supported compilers, use a hand-optimized inline assembly
	// encoder. Otherwise fall back on the SSSE3 encoder, but compiled with
	// AVX flags to generate better optimized AVX code.

#if BASE64_AVX_USE_ASM
	enc_loop_avx(&s, &slen, &o, &olen);
#else
	enc_loop_ssse3(&s, &slen, &o, &olen);
#endif

	//#include "../generic/enc_tail.c"
    //=============> generic/enc_tail.c  begine <===
            if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[*s >> 2];
            st.carry = (*s++ << 4) & 0x30;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 4)];
            st.carry = (*s++ << 2) & 0x3C;
            st.bytes++;
            olen += 1;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                break;
            }
            *o++ = base64_table_enc_6bit[st.carry | (*s >> 6)];
            *o++ = base64_table_enc_6bit[*s++ & 0x3F];
            st.bytes = 0;
            olen += 2;
        }
    }
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    //=============> generic/enc_tail.c  end <===
#else
	BASE64_ENC_STUB
#endif
}

BASE64_DEC_FUNCTION(avx)
{
#if HAVE_AVX
	//#include "../generic/dec_head.c"
    //=============> generic/dec_head.c  begine <===
    int ret = 0;
    const uint8_t *s = (const uint8_t *) src;
    uint8_t *o = (uint8_t *) out;
    uint8_t q;

    // Use local temporaries to avoid cache thrashing:
    size_t olen = 0;
    size_t slen = srclen;
    struct base64_state st;
    st.eof = state->eof;
    st.bytes = state->bytes;
    st.carry = state->carry;

    // If we previously saw an EOF or an invalid character, bail out:
    if (st.eof) {
        *outlen = 0;
        ret = 0;
        // If there was a trailing '=' to check, check it:
        if (slen && (st.eof == BASE64_AEOF)) {
            state->bytes = 0;
            state->eof = BASE64_EOF;
            ret = ((base64_table_dec_8bit[*s++] == 254) && (slen == 1)) ? 1 : 0;
        }
        return ret;
    }

    // Turn four 6-bit numbers into three bytes:
    // out[0] = 11111122
    // out[1] = 22223333
    // out[2] = 33444444

    // Duff's device again:
    switch (st.bytes)
    {
        for (;;)
        {
        case 0:
    //=============> generic/dec_head.c  end <===
	dec_loop_ssse3(&s, &slen, &o, &olen);
	//#include "../generic/dec_tail.c"
    //=============> generic/dec_tail.c  begine <===
            if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 0:
                break;
            }
            st.carry = q << 2;
            st.bytes++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 1:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.eof = BASE64_EOF;
                // Treat character '=' as invalid for byte 1:
                break;
            }
            *o++ = st.carry | (q >> 4);
            st.carry = q << 4;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 2:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes++;
                // When q == 254, the input char is '='.
                // Check if next byte is also '=':
                if (q == 254) {
                    if (slen-- != 0) {
                        st.bytes = 0;
                        // EOF:
                        st.eof = BASE64_EOF;
                        q = base64_table_dec_8bit[*s++];
                        ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                        break;
                    }
                    else {
                        // Almost EOF
                        st.eof = BASE64_AEOF;
                        ret = 1;
                        break;
                    }
                }
                // If we get here, there was an error:
                break;
            }
            *o++ = st.carry | (q >> 2);
            st.carry = q << 6;
            st.bytes++;
            olen++;

            // Deliberate fallthrough:
            BASE64_FALLTHROUGH

        case 3:	if (slen-- == 0) {
                ret = 1;
                break;
            }
            if ((q = base64_table_dec_8bit[*s++]) >= 254) {
                st.bytes = 0;
                st.eof = BASE64_EOF;
                // When q == 254, the input char is '='. Return 1 and EOF.
                // When q == 255, the input char is invalid. Return 0 and EOF.
                ret = ((q == 254) && (slen == 0)) ? 1 : 0;
                break;
            }
            *o++ = st.carry | q;
            st.carry = 0;
            st.bytes = 0;
            olen++;
        }
    }

    state->eof = st.eof;
    state->bytes = st.bytes;
    state->carry = st.carry;
    *outlen = olen;
    return ret;
    //=============> generic/dec_tail.c  end <===
#else
	BASE64_DEC_STUB
#endif
}
////=============> arch/avx/codec.c   end <=========================



////-:--:--:--:--:--:--:-- arch end --:--:--:--:--:--:--:--:--:--:--:--:--:-

////=============> codec_choose.c  begine <=========================
#include <stdbool.h>
//#include <stdint.h>
//#include <stddef.h>
#include <stdio.h>

//#include "../include/libbase64.h"
//#include "codecs.h"
//#include "config.h"
//#include "env.h"

#if (__x86_64__ || __i386__ || _M_X86 || _M_X64)
  #define BASE64_X86
  #if (HAVE_SSSE3 || HAVE_SSE41 || HAVE_SSE42 || HAVE_AVX || HAVE_AVX2 || HAVE_AVX512)
    #define BASE64_X86_SIMD
  #endif
#endif

#ifdef BASE64_X86
#ifdef _MSC_VER
	#include <intrin.h>
	#define __cpuid_count(__level, __count, __eax, __ebx, __ecx, __edx) \
	{						\
		int info[4];				\
		__cpuidex(info, __level, __count);	\
		__eax = info[0];			\
		__ebx = info[1];			\
		__ecx = info[2];			\
		__edx = info[3];			\
	}
	#define __cpuid(__level, __eax, __ebx, __ecx, __edx) \
		__cpuid_count(__level, 0, __eax, __ebx, __ecx, __edx)
#else
	#include <cpuid.h>
	#if HAVE_AVX512 || HAVE_AVX2 || HAVE_AVX
		#if ((__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 2) || (__clang_major__ >= 3))
			static inline uint64_t _xgetbv (uint32_t index)
			{
				uint32_t eax, edx;
				__asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
				return ((uint64_t)edx << 32) | eax;
			}
		#else
			#error "Platform not supported"
		#endif
	#endif
#endif

#ifndef bit_AVX512vl
#define bit_AVX512vl (1 << 31)
#endif
#ifndef bit_AVX512vbmi
#define bit_AVX512vbmi (1 << 1)
#endif
#ifndef bit_AVX2
#define bit_AVX2 (1 << 5)
#endif
#ifndef bit_SSSE3
#define bit_SSSE3 (1 << 9)
#endif
#ifndef bit_SSE41
#define bit_SSE41 (1 << 19)
#endif
#ifndef bit_SSE42
#define bit_SSE42 (1 << 20)
#endif
#ifndef bit_AVX
#define bit_AVX (1 << 28)
#endif

#define bit_XSAVE_XRSTORE (1 << 27)

#ifndef _XCR_XFEATURE_ENABLED_MASK
#define _XCR_XFEATURE_ENABLED_MASK 0
#endif

#define _XCR_XMM_AND_YMM_STATE_ENABLED_BY_OS 0x6
#endif

// Function declarations:
#define BASE64_CODEC_FUNCS(arch)	\
	BASE64_ENC_FUNCTION(arch);	\
	BASE64_DEC_FUNCTION(arch);	\

BASE64_CODEC_FUNCS(avx512)
BASE64_CODEC_FUNCS(avx2)
BASE64_CODEC_FUNCS(neon32)
BASE64_CODEC_FUNCS(neon64)
BASE64_CODEC_FUNCS(plain)
BASE64_CODEC_FUNCS(ssse3)
BASE64_CODEC_FUNCS(sse41)
BASE64_CODEC_FUNCS(sse42)
BASE64_CODEC_FUNCS(avx)

static bool
codec_choose_forced (struct codec *codec, int flags)
{
	// If the user wants to use a certain codec,
	// always allow it, even if the codec is a no-op.
	// For testing purposes.

	if (!(flags & 0xFFFF)) {
		return false;
	}

	if (flags & BASE64_FORCE_AVX2) {
		codec->enc = base64_stream_encode_avx2;
		codec->dec = base64_stream_decode_avx2;
		return true;
	}
	if (flags & BASE64_FORCE_NEON32) {
		codec->enc = base64_stream_encode_neon32;
		codec->dec = base64_stream_decode_neon32;
		return true;
	}
	if (flags & BASE64_FORCE_NEON64) {
		codec->enc = base64_stream_encode_neon64;
		codec->dec = base64_stream_decode_neon64;
		return true;
	}
	if (flags & BASE64_FORCE_PLAIN) {
		codec->enc = base64_stream_encode_plain;
		codec->dec = base64_stream_decode_plain;
		return true;
	}
	if (flags & BASE64_FORCE_SSSE3) {
		codec->enc = base64_stream_encode_ssse3;
		codec->dec = base64_stream_decode_ssse3;
		return true;
	}
	if (flags & BASE64_FORCE_SSE41) {
		codec->enc = base64_stream_encode_sse41;
		codec->dec = base64_stream_decode_sse41;
		return true;
	}
	if (flags & BASE64_FORCE_SSE42) {
		codec->enc = base64_stream_encode_sse42;
		codec->dec = base64_stream_decode_sse42;
		return true;
	}
	if (flags & BASE64_FORCE_AVX) {
		codec->enc = base64_stream_encode_avx;
		codec->dec = base64_stream_decode_avx;
		return true;
	}
	if (flags & BASE64_FORCE_AVX512) {
		codec->enc = base64_stream_encode_avx512;
		codec->dec = base64_stream_decode_avx512;
		return true;
	}
	return false;
}

static bool
codec_choose_arm (struct codec *codec)
{
#if (defined(__ARM_NEON__) || defined(__ARM_NEON)) && ((defined(__aarch64__) && HAVE_NEON64) || HAVE_NEON32)

	// Unfortunately there is no portable way to check for NEON
	// support at runtime from userland in the same way that x86
	// has cpuid, so just stick to the compile-time configuration:

	#if defined(__aarch64__) && HAVE_NEON64
	codec->enc = base64_stream_encode_neon64;
	codec->dec = base64_stream_decode_neon64;
	#else
	codec->enc = base64_stream_encode_neon32;
	codec->dec = base64_stream_decode_neon32;
	#endif

	return true;

#else
	(void)codec;
	return false;
#endif
}

static bool
codec_choose_x86 (struct codec *codec)
{
#ifdef BASE64_X86_SIMD

	unsigned int eax, ebx = 0, ecx = 0, edx;
	unsigned int max_level;

	#ifdef _MSC_VER
	int info[4];
	__cpuidex(info, 0, 0);
	max_level = info[0];
	#else
	max_level = __get_cpuid_max(0, NULL);
	#endif

	#if HAVE_AVX512 || HAVE_AVX2 || HAVE_AVX
	// Check for AVX/AVX2/AVX512 support:
	// Checking for AVX requires 3 things:
	// 1) CPUID indicates that the OS uses XSAVE and XRSTORE instructions
	//    (allowing saving YMM registers on context switch)
	// 2) CPUID indicates support for AVX
	// 3) XGETBV indicates the AVX registers will be saved and restored on
	//    context switch
	//
	// Note that XGETBV is only available on 686 or later CPUs, so the
	// instruction needs to be conditionally run.
	if (max_level >= 1) {
		__cpuid_count(1, 0, eax, ebx, ecx, edx);
		if (ecx & bit_XSAVE_XRSTORE) {
			uint64_t xcr_mask;
			xcr_mask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
			if (xcr_mask & _XCR_XMM_AND_YMM_STATE_ENABLED_BY_OS) {
				#if HAVE_AVX512
				if (max_level >= 7) {
					__cpuid_count(7, 0, eax, ebx, ecx, edx);
					if ((ebx & bit_AVX512vl) && (ecx & bit_AVX512vbmi)) {
						codec->enc = base64_stream_encode_avx512;
						codec->dec = base64_stream_decode_avx512;
						return true;
					}
				}
				#endif
				#if HAVE_AVX2
				if (max_level >= 7) {
					__cpuid_count(7, 0, eax, ebx, ecx, edx);
					if (ebx & bit_AVX2) {
						codec->enc = base64_stream_encode_avx2;
						codec->dec = base64_stream_decode_avx2;
						return true;
					}
				}
				#endif
				#if HAVE_AVX
				__cpuid_count(1, 0, eax, ebx, ecx, edx);
				if (ecx & bit_AVX) {
					codec->enc = base64_stream_encode_avx;
					codec->dec = base64_stream_decode_avx;
					return true;
				}
				#endif
			}
		}
	}
	#endif

	#if HAVE_SSE42
	// Check for SSE42 support:
	if (max_level >= 1) {
		__cpuid(1, eax, ebx, ecx, edx);
		if (ecx & bit_SSE42) {
			codec->enc = base64_stream_encode_sse42;
			codec->dec = base64_stream_decode_sse42;
			return true;
		}
	}
	#endif

	#if HAVE_SSE41
	// Check for SSE41 support:
	if (max_level >= 1) {
		__cpuid(1, eax, ebx, ecx, edx);
		if (ecx & bit_SSE41) {
			codec->enc = base64_stream_encode_sse41;
			codec->dec = base64_stream_decode_sse41;
			return true;
		}
	}
	#endif

	#if HAVE_SSSE3
	// Check for SSSE3 support:
	if (max_level >= 1) {
		__cpuid(1, eax, ebx, ecx, edx);
		if (ecx & bit_SSSE3) {
			codec->enc = base64_stream_encode_ssse3;
			codec->dec = base64_stream_decode_ssse3;
			return true;
		}
	}
	#endif

#else
	(void)codec;
#endif

	return false;
}

void
codec_choose (struct codec *codec, int flags)
{
	// User forced a codec:
	if (codec_choose_forced(codec, flags)) {
		return;
	}

	// Runtime feature detection:
	if (codec_choose_arm(codec)) {
		return;
	}
	if (codec_choose_x86(codec)) {
		return;
	}
	codec->enc = base64_stream_encode_plain;
	codec->dec = base64_stream_decode_plain;
}
////=============> codec_choose.c  end <=========================


////===================================== chief func =========================

// These static function pointers are initialized once when the library is
// first used, and remain in use for the remaining lifetime of the program.
// The idea being that CPU features don't change at runtime.
static struct codec codec = { NULL, NULL };

void
base64_stream_encode_init (struct base64_state *state, int flags)
{
	// If any of the codec flags are set, redo choice:
	if (codec.enc == NULL || flags & 0xFF) {
		codec_choose(&codec, flags);
	}
	state->eof = 0;
	state->bytes = 0;
	state->carry = 0;
	state->flags = flags;
}

void
base64_stream_encode
	( struct base64_state	*state
	, const char		*src
	, size_t		 srclen
	, char			*out
	, size_t		*outlen
	)
{
	codec.enc(state, src, srclen, out, outlen);
}

void
base64_stream_encode_final
	( struct base64_state	*state
	, char			*out
	, size_t		*outlen
	)
{
	uint8_t *o = (uint8_t *)out;

	if (state->bytes == 1) {
		*o++ = base64_table_enc_6bit[state->carry];
		*o++ = '=';
		*o++ = '=';
		*outlen = 3;
		return;
	}
	if (state->bytes == 2) {
		*o++ = base64_table_enc_6bit[state->carry];
		*o++ = '=';
		*outlen = 2;
		return;
	}
	*outlen = 0;
}

void
base64_stream_decode_init (struct base64_state *state, int flags)
{
	// If any of the codec flags are set, redo choice:
	if (codec.dec == NULL || flags & 0xFFFF) {
		codec_choose(&codec, flags);
	}
	state->eof = 0;
	state->bytes = 0;
	state->carry = 0;
	state->flags = flags;
}

int
base64_stream_decode
	( struct base64_state	*state
	, const char		*src
	, size_t		 srclen
	, char			*out
	, size_t		*outlen
	)
{
	return codec.dec(state, src, srclen, out, outlen);
}

#ifdef _OPENMP

	// Due to the overhead of initializing OpenMP and creating a team of
	// threads, we require the data length to be larger than a threshold:
	#define OMP_THRESHOLD 20000

	// Conditionally include OpenMP-accelerated codec implementations:
	//#include "lib_openmp.c"
////=============> lib_openmp.c  begine <=========================
// This code makes some assumptions on the implementation of
// base64_stream_encode_init(), base64_stream_encode() and base64_stream_decode().
// Basically these assumptions boil down to that when breaking the src into
// parts, out parts can be written without side effects.
// This is met when:
// 1) base64_stream_encode() and base64_stream_decode() don't use globals;
// 2) the shared variables src and out are not read or written outside of the
//    bounds of their parts, i.e.  when base64_stream_encode() reads a multiple
//    of 3 bytes, it must write no more then a multiple of 4 bytes, not even
//    temporarily;
// 3) the state flag can be discarded after base64_stream_encode() and
//    base64_stream_decode() on the parts.

static inline void
base64_encode_openmp
	( const char	*src
	, size_t	 srclen
	, char		*out
	, size_t	*outlen
	, int		 flags
	)
{
	size_t s;
	size_t t;
	size_t sum = 0, len, last_len;
	struct base64_state state, initial_state;
	int num_threads, i;

	// Request a number of threads but not necessarily get them:
	#pragma omp parallel
	{
		// Get the number of threads used from one thread only,
		// as num_threads is a shared var:
		#pragma omp single
		{
			num_threads = omp_get_num_threads();

			// Split the input string into num_threads parts, each
			// part a multiple of 3 bytes. The remaining bytes will
			// be done later:
			len = srclen / (num_threads * 3);
			len *= 3;
			last_len = srclen - num_threads * len;

			// Init the stream reader:
			base64_stream_encode_init(&state, flags);
			initial_state = state;
		}

		// Single has an implicit barrier for all threads to wait here
		// for the above to complete:
		#pragma omp for firstprivate(state) private(s) reduction(+:sum) schedule(static,1)
		for (i = 0; i < num_threads; i++)
		{
			// Feed each part of the string to the stream reader:
			base64_stream_encode(&state, src + i * len, len, out + i * len * 4 / 3, &s);
			sum += s;
		}
	}

	// As encoding should never fail and we encode an exact multiple
	// of 3 bytes, we can discard state:
	state = initial_state;

	// Encode the remaining bytes:
	base64_stream_encode(&state, src + num_threads * len, last_len, out + num_threads * len * 4 / 3, &s);

	// Finalize the stream by writing trailer if any:
	base64_stream_encode_final(&state, out + num_threads * len * 4 / 3 + s, &t);

	// Final output length is stream length plus tail:
	sum += s + t;
	*outlen = sum;
}

static inline int
base64_decode_openmp
	( const char	*src
	, size_t	 srclen
	, char		*out
	, size_t	*outlen
	, int		 flags
	)
{
	int num_threads, result = 0, i;
	size_t sum = 0, len, last_len, s;
	struct base64_state state, initial_state;

	// Request a number of threads but not necessarily get them:
	#pragma omp parallel
	{
		// Get the number of threads used from one thread only,
		// as num_threads is a shared var:
		#pragma omp single
		{
			num_threads = omp_get_num_threads();

			// Split the input string into num_threads parts, each
			// part a multiple of 4 bytes. The remaining bytes will
			// be done later:
			len = srclen / (num_threads * 4);
			len *= 4;
			last_len = srclen - num_threads * len;

			// Init the stream reader:
			base64_stream_decode_init(&state, flags);

			initial_state = state;
		}

		// Single has an implicit barrier to wait here for the above to
		// complete:
		#pragma omp for firstprivate(state) private(s) reduction(+:sum, result) schedule(static,1)
		for (i = 0; i < num_threads; i++)
		{
			int this_result;

			// Feed each part of the string to the stream reader:
			this_result = base64_stream_decode(&state, src + i * len, len, out + i * len * 3 / 4, &s);
			sum += s;
			result += this_result;
		}
	}

	// If `result' equals `-num_threads', then all threads returned -1,
	// indicating that the requested codec is not available:
	if (result == -num_threads) {
		return -1;
	}

	// If `result' does not equal `num_threads', then at least one of the
	// threads hit a decode error:
	if (result != num_threads) {
		return 0;
	}

	// So far so good, now decode whatever remains in the buffer. Reuse the
	// initial state, since we are at a 4-byte boundary:
	state = initial_state;
	result = base64_stream_decode(&state, src + num_threads * len, last_len, out + num_threads * len * 3 / 4, &s);
	sum += s;
	*outlen = sum;

	// If when decoding a whole block, we're still waiting for input then fail:
	if (result && (state.bytes == 0)) {
		return result;
	}
	return 0;
}

////=============> lib_openmp.c  end <=========================
#endif

void
base64_encode
	( const char	*src
	, size_t	 srclen
	, char		*out
	, size_t	*outlen
	, int		 flags
	)
{
	size_t s;
	size_t t;
	struct base64_state state;

	#ifdef _OPENMP
	if (srclen >= OMP_THRESHOLD) {
		base64_encode_openmp(src, srclen, out, outlen, flags);
		return;
	}
	#endif

	// Init the stream reader:
	base64_stream_encode_init(&state, flags);

	// Feed the whole string to the stream reader:
	base64_stream_encode(&state, src, srclen, out, &s);

	// Finalize the stream by writing trailer if any:
	base64_stream_encode_final(&state, out + s, &t);

	// Final output length is stream length plus tail:
	*outlen = s + t;
}

int
base64_decode
	( const char	*src
	, size_t	 srclen
	, char		*out
	, size_t	*outlen
	, int		 flags
	)
{
	int ret;
	struct base64_state state;

	#ifdef _OPENMP
	if (srclen >= OMP_THRESHOLD) {
		return base64_decode_openmp(src, srclen, out, outlen, flags);
	}
	#endif

	// Init the stream reader:
	base64_stream_decode_init(&state, flags);

	// Feed the whole string to the stream reader:
	ret = base64_stream_decode(&state, src, srclen, out, outlen);

	// If when decoding a whole block, we're still waiting for input then fail:
	if (ret && (state.bytes == 0)) {
		return ret;
	}
	return 0;
}
