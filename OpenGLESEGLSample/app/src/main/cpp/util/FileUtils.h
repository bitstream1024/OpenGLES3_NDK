//
// Created by chauncy on 2021/1/27.
//

#pragma once

#include <stdio.h>

class FileUtils {

public:
	static void WriteDateToFile(void* const data, const unsigned long &bufferLen, const char *const path, const bool &bAdd = false) {

		if (nullptr == data || 0 == bufferLen || nullptr == path) {
			return;
		}

		FILE* fp = nullptr;
		if (bAdd) {
			fp = fopen (path, "ab+");
		} else {
			fp = fopen (path, "wb");
		}
		if (fp) {
			fwrite(data, 1, bufferLen, fp);
			fclose(fp);
		}
	}

	static void WriteDataWithFile(void* const data, const unsigned long &bufferLen, FILE* const fp) {
		if (nullptr == data || 0 == bufferLen || nullptr == fp) {
			return;
		}
		fwrite(data, 1, bufferLen, fp);
	}

	static void ReadDataFromFile(void* dstData, unsigned long &bufferLen, FILE* const fp) {
		if (nullptr == dstData || nullptr == fp) {
			return;
		}
		if (!feof(fp)) {
			size_t lSize = fread(dstData, 1, bufferLen, fp);
			// if lSize < bufferLen, means read to the end
			if (lSize < bufferLen) {
				bufferLen = lSize;
			}
		}
	}
};