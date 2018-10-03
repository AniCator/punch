#include "PunchApplication.h"
#include "Vis_Layers.h"

#include <locale>
#include <codecvt>

int g_argc;
char **g_argv;

static int TickRate = -1;
static int FrameRate = -1;

CPunchApplication::CPunchApplication()
{
	ApplicationWindow = nullptr;
	MusicModuleInstance = nullptr;
	LayerManagerInstance = nullptr;
	ConfigurationManagerInstance = nullptr;

	LastTickTime = 0;
	TickTime = 0;
	LastFrameTime = 0;
	FrameTime = 0;
}

CPunchApplication::~CPunchApplication()
{

}

void CPunchApplication::Initialize()
{
	std::stringstream filePath;
	filePath << ExePath() << "/Punch2.ini";
	CConfigurationManager::GetInstance().Initialize( filePath.str() );

	InitializeGLFW();
	InitializeGLEW();
	InitializeMusicModule();
	InitializeLayerManager();

	InitializeProgressBar();
}

void CPunchApplication::InitializeMusicModule()
{
	if(MUSIC_MODULE_TYPE == MM_BASS)
		MusicModuleInstance = new CBASSModule();

	if(!MusicModuleInstance)
	{
		if(MUSIC_MODULE_TYPE == MM_UNDEFINED)
			Exit(APP_ERROR_MUSIC_MODULE_UNDEFINED);
		else
			Exit(APP_ERROR_UNDEFINED);
	}

	if(MusicModuleInstance->Initialize() == APP_ERROR_BASS_INIT)
		Exit(APP_ERROR_BASS_INIT);

	if( ( MusicModuleInstance->GetSupportFlags() & ESupportFlag::UseInput ) == 0 && CConfigurationManager::GetInstance().IsEnabled( "input_enabled" ))
	{
		MusicModuleInstance->RecordStop();
		MusicModuleInstance->RecordStart();
	}
	else
	{
		PopulatePlaylist();
		MusicModuleInstance->MusicPlayNextItem();
	}
}


void CPunchApplication::InitializeLayerManager()
{
	if( !LayerManagerInstance )
	{
		LayerManagerInstance = new CLayerManager();
	}

	LayerManagerInstance->Clear();

	const float LowLight = 0.2f;
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.001f, 0.05f, 0.3f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_1_frequency" ), FColor( 0.25f, 0.25f, 0.25f ) ) );
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.05f, 0.2f, 0.12f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_2_frequency" ), FColor( 0.025f, 0.5f, LowLight ) ) );
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.3f, 0.3f, 1.0f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_3_frequency" ), FColor( 1.0f, LowLight, 0.1f ) ) );
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.1f, 0.5f, 0.2f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_4_frequency" ), FColor( LowLight, LowLight, 2.0f ) ) );

	const int FrequencyOffset = CConfigurationManager::GetInstance().GetInteger( "radial_frequency_offset" );
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.002f, 0.1f, 0.4f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_1_frequency" ) + FrequencyOffset, FColor( 1.0f, 0.4f, LowLight ) ) );
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.1f, 0.4f, 0.24f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_2_frequency" ) + FrequencyOffset, FColor( LowLight, 0.5f, 0.1f ) ) );
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.1f, 0.1f, 0.75f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_3_frequency" ) + FrequencyOffset, FColor( 1.0f, LowLight, 0.25f ) ) );
	LayerManagerInstance->AddLayer( new VisualizationQuantumRing( 0.2f, 0.7f, 0.4f, CConfigurationManager::GetInstance().GetInteger( "radial_ring_4_frequency" ) + FrequencyOffset, FColor( LowLight, LowLight, 2.0f ) ) );

	int iCount = 10;
	for( int i = 1; i <= iCount; i++ )
	{
		int iFreqStep = i * i * 4;
		if( iFreqStep > DataArraySizeFFT )
			break;
		LayerManagerInstance->AddLayer( new VisualizationCubeWalking( iFreqStep ) );
	}

	LayerManagerInstance->AddLayer(new VisualizationSpectrum());
}

