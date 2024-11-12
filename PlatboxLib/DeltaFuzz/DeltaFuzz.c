#include "DeltaFuzz.h"

uint64_t g_state = 0x8644d6eb17b7ab1a;
uint64_t g_iter = 0;

uint64_t xrand() {        
	g_state ^= g_state << 13;
	g_state ^= g_state >> 17;
	g_state ^= g_state << 43;
	g_iter++;
	return g_state;
}



unsigned char special_values[] = { '<','>', '?', '>', ')', '(', '*', '&', '^', '%', '$', '#', '@', '/', '-', '+', '?', '~', '`', '|', '\\' };
unsigned char chars[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x3d, 0x3f, 0x40, 0x41, 0x7f, 0x80, 0x81, 0xfe, 0xff };
unsigned short shorts[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x3f, 0x40, 0x41, 0x7f, 0x80, 0x81, 0xff, 0x100, 0x101, 0x3fff, 0x4000, 0x4001, 0x7fff, 0x8000, 0x8001, 0xffff };
unsigned int ints[] = { 0,1,2,3,4,5,6,7,8,9,10,11,0x3f, 0x40, 0x41, 0x7f, 0x80, 0x81,  0xff, 0x100, 0x101, 0x3fff, 0x4000, 0x4001, 0x7fff, 0x8000, 0x8001, 0xffff, 0x10000, 0x10001, 0x3fffffff, 0x40000000, 0x40000001, 0x7fffffff, 0x80000000, 0x80000001, 0xffffffff };
unsigned long long qwords[] = { 0,1,2,3,4,5,6,7,8,9,10,11,0x3f, 0x40, 0x41, 0x7f, 0x80, 0x81,  0xff, 0x100, 0x101, 0x3fff, 0x4000, 0x4001, 0x7fff, 0x8000, 0x8001, 0xffff, 0x10000, 0x10001, 0x3fffffff, 0x40000000, 0x40000001, 0x7fffffff, 0x80000000, 0x80000001, 0xffffffff, 0x100000000, 0x100000001, 0x3fffffffffffffff, 0x4000000000000000, 0x4000000000000001, 0x7fffffffffffffff, 0x8000000000000000, 0x8000000000000001, 0xffffffffffffffff };

uint64_t tsc_aux = 0;

void(*fpMutatorsArray[6])(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end) = {
	&_mutateNullRange,
	&_mutateUnNullRange,
	&_mutateRangeSpecial,
	&_mutateRangeRandom,
	&_mutateBitFlipping,
	&_singleBitFlipping
};



int wrapper_rand(FUZZER_OBJECT *fuzzer) {
	fuzzer->Iterations++;
	return xrand();
}

