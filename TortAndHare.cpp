#include <map>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#define ROR(v, c) (((v) >> (c)) | ((v) << ((sizeof(v) * 8) - (c))))

using namespace std;

int BIT;
int NUMBYTES;//to be defined by bit and used in cmpBytes

void sha256(unsigned char * s, int bitLen, unsigned char * cont);
void printBytes(unsigned char * s, int byteLen);

int main(int argc, char* argv[]){
	unsigned char begin[13] = "JamesMarch";

	BIT = atoi(argv[1]);//number of bits to run for part 3

	NUMBYTES = BIT/8;
	if(BIT % 8 != 0){
		NUMBYTES++;
	}
	
	unsigned char * start = new unsigned char[NUMBYTES];
	memcpy(start, begin, NUMBYTES);
	
	for(int i = 0; i <= (7 - (BIT % 8)); i++){
		*(start + (BIT/8) ) &= ~(1 << i);
	}
	//start now holds hash starting points
	
	unsigned char * tort = new unsigned char[NUMBYTES];
	unsigned char * hare = new unsigned char[NUMBYTES];

	//tort moved one hash at a time, hare moved two
	sha256(start, BIT, tort);
	sha256(start, BIT, hare);
	sha256(hare, BIT, hare);

	while( memcmp(tort, hare, NUMBYTES) != 0){
		sha256(tort, BIT, tort);
		sha256(hare, BIT, hare);
		sha256(hare, BIT, hare);
	}

	unsigned char * prev1 = new unsigned char[NUMBYTES];
	unsigned char * prev2 = new unsigned char[NUMBYTES];
	
	memcpy(prev1, start, NUMBYTES);
	memcpy(prev2, hare, NUMBYTES);
	sha256(start, BIT, tort);
	sha256(hare, BIT, hare);
	while(memcmp(tort, hare, NUMBYTES) != 0){
		memcpy(prev1, start, NUMBYTES);
		memcpy(prev2, hare, NUMBYTES);
		sha256(tort, BIT, tort);
		sha256(hare, BIT, hare);
	}

	cout << "X: ";
	printBytes(prev1, NUMBYTES);
	cout << endl << "X': ";
	printBytes(prev2, NUMBYTES);
	cout << endl << "H(X): ";
	printBytes(tort, NUMBYTES);
	cout << endl << "H(X'): ";
	printBytes(hare, NUMBYTES);
	cout << endl;

	return(0);
}


