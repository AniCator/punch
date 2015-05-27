#include "PunchApplication.h"
#include "Vis_Layers.h"

int g_argc;
char **g_argv;

CMusicModule* CPunchApplication::m_pMusicModule = NULL;
CLayerManager* CPunchApplication::m_pLayerManager = NULL;
CConfigurationManager* CPunchApplication::m_pConfigurationManager = NULL;

CPunchApplication::CPunchApplication()
{

}

CPunchApplication::~CPunchApplication()
{

}

void CPunchApplication::Initialize()
{
	InitializeGLFW();
	InitializeGLEW();
	InitializeMusicModule();
	InitializeLayerManager();

	std::stringstream filePath;
	filePath << ExePath() << "/Punch2.ini";
	m_pConfigurationManager = new CConfigurationManager( filePath.str().c_str() );
}

void CPunchApplication::InitializeMusicModule()
{
	if(MUSIC_MODULE_TYPE == MM_BASS)
		m_pMusicModule = new CBASSModule();

	if(!m_pMusicModule)
	{
		if(MUSIC_MODULE_TYPE == MM_UNDEFINED)
			Exit(APP_ERROR_MUSIC_MODULE_UNDEFINED);
		else
			Exit(APP_ERROR_UNDEFINED);
	}

	if(m_pMusicModule->Initialize() == APP_ERROR_BASS_INIT)
		Exit(APP_ERROR_BASS_INIT);

	PopulatePlaylist();
	m_pMusicModule->MusicPlayNextItem();
}


void CPunchApplication::InitializeLayerManager()
{
	m_pLayerManager = new CLayerManager();

	m_pLayerManager->AddLayer(new CVis_TestLayer1(0.1f, 0.5f, 0.2f, 3, SimpleColor(0.4f,0.6f,1.0f)));
	m_pLayerManager->AddLayer(new CVis_TestLayer1(0.001f, 0.05f, 0.3f, 70, SimpleColor(1.0f,1.0f,1.0f)));
	m_pLayerManager->AddLayer(new CVis_TestLayer1(0.05f, 0.2f, 0.12f, 30, SimpleColor(0.2f,1.0f,0.4f)));

	m_pLayerManager->AddLayer(new CVis_TestLayer1());

	int iCount = 10;
	for(int i=1;i<=iCount;i++)
	{
		int iFreqStep = i * i * 4;
		if(iFreqStep > FFT_DATAARRAY_SIZE)
			break;
		m_pLayerManager->AddLayer(new CVis_CubeWalking(iFreqStep));
	}

	m_pLayerManager->AddLayer(new CVis_Spectrum());
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
		m_pMusicModule->PlaylistAddItem(g_argv[i]);
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
	//Initialize GLFW
	if (!glfwInit())
		Exit(APP_ERROR_GLFW_INIT);

	//Window creation
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
#ifdef WINDOW_NO_BORDER
	glfwWindowHint( GLFW_DECORATED, GL_FALSE ); //noborder
#endif

	// Get monitor video mode properties
	const GLFWvidmode* sVideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	
	//m_pAppWindow = glfwCreateWindow(sVideoMode->width, sVideoMode->height, APP_WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL); //fullscreen
#ifdef WINDOW_HALF_SIZE
	m_pAppWindow = glfwCreateWindow((int)(sVideoMode->width / 1.5), (int)(sVideoMode->height / 1.5), APP_WINDOW_TITLE, NULL, NULL);
#else
	m_pAppWindow = glfwCreateWindow(sVideoMode->width, sVideoMode->height, APP_WINDOW_TITLE, NULL, NULL);
#endif

	if (!m_pAppWindow)
	{
		glfwTerminate();
		Exit(APP_ERROR_GLFW_WINDOW);
	}

	//Make window's context current
	glfwMakeContextCurrent(m_pAppWindow);

	glfwSetKeyCallback( m_pAppWindow, CPunchApplication::HandleKeyEvents );
	glfwSetDropCallback( m_pAppWindow, CPunchApplication::HandlePathDrop );

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
	while (!glfwWindowShouldClose(m_pAppWindow))
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
	if(m_pMusicModule)
	{
		m_pMusicModule->Think();
		if(m_pLayerManager)
			m_pLayerManager->UpdateLayers(m_pMusicModule->GetFFTData());
	}
}

void CPunchApplication::HandleDraw()
{
	// Draw scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pLayerManager->DrawLayers();

	glPointSize(4.0f);

	//Swap front and back buffers
	glfwSwapBuffers(m_pAppWindow);
}

void CPunchApplication::HandleKeyEvents(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if(m_pMusicModule)
			m_pMusicModule->MusicPlayNextItem();
		m_pLayerManager->ResetLayers();
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if(m_pMusicModule)
			m_pMusicModule->MusicPlayPreviousItem();
	}
}

void CPunchApplication::HandlePathDrop( GLFWwindow* window, int count, const char** paths )
{
	for( int i = 0; i < count; i++ )
	{
		printf( "Adding dropped file to playlist: %s\n", paths[i] );
		m_pMusicModule->PlaylistAddItem( paths[i] );
	}

	unsigned int itemIndex = m_pMusicModule->PlaylistSize() - count;
	m_pMusicModule->MusicPlayItemAtIndex( itemIndex );
}

double CPunchApplication::GetDeltaTime()
{
	return glfwGetTime() - m_dTimeLastTick;
}

void CPunchApplication::UpdateDeltaTime()
{
	m_dTimeLastTick = glfwGetTime();
}

double CPunchApplication::GetDrawDeltaTime()
{
	return glfwGetTime() - m_dTimeLastFrame;
}

void CPunchApplication::UpdateDrawDeltaTime()
{
	m_dTimeLastFrame = glfwGetTime();
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

void CPunchApplication::Exit(int iErrorCode)
{
	printf("Exiting with error code %i\n", iErrorCode);
#ifdef _WIN32
	char cErrorMsg[128];
	sprintf_s(cErrorMsg,128,"Code: %i",iErrorCode);
	if(cErrorMsg)
	{
		MessageBoxA(glfwGetWin32Window(m_pAppWindow), cErrorMsg, "Error", MB_OK|MB_ICONERROR|MB_APPLMODAL);
	}
#endif
	exit(iErrorCode);
}

void CPunchApplication::Exit(int iErrorCode, const char* cErrorMessage)
{
	printf("Exiting with error code %i\n", iErrorCode);
#ifdef _WIN32
	char cErrorMsg[128];
	sprintf_s(cErrorMsg,128,"Code: %i\nMessage: %s",iErrorCode,cErrorMessage);
	if(cErrorMsg)
	{
		MessageBoxA(glfwGetWin32Window(m_pAppWindow), cErrorMsg, "Error", MB_OK|MB_ICONERROR|MB_APPLMODAL);
	}
#endif
	exit(iErrorCode);
}

//Entry point
int main(int argc, char *argv[])
{
	g_argc = argc;
	g_argv = argv;

	CPunchApplication cPunchApp;
	cPunchApp.Run(); //Run CPunchApplication
}