void CreateFuzzerObject(FUZZER_OBJECT **fuzzer, uint64_t seed, int FuzzGenerate, int FuzzMutate) {
	*fuzzer = (PFUZZER_OBJECT)calloc(1, sizeof(FUZZER_OBJECT));

	#ifdef __linux__
		if (seed) {
			g_state = seed;
		} else {
			uint64_t rax,rdx;
			asm volatile ( "rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (tsc_aux) : : );
			g_state = (rdx << 32) + rax;
		}
		
	#elif _WIN32
		g_state = (seed) ? seed : __rdtscp(&tsc_aux);
	#endif
	
	(*fuzzer)->Seed = g_state;	
	(*fuzzer)->FuzzPossibility = DEFAULT_FUZZ_POSSIBILITY;
	(*fuzzer)->UnmapPossibility = DEFAULT_UNMAP_POSSIBILITY;
	(*fuzzer)->Iterations = 0;
	(*fuzzer)->FuzzGenerate = FuzzGenerate;
	(*fuzzer)->FuzzMutate = FuzzMutate;
	(*fuzzer)->get_random = (void *)&wrapper_rand;
	(*fuzzer)->fuzz_blob = (void *)&FuzzBlob;
	(*fuzzer)->get_fuzzy_len = (void *)&get_fuzzy_len;
	(*fuzzer)->gen_int = (void *)&genInt;
}

void DestroyFuzzerObject(FUZZER_OBJECT *fuzzer) {
	free(fuzzer);
}


int genInt(FUZZER_OBJECT *fuzzer, unsigned long long *qp) {
	switch (fuzzer->get_random(fuzzer) % 4) {
	case 0:
		switch (fuzzer->get_random(fuzzer) % 2) {
		case 0:
			*qp = special_values[fuzzer->get_random(fuzzer) % (sizeof(special_values) / sizeof(special_values[0]))];
			break;
		case 1:
			*qp = chars[fuzzer->get_random(fuzzer) % sizeof(chars)];
			break;
		}
		return 1;
		break;
	case 1:
		*qp = shorts[fuzzer->get_random(fuzzer) % (sizeof(shorts) / sizeof(shorts[0]))];
		return 2;
		break;
	case 2:
		*qp = ints[fuzzer->get_random(fuzzer) % (sizeof(ints) / sizeof(ints[0]))];
		return 4;
		break;
	case 3:
		*qp = qwords[fuzzer->get_random(fuzzer) % (sizeof(qwords) / sizeof(qwords[0]))];
		return 8;
		break;
	}
	return -1;
}

unsigned int getOffset(FUZZER_OBJECT *fuzzer, unsigned int size, unsigned int SpaceNeeded, int *error) {
	if (SpaceNeeded > size || SpaceNeeded == 0) {
		*error = TRUE;
		return 0;
	}

	unsigned int maxOffset = size - (SpaceNeeded - 1);
	return fuzzer->get_random(fuzzer) % maxOffset;
}

unsigned char getCharDelta(FUZZER_OBJECT *fuzzer) {
	unsigned char c = 0;
	switch (fuzzer->get_random(fuzzer) % 10) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		c = fuzzer->get_random(fuzzer) % 10;
		break;
	case 9:
		switch (fuzzer->get_random(fuzzer) % 10) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			c = fuzzer->get_random(fuzzer) % 20;
			break;
		case 5:
		case 6:
		case 7:
			c = fuzzer->get_random(fuzzer) % 50;
			break;
		case 8:
			c = fuzzer->get_random(fuzzer) % 100;
			break;
		case 9:
			c = fuzzer->get_random(fuzzer) % 200;
			break;
		}
		break;
	}
	return c;
}

unsigned short getWordDelta(FUZZER_OBJECT *fuzzer) {
	unsigned short w = 0;
	if (fuzzer->get_random(fuzzer) % 10) {
		return getCharDelta(fuzzer);
	}
	switch (fuzzer->get_random(fuzzer) % 10) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		w = fuzzer->get_random(fuzzer) % 0x100;
		break;
	case 5:
	case 6:
	case 7:
		w = fuzzer->get_random(fuzzer) % 0x1000;
		break;
	case 8:
	case 9:
		w = fuzzer->get_random(fuzzer) % 0x10000;
		break;
	}
	return w;
}

unsigned int getDwordDelta(FUZZER_OBJECT *fuzzer) {
	unsigned int dw = 0;
	if (fuzzer->get_random(fuzzer) % 100) {
		return getWordDelta(fuzzer);
	}
	switch (fuzzer->get_random(fuzzer) % 10) {
	case 0:
	case 1:
	case 2:
		dw = fuzzer->get_random(fuzzer) % 0x100000;
		break;
	case 3:
	case 4:
	case 5:
		dw = fuzzer->get_random(fuzzer) % 0x1000000;
		break;
	case 6:
	case 7:
	case 8:
		dw = fuzzer->get_random(fuzzer) % 0x10000000;
		break;
	case 9:
		dw = fuzzer->get_random(fuzzer);
		break;

	}
	return dw;
}


unsigned long long getQwordDelta(FUZZER_OBJECT *fuzzer) {
	if (fuzzer->get_random(fuzzer) % 1000) {
		return getDwordDelta(fuzzer);
	}

	unsigned long long qw = fuzzer->get_random(fuzzer);
	qw <<= 32;
	qw |= fuzzer->get_random(fuzzer);

	switch (fuzzer->get_random(fuzzer) % 9) {
	case 0:
	case 1:
		qw %= 0x1000000000;
		break;
	case 2:
		qw %= 0x10000000000;
		break;
	case 3:
		qw %= 0x100000000000;
		break;
	case 4:
		qw %= 0x1000000000000;
		break;
	case 5:
		qw %= 0x10000000000000;
		break;
	case 6:
		qw %= 0x100000000000000;
		break;
	case 7:
		qw %= 0x1000000000000000;
		break;
	case 8:
		qw = qw;
		break;
	}
	return qw;
}





