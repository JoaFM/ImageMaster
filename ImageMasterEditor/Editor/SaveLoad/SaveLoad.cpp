#include "pch.h"
#include "SaveLoad.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define __STDC_LIB_EXT1__
#include "External/STB_libs/stb_image_write.h"
#include "External/STB_libs/stb_image.h"

bool SaveLoad::Save(char const* filename, int x, int y, const void* Bytedata , const float* FloatData)
{
	std::filesystem::path path = filename;
	if (path.extension() == ".hdr")
	{
		stbi_write_hdr(filename, x, y, 4, FloatData);
	}
	else if (path.extension() == ".png")
	{
		if (Bytedata) { stbi_write_png(filename, x, y, 4, Bytedata, x * 4 * 4); return true; }

		std::vector<UINT8> NewByteData(x*y*4,0);
		for (INT32 py = 0; py < y; py++)
		{
			for (INT32 px = 0; px < x; px++)
			{
				const INT32 index = (px + (py * x)) *4;
				const float readPixel = FloatData[index];
				NewByteData[index]		= (UINT8)std::clamp(FloatData[index] * 255, 0.0f, 255.0f);
				NewByteData[index + 1] = (UINT8)std::clamp(FloatData[index + 1] * 255, 0.0f, 255.0f);
				NewByteData[index + 2] = (UINT8)std::clamp(FloatData[index + 2] * 255, 0.0f, 255.0f);
				NewByteData[index + 3] = (UINT8)std::clamp(FloatData[index + 3] * 255, 0.0f, 255.0f);
			}
		}
		
		stbi_write_png(filename, x, y, 4, &NewByteData[0], x * 4); return true;

	}
	return true;

}
unsigned char* SaveLoad::Load(char const* filename, int* Width, int* heght, int* comp, int req_comp)
{
	return stbi_load(filename, Width, heght, comp, req_comp) ;
}

void SaveLoad::Load_FreeData(unsigned char* image_data)
{
	stbi_image_free(image_data);
}
