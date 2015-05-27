#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>

class CConfigurationManager
{
public:
	CConfigurationManager(const char* fileName);
	~CConfigurationManager();

	bool IsValidKey( const char* keyName );
	bool IsEnabled( const char* keyName );
	const char* GetString( const char* keyName );

private:
	const char* m_szFileName;

	std::map<std::string, std::string> m_settings;
};