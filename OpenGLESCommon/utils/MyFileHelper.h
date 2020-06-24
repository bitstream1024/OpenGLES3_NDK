//
// Created by wcg3031 on 2020/5/13.
//

#pragma once

#include <vector>
#include <string>

//using namespace std;

class MyFileHelper
{

public:
	static std::vector <std::string> StringSplit (const std::string str, const char cSeparator1, const char cSeparator2)
	{
		std::vector<std::string> strResult;
		std::string sTemp = "";
		bool bWithSep = false;
		for (auto val : str)
		{
			if (cSeparator1 == val || cSeparator2 == val)
			{
				strResult.push_back(sTemp);
				bWithSep = true;
				sTemp = "";
				continue;
			}
			sTemp += val;
		}
		if (bWithSep) strResult.push_back(sTemp);
		return strResult;
	}
};

