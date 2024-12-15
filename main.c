#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define PI   3.141592
#define P2   1.570796
#define P3   4.712389
#define VO   6.283185
#define DR   0.017453

#define mapX 8
#define mapY 8
#define mapS mapX*mapY

typedef struct
{
	int a,w,s,d;
}ButtonKeys;

typedef struct
{
	float px,py,pdx,pdy,pa;
}Player;

int windowH,windowW;
int moy=0,moym=200;
float sense=1.5;

ButtonKeys Keys;
Player pl;

// Utils

float dist(float ax, float ay, float bx, float by, float ang){
	return(sqrtf((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
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

// MAP

int mapW[]={
	1,1,1,1,1,1,1,1,
	1,0,0,1,0,1,0,1,
	1,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,1,
	1,1,0,1,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,1,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D(){
	int x,y,xo,yo,o=512/mapX;
	for(y=0;y<mapY;y++){
		for(x=0;x<mapX;x++){
			if(mapW[y*mapX+x]==1){glColor3f(1,1,1);}else{glColor3f(0,0,0);}
			xo=x*o; yo=y*o;
			glBegin(GL_QUADS);
			glVertex2i(xo  ,yo  );
			glVertex2i(xo  ,yo+o);
			glVertex2i(xo+o,yo+o);
			glVertex2i(xo+o,yo  );
			glEnd();
		}
	}
}

void drawFloor(){
	glColor3f(0,0,.54);
	glBegin(GL_QUADS);
	glVertex2i(windowW/2 + 13 ,windowH/2 + moy);
	glVertex2i(windowW/2 + 13 ,windowH  );
	glVertex2i(windowW   - 13 ,windowH  );
	glVertex2i(windowW   - 13 ,windowH/2 +moy);
	glEnd();
}

// Player

void drawplayer(){
	glColor3f(1,0,1);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(pl.px,pl.py);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(pl.px,pl.py);
	glVertex2i(pl.px+pl.pdx*4,pl.py+pl.pdy*4);
	glEnd();
	
	glLineWidth(3);
	glColor3f(0,1,0);
	glBegin(GL_LINES);
	glVertex2i(pl.px,pl.py);
	glVertex2i(pl.px-pl.pdx*3,pl.py-pl.pdy*3);
	glEnd();
	
	glLineWidth(3);
	glColor3f(0,0,1);
	glBegin(GL_LINES);
	glVertex2i(pl.px,pl.py);
	glVertex2i(pl.px-pl.pdy*3,pl.py+pl.pdx*3);
	glEnd();
	
	glLineWidth(3);
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex2i(pl.px,pl.py);
	glVertex2i(pl.px+pl.pdy*3,pl.py-pl.pdx*3);
	glEnd();
}

// Input shenenigans

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

void look(int x, int y){
	
	
	int dx = x - windowW/2;
    int dy = y - windowH/2;
 
    glutWarpPointer(windowW/2, windowH/2);
	
	if(dx < 0){pl.pa-=0.02*sense; if(pl.pa < 0 ){ pl.pa+=VO;} pl.pdx = cosf(pl.pa)*5;pl.pdy = sin(pl.pa)*5;}
	if(dx > 0){pl.pa+=0.02*sense; if(pl.pa > VO){ pl.pa-=VO;} pl.pdx = cosf(pl.pa)*5;pl.pdy = sin(pl.pa)*5;}
	if(dy < 0){moy+=2*sense; if(moy >= moym){moy=moym;}}
	if(dy > 0){moy-=2*sense; if(moy <= -moym){moy=-moym;}}
	
	glutPostRedisplay();
	
}

void move(){
	
	int xo=0;if(pl.pdx<0){xo=-20;} else{xo=20;}
	int yo=0;if(pl.pdy<0){yo=-20;} else{yo=20;}
	int ipx=pl.px/64.0, ipx_add_xo=(pl.px+xo)/64.0,ipx_sub_xo=(pl.px-xo)/64.0,ipx_add_yo=(pl.px+yo)/64.0,ipx_sub_yo=(pl.px-yo)/64.0;
	int ipy=pl.py/64.0, ipy_add_yo=(pl.py+yo)/64.0,ipy_sub_yo=(pl.py-yo)/64.0,ipy_add_xo=(pl.py+xo)/64.0,ipy_sub_xo=(pl.py-xo)/64.0;
	
	if(Keys.w==1){
		if(mapW[ipy*mapX        + ipx_add_xo]==0){pl.px+=pl.pdx*.2;}
		if(mapW[ipy_add_yo*mapX + ipx       ]==0){pl.py+=pl.pdy*.2;}
		
	}
	if(Keys.s==1){
		if(mapW[ipy*mapX        + ipx_sub_xo]==0){pl.px-=pl.pdx*.2;}
		if(mapW[ipy_sub_yo*mapX + ipx       ]==0){pl.py-=pl.pdy*.2;}
	}
	// hehe um dia eu descubro vou dar um tempo 
	if(Keys.a==1){
		if(mapW[ipy*mapX        + ipx_add_yo]==0){pl.px+=pl.pdy*.2;}
		if(mapW[ipy_sub_xo*mapX + ipx       ]==0){pl.py-=pl.pdx*.2;}
	}
	if(Keys.d==1){
		if(mapW[ipy*mapY        + ipx_sub_yo]==0){pl.px-=pl.pdy*.2;}
		if(mapW[ipy_add_xo*mapX + ipx       ]==0){pl.py+=pl.pdx*.2;}
		}
	
	glutPostRedisplay();
}

// RAYcasting

void drawRays2d(){
	int r,mx,my,mp,dof;
	float rx,ry,ra,xo,yo;
	
	ra= fixAng(pl.pa-DR*30);
	
	for(r=0;r<60;r++){
		float distH=10000,hx,hy,distV=10000,vx,vy,distT;
		//check horizontal lines
		dof=0;
		float atan= -1/tan(ra);
		if(ra>PI){ry=(((int)pl.py>>6)<<6)-0.0001; rx=(pl.py-ry)*atan+pl.px; yo=-64; xo=-yo*atan;}
		if(ra<PI){ry=(((int)pl.py>>6)<<6)+64    ; rx=(pl.py-ry)*atan+pl.px; yo= 64; xo=-yo*atan;}
		if(ra==0 || ra==PI){hx=pl.px;hy=pl.py;dof=8;}
		
		while(dof<8){
			mx=(int)(rx)>>6;my=(int)(ry)>>6;mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && mapW[mp]==1){hx=rx;hy=ry;distH=dist(pl.px,pl.py,hx,hy,ra);dof=8;} //end loop
			else{rx+=xo; ry+=yo; dof+=1;}
		}
		
		xo=0;yo=0;
		//check vertical lines
		dof=0;
		float ntan= -tan(ra);
		if(ra>P2 && ra<P3){rx=(((int)pl.px>>6)<<6)-0.0001; ry=(pl.px-rx)*ntan+pl.py; xo=-64; yo=-xo*ntan;}
		if(ra<P2 || ra>P3){rx=(((int)pl.px>>6)<<6)+64    ; ry=(pl.px-rx)*ntan+pl.py; xo= 64; yo=-xo*ntan;}
		if(ra==0 || ra==PI){vx=pl.px;vy=pl.py;dof=8;}
		
		while(dof<8){
			mx=(int)(rx)>>6;my=(int)(ry)>>6;mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && mapW[mp]==1){vx=rx;vy=ry;distV=dist(pl.px,pl.py,vx,vy,ra);dof=8;} //end loop
			else{rx+=xo; ry+=yo; dof+=1;}
		}
		
		if(distH<distV){rx=hx;ry=hy;distT=distH;}
		else           {rx=vx;ry=vy;distT=distV;}
		
		float idistT = 1/(distT*.01);
		
		glColor3f(idistT ,idistT ,idistT );
		glLineWidth(3);glBegin(GL_LINES);glVertex2i(pl.px,pl.py);glVertex2i(rx,ry);glEnd();
		
		// Draw the 3D
		
		float da = fixAng(pl.pa-ra); distT=distT*cosf(da); // fish eye correction
		
		float lineH=(mapS*320)/distT; if(lineH>512+(moym*2)){lineH=512+(moym*2);}
		float lineO=(260-lineH/2);
		
		glLineWidth(8);glBegin(GL_LINES);glVertex2i(r*8+530,lineO+moy);glVertex2i(r*8+530,lineH+lineO+moy);glEnd();
		
		ra=fixAng(ra+DR);
	}
}

// Window

float frame1,frame2,fps;
char buffer[32];

void display()
{
	frame2=glutGet(GLUT_ELAPSED_TIME);fps=(frame2-frame1);frame1=glutGet(GLUT_ELAPSED_TIME); // Fps
	
	snprintf(buffer, sizeof buffer, "%.0f", fps); // Fps buffer value
		
	move();
	
    glClear(GL_COLOR_BUFFER_BIT); // clear 
    
    drawMap2D();
    drawFloor();
    drawRays2d();
    drawplayer();
    //RenderString(buffer);// Fps show
    
    glutSwapBuffers(); // redraw
}

void resize(int w, int h){
	windowW = w; windowH =h;
	float ratio =  w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(0,ratio,1,100);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


void init(){
	glClearColor(0,0,0,0);
	gluOrtho2D(0,1024,512,0);
	windowW = glutGet(GLUT_WINDOW_WIDTH); windowH =glutGet(GLUT_WINDOW_HEIGHT);
	pl.pa=PI;pl.px=300;pl.py=300;pl.pdx = cosf(pl.pa)*5;pl.pdy = sin(pl.pa)*5;
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
    glutPassiveMotionFunc(look);
    glutMainLoop();
    return 0;
}