void CPunchApplication::PopulatePlaylist()
{
#if 0
	if(g_argc < 2)
	{
		printf("Error: No audio files added.\n");
		//MessageBoxA(glfwGetWin32Window(m_pAppWindow), , "Error", MB_OK|MB_ICONERROR|MB_APPLMODAL);
		Exit(APP_ERROR_PLAYLIST_INVALID,"To use Punch you will have to drag an audio file onto the .exe");
	}
#endif

#ifdef WIN32
	int CommandLineArguments;

	LPWSTR* CommandLineArgumentVector = CommandLineToArgvW( GetCommandLineW(), &CommandLineArguments );
#endif

#ifndef WIN32
	for(int i=1;i<g_argc;i++)
	{
		printf("Adding to playlist: %s\n",g_argv[i]);

		std::string PlaylistItemFile = g_argv[i];
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> StringConverter;
		std::wstring PlaylistItem = StringConverter.from_bytes( PlaylistItemFile );

		MusicModuleInstance->PlaylistAddItem( PlaylistItem );
	}
#else
	for( int i = 1; i < CommandLineArguments; i++ )
	{
		std::wstring PlaylistItem = std::wstring( CommandLineArgumentVector[i] );

		printf( "Adding to playlist: %s\n", PlaylistItem.c_str() );

		MusicModuleInstance->PlaylistAddItem( PlaylistItem );
	}
#endif
}

void CPunchApplication::InitializeGLEW()
{
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		printf("Error: '%s'\n", glewGetErrorString(res));
		Exit(APP_ERROR_GLEW_INIT);
	}
}

void CPunchApplication::InitializeGLFW()
{
	//Initialize GLFW
	if (!glfwInit())
		Exit(APP_ERROR_GLFW_INIT);

	CreateApplicationWindow();
}

void CallbackKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	CPunchApplication::GetInstance().HandleKeyEvents( window, key, scancode, action, mods );
}

void CallbackPathDrop( GLFWwindow* window, int count, const char** paths )
{
	CPunchApplication::GetInstance().HandlePathDrop( window, count, paths );
}

void CPunchApplication::CreateApplicationWindow()
{
	bool FullScreen = CConfigurationManager::GetInstance().IsEnabled( "fullscreen" );
	bool NoBorder = CConfigurationManager::GetInstance().IsEnabled( "noborder" );
	int WindowWidth = CConfigurationManager::GetInstance().GetInteger( "width" );
	int WindowHeight = CConfigurationManager::GetInstance().GetInteger( "height" );
	int TargetMonitor = CConfigurationManager::GetInstance().GetInteger( "monitor" );

	TickRate = CConfigurationManager::GetInstance().IsValidKey( "tickrate" ) ? CConfigurationManager::GetInstance().GetInteger( "tickrate" ) : -1;
	FrameRate = CConfigurationManager::GetInstance().IsValidKey( "framerate" ) ? CConfigurationManager::GetInstance().GetInteger( "framerate" ) : -1;

	if( ApplicationWindow )
	{
		glfwSetWindowShouldClose( ApplicationWindow, GL_TRUE );
		glfwPollEvents();
		glfwDestroyWindow( ApplicationWindow );
	}

	GLFWmonitor* Monitor = glfwGetPrimaryMonitor();
	if( TargetMonitor > -1 )
	{
		int MonitorCount;
		GLFWmonitor** Monitors = glfwGetMonitors( &MonitorCount );

		if( TargetMonitor < MonitorCount )
		{
			Monitor = Monitors[TargetMonitor];
		}
	}

	// Get monitor video mode properties
	const GLFWvidmode* VideoMode = glfwGetVideoMode( Monitor );

	//Window creation
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
	glfwWindowHint( GLFW_DECORATED, NoBorder ? GL_FALSE : GL_TRUE );
	glfwWindowHint( GLFW_FLOATING, !FullScreen && NoBorder ? GL_FALSE : GL_TRUE );

	if( FullScreen && !NoBorder )
	{
		glfwWindowHint( GLFW_RED_BITS, VideoMode->redBits );
		glfwWindowHint( GLFW_GREEN_BITS, VideoMode->greenBits );
		glfwWindowHint( GLFW_BLUE_BITS, VideoMode->blueBits );
		glfwWindowHint( GLFW_REFRESH_RATE, VideoMode->refreshRate );

		ApplicationWindow = glfwCreateWindow( VideoMode->width, VideoMode->height, APP_WINDOW_TITLE, Monitor, NULL );
	}
	else
	{
		if( WindowWidth < 0 || WindowHeight < 0 )
		{
			ApplicationWindow = glfwCreateWindow( VideoMode->width, VideoMode->height, APP_WINDOW_TITLE, NULL, NULL );
		}
		else
		{
			ApplicationWindow = glfwCreateWindow( WindowWidth, WindowHeight, APP_WINDOW_TITLE, NULL, NULL );
		}

		if( NoBorder )
		{
			int MonitorPosition[2];
			glfwGetMonitorPos( Monitor, &MonitorPosition[0], &MonitorPosition[1] );
			glfwSetWindowPos( ApplicationWindow, MonitorPosition[0], MonitorPosition[1] );
		}
	}

	if( !ApplicationWindow )
	{
		glfwTerminate();
		Exit( APP_ERROR_GLFW_WINDOW );
	}

	//Make window's context current
	glfwMakeContextCurrent( ApplicationWindow );

	glfwSetKeyCallback( ApplicationWindow, CallbackKeyEvents );
	glfwSetDropCallback( ApplicationWindow, CallbackPathDrop );

	glfwSwapInterval( 0 );
}

