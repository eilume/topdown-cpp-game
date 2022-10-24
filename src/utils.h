#pragma once

#include <string>

// Source: https://stackoverflow.com/a/985807
#ifdef NDEBUG
#define ASSERT(x)        \
	do {                 \
		(void)sizeof(x); \
	} while (0)
#else
#include <assert.h>
#define ASSERT(x) assert(x)
#endif

namespace Utils {
std::string PrePadString(std::string str, int goalLength, char val);
}  // namespace Utils