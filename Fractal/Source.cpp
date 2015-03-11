#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <iostream>
#include <cstdio>
#include <vector>
#include <omp.h>
#include <amp.h>
using namespace Concurrency;
#include"IPoint.h"
using namespace std;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
int WIDTH = 1024;
int HEIGHT = 1024;
int DEPTH = 20;
double ZOOM = 1;

vector<vector<int>> paintP;
double curWS=-2.5,curWE=1,curHS=-1.5,curHE=1.5;
void InitGraphics(int argc, char *argv[]);
void SetTransformations() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-WIDTH/2.0, WIDTH/2.0, -HEIGHT/2.0, HEIGHT/2.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //CT <- I
}
int computeZ(IPoint Z,IPoint C)
{
	int depth =0;
	while(Z.absSQ()<=4 && depth < DEPTH)
		Z = Z.sq()+C,depth++;
	return depth;
}
void Update()
{
	double midH =((curHE+curHS)/2.0)  ;
	double midW =((curWE+curWS)/2.0)  ;
	if(abs(ZOOM-1)>0.0001)
	{
		curHS = ((curHS-midH)*ZOOM) + midH; 
		curHE = ((curHE-midH)*ZOOM) + midH; 
		curWS = ((curWS-midW)*ZOOM) + midW; 
		curWE = ((curWE-midW)*ZOOM) + midW; 
	}
	ZOOM=1;
	for(int i=0;i<HEIGHT;++i)
		for(int j=0;j<WIDTH;++j)
			paintP[i][j] = 0;
	double t = omp_get_wtime();
#pragma omp parallel for num_threads(64)
	for(int ii=0;ii<HEIGHT*WIDTH;++ii)
		{
			int i=ii/WIDTH;
			int j=ii%WIDTH;
			//cout<<omp_get_num_threads()<<endl;
			double stepJ = abs(curWE - curWS)/(WIDTH-1);
			double stepI = abs(curHE - curHS)/(HEIGHT-1);
			int ZVal = computeZ(IPoint(0,0),IPoint(curWS + j*stepJ,curHE - i*stepI));
			paintP[i][j] = ZVal < DEPTH?ZVal:0;
		}
		//cout<<omp_get_wtime()-t <<endl;

	char fpsString[128];
	sprintf(fpsString, "%f fps", 1/(omp_get_wtime()-t));
	glutSetWindowTitle(fpsString);
}

void OnKeyPressed(unsigned char key,int x,int y)
{
	if(key == 'd')
		DEPTH+=5;
	if(key == 'a')
		DEPTH-=5;
	if(key == '-')
		ZOOM=1.2;
	if(key == '+' || key == '=')
		ZOOM=0.8;
}
void OnSpecialKey(int key,int x,int y)
{
	double sizeW = curWE - curWS;
	double sizeH = curHE - curHS;
	if(key == GLUT_KEY_RIGHT)
		curWS+=0.1*sizeW,curWE+=0.1*sizeW;
	if(key == GLUT_KEY_LEFT)
		curWS-=0.1*sizeW,curWE-=0.1*sizeW;
	if(key == GLUT_KEY_UP)
		curHS-=0.1*sizeH,curHE-=0.1*sizeH;
	if(key == GLUT_KEY_DOWN)
		curHS+=0.1*sizeH,curHE+=0.1*sizeH;
}
void OnDisplay() {
	
	Update();

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	SetTransformations();
	glBegin(GL_POINTS);
	for(int i=0;i<HEIGHT;++i)
		for(int j=0;j<WIDTH;++j)
			if(paintP[i][j])
			{
				double dd = (paintP[i][j]/(double)DEPTH);
				glColor3d((paintP[i][j]/(double)DEPTH),0,0);
				glVertex2d(j-(WIDTH/2.0),i-(HEIGHT/2.0));
			}
	glEnd();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}
void InitGraphics(int argc, char *argv[]) {
	
	paintP = vector<vector<int>>(HEIGHT);
	for(int i=0;i<paintP.size();++i)
		paintP[i] = vector<int>(WIDTH,false);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100); 
	glutInitWindowSize(800, 800);
	glutCreateWindow("OpenGL Lab");
	glutDisplayFunc(OnDisplay);
	glutSpecialFunc(OnSpecialKey);
	glutKeyboardFunc(OnKeyPressed);

	glutMainLoop();
}

void showHelp()
{
	cout<<"Use the arrows to move\nUse '+' and '-' to zoom\nUse 'a' and 'd' to decrease and increase the depth";
}
int main(int argc, char* argv[]) 
{
	showHelp();
	InitGraphics(argc, argv);
}
