#include "ConfigurationManager.h"


CConfigurationManager::CConfigurationManager(const char* filePath)
{
	m_szFileName = filePath;

	printf( "Loading configuration file \"%s\"\n", filePath );

	std::regex filterSettings( "(?!#)(?!;)(.*)=(.*)" );

	std::ifstream configurationFileStream;
	configurationFileStream.open( filePath );

	std::string line;
	while( std::getline( configurationFileStream, line ) )
	{
		std::smatch match;
		if( std::regex_search( line, match, filterSettings ) )
		{
			printf( "Key: %s Value: %s\n", match[0], match[1] );
		}
	}

	configurationFileStream.close();
}

CConfigurationManager::~CConfigurationManager()
{

}

bool CConfigurationManager::IsValidKey( const char* keyName )
{

}

bool CConfigurationManager::IsEnabled( const char* keyName )
{

}

const char* CConfigurationManager::GetString( const char* keyName )
{

}
