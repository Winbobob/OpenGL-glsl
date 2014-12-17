#define GLUT_DISABLE_ATEXIT_HACK
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "textfile.h"
#include "tiny_obj_loader.h"
#include "FreeImage.h"
using namespace std;

//global variables
GLint loc = 1;
GLuint v,f,f2,p;
float angle = 1;  //ratate angle
float outlineColor[3]	= { 0.0f, 0.0f, 0.0f };	 // Color Of The Lines
float outlineWidth	= 3.0f;	 // Width Of The Lines
char* vert_filename = "textureComb.vert";
char* frag_filename = "textureComb.frag";
vector<tinyobj::shape_t> shapes; //store the obj file message
float window[2]={512, 384};  //store the parameters from window.txt
float project[4]={0, 0, 512, 384};
float eye[3]={0, 0, -2};
float viewUp[3]={0, 0, 1};
float lookAt[3]={0, 1, 0};

//light variables
GLfloat lightposition[] = {5.0, 20.0, 5.0,1.0};
GLfloat	light0_ambient[]={0.4 , 0.4 , 1.0, 1.0 };
GLfloat	light0_diffuse[]={ 1.0 , 1.0 , 1.0, 1.0 };
GLfloat	light0_specular[]={ 1.0 , 1.0 , 1.0, 1.0 };
GLfloat matambient[] = { 0.4, 0.5, 0.5, 1.0 };
GLfloat matdiffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat matspecular[] = { 0.3, 0.2, 0.2, 1.0 };
GLfloat matshininess = 120.0;  //0.0~128.0

//texture variables
unsigned char* data;
unsigned int width, height;
FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; //file format
int bpp = 0;	//bit depth

//add light source
GLfloat addLight_position[4];
GLfloat addLight_color[4];
GLfloat triangle[9]; //vertex coordinate array
GLfloat triangle2[3],triangle3[3],triangle4[3]; //coordinate for outline
static GLuint texName=0;
void setShaders(char* vert_filename, char* frag_filename);

void redisplay_all()
{
    glutPostRedisplay();
}

//load OBJ file
static vector<tinyobj::shape_t> TestLoadObj(const char* filename, const char* basepath = NULL)
{
	std::cout << "Loading " << filename << std::endl;
	
	std::vector<tinyobj::shape_t> shapes;
	std::string err = tinyobj::LoadObj(shapes, filename, basepath);
	
	return shapes;
}

//read the txt file line number
int lineNum(char file[])
{
	ifstream infile(file);  //The default file open mode is "open" in the form of output
	char str[100];			//N is a constant, the purpose is to read a long enough line
	int n=0;				//line number
	while(!infile.eof())
	{
		infile.getline(str, sizeof(str));//The default identifier for termination is‘\n’
		n++;
	}
	 return n; 
}

//read window.txt file
void readWindow()
{
	ifstream windowFile;
	windowFile.open("inputs/window.txt");
	for (int j=0;j<2;j++)
	{
		windowFile>>(float)window[j];
	}
	windowFile.close();
}

//read view.txt file
void readView()
{
	ifstream windowFile;
	windowFile.open("inputs/view.txt");
	for (int j=0;j<3;j++)
	{
		windowFile>>(float)eye[j];
	}
	for (int j=0;j<3;j++)
	{
		windowFile>>(float)lookAt[j];
	}
	for (int j=0;j<3;j++)
	{
		windowFile>>(float)viewUp[j];
	}
	windowFile.close();
}

//read project.txt file
void readProject()
{
	ifstream windowFile;
	windowFile.open("inputs/project.txt");
	for (int j=0;j<4;j++)
	{
		windowFile>>(float)project[j];
	}
	windowFile.close();
}

