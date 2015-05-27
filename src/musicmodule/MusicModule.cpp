#include "MusicModule.h"

CMusicModule::CMusicModule()
{

}

CMusicModule::~CMusicModule()
{

}

void CMusicModule::PlaylistAddItem(std::string szItemLocation)
{
	szPlaylist.push_back(szItemLocation);
}

void CMusicModule::PlaylistClear()
{
	szPlaylist.clear();
}

unsigned int CMusicModule::PlaylistSize()
{
	return szPlaylist.size();
}