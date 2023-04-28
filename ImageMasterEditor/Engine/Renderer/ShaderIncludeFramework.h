#pragma once
#include "MainApp/Default_Headers.h"
#include <Utils/IM_Math.h>
#include <DirectXMath.h>
#include "ShaderIncludes.h"

// for the reflection
#pragma comment(lib, "dxguid.lib")
// learn more about com
//https://docs.microsoft.com/en-us/windows/win32/prog-dx-with-com

// give objects some names for debug
// https://walbourn.github.io/object-naming/





class FrameworkInclude : public ID3DInclude
{

	//https://github.com/TheRealMJP/BakingLab/blob/master/SampleFramework11/v1.02/Graphics/ShaderCompilation.cpp#L131
	HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
	{
		std::wstring filePath;
		if (IncludeType == D3D_INCLUDE_LOCAL)
			filePath = TAUtils::CharToWString(pFileName);
		else if (IncludeType == D3D_INCLUDE_SYSTEM)
			filePath = TAUtils::Paths::instance().ABS_Path(TAUtils::CharToWString(pFileName));
		else
			return E_FAIL;

		std::string FileData = ShaderIncludes::Instance().GetInclude(std::string(pFileName));

		if (FileData.length() == 0)
		{
			if (TAUtils::FileExist(filePath) == false)
			{
				return E_FAIL;
			}
			FileData = TAUtils::GetFileAsSting(filePath);
		}

		*pBytes = UINT(FileData.length());
		UINT8* data = reinterpret_cast<UINT8*>(std::malloc(*pBytes));
		std::memcpy(data, FileData.c_str(), *pBytes);
		*ppData = data;
		return S_OK;
	}

	HRESULT Close(LPCVOID pData) override
	{
		std::free(const_cast<void*>(pData));
		return S_OK;
	}

};
