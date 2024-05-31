#pragma once
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <queue>
#include <deque>
#include <stack>
#include <cassert>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <functional>
#include <chrono>
#include <filesystem>
#include <random>
#include <mutex>
#include <bit>
#include <thread>
#include <cstring>
#include <array>
#include <numeric>

#define _USE_MATH_DEFINES // for M_PI and similar
#include <cmath>
#include <math.h>

#ifdef _DEBUG
#define ME_DEBUG
#endif

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)

#define CLAMP(val, min, max) MIN(MAX(val, min), max)

#define SGN(val) ((val > 0) - (val < 0))

#define LOG(s) { std::cout << std::dec << s << std::endl; }

// Current millisecond time
#define CUR_MS() (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())

#define STR(s) ([=]{ std::stringstream __macroStream; __macroStream << s; return __macroStream.str(); }())

#define ERR_CLOSE(s) { \
	std::stringstream _errorStream; \
	_errorStream << "FATAL ERROR: " << s; \
	LOG(_errorStream.str()); \
	throw std::runtime_error(_errorStream.str()); \
	exit(EXIT_FAILURE); \
}

#define ERR(s) throw std::runtime_error(STR(s))

/////////////////////////

// Make a member ToString() function into an ostream overload
#define PRINTABLE(type) \
friend std::ostream& operator <<(std::ostream& s, const type& val) { \
	s << val.ToString(); \
	return s; \
}

#define ASSERT assert

// Release-mode assert
#define RASSERT(cond) { if (!(cond)) { ERR("Assertion failed: \"" #cond "\""); }}