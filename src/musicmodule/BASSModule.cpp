#include "BASSModule.h"

//Initialize the music module
int CBASSModule::Initialize()
{
	if(!BASS_Init(-1, 44100, 0, 0, NULL))
	{
		return APP_ERROR_BASS_INIT;
	}

	LoadPlugins();

	return APP_ERROR_NONE;
}

void CBASSModule::Think()
{
	if(HasEnded())
		MusicPlayNextItem();
	else
	{
		BASS_ChannelGetData(iStream,flFFT_DataArray.fft_data,BASS_DATA_FFT1024|BASS_DATA_FLOAT);

		for(int i=0;i<FFT_DATAARRAY_SIZE;i++)
		{
			flFFT_DataArray.fft_data[i] *= (((float)i + 0.1f) / (float)FFT_DATAARRAY_SIZE);
			flFFT_DataArray.fft_data[i] *= (float)FFT_DATAARRAY_SIZE - (float)i;
			flFFT_DataArray.fft_data[i] /= 50.0f;
		}
	}
}

void CBASSModule::LoadPlugins()
{
	printf( "Loading plugins.\n" );
	//Load FLAC library
#ifdef _WIN32
	BASS_PluginLoad( "bassflac.dll", 0 );
	if( BASS_ErrorGetCode() != 0 )
	{
		printf( "Could not load FLAC plugin. Error: %i\n", BASS_ErrorGetCode() );
	}
#endif
}

//Playback functions
void CBASSModule::MusicLoad()
{
	if(iStream)
		BASS_StreamFree(iStream);
	BASS_INT iNewStream = BASS_StreamCreateFile(false,szPlaylist[iCurrentItemIdx].c_str(),0,0,0);
	if(iNewStream)
		iStream = iNewStream;
}

void CBASSModule::MusicPlay()
{
	if(iStream)
		BASS_ChannelPlay(iStream,true);
}

void CBASSModule::MusicPause()
{
	if(iStream)
		BASS_ChannelPause(iStream);
}

void CBASSModule::MusicStop()
{
	if(iStream)
		BASS_ChannelStop(iStream);
}

//Playback functions - playlist related
void CBASSModule::MusicPlayPreviousItem()
{
	if(szPlaylist.size() == 0)
		return;

	if(iCurrentItemIdx != 0)
	{
		iCurrentItemIdx -= 1;
	}
	else
	{
		iCurrentItemIdx = szPlaylist.size() - 1;
	}

	MusicStop();
	MusicLoad();
	MusicPlay();
}

void CBASSModule::MusicPlayNextItem()
{
	if(szPlaylist.empty())
		return;

	if(iCurrentItemIdx < szPlaylist.size() - 1)
	{
		iCurrentItemIdx += 1;
	}
	else
	{
		iCurrentItemIdx = 0;
	}

	MusicStop();
	MusicLoad();
	MusicPlay();

	printf( "Playing: %s\n", szPlaylist[iCurrentItemIdx] );
}

void CBASSModule::MusicPlayItemAtIndex( unsigned int itemIndex )
{
	iCurrentItemIdx = max( 0, min( itemIndex, szPlaylist.size() ) );

	MusicStop();
	MusicLoad();
	MusicPlay();

	printf( "Playing: %s\n", szPlaylist[iCurrentItemIdx] );
}

//Returns FFT data array
FFT_DataArray CBASSModule::GetFFTData()
{
	return flFFT_DataArray;
}

//Verification functions
bool CBASSModule::IsPlaying()
{
	bool bHasStopped = HasEnded();
	bool bIsPaused = IsPaused();
	return !bHasStopped || !bIsPaused;
}

bool CBASSModule::IsPaused()
{
	return BASS_ChannelIsActive(iStream)==BASS_ACTIVE_PAUSED;
}

bool CBASSModule::HasEnded()
{
	return BASS_ChannelIsActive(iStream)==BASS_ACTIVE_STOPPED;
}