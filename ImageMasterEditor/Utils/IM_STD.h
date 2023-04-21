#pragma once


#include <iostream>
#include <string>
#include  <io.h>
#include <fstream>
#include <streambuf>
#include <windows.h>
#include <algorithm>
#include <d3d11.h>
class TA_STD
{
};

class TAError
{

public:

	static TAError& Instance()
	{
		static TAError  instance; // Guaranteed to be destroyed.
								 // Instantiated on first use.
		return instance;
	}
	TAError(TAError const&) = delete;
	void operator=(TAError const&) = delete;




	void ErrorWS(std::wstring ErrorToAdd) const ;
	void ErrorC(char* ErrorToAdd) const;

	void WarnWS(std::wstring ErrorToAdd);
	void WarnC(char* ErrorToAdd);

	void LogWS(std::wstring ErrorToAdd);
	void LogC(char* ErrorToAdd);

	std::wstring GetAndConsumeErrorBuffer();

private:
	TAError() {}
	std::wstring m_CurrentErrorBuffer;

};


//TAERROR_WS(L"My error");
#define TA_ERROR_WS(_phrase_) TAError::Instance().ErrorWS( _phrase_ )
#define TA_ERROR_C(_phrase_) TAError::Instance().ErrorC( _phrase_ )

#define TA_WARN_WS(_phrase_) TAError::Instance().WarnWS( _phrase_ )
#define TA_WARN_C(_phrase_) TAError::Instance().WarnC( _phrase_ )

#define TA_LOG_WS(_phrase_) TAError::Instance().LogWS( _phrase_ )
#define TA_LOG_C(_phrase_) TAError::Instance().LogC( _phrase_ )


#define TA_SAFERELEASE(_phrase_) if(_phrase_ != nullptr){_phrase_ ->Release(); _phrase_= nullptr; }

#define TA_HRCHECK(_phrase_, _Error_)  	\
{										\
	HRESULT TA_HRCHECK_HR = _phrase_; 	\
	if (FAILED(TA_HRCHECK_HR))			\
	{									\
		TA_ERROR_WS(_Error_);			\
		LoadedAndValid = false;			\
		return false;					\
										\
	}									\
}		

#define TA_HRCHECK_Simple(_phrase_, _Error_)  	\
{										\
	HRESULT TA_HRCHECK_HR = _phrase_; 	\
	if (FAILED(TA_HRCHECK_HR))			\
	{									\
		TA_ERROR_WS(_Error_);			\
	}									\
}



namespace TAUtils
{


	static std::string RandomString(const int len)
	{
		static const char alphanum[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::string tmp_s;
		tmp_s.reserve(len);

		for (int i = 0; i < len; ++i) {
			tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
		}
		return tmp_s;
	}


	static std::string WStringToChar(const WCHAR* wideString)
	{
		char buffer[512];
		WideCharToMultiByte(CP_ACP, 0, wideString, -1, buffer, 512, NULL, NULL);
		return std::string(buffer);
	}
	static std::wstring CharToWString(const char* ansiString)
	{
		WCHAR WidePathPtr[255];
		int size = MultiByteToWideChar(CP_ACP, 0, (char*)ansiString, -1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, (char*)ansiString, -1, (LPWSTR)WidePathPtr, size);
		LPCWSTR NewPath = LPCWSTR(WidePathPtr);
		return std::wstring(WidePathPtr);
	}



	static std::string GetFileAsSting(const std::wstring& Filename)
	{
		std::ifstream t(Filename);
		std::string ResultString;

		t.seekg(0, std::ios::end);
		ResultString.reserve(t.tellg());
		t.seekg(0, std::ios::beg);

		ResultString.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		return ResultString;
	}

	static bool FileExist(const std::wstring& Filename)
	{
		//https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/access-waccess?view=msvc-160
		return ((_access(WStringToChar(Filename.c_str()).c_str(), 0)) != -1);
	}


	// Set the name of a render objects (is safe function)
	static void SetDebugObjectName(struct ID3D11DeviceChild* resource, std::string Name)
	{
#if defined(_DEBUG) || defined(PROFILE)
		if (!resource) return;
		std::string FinalName = std::string(Name);
		std::replace(FinalName.begin(), FinalName.end(), '/', '_');
		std::replace(FinalName.begin(), FinalName.end(), '.', '_');
		std::replace(FinalName.begin(), FinalName.end(), '\\', '_');
		std::replace(FinalName.begin(), FinalName.end(), '#', '_');
		resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)FinalName.size() , FinalName.c_str());
#endif
	}
	
	
	static void SetDebugObjectName(struct ID3D11DeviceChild* resource, std::wstring Name)
	{
		if (!resource) return;
#if defined(_DEBUG) || defined(PROFILE)
		TAUtils::SetDebugObjectName(resource, WStringToChar(Name.c_str()));
#endif
	}

	static void SetDebugObjectName(struct IDXGISwapChain* resource, std::string Name)
	{
		if (!resource) return;
#if defined(_DEBUG) || defined(PROFILE)
		resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)Name.size(), Name.c_str());
#endif
	}

	class Paths
	{
	private:
		Paths() {};
		std::wstring m_RootPath;
	public:
		// Get path to the root of the project //
		std::wstring GetRootPath() { return m_RootPath; }

		//Ralative_to_ABS_path
		std::wstring ABS_Path(std::wstring LocalPath);

		Paths(Paths const&) = delete;
		void operator=(Paths const&) = delete;


		void SetProjectPath(std::wstring NewPath)
		{
			m_RootPath = NewPath;
		}
		static Paths& instance()
		{
			static Paths INSTANCE;
			return INSTANCE;
		}

	
	};

	//static bool FileExist(std::wstring FilePath);
	
}