void CPunchApplication::MainLoop()
{
	TimeKeeper::GetInstance().UpdateTime( glfwGetTime() );
	UpdateDeltaTime();
	UpdateDrawDeltaTime();

	//Loop as long as the window is open
	while (!glfwWindowShouldClose(ApplicationWindow))
	{
		TimeKeeper::GetInstance().UpdateTime( glfwGetTime() );

		const float DeltaTime = static_cast<float>( GetDeltaTime() );
		if( TickRate == -1 || DeltaTime > 1.0f / static_cast<float>( TickRate ) )
			HandleLogic();

		const float DrawDeltaTime = static_cast<float>( GetDrawDeltaTime() );
		if( FrameRate == -1 || DrawDeltaTime > 1.0f / static_cast<float>( FrameRate ) )
			HandleDraw();

		//Poll events
		glfwPollEvents();
	}
}

void CPunchApplication::HandleLogic()
{
	TimeKeeper::GetInstance().UpdateTime( glfwGetTime() );

	if(MusicModuleInstance)
	{
		MusicModuleInstance->Think();
		if(LayerManagerInstance)
			LayerManagerInstance->UpdateLayers(MusicModuleInstance->GetFFTData());
	}

	UpdateProgressBar();
	UpdateDeltaTime();
}

void CPunchApplication::HandleDraw()
{
	// Draw scene
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	LayerManagerInstance->DrawLayers();

	DrawProgressBar();

	glPointSize( 4.0f );

	UpdateDrawDeltaTime();

	//Swap front and back buffers
	glfwSwapBuffers( ApplicationWindow );
}

GLuint ProgressBarVBO;
std::vector<glm::vec3> ProgressBarVertices;
void CPunchApplication::InitializeProgressBar()
{
	glGenBuffers( 1, &ProgressBarVBO );
}

void CPunchApplication::UpdateProgressBar()
{
	ProgressBarVertices.clear();

	const float ProgressBarLength = MusicModuleInstance->IsPlaying() ? static_cast<float>( MusicModuleInstance->MusicGetPosition() / MusicModuleInstance->MusicGetLength() ) : 0.0f;
	const float ProgressBarVertexLocation = ProgressBarLength * 2.0f - 1.0f;
	
	const float ProgressBarHeight = CConfigurationManager::GetInstance().IsValidKey( "progress_bar_height" ) ? static_cast<float>( CConfigurationManager::GetInstance().GetDouble( "progress_bar_height" ) ) : 0.05f;
	const float ProgressBarVertexHeight = 1.0f - ProgressBarHeight;

	ProgressBarVertices.push_back( glm::vec3( -1.0f, 1.0f, 0.0f ) );
	ProgressBarVertices.push_back( glm::vec3( ProgressBarVertexLocation, 1.0f, 0.0f ) );
	ProgressBarVertices.push_back( glm::vec3( -1.0f, ProgressBarVertexHeight, 0.0f ) );

	ProgressBarVertices.push_back( glm::vec3( -1.0f, ProgressBarVertexHeight, 0.0f ) );
	ProgressBarVertices.push_back( glm::vec3( ProgressBarVertexLocation, 1.0f, 0.0f ) );
	ProgressBarVertices.push_back( glm::vec3( ProgressBarVertexLocation, ProgressBarVertexHeight, 0.0f ) );

	glBindBuffer( GL_ARRAY_BUFFER, ProgressBarVBO );
	glBufferData( GL_ARRAY_BUFFER, ProgressBarVertices.size() * sizeof( glm::vec3 ), &ProgressBarVertices[0], GL_DYNAMIC_DRAW );
}

void CPunchApplication::DrawProgressBar()
{
	glColor4f( 0.25f, 0.25f, 0.25f, 0.5f );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, ProgressBarVBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glDrawArrays( GL_TRIANGLES, 0, ProgressBarVertices.size() );
	glDisableVertexAttribArray( 0 );
}

