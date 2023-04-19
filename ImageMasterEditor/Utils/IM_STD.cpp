#include "IM_STD.h"

void TAError::ErrorWS(std::wstring ErrorToAdd) const
{
	OutputDebugString(ErrorToAdd.c_str());
}

void TAError::ErrorC(char* ErrorToAdd) const
{
	std::string str(ErrorToAdd);
	std::wstring strw(str.begin(), str.end());
	ErrorWS(strw);
}

void TAError::WarnWS(std::wstring ErrorToAdd)
{
	m_CurrentErrorBuffer += L"Warning: " + ErrorToAdd + L"\n";
}

void TAError::WarnC(char* ErrorToAdd)
{
	std::string str(ErrorToAdd);
	std::wstring strw(str.begin(), str.end());
	WarnWS(strw);
}

void TAError::LogWS(std::wstring ErrorToAdd)
{
	m_CurrentErrorBuffer += L"Log: " + ErrorToAdd + L"\n";
}

void TAError::LogC(char* LogToAdd)
{
	std::string str(LogToAdd);
	std::wstring strw(str.begin(), str.end());
	LogWS(strw);
}

std::wstring TAError::GetAndConsumeErrorBuffer()
{
	std::wstring result = m_CurrentErrorBuffer;
	m_CurrentErrorBuffer.clear();
	return result;
}

std::wstring TAUtils::Paths::ABS_Path(std::wstring LocalPath)
{
	return GetRootPath() + LocalPath;

}

