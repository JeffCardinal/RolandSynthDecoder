#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

int main() {
	FILE * file_path = fopen("D-50/Roland-D50-PCM-ROM-B-IC29.bin", "r");
	if (file_path == NULL) perror ("Error opening file");
	int FILE_SIZE = 524288;
	int16_t sample_data[FILE_SIZE / 2];

	// The file to write to
	ofstream output;

	// Main decode loop
	for (int i = 0; i < FILE_SIZE / 2; i++) {
		int16_t raw_sample;

		*(((int8_t *) &raw_sample) + 1) = fgetc(file_path);
		*(((int8_t *) &raw_sample) + 0) = fgetc(file_path);

		int16_t ordered_sample;

		// the bits have to be moved to these positions
		// 15, 06, 14, 13, 12, 11, 10, 09, 08, 05, 04, 03, 02, 01, 00, 07
		// EG: bit 15 stays, bit 6 moves to 14, bit 14 moves to bit 13...

		ordered_sample = raw_sample & 0x8000;
		ordered_sample |= raw_sample << 8 & 0x4000;
		ordered_sample |= raw_sample >> 1 & 0x3F80;
		ordered_sample |= raw_sample << 1 & 0x007E;

		// decode the data
		double float_sample = pow(2.0f, (((ordered_sample & 0x7FFF) - 32767.0f) / 2048.0f));
		sample_data[i] = (int16_t)(float_sample * 32768.0f * ((ordered_sample & 0x8000) ? -1.0f : 1.0f));
	}

	// Open the output file and write to it
	output.open ("output.txt");
	for (int i = (FILE_SIZE / 2) - 1; i >= 0; i--) {
    	output << std::setfill('0') << std::setw(2) << std::hex << (0xFF & sample_data[i]);
	}
	output.close();

	return 0;
}
