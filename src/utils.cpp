#include "utils.h"

namespace Utils {

std::string PrePadString(std::string str, int goalLength, char val) {
	if (str.length() >= goalLength) return str;

	std::string output;
	for (int i = str.length(); i < goalLength; ++i) {
		output.push_back(val);
	}

	for (int i = 0; i < str.length(); ++i) {
		output.push_back(str.c_str()[i]);
	}

	return output;
}
}  // namespace Utils