//read lights.txt file
void readLights()
{
	int LineNum = lineNum("inputs/lights.txt");
	ifstream windowFile;
	windowFile.open("inputs/lights.txt");
	for(int i=1; i<=LineNum; i++)
	{
		for (int j=0;j<4;j++)
		{
			windowFile>>(float)addLight_position[j];
		}
		for (int j=0;j<4;j++)
		{
			windowFile>>(float)addLight_color[j];
		}

		//OpenGL can add at most 8 light sources
		switch (i) {
		case 1:
			glLightfv(GL_LIGHT1, GL_POSITION, addLight_position); 
			glLightfv(GL_LIGHT1, GL_AMBIENT, addLight_color);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, addLight_color);
			glLightfv(GL_LIGHT1, GL_SPECULAR,addLight_color);
			glEnable(GL_LIGHT1);
			break;
		case 2:
			glLightfv(GL_LIGHT2, GL_POSITION, addLight_position); 
			glLightfv(GL_LIGHT2, GL_AMBIENT, addLight_color);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, addLight_color);
			glLightfv(GL_LIGHT2, GL_SPECULAR,addLight_color);
			glEnable(GL_LIGHT2);
			break;
		case 3:
			glLightfv(GL_LIGHT3, GL_POSITION, addLight_position); 
			glLightfv(GL_LIGHT3, GL_AMBIENT, addLight_color);
			glLightfv(GL_LIGHT3, GL_DIFFUSE, addLight_color);
			glLightfv(GL_LIGHT3, GL_SPECULAR,addLight_color);
			glEnable(GL_LIGHT3);
			break;
		case 4:
			glLightfv(GL_LIGHT4, GL_POSITION, addLight_position); 
			glLightfv(GL_LIGHT4, GL_AMBIENT, addLight_color);
			glLightfv(GL_LIGHT4, GL_DIFFUSE, addLight_color);
			glLightfv(GL_LIGHT4, GL_SPECULAR,addLight_color);
			glEnable(GL_LIGHT4);
			break;
		case 5:
			glLightfv(GL_LIGHT5, GL_POSITION, addLight_position); 
			glLightfv(GL_LIGHT5, GL_AMBIENT, addLight_color);
			glLightfv(GL_LIGHT5, GL_DIFFUSE, addLight_color);
			glLightfv(GL_LIGHT5, GL_SPECULAR,addLight_color);
			glEnable(GL_LIGHT5);
			break;
		case 6:
			glLightfv(GL_LIGHT6, GL_POSITION, addLight_position); 
			glLightfv(GL_LIGHT6, GL_AMBIENT, addLight_color);
			glLightfv(GL_LIGHT6, GL_DIFFUSE, addLight_color);
			glLightfv(GL_LIGHT6, GL_SPECULAR,addLight_color);
			glEnable(GL_LIGHT6);
			break;
		case 7:
			glLightfv(GL_LIGHT7, GL_POSITION, addLight_position); 
			glLightfv(GL_LIGHT7, GL_AMBIENT, addLight_color);
			glLightfv(GL_LIGHT7, GL_DIFFUSE, addLight_color);
			glLightfv(GL_LIGHT7, GL_SPECULAR,addLight_color);
			glEnable(GL_LIGHT7);
			break;
		}
	}
	windowFile.close();
}

//光照有点问题！！多光源没效果？？？？？？？？？？？？？？？加上Toon shading 和 blinn-hong illumin的文字

bool loadPic(const char* path, unsigned char*& data, unsigned int& width, unsigned int& height,FREE_IMAGE_FORMAT& fif, int& bpp)
{
	//initial FreeImage
    FreeImage_Initialise(TRUE);
	//image format
	fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(path, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(path);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, path);
	//if the image failed to load, return failure
	if(!dib)
		return false;
	//retrieve the image data
	data = FreeImage_GetBits(dib);
	//get the image width, height and bit depth
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	bpp = FreeImage_GetBPP(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((data == 0) || (width == 0) || (height == 0))
		return false;
}

void init()
{
	//initial FreeImage
    FreeImage_Initialise(TRUE);

	shapes = TestLoadObj("inputs/Jeep.obj");
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matdiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matspecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matshininess);	

	glLightfv(GL_LIGHT0, GL_POSITION, lightposition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR,light0_specular);
	glEnable(GL_LIGHT0);
	//readLights(); //read external light.txt

	//load bmp picture
	if(!data)
	{
		if (!loadPic("inputs/Brick.jpg",data, width, height,fif, bpp))
			cout<<"Error loading texture\n";
		else
			cout<<"Loaded texture successfully!\n";
	}

	//Texture generate
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texName);//generate
	glBindTexture(GL_TEXTURE_2D, texName);//bind to a 2-D texture
	//set texture mapping method
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//how to treat magnification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//how to treat de-magnification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//wrap the image along s coordinate (width)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//wrap the image along t coordinate (height)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glActiveTexture(GL_TEXTURE0);
	
	//bmp stores BGR instead of RGB
	if((fif == FIF_PNG)||(fif == FIF_BMP && bpp != 24))
	{
		//in order to load the PNG file successfully, we have to use GL_RGBA and GL_BGRA_EXT
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}

	 
}

