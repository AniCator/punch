#include "BASSModule.h"
#include "util/util.h"

float CBASSModule::ReferenceVolume = -1.0f;

//Initialize the music module
int CBASSModule::Initialize()
{
	BASS_Free();
	if( !BASS_Init( -1, 44100, 0, 0, NULL ) )
	{
		return APP_ERROR_BASS_INIT;
	}

	SupportFlags = UseInput;
	UseAutoGain = CConfigurationManager::GetInstance().IsEnabled( "auto_gain" );
	AutoGainMultiplier = (float) max( 0.0, min( 1.0, CConfigurationManager::GetInstance().GetDouble( "auto_gain_volume" ) ) );
	ModuleGain = 0.0f;

	AGCReferenceGain = pow( 10.0f, ( -18.0f / 10.0f ) ); //-18dBFS Convert to Linear
	AGCRate = (float) max( 0.0, CConfigurationManager::GetInstance().GetDouble( "auto_gain_rate" ) ); // coefficient when increasing/decreasing gain

	for( int i = 0; i < DataArraySizeFFT; i++ )
	{
		DataArray.Data[i] = 0.0f;
	}

	LoadPlugins();

	return APP_ERROR_NONE;
}

float CBASSModule::PerformAutomaticGainControl( float InputGain )
{
	InputGain = InputGain * ModuleGain; //scale input(x)

	ModuleGain += ( AGCReferenceGain - ( fabs( InputGain ) * fabs( InputGain ) ) ) * AGCRate;

	return InputGain * AutoGainMultiplier;
}

