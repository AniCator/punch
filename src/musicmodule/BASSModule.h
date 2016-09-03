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
	virtual void MusicPlay();
	virtual void MusicPause();
	virtual void MusicStop();

	//Recording functions
	virtual void RecordStart();
	virtual void RecordStop();
	static BOOL CALLBACK RecordCallback( HRECORD handle, const void *buffer, DWORD length, void *user );

	//Playback functions - playlist related
	virtual void MusicPlayPreviousItem();
	virtual void MusicPlayNextItem();
	virtual void MusicPlayItemAtIndex( unsigned int itemIndex );

	//Returns FFT data array
	virtual FFTDataArray GetFFTData();

	//Verification functions
	virtual bool IsPlaying();
	virtual bool IsPaused();
	virtual bool HasEnded();

	float PerformAutomaticGainControl( float x );

	BASS_INT iStream;
private:
	void LoadPlugins();

	void MusicLoad();

	unsigned int				m_iCurrentItemIdx;
	static float m_flReferenceVolume;
	float m_flGain;
	float m_flAGCReference;
	float m_flAGCRate;

	FFTDataArray m_flFFTDataArray;
};