#pragma once
#include "MainApp/Default_Headers.h"

class SaveLoad
{
public:

	static bool Save(char const* filename, int x, int y, const void* Bytedata = nullptr, const float* FloatData = nullptr);

};