void display(void) {

	//set background color, glClearColor should be put ahead.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	
	gluLookAt(eye[0],eye[1],eye[2],lookAt[0],lookAt[1],lookAt[2],viewUp[0],viewUp[1],viewUp[2]);
	glRotatef(angle, 0.0, 1.0, 0.0);
	
	//using two loops to draw triangles one by one
	for(int i = 0; i < shapes.size(); i++) //outer loop, in order to count the num of groups
	{
		for (int v = 0; v < shapes[i].mesh.indices.size() / 3; v++) 
		{   //triangle[0],triangle[1],triangle[2] represents the x,y,z coordinate of 1st point
		    //triangle[3],triangle[4],triangle[5] represents the x,y,z coordinate of 2nd point
		    //triangle[6],triangle[7],triangle[8] represents the x,y,z coordinate of 3rd point
			triangle[0] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+0]+0];
			triangle[1] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+0]+1];
			triangle[2] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+0]+2];
			triangle[3] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+1]+0];
			triangle[4] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+1]+1];
			triangle[5] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+1]+2];
			triangle[6] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+2]+0];
			triangle[7] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+2]+1];
			triangle[8] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+2]+2];
		
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableVertexAttribArray(loc);
			glVertexPointer(3,GL_FLOAT,0,triangle);//the first variable is 3, which means the coordinate num of every vertex
			glVertexAttribPointer(loc,3,GL_FLOAT,0,0,triangle);//the sixth variable is the poiner to the vertex coordinate array
			glDrawArrays(GL_TRIANGLE_STRIP,0,3); //draw triangles one by one
			loc++;
		}

	}

	if (true)											// Check To See If We Want Anti-Aliased Lines  
	{
		glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);				// Use The Good Calculations  
		glEnable (GL_LINE_SMOOTH);								// Enable Anti-Aliasing  
	}
	else														// We Don't Want Smooth Lines  
		glDisable (GL_LINE_SMOOTH);								// Disable Anti-Aliasing  

	// draw outline
	if (true)											// Check To See If We Want To Draw The Outline  
	{
		glEnable (GL_BLEND);									// Enable Blending  
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);		// Set The Blend Mode  
		glPolygonMode (GL_BACK, GL_LINE);						// Draw Backfacing Polygons As Wireframes  
		glLineWidth (outlineWidth);								// Set The Line Width  
		glCullFace (GL_FRONT);									// Don't Draw Any Front-Facing Polygons  
		glDepthFunc (GL_LEQUAL);								// Change The Depth Mode  
		glColor3fv (&outlineColor[0]);							// Set The Outline Color  

		// Tell OpenGL What We Want To Draw
		glBegin (GL_TRIANGLES);	
				for (int i = 0; i < shapes.size(); i++)						// Loop Through Each Polygon  
				{
					for (int v = 0; v < shapes[i].mesh.indices.size() / 3; v++)		// Loop Through Each Vertex
					{
						// first render a outline, slightly larger than the object itself
						triangle2[0] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+0]+0]+=0.000000001;
						triangle2[1] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+0]+1]+=0.000000001;
						triangle2[2] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+0]+2]+=0.000000001;
						triangle3[0] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+1]+0]+=0.000000001;
						triangle3[1] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+1]+1]+=0.000000001;
						triangle3[2] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+1]+2]+=0.000000001;
						triangle4[0] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+2]+0]+=0.000000001;
						triangle4[1] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+2]+1]+=0.000000001;
						triangle4[2] = shapes[i].mesh.positions[(int)3*shapes[i].mesh.indices[3*v+2]+2]+=0.000000001;

						glVertex3fv (triangle2);	// Send The Vertex Position
						glVertex3fv (triangle3);
						glVertex3fv (triangle4);
					}
				}
		glEnd ();							// Tell OpenGL We've Finished
		glDepthFunc (GL_LESS);				// Reset The Depth-Testing Mode  
		glCullFace (GL_BACK);				// Reset The Face To Be Culled  
		glPolygonMode (GL_BACK, GL_FILL);	// Reset Back-Facing Polygon Drawing Mode  
		glDisable (GL_BLEND);				// Disable Blending  
	}
	setShaders(vert_filename, frag_filename);
	glutSwapBuffers();
}

