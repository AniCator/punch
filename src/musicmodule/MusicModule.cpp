#include "MusicModule.h"

CMusicModule::CMusicModule()
{

}

CMusicModule::~CMusicModule()
{

}

int CMusicModule::GetSupportFlags()
{
	return m_eSupportFlags;
}

void CMusicModule::PlaylistAddItem(std::string szItemLocation)
{
	m_szPlaylist.push_back(szItemLocation);
}

void CMusicModule::PlaylistClear()
{
	m_szPlaylist.clear();
}

unsigned int CMusicModule::PlaylistSize()
{
	return m_szPlaylist.size();
}