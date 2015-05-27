#include "MusicModule.h"
#include "bass.h"

typedef int BASS_INT;

class CBASSModule : public CMusicModule
{
public:
	//Initialize the music module
	virtual int Initialize();

	virtual void Think();

	//Playback functions
	virtual void MusicPlay();
	virtual void MusicPause();
	virtual void MusicStop();

	//Playback functions - playlist related
	virtual void MusicPlayPreviousItem();
	virtual void MusicPlayNextItem();
	virtual void MusicPlayItemAtIndex( unsigned int itemIndex );

	//Returns FFT data array
	virtual FFT_DataArray GetFFTData();

	//Verification functions
	virtual bool IsPlaying();
	virtual bool IsPaused();
	virtual bool HasEnded();
private:
	void LoadPlugins();

	void MusicLoad();

	unsigned int				iCurrentItemIdx;

	BASS_INT iStream;
	FFT_DataArray flFFT_DataArray;
};