void CBASSModule::Think()
{
	if( HasEnded() )
		MusicPlayNextItem();
	else
	{
		if( IsPlaying() )
		{
			BASS_ChannelGetData( MusicStream, DataArray.Data, BASS_DATA_FFT1024 | BASS_DATA_FLOAT | BASS_DATA_FFT_NOWINDOW );

			for( int i = 0; i < DataArraySizeFFT; i++ )
			{
				DataArray.Data[i] *= ( ( (float) i + 0.1f ) / (float) DataArraySizeFFT );
				DataArray.Data[i] *= (float) DataArraySizeFFT - (float) i;
				DataArray.Data[i] /= 50.0f;

				if( ReferenceVolume > 0.0f )
				{
					DataArray.Data[i] *= ReferenceVolume;
				}

				DataArray.Data[i] = min( 1.0f, DataArray.Data[i] );
				DataArray.Data[i] = max( 0.0f, DataArray.Data[i] );

				if( UseAutoGain )
				{
					DataArray.Data[i] = PerformAutomaticGainControl( DataArray.Data[i] );
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
	if( MusicStream )
		BASS_StreamFree( MusicStream );

	BASS_INT iNewStream = BASS_StreamCreateFile( false, Playlist[CurrentItemIndex].c_str(), 0, 0, BASS_UNICODE );
	if( iNewStream )
	{
		MusicStream = iNewStream;

		BASS_ChannelGetAttribute( MusicStream, BASS_ATTRIB_FREQ, &BaseSampleRate );
	}
}

void CBASSModule::MusicPlay( bool bRestart )
{
	if( MusicStream )
		BASS_ChannelPlay( MusicStream, bRestart );
}

void CBASSModule::MusicPause()
{
	if( MusicStream )
		BASS_ChannelPause( MusicStream );
}

void CBASSModule::MusicStop()
{
	if( MusicStream )
		BASS_ChannelStop( MusicStream );
}

void CBASSModule::MusicSpeed( float Percentage )
{
	// NOTHING!
}

//Recording functions
int CALLBACK CBASSModule::RecordCallback( HRECORD handle, const void *buffer, DWORD length, void *user )
{
	CBASSModule* module = reinterpret_cast<CBASSModule*>( user );
	if( !module->MusicStream )
		module->MusicStream = BASS_StreamCreate( 44100, 2, BASS_STREAM_DECODE, STREAMPROC_PUSH, NULL );

	// Limit the data to the amount required by the FFT
	const int MaximumDataStreamLength = ( DataArraySizeFFT * 2 );
	DWORD fftlen = min( length, MaximumDataStreamLength * 2 * 2 );

	BASS_StreamPutData( module->MusicStream, buffer, fftlen );

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
	printf( "\nListening to device: %s", info.name );

	MusicStream = BASS_RecordStart( 44100, 2, MAKELONG( 0, 5 ), &CBASSModule::RecordCallback, this );

	int nReferenceVolume = CConfigurationManager::GetInstance().GetInteger( "input_reference_volume" );
	if( nReferenceVolume > 0 )
	{
		nReferenceVolume = max( 1, min( 100, nReferenceVolume ) );
		float flVolume = (float) nReferenceVolume / 100.0f;
		flVolume = 1.0f + ( 1.0f - flVolume );
		ReferenceVolume = flVolume * 2.0f;
		printf( "Compensating reference volume: %.2f\n", ReferenceVolume );
	}
}

void CBASSModule::RecordStop()
{
	BASS_RecordFree();
	if( MusicStream )
		BASS_StreamFree( MusicStream );
}

//Playback functions - playlist related
void CBASSModule::MusicPlayPreviousItem()
{
	if( Playlist.size() == 0 )
		return;

	if( CurrentItemIndex != 0 )
	{
		CurrentItemIndex -= 1;
	}
	else
	{
		CurrentItemIndex = Playlist.size() - 1;
	}

	MusicStop();
	MusicLoad();
	MusicPlay( true );
}

void CBASSModule::MusicPlayNextItem()
{
	if( Playlist.empty() )
		return;

	if( CurrentItemIndex < Playlist.size() - 1 )
	{
		CurrentItemIndex += 1;
	}
	else
	{
		CurrentItemIndex = 0;

		if( CConfigurationManager::GetInstance().GetInteger( "playlist_loop" ) != 1 )
		{
			MusicStop();
			return;
		}
	}

	MusicStop();
	MusicLoad();
	MusicPlay( true );

	printf( "Playing: %s\n", Playlist[CurrentItemIndex].c_str() );
}

void CBASSModule::MusicPlayItemAtIndex( unsigned int itemIndex )
{
	CurrentItemIndex = max( 0, min( itemIndex, Playlist.size() ) );

	MusicStop();
	MusicLoad();
	MusicPlay( true );

	printf( "Playing: %s\n", Playlist[CurrentItemIndex].c_str() );
}

void CBASSModule::MusicForwardTime( double Increment )
{
	const QWORD CurrentPosition = BASS_ChannelGetPosition( MusicStream, BASS_POS_BYTE );
	const QWORD StreamLength = BASS_ChannelGetLength( MusicStream, BASS_POS_BYTE );

	const double Time = BASS_ChannelBytes2Seconds( MusicStream, StreamLength );
	const double TimeOneByte = Time / (double) StreamLength;

	double CurrentTime = (double) CurrentPosition * TimeOneByte;
	CurrentTime += Increment;
	const QWORD NewPosition = (QWORD) ( CurrentTime / TimeOneByte );

	BASS_ChannelSetPosition( MusicStream, NewPosition, BASS_POS_BYTE );
}

double CBASSModule::MusicGetPosition()
{
	const QWORD CurrentPosition = BASS_ChannelGetPosition( MusicStream, BASS_POS_BYTE );
	return BASS_ChannelBytes2Seconds( MusicStream, CurrentPosition );
}

double CBASSModule::MusicGetLength()
{
	const QWORD StreamLength = BASS_ChannelGetLength( MusicStream, BASS_POS_BYTE );
	return BASS_ChannelBytes2Seconds( MusicStream, StreamLength );
}

DataArrayFFT CBASSModule::GetFFTData()
{
	return DataArray;
}

//Verification functions
bool CBASSModule::IsPlaying()
{
	bool bHasStopped = HasEnded();
	bool bIsPaused = IsPaused();
	return MusicStream && ( !bHasStopped || !bIsPaused );
}

bool CBASSModule::IsPaused()
{
	return BASS_ChannelIsActive( MusicStream ) == BASS_ACTIVE_PAUSED;
}

bool CBASSModule::HasEnded()
{
	return BASS_ChannelIsActive( MusicStream ) == BASS_ACTIVE_STOPPED;
}
