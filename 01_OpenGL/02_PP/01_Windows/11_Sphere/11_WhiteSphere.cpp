//Common - Windows Header files
# include <windows.h> // win32 API
# include <stdio.h>   // for file I/O
# include <stdlib.h>  // for for exit() func

//OPENGL Header Files
HDC ghdc=NULL;		// DC - Device context
HGLRC ghrc = NULL;  // RC - Rendering Context

# include <gl/glew.h> // this must be before # include<gl/GL.h>
# include<gl/GL.h> // PATH - C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\um\gl
# include "OGL.h" //user cha - resorce file for icon MYICON

# include "Sphere.h" //for sphere

# include "vmath.h"
using namespace vmath;

// link with openl library
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Sphere.lib") //for sphere

// global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Macros 
#define WIN_WIDTH 800
#define WIN_HEIGHT 600


// OpenGl - Global variables
FILE *gpFile = NULL; // handle for file

HWND ghwnd=NULL; //global window handle
BOOL gbActive = FALSE;

DWORD dwStyle = 0; // global window style 32 bit =16+16 /8+8+8+8 lowword/highword
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)}; 

BOOL gbFullscreen = FALSE; 

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

//vertex shader
GLuint shaderProgramObject = 0;

//variable for triangle
unsigned short gNumVertices = 0;
unsigned short gNumElements = 0;
GLuint gVao_sphere = 0;

GLuint gVbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0;
GLuint gVbo_sphere_element = 0;

mat4 perspectiveProjectionMatrix;
GLuint mvpMatrixUniform = 0;

// own data type enum
enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL
};

// entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Local Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("SUM Window");

	// Function Declarations / Prototype

	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	int iResult = 0;
	BOOL bDone = FALSE;

	HDC hdc; // One of the two arguments required for GetDeviceCaps
	hdc = GetDC(NULL);

	// Centering Window
	// X, Y coordinates for SCREEN
	int ScreenWidth = GetDeviceCaps(hdc, HORZRES);
	int ScreenHeight = GetDeviceCaps(hdc, VERTRES);

	// X, Y coordinates for WINDOW
	int WindowX = (ScreenWidth / 2) - (WIN_WIDTH / 2);
	int WindowY = (ScreenHeight / 2) - (WIN_HEIGHT / 2);

	// Code	
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Log File cannot be opened"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program started Successfully...\n\n");

	// WNDCLASSEX Initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);                                   //1
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;                    //2		// The window context that is created by CreateWindow() is under wndclass.style
	wndclass.cbClsExtra = 0;                                                //3  
	wndclass.cbWndExtra = 0;                                                //4
	wndclass.lpfnWndProc = WndProc;                                         //5
	wndclass.hInstance = hInstance;                                         //6
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);           //7
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));          //8
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                         //9
	wndclass.lpszClassName = szAppName;                                     //10
	wndclass.lpszMenuName = NULL;                                           //11
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));        //12

	// Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	// Create the Window
	// Change CreateWindow to CreateWindowEx
	// CreateWindowEx brings the Window above the task bar
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("Srushti Umesh Moghe"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		WindowX,
		WindowY,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// Initialization
	iResult = initialize();

	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize() failed !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);

		DestroyWindow(hwnd);
	}

	// Show the Window
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);

	SetFocus(hwnd);

	// UpdateWindow(hwnd); is sent to WM_PAINT hence to be removed

	// GAME LOOP
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// PM_REMOVE - removes message from the message queue
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActive == TRUE)
			{
				// RENDER
				display();

				// Update (For animation and changes)
				update();

			}
		}
	}
	// Uninitialization
	uninitialize();

	return((int)msg.wParam);
}

// Claaback Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Declarations

	void ToggleFullscreen(void);
	void resize(int, int);
	// void resize(width, height);

	// Code
	// Body of the callback() - WndProc [User Defined]
	switch (iMsg)
	{
	case WM_SETFOCUS:		// Message for Active Window 
		gbActive = TRUE;
		break;

	case WM_KILLFOCUS:		// Message that deviates focus from the active window
		gbActive = FALSE;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		// void resize(width, height);
		break;

	case WM_ERASEBKGND:
		return(0);	// Message does not go to DefWindowProc()
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;

	case WM_RBUTTONDOWN:		// Mouse Button click
		DestroyWindow(hwnd);
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':

			if (gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = FALSE;
			}
			break;

		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);	// To destroy the program running in the background when the window is closed
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// Local Variable Declarations
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);		// iii - B
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

