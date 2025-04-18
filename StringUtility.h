#pragma once
#include <string>
class StringUtility {
public:
	static std::wstring ConvertString(const std::string& str);
	static std::string ConvertString(const std::wstring& str);
};
