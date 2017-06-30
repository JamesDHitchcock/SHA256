#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#define ROR(v, c) (((v) >> (c)) | ((v) << ((sizeof(v) * 8) - (c))))

using namespace std;

int main(int argc, char* argv[]){
	
	//for now assume that input is string of chars
	int sLength = strlen(argv[1]);

	int len = sLength * 8;

	int k; //bits to be appended to message block
	
	k = 512 - ((len + 1 + 64) % 512);

	int noBits = len + 1 + 64 + k;
	int noBytes = (noBits / 8);

	unsigned int hash[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

	const int blo[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
	
	unsigned char *msg =  new unsigned char[noBytes];
	memset(msg, 0, noBytes );
	memcpy(msg, argv[1], sLength);
	*(msg + sLength ) = 0x80; //appends 1000000 at end of length


	//append message length at end, but it must be in big endian notation
	//where the bit at the furthest end is the smallest.
	unsigned long long int sLengthBits = 8 * sLength;

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
			//cout << hex << j << " " << "tmp1 " << tmp1 << " h " << h << " s1 " << s1 << " ch " << ch << " blo " << blo[j] << " w " << w[j] << endl;
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

	for(int i = 0; i < 8; i++){
		cout << hex << hash[i]<< " ";
	}
	cout << endl;
	delete msg;

	
	return(0);
}
