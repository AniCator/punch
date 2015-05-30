#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <regex>

class CConfigurationManager
{
public:
	bool IsValidKey( const char* keyName );
	bool IsEnabled( const char* keyName );
	const char* GetString( const char* keyName );
	int GetInteger( const char* keyName );

	void Initialize( std::string filePath );

private:
	std::string m_szFileName;
	std::map<std::string, std::string> m_settings;

public:
	static CConfigurationManager& GetInstance()
	{
		static CConfigurationManager    instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
private:
	CConfigurationManager() {};

	CConfigurationManager( CConfigurationManager const& ) = delete;
	void operator=( CConfigurationManager const& ) = delete;
};