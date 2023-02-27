#pragma once
#ifndef __DeltaFuzz_h__
#define __DeltaFuzz_h__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <intrin.h>
#endif

/* Definitions */

#define DEFAULT_FUZZ_POSSIBILITY 100
#define DEFAULT_UNMAP_POSSIBILITY 0

#define TRUE 1
#define FALSE 0

/* Structures and Variables */

typedef struct _FUZZER_OBJECT {
	unsigned int Seed;
	double FuzzPossibility;
	double UnmapPossibility;
	uint64_t Iterations;
	int FuzzGenerate;
	int FuzzMutate;
	int(*get_random)(void *fuzzer);
	void(*fuzz_blob)(void *fuzzer, unsigned char *blob, unsigned int size);
	unsigned int(*get_fuzzy_len)(void *fuzzer, unsigned int max);
	int (*gen_int)(void *fuzzer, unsigned long long *qp);
} FUZZER_OBJECT, *PFUZZER_OBJECT;


int wrapper_rand(FUZZER_OBJECT *fuzzer);
void _swapValues_UINT(unsigned int *a, unsigned int *b);
void _mutateNullRange(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end);
void _mutateRangeSpecial(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end);
void _mutateUnNullRange(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end);
void _mutateRangeRandom(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end);
void _mutateBitFlipping(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end);
void _singleBitFlipping(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end);
unsigned int _getRandomRange(FUZZER_OBJECT *fuzzer, unsigned int min, unsigned int max);

void CreateFuzzerObject(FUZZER_OBJECT **fuzzer, unsigned int seed, int FuzzGenerate, int FuzzMutate);
void DestroyFuzzerObject(FUZZER_OBJECT *fuzzer);

void FuzzBlob(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int size);
unsigned int get_fuzzy_len(FUZZER_OBJECT *fuzzer, unsigned int max);
int genInt(FUZZER_OBJECT *fuzzer, unsigned long long *qp);
unsigned int getOffset(FUZZER_OBJECT *fuzzer, unsigned int size, unsigned int SpaceNeeded, int *error);
unsigned char getCharDelta(FUZZER_OBJECT *fuzzer);
unsigned short getWordDelta(FUZZER_OBJECT *fuzzer);
unsigned int getDwordDelta(FUZZER_OBJECT *fuzzer);
unsigned long long getQwordDelta(FUZZER_OBJECT *fuzzer);
unsigned char mutateBit(FUZZER_OBJECT *fuzzer, unsigned char b);
unsigned char mutateBit(FUZZER_OBJECT *fuzzer, unsigned char b);
unsigned short mutateWord(FUZZER_OBJECT *fuzzer, unsigned short w);
unsigned int mutateDword(FUZZER_OBJECT *fuzzer, unsigned int dw);
unsigned long long mutateQword(FUZZER_OBJECT *fuzzer, unsigned long long qw);

int _shouldFuzz(FUZZER_OBJECT *fuzzer);

#endif



