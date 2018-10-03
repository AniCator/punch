#include "MusicModule.h"

CMusicModule::CMusicModule()
{

}

CMusicModule::~CMusicModule()
{

}

int CMusicModule::GetSupportFlags()
{
	return SupportFlags;
}

void CMusicModule::PlaylistAddItem(std::wstring szItemLocation)
{
	Playlist.push_back(szItemLocation);
}

void CMusicModule::PlaylistClear()
{
	Playlist.clear();
}

unsigned int CMusicModule::PlaylistSize()
{
	return Playlist.size();
}