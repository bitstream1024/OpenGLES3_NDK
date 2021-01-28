//
// Created by wcg3031 on 2021/1/27.
//

#pragma once

#include <stdio.h>

class FileUtils {

public:
	static void WriteDateToFile(void* const data, const unsigned long &length, const char *const path, const bool &bAdd = false) {

		if (nullptr == data || 0 == length || nullptr == path) {
			return;
		}

		FILE* fp = nullptr;
		if (bAdd) {
			fp = fopen (path, "ab+");
		} else {
			fp = fopen (path, "wb");
		}
		if (fp) {
			fwrite(data, 1, length, fp);
			fclose(fp);
		}
	}
};