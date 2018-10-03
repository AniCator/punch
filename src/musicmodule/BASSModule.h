#include "MusicModule.h"
#include "bass.h"
#include "ConfigurationManager.h"

typedef int BASS_INT;

class CBASSModule : public CMusicModule
{
public:
	//Initialize the music module
	virtual int Initialize();

	virtual void Think();

	//Playback functions
	virtual void MusicPlay( bool bRestart );
	virtual void MusicPause();
	virtual void MusicStop();
	virtual void MusicSpeed( float Percentage );

	//Recording functions
	virtual void RecordStart();
	virtual void RecordStop();
	static BOOL CALLBACK RecordCallback( HRECORD handle, const void *buffer, DWORD length, void *user );

	//Playback functions - playlist related
	virtual void MusicPlayPreviousItem();
	virtual void MusicPlayNextItem();
	virtual void MusicPlayItemAtIndex( unsigned int itemIndex );
	virtual void MusicForwardTime( double Increment );
	virtual double MusicGetPosition();
	virtual double MusicGetLength();

	//Returns FFT data array
	virtual DataArrayFFT GetFFTData();

	//Verification functions
	virtual bool IsPlaying();
	virtual bool IsPaused();
	virtual bool HasEnded();

	float PerformAutomaticGainControl( float x );

	BASS_INT MusicStream;
private:
	void LoadPlugins();

	void MusicLoad();

	unsigned int CurrentItemIndex;
	static float ReferenceVolume;
	float ModuleGain;
	float AGCReferenceGain;
	float AGCRate;

	float BaseSampleRate;

	DataArrayFFT DataArray;
};