void reshape(int w, int h) //input the width and the height of window
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0 * w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(project[0], project[1], project[2], project[3]);

	// Set the correct perspective.
	gluPerspective(60, ratio, 1, 5000);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) 
	{
		case 27:    //Use "Esc" to quit
			exit(0);
			break;
		case 't':   //Use "t" to change the Blinn-Phong results to Toon shading
			if(vert_filename == "Toon.vert" && frag_filename == "Toon.frag")
			{
				vert_filename = "Blinn_Phong_illum.vert";
				frag_filename = "Blinn_Phong_illum.frag";
				setShaders(vert_filename, frag_filename);	
				printf("---->Blinn-Phong illumination");
				break;
			}
			else
			{
				vert_filename = "Toon.vert";
				frag_filename = "Toon.frag";
				setShaders(vert_filename, frag_filename);	
				printf("---->Toon shading with inked contouring");
				break;
			}			
	/*	case 'a':
			angle+=0.5;
			break;*/
		default:
			break;
	}
	redisplay_all();
}

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{
    // Returns 1 if an OpenGL error occurred, 0 otherwise.

    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void setShaders(char* vert_filename, char* frag_filename) {

	char *vs = NULL,*fs = NULL,*fs2 = NULL;

	//Create Vertex Shader and Fragment Shader
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	f2 = glCreateShader(GL_FRAGMENT_SHADER);

	/*vs = textFileRead("toon.vert");
	fs = textFileRead("toon.frag");
	vs = textFileRead("Blinn_Phong_illum.vert");
	fs = textFileRead("Blinn_Phong_illum.frag");*/
	vs = textFileRead(vert_filename);
	fs = textFileRead(frag_filename);

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs);free(fs);

	glCompileShader(v);
	glCompileShader(f);

	printShaderInfoLog(v);
	printShaderInfoLog(f);
	printShaderInfoLog(f2);

	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);

	glLinkProgram(p);
	printProgramInfoLog(p);

	glUseProgram(p);

	//set Uniform
	glUniform1i(glGetUniformLocation(p, "tex"), 0);
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(600,100);
	readWindow();//read external window.txt
	readView();  //read external view.txt
	readProject();//read external project.txt
	glutInitWindowSize(window[0],window[1]);
	glutCreateWindow("Pg3_shader_zhu6");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
	glEnable(GL_CULL_FACE);

	glewInit();
	if (glewIsSupported("GL_VERSION_2_0"))
	{
		 const GLubyte *renderer = glGetString( GL_RENDERER );  
		 const GLubyte *vendor = glGetString( GL_VENDOR );  
		 const GLubyte *version = glGetString( GL_VERSION );  
		 const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );  
		 GLint major, minor;  
		 glGetIntegerv(GL_MAJOR_VERSION, &major);  
		 glGetIntegerv(GL_MINOR_VERSION, &minor);  
		 cout << "GL Vendor    :" << vendor << endl;  
		 cout << "GL Renderer  : " << renderer << endl;  
		 cout << "GL Version (string)  : " << version << endl;  
		 cout << "GL Version (integer) : " << major << "." << minor << endl;  
		 cout << "GLSL Version : " << glslVersion << endl;   
	}
	else 
	{
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}
	printf("---------------------------------------------------------------------\n");
	printf("Use 't' to switch between Toon shading and Blinn-Phong illumination\n");
	printf("---->Texture Mapping");

	setShaders(vert_filename, frag_filename);	
	redisplay_all();	
	glutMainLoop();
	delete data;
	return 0;
}

