#pragma once
#include "appframework/errordefs.h"
#include <stdio.h>

#include <vector>
#include <string>

#define FFT_DATAARRAY_SIZE 512

//Holds FFT data
struct FFT_DataArray
{
	float fft_data[FFT_DATAARRAY_SIZE];
};

enum ESupportFlag
{
	USE_INPUT_DEVICES,
};

class CMusicModule
{
public:
	CMusicModule();
	~CMusicModule();

	//Initialize the music module
	virtual int Initialize() = 0;

	virtual void Think() = 0;

	//Support flag functions
	virtual int GetSupportFlags();

	//Playback functions
	virtual void MusicPlay() = 0;
	virtual void MusicPause() = 0;
	virtual void MusicStop() = 0;

	//Playback functions - playlist related
	virtual void MusicPlayPreviousItem() = 0;
	virtual void MusicPlayNextItem() = 0;
	virtual void MusicPlayItemAtIndex(unsigned int itemIndex) = 0;

	//Playlist functions
	virtual void PlaylistAddItem(std::string szItemLocation);
	virtual void PlaylistClear();
	virtual unsigned int PlaylistSize();

	//Returns FFT data array
	virtual FFT_DataArray GetFFTData() = 0;

	//Verification functions
	virtual bool IsPlaying() = 0;
	virtual bool IsPaused() = 0;
	virtual bool HasEnded() = 0;
protected:
	std::vector<std::string>	szPlaylist;

	int m_eSupportFlags;
private:
	//
};