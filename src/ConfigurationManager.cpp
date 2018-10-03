#include "ConfigurationManager.h"

bool CConfigurationManager::IsValidKey( const char* keyName )
{
	std::stringstream filterString;
	filterString << "(?!#)(?!;)";
	filterString << keyName;
	filterString << "=(.*)";
	std::regex filterSettings( filterString.str().c_str() );

	std::ifstream configurationFileStream;
	configurationFileStream.open( m_szFileName.c_str() );

	std::string line;
	while( std::getline( configurationFileStream, line ) )
	{
		std::smatch match;
		if( std::regex_search( line, match, filterSettings ) )
		{
			configurationFileStream.close();
			return true;
		}
	}

	configurationFileStream.close();
	printf( "Key not found \"%s\"\n", keyName );
	return false;
}

bool CConfigurationManager::IsEnabled( const char* keyName )
{
	std::stringstream filterString;
	filterString << "(?!#)(?!;)";
	filterString << keyName;
	filterString << "=(.*)";
	std::regex filterSettings( filterString.str().c_str() );

	std::ifstream configurationFileStream;
	configurationFileStream.open( m_szFileName.c_str() );

	std::string line;
	while( std::getline( configurationFileStream, line ) )
	{
		std::smatch match;
		if( std::regex_search( line, match, filterSettings ) )
		{
			std::ssub_match value = match[1];

			if( value.str().compare( "1" ) == 0 || value.str().compare( "true" ) == 0 )
			{
				configurationFileStream.close();
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	configurationFileStream.close();
	printf( "Key not found \"%s\"\n", keyName );
	return false;
}

const char* CConfigurationManager::GetString( const char* keyName )
{
	std::stringstream filterString;
	filterString << "(?!#)(?!;)";
	filterString << keyName;
	filterString << "=(.*)";
	std::regex filterSettings( filterString.str().c_str() );

	std::ifstream configurationFileStream;
	configurationFileStream.open( m_szFileName.c_str() );

	std::string line;
	while( std::getline( configurationFileStream, line ) )
	{
		std::smatch match;
		if( std::regex_search( line, match, filterSettings ) )
		{
			std::ssub_match value = match[1];

			configurationFileStream.close();
			return value.str().c_str();
		}
	}

	configurationFileStream.close();
	printf( "Key not found \"%s\"\n", keyName );
	return "undefined";
}

int CConfigurationManager::GetInteger( const char* keyName )
{
	std::stringstream filterString;
	filterString << "(?!#)(?!;)";
	filterString << keyName;
	filterString << "=(.*)";
	std::regex filterSettings( filterString.str().c_str() );

	std::ifstream configurationFileStream;
	configurationFileStream.open( m_szFileName.c_str() );

	std::string line;
	while( std::getline( configurationFileStream, line ) )
	{
		std::smatch match;
		if( std::regex_search( line, match, filterSettings ) )
		{
			std::ssub_match value = match[1];

			configurationFileStream.close();
			return atoi(value.str().c_str());
		}
	}

	configurationFileStream.close();
	printf( "Key not found \"%s\"\n", keyName );
	return 0;
}

double CConfigurationManager::GetDouble( const char* keyName )
{
	std::stringstream filterString;
	filterString << "(?!#)(?!;)";
	filterString << keyName;
	filterString << "=(.*)";
	std::regex filterSettings( filterString.str().c_str() );

	std::ifstream configurationFileStream;
	configurationFileStream.open( m_szFileName.c_str() );

	std::string line;
	while( std::getline( configurationFileStream, line ) )
	{
		std::smatch match;
		if( std::regex_search( line, match, filterSettings ) )
		{
			std::ssub_match value = match[1];

			configurationFileStream.close();
			return atof( value.str().c_str() );
		}
	}

	configurationFileStream.close();
	printf( "Key not found \"%s\"\n", keyName );
	return 0.0;
}

float CConfigurationManager::GetFloat( const char* keyName )
{
	return static_cast<float>( GetDouble( keyName ) );
}

void CConfigurationManager::Initialize( std::string filePath )
{
	m_szFileName = filePath;
	std::ifstream configurationFileStream;
	configurationFileStream.open( m_szFileName.c_str(), std::ios::binary );
	if( configurationFileStream.fail() )
	{
		configurationFileStream.open( "Punch2.default.ini" );
		if( !configurationFileStream.fail() )
		{
			std::ofstream defaultConfigurationStream( m_szFileName, std::ios::binary );
			defaultConfigurationStream << configurationFileStream.rdbuf();
		}
	}
}