#include "BASSModule.h"

float CBASSModule::m_flReferenceVolume = -1.0f;

//Initialize the music module
int CBASSModule::Initialize()
{
	BASS_Free();
	if(!BASS_Init(-1, 44100, 0, 0, NULL))
	{
		return APP_ERROR_BASS_INIT;
	}

	m_eSupportFlags = USE_INPUT;
	m_bAutoGain = CConfigurationManager::GetInstance().IsEnabled( "auto_gain" );
	m_bAutoGainMultiplier = ( float ) max( 0.0, min( 1.0, CConfigurationManager::GetInstance().GetDouble( "auto_gain_volume" ) ) );
	m_flGain = 0.0f;

	m_flAGCReference = pow( 10.0f, ( -18.0f / 10.0f ) ); //-18dBFS Convert to Linear
	m_flAGCRate = ( float ) max(0.0, CConfigurationManager::GetInstance().GetDouble( "auto_gain_rate" )); // coefficient when increasing/decreasing gain

	for( int i = 0; i < FFT_DATAARRAY_SIZE; i++ )
	{
		m_flFFTDataArray.fft_data[i] = 0.0f;
	}

	LoadPlugins();

	return APP_ERROR_NONE;
}

float CBASSModule::PerformAutomaticGainControl( float InputGain )
{
	InputGain = InputGain * m_flGain; //scale input(x)

	m_flGain += ( m_flAGCReference - ( fabs( InputGain ) * fabs( InputGain ) ) ) * m_flAGCRate;

	return InputGain * m_bAutoGainMultiplier;
}

void CBASSModule::Think()
{
	if(HasEnded())
		MusicPlayNextItem();
	else
	{
		if( IsPlaying() )
		{
			BASS_ChannelGetData( iStream, m_flFFTDataArray.fft_data, BASS_DATA_FFT1024 | BASS_DATA_FLOAT );

			for( int i = 0; i < FFT_DATAARRAY_SIZE; i++ )
			{
				m_flFFTDataArray.fft_data[i] *= ( ( (float) i + 0.1f ) / (float) FFT_DATAARRAY_SIZE );
				m_flFFTDataArray.fft_data[i] *= (float) FFT_DATAARRAY_SIZE - (float) i;
				m_flFFTDataArray.fft_data[i] /= 50.0f;

				if( m_flReferenceVolume > 0.0f )
				{
					m_flFFTDataArray.fft_data[i] *= m_flReferenceVolume;
				}

				if( m_bAutoGain )
				{
					m_flFFTDataArray.fft_data[i] = PerformAutomaticGainControl( m_flFFTDataArray.fft_data[i] );
				}
			}
		}
	}
}

void CBASSModule::LoadPlugins()
{
	printf( "Loading plugins.\n" );
	//Load FLAC library
#ifdef _WIN32
	BASS_PluginLoad( "bassflac.dll", 0 );
	if( BASS_ErrorGetCode() != 0 && BASS_ErrorGetCode() != BASS_ERROR_ALREADY )
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
	BASS_INT iNewStream = BASS_StreamCreateFile(false,m_szPlaylist[m_iCurrentItemIdx].c_str(),0,0,0);
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

//Recording functions
int CALLBACK CBASSModule::RecordCallback( HRECORD handle, const void *buffer, DWORD length, void *user )
{
	CBASSModule* module = reinterpret_cast< CBASSModule* >( user );
	if( !module->iStream )
		module->iStream = BASS_StreamCreate( 44100, 2, BASS_STREAM_DECODE, STREAMPROC_PUSH, NULL );

	DWORD fftlen = min( length, 1024 * 2 * 2 ); // limit the data to the amount required by the FFT (1024 samples)
	BASS_StreamPutData( module->iStream, buffer, fftlen );

	return true;
}

void CBASSModule::RecordStart()
{
	printf( "--List of devices--\n" );

	BASS_DEVICEINFO info;
	for( int a = 0; BASS_RecordGetDeviceInfo( a, &info ); a++ )
	{
		if( info.flags & BASS_DEVICE_ENABLED )
		{
			printf( "Device %i: %s\n", a, info.name );
		}
	}

	int nDevice = CConfigurationManager::GetInstance().GetInteger( "input_device" );
	BASS_RecordInit( nDevice );

	BASS_RecordGetDeviceInfo( nDevice, &info );
	printf( "\nListening to device: %s\n", info.name );

	iStream = BASS_RecordStart( 44100, 2, MAKELONG( 0, 5 ), &CBASSModule::RecordCallback, this );

	int nReferenceVolume = CConfigurationManager::GetInstance().GetInteger( "input_reference_volume" );
	if( nReferenceVolume > 0 )
	{
		nReferenceVolume = max( 1, min( 100, nReferenceVolume ) );
		float flVolume = ( float ) nReferenceVolume / 100.0f;
		flVolume = 1.0f + ( 1.0f - flVolume );
		m_flReferenceVolume = flVolume * 2.0f;
		printf( "Compensating reference volume: %.2f\n", m_flReferenceVolume );
	}
}

void CBASSModule::RecordStop()
{
	BASS_RecordFree();
	if( iStream )
		BASS_StreamFree( iStream );
}

//Playback functions - playlist related
void CBASSModule::MusicPlayPreviousItem()
{
	if(m_szPlaylist.size() == 0)
		return;

	if(m_iCurrentItemIdx != 0)
	{
		m_iCurrentItemIdx -= 1;
	}
	else
	{
		m_iCurrentItemIdx = m_szPlaylist.size() - 1;
	}

	MusicStop();
	MusicLoad();
	MusicPlay();
}

void CBASSModule::MusicPlayNextItem()
{
	if(m_szPlaylist.empty())
		return;

	if(m_iCurrentItemIdx < m_szPlaylist.size() - 1)
	{
		m_iCurrentItemIdx += 1;
	}
	else
	{
		m_iCurrentItemIdx = 0;
	}

	MusicStop();
	MusicLoad();
	MusicPlay();

	printf( "Playing: %s\n", m_szPlaylist[m_iCurrentItemIdx].c_str() );
}

void CBASSModule::MusicPlayItemAtIndex( unsigned int itemIndex )
{
	m_iCurrentItemIdx = max( 0, min( itemIndex, m_szPlaylist.size() ) );

	MusicStop();
	MusicLoad();
	MusicPlay();

	printf( "Playing: %s\n", m_szPlaylist[m_iCurrentItemIdx].c_str() );
}

FFTDataArray CBASSModule::GetFFTData()
{
	return m_flFFTDataArray;
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
