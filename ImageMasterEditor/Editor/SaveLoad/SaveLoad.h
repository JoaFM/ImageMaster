#pragma once
#include "MainApp/Default_Headers.h"

class SaveLoad
{
public:

	static bool Save(char const* filename, int x, int y, const void* Bytedata = nullptr, const float* FloatData = nullptr);
	static unsigned char* Load(char const* filename, int* Width, int* heght, int* comp, int req_comp);
	static void Load_FreeData(unsigned char* image_data);
};
