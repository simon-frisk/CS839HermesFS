#include <iostream>
#include <chrono>
#include "hermesfs.h"
#include "util.h"

using namespace std::chrono;


void manyAppendsBenchMark(int dataLength) {
	std::cout << "Running many file appends benchmark with " << dataLength << " bytes ... ";
	// Data to write
	unsigned char* data = new unsigned char[dataLength];
	// Set up fs
	HermesFS fs(dataLength * 10, 10);
	fs.createDirectory("/dir");
	fs.createFile("/dir/test.txt", data, 1);

	auto start = high_resolution_clock::now();

	// Each iteration, update the file to include one more byte of data
	for (int i = 2; i <= dataLength; i++) {
		fs.updateFile("/dir/test.txt", data, i);
	}

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start).count();

	std::cout << "Benchmark took: " << duration << "ms" << std::endl;
}

void manyFilesInFolderBenchMark(int files) {
	std::cout << "Running many nested files benchmark with " << files << " files ... ";
	unsigned char data = 'a';
	unsigned char* dataBuf = &data;
	// Set up fs
	HermesFS fs(files * 20, files + 20);
	fs.createDirectory("/dir");

	auto start = high_resolution_clock::now();

	for (int i = 0; i < files; i++) {
		std::string path = "/dir/file" + std::to_string(i);
		fs.createFile(path, dataBuf, 1);
	}

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start).count();

	std::cout << "Benchmark took: " << duration << "ms" << std::endl;
}

int main() {
	manyFilesInFolderBenchMark(10000);
	manyAppendsBenchMark(10000);

	return 0;
}
