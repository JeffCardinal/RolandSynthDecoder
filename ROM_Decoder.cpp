#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// This is using the BIT and BITSWAP macros from the older C versions of MAME (this specific code in question was always BSD licensed) and is reproduced below:
// from MAME, 3bsd
#define BIT(x,n) (((x)>>(n))&1)

#define BITSWAP8(val,B7,B6,B5,B4,B3,B2,B1,B0) \
((BIT(val,B7) << 7) | \
(BIT(val,B6) << 6) | \
(BIT(val,B5) << 5) | \
(BIT(val,B4) << 4) | \
(BIT(val,B3) << 3) | \
(BIT(val,B2) << 2) | \
(BIT(val,B1) << 1) | \
(BIT(val,B0) << 0))

#define BITSWAP16(val,B15,B14,B13,B12,B11,B10,B9,B8,B7,B6,B5,B4,B3,B2,B1,B0) \
((BIT(val,B15) << 15) | \
(BIT(val,B14) << 14) | \
(BIT(val,B13) << 13) | \
(BIT(val,B12) << 12) | \
(BIT(val,B11) << 11) | \
(BIT(val,B10) << 10) | \
(BIT(val, B9) << 9) | \
(BIT(val, B8) << 8) | \
(BIT(val, B7) << 7) | \
(BIT(val, B6) << 6) | \
(BIT(val, B5) << 5) | \
(BIT(val, B4) << 4) | \
(BIT(val, B3) << 3) | \
(BIT(val, B2) << 2) | \
(BIT(val, B1) << 1) | \
(BIT(val, B0) << 0))

int main() {
	int FILE_SIZE = 524288;

	char dumped_data[FILE_SIZE];

	// Load the data into an array
	ifstream infile;
	infile.open("D-70/D-70_PCM_Wave_A_IC28.raw", ios::in | ios::binary);
	infile.read(dumped_data, FILE_SIZE);

	// The file to write to
	// ofstream output;
	FILE * output;
	output = fopen("output.bin", "w");

	for (int smpNum = 0; smpNum < FILE_SIZE; smpNum += 2) {
		// Parse out one sample
		// First descramble the sample, which while stored in big endian,
		// has the absolutely bizarre bit order of:
		// 15, 6, 14, 13, 12, 11, 10, 9, 8, 5, 4, 3, 2, 1, 0, 7
		int16_t rawSample = ( ((uint16_t)((dumped_data[smpNum]) << 8) &0xFF00) | ((uint16_t)((dumped_data[smpNum + 1]) << 0) &0xFF) );
		rawSample = BITSWAP16(rawSample, 15, 6, 14, 13, 12, 11, 10, 9, 8, 5, 4, 3, 2, 1, 0, 7);

		double floatSample = pow(2.0f,(((rawSample & 0x7fff) - 32767.0f) / 2048.0f));
		int16_t pcmSample = (floatSample * 32768.0f * ((rawSample&0x8000) ? -1.0f : 1.0f));

		cout << (pcmSample & 0xFF);
		cout << ((pcmSample >> 8) & 0xFF);
		cout << " ";
		cout << "\n";

		// Little Endian output because .wav
		fputc( (uint8_t) (pcmSample & 0xFF), output);
		fputc( (uint8_t) (pcmSample >> 8) & 0xFF, output);
	}

	return 0;
}