unsigned char mutateBit(FUZZER_OBJECT *fuzzer, unsigned char b) {
	b ^= 1 << (fuzzer->get_random(fuzzer) % 8);
	return b;

}

unsigned char mutateChar(FUZZER_OBJECT *fuzzer, unsigned char c) {
	unsigned char delta = getCharDelta(fuzzer);
	if (fuzzer->get_random(fuzzer) % 2) {
		c += delta;
	}
	else {
		c -= delta;
	}
	return c;
}

unsigned short mutateWord(FUZZER_OBJECT *fuzzer, unsigned short w) {
	unsigned short delta = getWordDelta(fuzzer);
	if (fuzzer->get_random(fuzzer) % 2) {
		w += delta;
	}
	else {
		w -= delta;
	}
	return w;
}

unsigned int mutateDword(FUZZER_OBJECT *fuzzer, unsigned int dw) {
	unsigned int delta = getDwordDelta(fuzzer);
	if (fuzzer->get_random(fuzzer) % 2) {
		dw += delta;
	}
	else {
		dw -= delta;
	}
	return dw;

}

unsigned long long mutateQword(FUZZER_OBJECT *fuzzer, unsigned long long qw) {
	unsigned long long delta = getQwordDelta(fuzzer);
	if (fuzzer->get_random(fuzzer) % 2) {
		qw += delta;
	}
	else {
		qw -= delta;
	}
	return qw;
}



void FuzzOnce(FUZZER_OBJECT *fuzzer, unsigned char *data, unsigned int size) {
	unsigned char c, *cp;
	unsigned short w, *wp;
	unsigned int dw, *dwp;
	unsigned long long qw, *qwp;
	unsigned int offset;
	int error = 0;
	/*
	- known ints  (4/10)
	- random value (1/10)
	- mutate value (5/10)
	*/
	cp = (unsigned char *)data;
	switch (fuzzer->get_random(fuzzer) % 10) {
	case 0:
	case 1:
	case 2:
	case 3:
		switch (fuzzer->get_random(fuzzer) % 4) {
		case 0: // char
			switch (fuzzer->get_random(fuzzer) % 2) {
			case 0: //Normal
				c = chars[fuzzer->get_random(fuzzer) % (sizeof(chars) / sizeof(chars[0]))];
				break;
			case 1: // Special
				c = special_values[fuzzer->get_random(fuzzer) % (sizeof(special_values) / sizeof(special_values[0]))];
			}
			offset = getOffset(fuzzer, size, sizeof(c), &error);
			if (error) return;
			cp += offset;
			*cp = c;
			break;
		case 1: // word
			w = shorts[fuzzer->get_random(fuzzer) % (sizeof(shorts) / sizeof(shorts[0]))];
			offset = getOffset(fuzzer, size, sizeof(w), &error);
			if (error) return;
			cp += offset;
			wp = (unsigned short *)cp;
			*wp = w;
			break;
		case 2: // dword
			dw = ints[fuzzer->get_random(fuzzer) % (sizeof(ints) / sizeof(ints[0]))];
			offset = getOffset(fuzzer, size, sizeof(dw), &error);
			if (error) return;
			cp += offset;
			dwp = (unsigned int *)cp;
			*dwp = dw;
			break;
		case 3: // qword
			qw = qwords[fuzzer->get_random(fuzzer) % (sizeof(qwords) / sizeof(qwords[0]))];
			offset = getOffset(fuzzer, size, sizeof(qw), &error);
			if (error) return;
			cp += offset;
			qwp = (unsigned long long *) cp;
			*qwp = qw;
			break;
		}
		break;
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		switch (fuzzer->get_random(fuzzer) % 5) {
		case 0:
			offset = getOffset(fuzzer, size, sizeof(c), &error);
			cp += offset;
			c = *cp;
			c = mutateBit(fuzzer, c);
			*cp = c;
			break;
		case 1:
			offset = getOffset(fuzzer, size, sizeof(c), &error);
			cp += offset;
			c = *cp;
			c = mutateChar(fuzzer, c);
			*cp = c;
			break;
		case 2:
			offset = getOffset(fuzzer, size, sizeof(w), &error);
			cp += offset;
			wp = (unsigned short *)cp;
			w = *wp;
			w = mutateWord(fuzzer, w);
			*wp = w;
			break;
		case 3:
			offset = getOffset(fuzzer, size, sizeof(dw), &error);
			cp += offset;
			dwp = (unsigned int *)cp;
			dw = *dwp;
			dw = mutateDword(fuzzer, dw);
			*dwp = dw;
			break;
		case 4:
			offset = getOffset(fuzzer, size, sizeof(qw), &error);
			cp += offset;
			qwp = (unsigned long long *)cp;
			qw = *qwp;
			qw = mutateQword(fuzzer, qw);
			*qwp = qw;

			break;
		}
		break;
	case 9:
		switch (fuzzer->get_random(fuzzer) % 4) {
		case 0: // byte
			c = fuzzer->get_random(fuzzer) % 256;
			offset = getOffset(fuzzer, size, sizeof(c), &error);
			if (error) return;
			cp += offset;
			*cp = c;
			break;
		case 1: // word 
			w = fuzzer->get_random(fuzzer) % 0x10000;
			offset = getOffset(fuzzer, size, sizeof(w), &error);
			if (error) return;
			cp += offset;
			wp = (unsigned short *)cp;
			*wp = w;
			break;
		case 2: // dword
			dw = fuzzer->get_random(fuzzer);
			offset = getOffset(fuzzer, size, sizeof(dw), &error);
			if (error) return;
			cp += offset;
			dwp = (unsigned int *)cp;
			*dwp = dw;
			break;
		case 3:  // qword
			qw = fuzzer->get_random(fuzzer);
			qw <<= 32;
			qw |= fuzzer->get_random(fuzzer);
			offset = getOffset(fuzzer, size, sizeof(qw), &error);
			if (error) return;
			cp += offset;
			qwp = (unsigned long long *) cp;
			*qwp = qw;
			break;
		}
		break;
	}

	return;
}

