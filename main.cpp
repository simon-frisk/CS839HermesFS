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
	HermesFS fs(dataLength * 10 * PAGE_SIZE, 10);
	fs.createDirectory("/dir");
	fs.createFile("/dir/test.txt", data, 1);

	auto start = high_resolution_clock::now();

	// Each iteration, update the file to include one more byte of data
	for (int i = 2; i <= dataLength; i++) {
		fs.appendFile("/dir/test.txt", data, i);
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
	HermesFS fs(files * PAGE_SIZE, files + 20);
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

void test1() {
	std::cout << "Running test 1 ... ";
	bool success = true;

	unsigned char data = 'a';
	unsigned char* dataBuf = &data;

	HermesFS fs(PAGE_SIZE * 100, 20);
	fs.createDirectory("/dir");
	fs.createDirectory("/dir/test1");

	fs.createFile("/dir/test2", dataBuf, 1);
	unsigned char res1;
	int rlen1;
	fs.readFile("/dir/test2", &res1, &rlen1);
	success &= res1 == 'a';
	
	fs.createFile("/dir/test1/test3", dataBuf, 1);
	unsigned char res2;
	int rlen2;
	fs.readFile("/dir/test1/test3", &res2, &rlen2);
	success &= res2 == 'a';

	fs.appendFile("/dir/test1/test3", dataBuf, 1);
	unsigned char res3[2];
	int rlen3;
	fs.readFile("/dir/test1/test3", res3, &rlen3);
	success &= res3[0] == 'a' && res3[1] == 'a';

	if (success)
		std::cout << "Success" << std::endl;
	else
		std::cout << "Fail" << std::endl;
}

int main() {
	// Tests
	test1();

	// Benchmarks for measuring performance
	manyFilesInFolderBenchMark(10000);
	manyAppendsBenchMark(10000);

	return 0;
}
