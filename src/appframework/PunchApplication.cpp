#include "PunchApplication.h"
#include "Vis_Layers.h"

int g_argc;
char **g_argv;

CMusicModule* CPunchApplication::MusicModuleInstance = NULL;
CLayerManager* CPunchApplication::LayerManagerInstance = NULL;
CConfigurationManager* CPunchApplication::ConfigurationManagerInstance = NULL;
GLFWwindow* CPunchApplication::ApplicationWindow = NULL;

CPunchApplication::CPunchApplication()
{

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

	UpdateDeltaTime();
	UpdateDrawDeltaTime();
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

	if( ( MusicModuleInstance->GetSupportFlags() & ESupportFlag::USE_INPUT ) == 0 && CConfigurationManager::GetInstance().IsEnabled( "input_enabled" ))
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
	LayerManagerInstance = new CLayerManager();

	LayerManagerInstance->AddLayer(new CVis_TestLayer1(0.1f, 0.5f, 0.2f, 3, SimpleColor(0.1f,0.3f,1.0f)));
	LayerManagerInstance->AddLayer(new CVis_TestLayer1(0.001f, 0.05f, 0.3f, 70, SimpleColor(1.0f,1.0f,1.0f)));
	LayerManagerInstance->AddLayer(new CVis_TestLayer1(0.05f, 0.2f, 0.12f, 30, SimpleColor(0.0f,1.0f,0.3f)));

	LayerManagerInstance->AddLayer(new CVis_TestLayer1());

	int iCount = 10;
	for(int i=1;i<=iCount;i++)
	{
		int iFreqStep = i * i * 4;
		if(iFreqStep > FFT_DATAARRAY_SIZE)
			break;
		LayerManagerInstance->AddLayer(new VisualizationCubeWalking(iFreqStep));
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

	for(int i=1;i<g_argc;i++)
	{
		printf("Adding to playlist: %s\n",g_argv[i]);
		MusicModuleInstance->PlaylistAddItem(g_argv[i]);
	}
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
	bool bFullScreen = CConfigurationManager::GetInstance().IsEnabled( "fullscreen" );
	bool bNoBorder = CConfigurationManager::GetInstance().IsEnabled( "noborder" );
	int nWidth = CConfigurationManager::GetInstance().GetInteger( "width" );
	int nHeight = CConfigurationManager::GetInstance().GetInteger( "height" );

	//Initialize GLFW
	if (!glfwInit())
		Exit(APP_ERROR_GLFW_INIT);

	//Window creation
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
	if( bNoBorder )
	{
		glfwWindowHint( GLFW_DECORATED, GL_FALSE );
	}

	// Get monitor video mode properties
	const GLFWvidmode* sVideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	if( bFullScreen && !bNoBorder )
	{
		ApplicationWindow = glfwCreateWindow( sVideoMode->width, sVideoMode->height, APP_WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL );
	}
	else
	{
		if( nWidth < 0 || nHeight < 0 )
		{
			ApplicationWindow = glfwCreateWindow( sVideoMode->width, sVideoMode->height, APP_WINDOW_TITLE, NULL, NULL );
		}
		else
		{
			ApplicationWindow = glfwCreateWindow( nWidth, nHeight, APP_WINDOW_TITLE, NULL, NULL );
		}
	}

	if (!ApplicationWindow)
	{
		glfwTerminate();
		Exit(APP_ERROR_GLFW_WINDOW);
	}

	//Make window's context current
	glfwMakeContextCurrent(ApplicationWindow);

	glfwSetKeyCallback( ApplicationWindow, CPunchApplication::HandleKeyEvents );
	glfwSetDropCallback( ApplicationWindow, CPunchApplication::HandlePathDrop );

	glfwSwapInterval(0);
}

GLuint VBO;					//temp
std::vector<glm::vec3> points;	//temp
void CPunchApplication::MainLoop()
{
	
	points.push_back(glm::vec3(0.0f,0.0f,0.0f));

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

	//Loop as long as the window is open
	while (!glfwWindowShouldClose(ApplicationWindow))
	{
		//Logic tick
		if( GetDeltaTime() > 0.016667 )
		{			
			HandleLogic();

			UpdateDeltaTime();
		}

		if( GetDrawDeltaTime() > 0.00333 )
		{
			HandleDraw();

			UpdateDrawDeltaTime();
		}

		//Poll events
		glfwPollEvents();
	}

	points.clear();
}

void CPunchApplication::HandleLogic()
{
	if(MusicModuleInstance)
	{
		MusicModuleInstance->Think();
		if(LayerManagerInstance)
			LayerManagerInstance->UpdateLayers(MusicModuleInstance->GetFFTData());
	}
}

void CPunchApplication::HandleDraw()
{
	// Draw scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	LayerManagerInstance->DrawLayers();

	glPointSize(4.0f);

	//Swap front and back buffers
	glfwSwapBuffers(ApplicationWindow);
}

void CPunchApplication::HandleKeyEvents(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if(MusicModuleInstance)
			MusicModuleInstance->MusicPlayNextItem();
		LayerManagerInstance->ResetLayers();
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if(MusicModuleInstance)
			MusicModuleInstance->MusicPlayPreviousItem();
	}
	if( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
	{
		printf( "Restarting music module...\n" );
		delete MusicModuleInstance;
		InitializeMusicModule();
	}
}

void CPunchApplication::HandlePathDrop( GLFWwindow* window, int count, const char** paths )
{
	if( CConfigurationManager::GetInstance().IsEnabled( "input_enabled" ) )
		return;

	for( int i = 0; i < count; i++ )
	{
		printf( "Adding dropped file to playlist: %s\n", paths[i] );
		MusicModuleInstance->PlaylistAddItem( paths[i] );
	}

	unsigned int itemIndex = MusicModuleInstance->PlaylistSize() - count;
	MusicModuleInstance->MusicPlayItemAtIndex( itemIndex );
}

double CPunchApplication::GetDeltaTime()
{
	return glfwGetTime() - LastTickTime;
}

void CPunchApplication::UpdateDeltaTime()
{
	LastTickTime = glfwGetTime();
}

double CPunchApplication::GetDrawDeltaTime()
{
	return glfwGetTime() - LastFrameTime;
}

void CPunchApplication::UpdateDrawDeltaTime()
{
	LastFrameTime = glfwGetTime();
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

	CPunchApplication cPunchApp;
	cPunchApp.Run(); //Run CPunchApplication
}