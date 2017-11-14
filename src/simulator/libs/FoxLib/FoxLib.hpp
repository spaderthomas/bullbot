//Includes all of FoxLib
#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <new>

#ifdef _WIN32
#	include <Windows.h>
#endif

namespace foxlib {
#include "Macros.hpp"
#include "Types.hpp"
#include "Array.hpp"
#include "String.hpp"
#include "Allocator.hpp"
#include "ADT.hpp"
#include "Log.hpp"
}

