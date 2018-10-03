#pragma once
#include "appframework/errordefs.h"
#include <stdio.h>

#include <vector>
#include <string>
#include <stdint.h>

static const uint32_t DataArraySizeFFT = 512;

//Holds FFT data
struct DataArrayFFT
{
	float Data[DataArraySizeFFT];
};

enum ESupportFlag
{
	UseInput,
};

class CMusicModule
{
public:
	CMusicModule();
	~CMusicModule();

	// Initialize the music module
	virtual int Initialize() = 0;

	virtual void Think() = 0;

	// Support flag functions
	virtual int GetSupportFlags();

	// Playback functions
	virtual void MusicPlay( bool bRestart ) = 0;
	virtual void MusicPause() = 0;
	virtual void MusicStop() = 0;
	virtual void MusicSpeed( float Percentage ) = 0;

	// Recording functions
	virtual void RecordStart() {};
	virtual void RecordStop() {};

	// Playback functions - playlist related
	virtual void MusicPlayPreviousItem() = 0;
	virtual void MusicPlayNextItem() = 0;
	virtual void MusicPlayItemAtIndex(unsigned int itemIndex) = 0;
	virtual void MusicForwardTime( double Increment ) = 0;
	virtual double MusicGetPosition() = 0;
	virtual double MusicGetLength() = 0;

	// Playlist functions
	virtual void PlaylistAddItem(std::wstring szItemLocation);
	virtual void PlaylistClear();
	virtual unsigned int PlaylistSize();

	// Returns FFT data array
	virtual DataArrayFFT GetFFTData() = 0;

	// Verification functions
	virtual bool IsPlaying() = 0;
	virtual bool IsPaused() = 0;
	virtual bool HasEnded() = 0;
protected:
	std::vector<std::wstring>	Playlist;

	int SupportFlags;
	
	bool UseAutoGain;
	float AutoGainMultiplier;
private:
	//
};