int initialize(void)
{
	// Function Declarations
	void printGLInfo(void);
	void uninitialize(void);
	void resize(int, int);

	// Prototype Declarations

	// Code
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));		// struct initialization method02	

	// Initialization of PIXELFORMATDESCRIPTOR
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;		// Conventionally version 1
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	// Enabling Depth
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "GetDC() Failed...\n");
		return(-1);
	}

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) // NON ZERO POSITIVE value is the success of iPixelFormatIndex
	{
		fprintf(gpFile, "ChoosePixelFormat() Failed...\n");
		return(-2);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() Failed...\n");
		return(-3);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() Failed...\n");
		return(-4);
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() Failed...\n");
		return(-5);
	}

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() Failed...\n");
		return(-6);
	}

	// Print GL info
	printGLInfo();

	// step1 - vertex shader code
	const GLchar * vertexShaderSourceCode = 
											"#version 460 core\n" \
											"\n" \
											"in vec4 aPosition;\n" \
											"uniform mat4 uMVPMatrix;\n" \
											"out vec4 oColor;\n" \
											"void main(void)\n" \
											"{\n" \
											"gl_Position= uMVPMatrix*aPosition;\n" \
											"}\n";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject,1,(const GLchar **)&vertexShaderSourceCode,NULL);
	glCompileShader(vertexShaderObject);
	GLint status=0;
	GLint infoLogLength=0;
	GLchar *szInfoLog = NULL;
	glGetShaderiv(vertexShaderObject,GL_COMPILE_STATUS,&status);
	if(status==GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject,GL_INFO_LOG_LENGTH,&infoLogLength); 
		if(infoLogLength >0)
		{
			szInfoLog = (GLchar*) malloc(infoLogLength);		
			if(infoLogLength != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject,infoLogLength,NULL,szInfoLog);
				fprintf(gpFile,"vertex Shader compilation error -  %s \n",szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize();
	}


	// Fragment shader code

	const GLchar *fragementShaderSourceCode = 
											"#version 460 core \n" \
											"\n" \
											"out vec4 FragColor; \n" \
											"void main(void) \n" \
											"{ \n" \
											"FragColor = vec4(1.0f,1.0f,1.0f,1.0f); \n" \
											"} \n";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject,1,(const GLchar **)&fragementShaderSourceCode,NULL);
	glCompileShader(fragmentShaderObject);
	status=0;
	infoLogLength=0;
	szInfoLog = NULL;
	glGetShaderiv(fragmentShaderObject,GL_COMPILE_STATUS,&status);
	if(status==GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject,GL_INFO_LOG_LENGTH,&infoLogLength);
		if(infoLogLength >0)
		{
			szInfoLog = (GLchar*) malloc(infoLogLength);		
			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject,infoLogLength,NULL,szInfoLog);
				fprintf(gpFile,"Fragment Shader compilation error -  %s \n",szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				uninitialize();
			}

		}
		uninitialize();
	}

	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject,vertexShaderObject);
	glAttachShader(shaderProgramObject,fragmentShaderObject);
	
	// prelinking code
	glBindAttribLocation(shaderProgramObject,AMC_ATTRIBUTE_POSITION,"aPosition"); //check spelling of apostion
	
	glLinkProgram(shaderProgramObject);

	status=0;
	infoLogLength=0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject,GL_LINK_STATUS,&status);
	if(status==GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject,GL_INFO_LOG_LENGTH,&infoLogLength);
		if(infoLogLength >0)
		{
			szInfoLog = (GLchar*) malloc(infoLogLength);		
			if(infoLogLength != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject,infoLogLength,NULL,szInfoLog);
				fprintf(gpFile,"Shader Program Linking error -  %s \n",szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				uninitialize();
			}
		}
		uninitialize();
	}

	//get Shader Uniform Locations
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject,"uMVPMatrix");
	

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// SHPERE

	// vao
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// position vbo
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	// Enabling Depth

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	perspectiveProjectionMatrix = vmath::mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);

}

void printGLInfo(void)
{
	// Variable Declarations
	GLint numExtensions;
	GLint i;

	// Code

	fprintf(gpFile, "OpenGL Vendor   : %s \n", glGetString(GL_VENDOR));						// OpenGL Graphic Card
	fprintf(gpFile, "OpenGL Renderer : %s \n", glGetString(GL_RENDERER));						// OpenGL Driver Version
	fprintf(gpFile, "OpenGL Version  : %s \n", glGetString(GL_VERSION));					// OpenGL Version
	fprintf(gpFile, "GLSL Version    : %s \n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));	// Graphic Library Shading Language Version

	// Listing of Supported Extensions
	glGetIntegerv(GL_EXTENSIONS, &numExtensions);

	for (i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "%s \n", glGetStringi(GL_EXTENSIONS, i));
	}

}

void resize(int width ,int height)
{
	//code
	if (height <= 0)
		height = 1; // precaution for division	
	glViewport(0, 0, (GLsizei)width, (GLsizei)height ); //zoomed binacular view (limited)

	// set perspective projection matrix
	perspectiveProjectionMatrix = vmath::perspective(45.0f,( (GLfloat)width / (GLfloat)height ), 0.1f, 100.0f);	
}

void display(void)
{
	//Code

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramObject);

	// Transformation
	mat4 modelViewMatrix = vmath::mat4::identity();
	mat4 translationMatrix = vmath::mat4::identity();
	mat4 rotationMatrix = vmath::mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	// rotationMatrix = vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
	modelViewMatrix = translationMatrix;
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; 
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// *** bind vao ***
	glBindVertexArray(gVao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
		
// Code
	
}

void uninitialize(void)
{
	//function declarations
	
	void ToggleFullscreen(void);
	
	//code
	if(shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLint numShaders =0;	
		glGetProgramiv(shaderProgramObject,GL_ATTACHED_SHADERS,&numShaders);
		if(numShaders > 0)
		{
			GLuint *pShaders = (GLuint*) malloc(numShaders * sizeof(GLuint));

			if(numShaders !=NULL)
			{
				glGetAttachedShaders(shaderProgramObject,numShaders,NULL,pShaders);
			}
			for(GLint i =0;i< numShaders;i++)
			{
				glDetachShader(shaderProgramObject,pShaders[i]);
				glDeleteShader(pShaders[i]);
				pShaders[i]=0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}
	
	// Sphere
	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}
	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	//delete vao_Shere
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	//application is exitting in full screen
	if(gbFullscreen == TRUE) //on ESCAPE Key- pressed event
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
	}

	//Make the hdc as current dc

	if(wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL,NULL);
	}

	//Destroy rendering context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	//Release hdc
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// Destroy window
	if(ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd=NULL;

	}

	//close the log file
	if(gpFile)
	{
		fprintf(gpFile,"Program ended successfully...!!\n");
		fclose(gpFile);
		gpFile=NULL;
	}	
}