void _mutateNullRange(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end)
{
	memset(blob + start, 0, (end - start));
}

void _mutateUnNullRange(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end) {
	int i;
	for (i = start; i <= end; i++) {
		if (blob[i] == '\x00') {
			blob[i] = (unsigned char)(fuzzer->get_random(fuzzer) % (255 + 1));
		}
	}
}

void _mutateRangeSpecial(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end) {
	int i;
	for (i = start; i <= end; i++) {
		blob[i] = special_values[fuzzer->get_random(fuzzer) % sizeof(special_values)];
	}
}

void _mutateRangeRandom(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end) {
	int i;
	for (i = start; i <= end; i++) {
		blob[i] = (unsigned char)(fuzzer->get_random(fuzzer) % (255 + 1));
	}
}

void _singleBitFlipping(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end) {
	unsigned int bit_to_change = fuzzer->get_random(fuzzer) % 8;
	unsigned int target_byte = fuzzer->get_random(fuzzer) % (end - start);
	blob[target_byte] = blob[target_byte] ^ (1 << bit_to_change);
}

void _mutateBitFlipping(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int start, unsigned int end) {
	unsigned int bits_to_change = _getRandomRange(fuzzer, 0, (end - start) * 8);
	unsigned int index;
	int i;
	unsigned char bit;

	for (i = 0; i < bits_to_change; i++) {
		index = fuzzer->get_random(fuzzer) % (end - start); // random index
		bit = 1 << (fuzzer->get_random(fuzzer) % 8);  // random bit
		blob[start + index] ^= bit;
	}
}

unsigned int get_fuzzy_len(FUZZER_OBJECT *fuzzer, unsigned int max) {
	unsigned int len; // = rand() % max;
	switch (fuzzer->get_random(fuzzer) % 5) {
	case 0:
	case 3:
	case 4:
		len = fuzzer->get_random(fuzzer) % 9;
		break;
	case 1:
		len = fuzzer->get_random(fuzzer) & 256;
		break;
	case 2:
		len = fuzzer->get_random(fuzzer) % max;
		break;
	}
	return len;
}


