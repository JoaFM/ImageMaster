#pragma once
#include <iostream>
#include <string>


class IMBase
{
public:
	virtual void OnNameOpdate() {};
	std::string GetFriendlyName() { return m_FriendlyName; };
	void SetFriendlyName(std::string newName) { m_FriendlyName = newName; OnNameOpdate(); }

private:
	std::string m_FriendlyName = "";
};