void sha256(unsigned char * s, int bitLen, unsigned char* cont){
	//res should be an unsigned int array of size 8
	
	int sLength, len;

	sLength = bitLen / 8;
	if(bitLen % 8 != 0){
		sLength++;
	}
	len = bitLen;

	int k; //bits to be appended to message block
	
	k = 512 - ((len + 1 + 64) % 512);

	int noBits = len + 1 + 64 + k;
	int noBytes = (noBits / 8);

	unsigned int hash[8];
	hash[0] = 0x6a09e667;
	hash[1] = 0xbb67ae85;
	hash[2] = 0x3c6ef372;
	hash[3] = 0xa54ff53a;
	hash[4] = 0x510e527f;
	hash[5] = 0x9b05688c;
	hash[6] = 0x1f83d9ab;
	hash[7] = 0x5be0cd19;

	const int blo[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
	
	unsigned char *msg =  new unsigned char[noBytes];
	memset(msg, 0, noBytes );
	memcpy(msg, s, sLength);



	*(msg + (bitLen/8) ) |= (1 << (7 - (bitLen % 8) ) );

	for(int i = 0; i <= (6 - (bitLen % 8)); i++){
		*(msg + (bitLen/8) ) &= ~(1 << i);
	}


	//*(msg + (bitLen) ) &= 
	//*(msg + sLength ) = 0x80; //appends 1000000 at end of length


	//append message length at end, but it must be in big endian notation
	//where the bit at the furthest end is the smallest.
	unsigned long long int sLengthBits = len;

	for(int i = 1; i < 9; i++){
		//each shift you take a byte from sLengthBits
		//shifting by 8 means you move 1 byte over in sLength
		*(msg + noBytes - i) = ( sLengthBits >> (8 * (i - 1) ) );
	}

	//end of pre-processsing

	//split into 512 blocks and apply operations

	unsigned int w[64]; //32 bit splits
	unsigned int a, b, c, d, e, f, g, h, s0, s1, ch, maj, tmp1, tmp2;

	int chunks = noBits / 512;
	unsigned char * chkp = msg;//will traverse the msg and be up to date with chunks

	for(int i = 0; i < chunks; i++){
		unsigned char* tmpb = (unsigned char*) w;

		//code is for little endian machines.
		//you move the memory bits into their proper order from the memory 
		//of the individual bytes 
		//Don't try this shit on big endian
		//big endian just do a memcpy

		for(int j = 0; j < 64; j += 4){
			tmpb[j] = chkp[j+3];
			tmpb[j+1] = chkp[j+2];
			tmpb[j+2] = chkp[j+1];
			tmpb[j+3] = chkp[j];
		}
		chkp = chkp + 64; //updates the chunk pointer to move to the next chunk

		for(int j = 16; j < 64; j++){
			s0 =  ROR(w[j-15], 7)  ^  ROR(w[j-15], 18)  ^ (w[j-15] >> 3);
			s1 =  ROR(w[j-2], 17)  ^  ROR(w[j-2], 19)  ^ (w[j-2] >> 10);
			w[j] = (w[j-16] + s0 + w[j-7] + s1);
		}
	
		a = hash[0];
		b = hash[1];
		c = hash[2];
		d = hash[3];
		e = hash[4];
		f = hash[5];
		g = hash[6];
		h = hash[7];

		for(int j = 0; j < 64; j++){
			s1 = ROR(e, 6) ^ ROR(e, 11) ^ ROR(e, 25);
			ch = (e & f) ^ ( (~e) & g);
			tmp1 = h + s1 + ch + blo[j] + w[j];
			s0 = ROR(a, 2) ^ ROR(a, 13) ^ ROR(a, 22);
			maj = (a & b) ^ (a & c) ^ (b & c);
			tmp2 = s0 + maj;
	
			h = g;
			g = f;
			f = e;
			e = d + tmp1;
			d = c;
			c = b;
			b = a;
			a = tmp1 + tmp2;
		}

		hash[0] = hash[0] + a;
		hash[1] = hash[1] + b;
		hash[2] = hash[2] + c;
		hash[3] = hash[3] + d;
		hash[4] = hash[4] + e;
		hash[5] = hash[5] + f;
		hash[6] = hash[6] + g;
		hash[7] = hash[7] + h;

	}

	unsigned char * tra = (unsigned char *) hash;

	int i = 1;
	for(int j = 0; j < NUMBYTES; j++){
		cont[j] = tra[3+(4*(j/4))-(j%4)];
	}
	// j/4 is integerdivision to show how many times we've looped through the
	// number 4 (as we only have 4 bytes for the integer before we come to the
	// next integer set where we need its most significant digit first again)
	// we start at 3 before for j = 0, we want the 3rd byte (the most sig.
	// digit). But for j = 1, we need the second byte. So every 4 bytes we need
	// to grab the digit that is 3 away from our current j value and then we
	//count down.
	// j | n | j | n  etc...
	// 0   3 | 4   7
	// 1   2 | 5   6
	// 2   3 | 6   5
	// 3   0 | 7   4
	//I just wanted to come up with a way of doing lines 154 to 157 in a single line for fun.


	//sets any extra bits in the bytes to 0
	
	for(int i = 0; i <= (7 - (BIT % 8)); i++){
		*(cont + (BIT/8) ) &= ~(1 << i);
	}

	delete [] msg;
}

void printBytes(unsigned char *s, int byteLen){
	for(int i = 0; i < byteLen; i++){
		cout << hex << (int)s[i];
		if(i % 4 == 0 && i != 0 && i){
			cout << " ";
		}
	}
	cout << endl;
}