/** FUZZER PRIMITIVES **/

void _performBlobFuzzing(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int size) {
	void(*fpMutator)(FUZZER_OBJECT *, unsigned char *, unsigned int, unsigned int);
	unsigned int start = fuzzer->get_random(fuzzer) % size;
	unsigned int end = fuzzer->get_random(fuzzer) % size;
	// Cannot be equal
	while (start == end) {
		end = fuzzer->get_random(fuzzer) % size;
	}
	if (start > end) {
		_swapValues_UINT(&start, &end);
	}

	fpMutator = fpMutatorsArray[fuzzer->get_random(fuzzer) % (sizeof(fpMutatorsArray) / sizeof(void *))];
	fpMutator(fuzzer, blob, start, end);
}

unsigned char fill_values[] = {
	0x00, 0x01, 0x02, 0x03, 0x07, 0x0A, 0x0C,
	0x1F, 0x2F, 0x3F, 0x4F, 0x6F, 0x7F, 0x40,
	0x80, 0x0F, 0xCC, 0x90, 0xFF, 0xFE, 0x41 
};

void generate_content(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int size) {

	if ((fuzzer->get_random(fuzzer) % 100) < 20) {
		memset(blob, fill_values[fuzzer->get_random(fuzzer) % sizeof(fill_values)], size);
	}
	else {
		unsigned long long qw;
		int ilen = 0;
		int i = 0;
		for (i = 0; i < size;) {
			do {
				ilen = genInt(fuzzer, &qw);
			} while (ilen >(size - i));

			switch (ilen) {
			case 1:
				*(char *)(blob + i) = (unsigned char)qw;
				i++;
				break;
			case 2:
				*(short *)(blob + i) = (unsigned short)qw;
				i += sizeof(short);
				break;
			case 4:
				*(int *)(blob + i) = (unsigned int)qw;
				i += sizeof(int);
				break;
			case 8:
				*(unsigned long long *)(blob + i) = (unsigned long long)qw;
				i += sizeof(unsigned long long);
				break;
			}
		}
	}

}

void FuzzBlob(FUZZER_OBJECT *fuzzer, unsigned char *blob, unsigned int size) {
	if (size > 0 && blob != NULL) {
		//if (((int)size) < 0) {
		//	__debugbreak();
		//}
		if (_shouldFuzz(fuzzer))
		{
			if (fuzzer->FuzzGenerate) {
				generate_content(fuzzer, blob, size);
			}

			switch (fuzzer->get_random(fuzzer) % 2) {
			case 0:
				if (size > 1) {
					_performBlobFuzzing(fuzzer, blob, size);
				}
				else {
					FuzzOnce(fuzzer, blob, size);
				}
				break;
			case 1:
				FuzzOnce(fuzzer, blob, size);
				break;
			}
		}
	}
}






/** FUZZER_OBJECT Functions **/

int _shouldFuzz(FUZZER_OBJECT *fuzzer) {
	double chance = fuzzer->get_random(fuzzer) % 100;
	if (chance < fuzzer->FuzzPossibility) {
		return TRUE;
	}
	return FALSE;
}

int _shouldUnmap(FUZZER_OBJECT *fuzzer) {
	double chance = fuzzer->get_random(fuzzer) % 100;
	if (chance < fuzzer->UnmapPossibility) {
		return TRUE;
	}
	return FALSE;
}



unsigned int _getRandomRange(FUZZER_OBJECT *fuzzer, unsigned int min, unsigned int max)
{
	return (unsigned int)((fuzzer->get_random(fuzzer) % ((max - min) + 1)) + min);
}

void _swapValues_UINT(unsigned int *a, unsigned int *b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

/*
int WINAPI DllMain(HINSTANCE hDllHandle, int nReason, LPVOID Reserved) {
switch (nReason)
{
case DLL_PROCESS_ATTACH:
case DLL_PROCESS_DETACH:
case DLL_THREAD_ATTACH:
case DLL_THREAD_DETACH:
break;
}
return TRUE;
}

int main(void) {
RunTests();
}
*/
