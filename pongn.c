#include <stdio.h>
#include <stdlib.h>
#include "C:/MinGW/include/GL/glut.h"
#include <math.h>
#include<time.h>

#define FPS 60
#define PI   3.141592 // 180
#define P2   PI/2     // 90
#define P3   PI/4     // 45
#define VO   PI*2     // 360
#define DR   0.017453 // 1

typedef struct
{
	int a,w,s,d;
}ButtonKeys;

typedef struct
{
	float x,y;
}Point;

typedef struct
{
	Point p;
	float ang,v;
}Ball;

typedef struct
{
	Point p1,p2;
	float normal,color;
	int flag;
}Wall;


int windowH,windowW;
int hwindowH,hwindowW;

ButtonKeys Keys;
Ball ball;
Point* points;
Wall* walls;

float wSize;

// Utils

float dist(Point a, Point b){
	return(sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y)));
}

float fixAng(float ang){
	if(ang<0) {ang+=VO;}
	if(ang>VO){ang-=VO;}
	return ang;
}

void RenderString(const char* s){  
	int i,l=strlen(s);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 1024, 0.0, 512);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();  
	
	glColor3f(1, 0, 0); // Green
	
	glRasterPos2i(10, 10);

	for (i=0; i<l; ++i)
	{
	    char c = s[i];
	    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glEnd();
}

float dotProduct(Ball b, Wall w){
	return ((b.v*cos(b.ang))*cos(w.normal))+((b.v*sin(b.ang))*sin(w.normal));
}

float disAng(float ux,float uy,float wx,float wy){
	float vx = wx - ux;
	float vy = wy - uy;

	float ang = -atan2(-vy,vx);

	return ang; 
}

// MAP

int sides = 7, size = 200;

void drawMap2D(){
	
	int i;
	for(i=0; i<sides;i++){
		glColor3f(walls[i].color,1,0);
		glLineWidth(5);
		glBegin(GL_LINES);
		glVertex2f(walls[i].p1.x,walls[i].p1.y);
		glVertex2f(walls[i].p2.x,walls[i].p2.y);
		glEnd();
	}
	

}

// Player

void drawBall(){
	
	// move
	
	ball.p.x += cos(ball.ang)*ball.v;ball.p.y += sin(ball.ang)*ball.v;
	
	// collision
	
	int i,j;
	float sumDist,m = ball.v*.25;
	float marD = wSize-m, marS = wSize+m; // margin of precision
	
	for(i=0;i<sides;i++){
		sumDist = dist(ball.p,walls[i].p1)+dist(ball.p,walls[i].p2);
		if(sumDist >= marD && sumDist <= marS){
			if(walls[i].flag == 1){
				
				float dp = dotProduct(ball,walls[i]);
				float ux = dp*cos(walls[i].normal);
				float uy = dp*sin(walls[i].normal);
				float wx = (ball.v*cos(ball.ang)) - ux;
				float wy = (ball.v*sin(ball.ang)) - uy;
				ball.ang = fixAng(disAng(ux,uy,wx,wy));
				ball.v += .1;
				walls[i].flag = 0;
				walls[i].color = 1;
				
				for(j=0;j<sides;j++){
					if(j!=i){
						walls[j].flag = 1;
						walls[j].color = 0;
					}
				}
			}
			
		}
	}
	
	glColor3f(1,0,0);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2f(ball.p.x,ball.p.y);
	glEnd();
}

// Input sh

void keysUp(unsigned char key, int x, int y){
	if(key=='a'){Keys.a=0;}
	if(key=='d'){Keys.d=0;}
	if(key=='w'){Keys.w=0;}
	if(key=='s'){Keys.s=0;}
}

void keysDown(unsigned char key, int x, int y){
	if(key=='a'){Keys.a=1;}
	if(key=='d'){Keys.d=1;}
	if(key=='w'){Keys.w=1;}
	if(key=='s'){Keys.s=1;}
}

// Window

char buffer[32];

void display(){		
	snprintf(buffer, sizeof buffer, "%.1f", ball.v); // Fps buffer value
	
    glClear(GL_COLOR_BUFFER_BIT); // clear 
    
    drawMap2D();
    drawBall();
    RenderString(buffer);// Fps show
    
    glFlush();
    glutSwapBuffers(); // redraw
}

void resize(int w, int h){
	windowW = w; windowH =h;
	hwindowW = windowW/2;hwindowH = windowH/2;
	
	float ratio =  w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(0,ratio,1,100);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void timer(int v) {
	
	glutPostRedisplay();
	glutTimerFunc(1000/FPS, timer, v);

}

void init(){
	glClearColor(0,0,0,0);
	gluOrtho2D(0,1024,512,0);
	windowW = glutGet(GLUT_WINDOW_WIDTH); windowH =glutGet(GLUT_WINDOW_HEIGHT);
	hwindowW = windowW/2;hwindowH = windowH/2;
	
	// create polygon
	
	int i; 
	float ang;
	
	points = (Point*)malloc(sides * sizeof(Point));
	walls = (Wall*)malloc(sides * sizeof(Wall));
	
	ang = (VO / sides);
		
	points[0].x = hwindowW +(cos(ang)*size);
	points[0].y = hwindowH +(sin(ang)*size);
	walls[0].normal = fixAng(ang + (VO / (sides*2))  + PI);  
	walls[0].flag = 1;
	
	for(i=1; i < sides;i++){
		
		ang = fixAng(ang + (VO / sides));
		
		points[i].x = hwindowW +(cos(ang)*size);
		points[i].y = hwindowH +(sin(ang)*size);
		walls[i].normal = fixAng(ang + (VO / (sides*2))  + PI);
		walls[i].flag = 1;
		
		walls[i-1].p1 = points[i-1];
		walls[i-1].p2 = points[i];		
	}
	
	walls[sides-1].p1 = points[sides-1];
	walls[sides-1].p2 = points[0];
	
	wSize = dist(walls[0].p1,walls[0].p2);
	
	// ball init
	
	srand(time(NULL));
	ball.ang = fixAng((((rand()%sides)+1)*(VO / sides))+P2);
	ball.p.x = hwindowW;ball.p.y = hwindowH;ball.v = 5;
	
}

int main(int argc, char** argv)
{ 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024,512);
    glutCreateWindow("something fishy");
    
    init();
    
	glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keysDown);
    glutKeyboardUpFunc(keysUp);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();

    return 0;
}