static float TemporaryPitch = 100.0f;
static const float PitchShiftAmount = 5.0f;
void CPunchApplication::HandleKeyEvents(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if(MusicModuleInstance)
			MusicModuleInstance->MusicPlayNextItem();
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if(MusicModuleInstance)
			MusicModuleInstance->MusicPlayPreviousItem();
	}
	if( key == GLFW_KEY_HOME && action == GLFW_PRESS )
	{
		if( MusicModuleInstance )
			MusicModuleInstance->MusicForwardTime( CConfigurationManager::GetInstance().GetDouble( "scrobble_forward_step" ) );
	}
	if( key == GLFW_KEY_SPACE && action == GLFW_PRESS )
	{
		if( MusicModuleInstance )
		{
			if( !MusicModuleInstance->IsPaused() )
			{
				MusicModuleInstance->MusicPause();
			}
			else
			{
				MusicModuleInstance->MusicPlay( false );
			}
		}
	}

	if( key == GLFW_KEY_KP_ADD && ( action == GLFW_PRESS || action == GLFW_REPEAT ) )
	{
		TemporaryPitch += PitchShiftAmount;
		if( MusicModuleInstance )
		{
			MusicModuleInstance->MusicSpeed( TemporaryPitch );
		}
	}

	if( key == GLFW_KEY_KP_SUBTRACT && ( action == GLFW_PRESS || action == GLFW_REPEAT ) )
	{
		TemporaryPitch -= PitchShiftAmount;
		if( MusicModuleInstance )
		{
			MusicModuleInstance->MusicSpeed( TemporaryPitch );
		}
	}

	// Re-initialization and resetting options
	if( key == GLFW_KEY_END && action == GLFW_PRESS )
	{
		printf( "Restarting music module...\n" );
		delete MusicModuleInstance;
		InitializeMusicModule();
	}
	if( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
	{
		InitializeLayerManager();
	}
	if( key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS )
	{
		CreateApplicationWindow();
	}
}

void CPunchApplication::HandlePathDrop( GLFWwindow* window, int count, const char** paths )
{
	if( CConfigurationManager::GetInstance().IsEnabled( "input_enabled" ) )
		return;

	for( int i = 0; i < count; i++ )
	{
		printf( "Adding dropped file to playlist: %s\n", paths[i] );

		std::string PlaylistItemFile = paths[i];
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> StringConverter;
		std::wstring PlaylistItem = StringConverter.from_bytes( PlaylistItemFile );

		MusicModuleInstance->PlaylistAddItem( PlaylistItem );
	}

	unsigned int itemIndex = MusicModuleInstance->PlaylistSize() - count;
	MusicModuleInstance->MusicPlayItemAtIndex( itemIndex );
}

double CPunchApplication::GetDeltaTime()
{
	return TimeKeeper::GetInstance().GetTime() - LastTickTime;
}

void CPunchApplication::UpdateDeltaTime()
{
	LastTickTime = TimeKeeper::GetInstance().GetTime();
}

double CPunchApplication::GetDrawDeltaTime()
{
	return TimeKeeper::GetInstance().GetTime() - LastFrameTime;
}

void CPunchApplication::UpdateDrawDeltaTime()
{
	LastFrameTime = TimeKeeper::GetInstance().GetTime();
}

void CPunchApplication::Run()
{
	printf("Starting %s!\n", APP_WINDOW_TITLE);
	Initialize();
	MainLoop();
}

void CPunchApplication::Exit()
{
	exit(0);
}

void CPunchApplication::Exit(int ErrorCode)
{
	printf("Exiting with error code %i\n", ErrorCode);
#ifdef _WIN32
	char cErrorMsg[128];
	sprintf_s(cErrorMsg,128,"Code: %i",ErrorCode);
	if(cErrorMsg)
	{
		MessageBoxA(glfwGetWin32Window(ApplicationWindow), cErrorMsg, "Error", MB_OK|MB_ICONERROR|MB_APPLMODAL);
	}
#endif
	exit(ErrorCode);
}

void CPunchApplication::Exit(int ErrorCode, const char* ErrorMessage)
{
	printf("Exiting with error code %i\n", ErrorCode);
#ifdef _WIN32
	char cErrorMsg[128];
	sprintf_s(cErrorMsg,128,"Code: %i\nMessage: %s",ErrorCode,ErrorMessage);
	if(cErrorMsg)
	{
		MessageBoxA(glfwGetWin32Window(ApplicationWindow), cErrorMsg, "Error", MB_OK|MB_ICONERROR|MB_APPLMODAL);
	}
#endif
	exit(ErrorCode);
}

//Entry point
int main(int argc, char *argv[])
{
	g_argc = argc;
	g_argv = argv;

	CPunchApplication::GetInstance().Run();
}