#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define BOARDWIDTH   10
#define BOARDHEIGHT  20
#define BOXWIDTH     2
#define BOXHEIGHT    2
#define GRIDGAP      1

uint32_t StepTime = 20; 
// Time step between updates in ms 
uint32_t ElapsedTime = 0; 
// Total elapsed time since the last update 
uint16_t key_board_elapsed_time = 0; 
// Score
uint32_t score = 0;

// I
const uint8_t i_piece[4][4] = {{0,0,0,0},
                               {1,1,1,1},
                               {0,0,0,0},
                               {0,0,0,0}};
// T
const uint8_t t_piece[3][3] = {{0,1,0},
                               {1,1,1},
                               {0,0,0}};
// L
const uint8_t l_piece[3][3] = {{0,0,1},
                               {1,1,1},
                               {0,0,0}};
// J
const uint8_t j_piece[3][3] = {{1,0,0},
                               {1,1,1},
                               {0,0,0}};
// s
const uint8_t s_piece[3][3] = {{0,1,1},
                               {1,1,0},
                               {0,0,0}};
// z
const uint8_t z_piece[3][3] = {{1,1,0},
                               {0,1,1},
                               {0,0,0}};
// O
const uint8_t o_piece[2][2] = {{1,1},
                               {1,1}};

uint8_t board[BOARDWIDTH][BOARDHEIGHT];

typedef struct
{
    uint8_t arr[4][4];
    int8_t x;
    int8_t y;
    uint8_t r; 
    uint8_t size;
} piece;

typedef struct
{
	uint8_t a,w,s,d,q,e,space;
}ButtonKeys;

piece active_piece;
piece hold_piece;
piece test_piece;
ButtonKeys Keys;
uint8_t bag[7] = {0,1,2,3,4,5,6};
uint8_t bag_pointer=0;
uint8_t changed = 0;
char buffer[20];

enum place_states
{
    CAN_PLACE,
    BLOCKED,
    OFFSCREEN
};

enum place_states canPlace(int8_t x,int8_t y,piece *p)
{
    int8_t dim = active_piece.size;
    
    for (int8_t px = 0; px < dim; px++)
    {
        for (int8_t py = 0; py < dim; py++)
        {
            int8_t coordx = x + px;
            int8_t coordy = y + py;

            if (p->arr[px][py]!=0)
            {
                if (coordx < 0 || coordx >= BOARDWIDTH)
                {
                    return OFFSCREEN;
                }
                if (coordy >= BOARDHEIGHT || board[coordx][coordy]!=0)
                {
                    return BLOCKED;
                }                
            }
        }
    }

    return CAN_PLACE;
}

void removeCompleteLines()
{
    uint8_t mult = 0;
    for (int8_t y = BOARDHEIGHT-1; y >= 0; y--)
    {
        uint8_t is_complete = 1;
        for (uint8_t x = 0; x < BOARDWIDTH; x++)
        {
            if (board[x][y]==0)
            {
                is_complete=0;
                break;
            }
            
        }
        if (is_complete)
        {
            for (uint8_t yc = y; yc > 0; yc--)
            {
                for (uint8_t x = 0; x < BOARDWIDTH; x++)
                {
                    board[x][yc]=board[x][yc-1];
                }
                
            }
            y++;
            mult++;
        }
    }
    score+=100*mult;
}

void place(uint8_t x,uint8_t y,piece *p)
{
    uint8_t dim = p->size;
    for (uint8_t px = 0; px < dim; px++)
    {
        for (uint8_t py = 0; py < dim; py++) 
        {
            int8_t coordx = x + px; 
            int8_t coordy = y + py; 
            if (p->arr[px][py] != 0) 
            {
                board[coordx][coordy] = 1;
            }
        }
    }
    removeCompleteLines();
    changed = 0;
}

void Rotate(uint8_t left,piece *p) 
{ 
    int dim = p->size;
    uint8_t tmpp[dim][dim];
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (left)
            {
                tmpp[j][i] = p->arr[i][dim - 1 - j];
            } 
            else 
            {
                tmpp[j][i] = p->arr[dim - 1 - i][j];
            }
        }
    }
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            p->arr[j][i] = tmpp[j][i];
        }
    }
    p->r = (p->r + (left ? 1 : 3))% 4;
    
}

void changePiece(uint8_t size ,const uint8_t (*piece_array)[size])
{
    active_piece.size = size;
    for(uint8_t i = 0;i < size;i++)
    {
        for(uint8_t j = 0;j < size;j++)
        {
            active_piece.arr[i][j] = piece_array[i][j];
        }   
    }
}

void shuffleBag()
{
    int16_t r;
    for(int8_t i = 6;i>0;i--)
    {
        r = rand() % i;
        bag[i] = bag[i]^bag[r];
        bag[r] = bag[i]^bag[r];
        bag[i] = bag[i]^bag[r];
    } 
}

void SpawnPiece()
{
    switch (bag[bag_pointer])
    {
    case 0:
        changePiece(4,(&i_piece)[0]);
        break;
    case 1:
        changePiece(3,(&t_piece)[0]);
        break;
    case 2:
        changePiece(3,(&l_piece)[0]);
        break;
    case 3:
        changePiece(3,(&j_piece)[0]);
        break;
    case 4:
        changePiece(3,(&s_piece)[0]);
        break;
    case 5:
        changePiece(3,(&z_piece)[0]);
        break;
    case 6:
        changePiece(2,(&o_piece)[0]);
        break;
    default:
        break;
    }
    active_piece.x = 4;
    active_piece.y = 0;
    active_piece.r = 0;
    bag_pointer++;
    if(bag_pointer > 6)
    {
        shuffleBag();
        bag_pointer = 0;
    }
}

void holdPiece()
{
    if (hold_piece.size != 0)
    {
        memcpy(&test_piece,&hold_piece,sizeof(piece));
        memcpy(&hold_piece,&active_piece,sizeof(piece));
        memcpy(&active_piece,&test_piece,sizeof(piece));
        active_piece.x = 4;
        active_piece.y = 0;
    }
    else
    {
        memcpy(&hold_piece,&active_piece,sizeof(piece));
        SpawnPiece();
    }
    while (hold_piece.r != 0)
    {
        Rotate(0,&hold_piece);
    }
}

void update() 
{ 
    if (ElapsedTime > StepTime) 
    { 
        // Create a new location for this spawned piece to go to on the next update 
        uint8_t NewSpawnedPieceLocationy = active_piece.y + 1;
        // Now check to see if we can place the piece at that new location 
        uint8_t ps = canPlace(active_piece.x, NewSpawnedPieceLocationy,&active_piece);
        if (ps != CAN_PLACE) 
        { 
            // We can't move down any further, so place the piece where ittrue is currently 
            place(active_piece.x, active_piece.y,&active_piece); 
            SpawnPiece(); 
            
            // This is just a check to see if the newly spawned piece is already blocked, in which case the 
            // game is over 
            ps = canPlace(active_piece.x,active_piece.y,&active_piece); 
            if (ps == BLOCKED) 
            { 
                return;
            }
        } 
        else 
        { 
            // We can move our piece into the new location, so update the existing piece location 
            active_piece.y = NewSpawnedPieceLocationy; 
        } 
        ElapsedTime = 0;
    }
    ElapsedTime++; 
    if (key_board_elapsed_time > 5) 
    { 
        if (Keys.a || Keys.d) 
        { 
            // Create a new location that contains where we WANT to move the piece 
            int8_t newlocation = active_piece.x + (Keys.a ? -1 : 1);
            // Next, check to see if we can actually place the piece there 
            uint8_t ps = canPlace(newlocation, active_piece.y,&active_piece);
            if (ps == CAN_PLACE)
            { 
                active_piece.x = newlocation; 
            } 
            key_board_elapsed_time = 0;
        }
        if(Keys.q || Keys.e) 
        { 
            memcpy(&test_piece,&active_piece,sizeof(piece));
            Rotate(Keys.q,&test_piece); 
            uint8_t ps = canPlace(active_piece.x, active_piece.y,&test_piece); 
            if (ps == CAN_PLACE) 
            { 
                memcpy(&active_piece,&test_piece,sizeof(piece)); 
            } 
            key_board_elapsed_time = 0; 
        }
        if(Keys.w && !changed)
        { 
            holdPiece();
            changed = 1;
        } 
        if(Keys.s)
        { 
            ElapsedTime = StepTime + 1; key_board_elapsed_time = 5;
        } 
        
    }
    key_board_elapsed_time++;
}

void reshape(GLint w, GLint h) 
{
  glViewport(0, 0, w, h);
  GLfloat aspect = (GLfloat)w / (GLfloat)h;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h) {
    // width is smaller, go from -50 .. 50 in width
    glOrtho(-50.0, 50.0, -50.0/aspect, 50.0/aspect, -1.0, 1.0);
  } else {
    // height is smaller, go from -50 .. 50 in height
    glOrtho(-50.0*aspect, 50.0*aspect, -50.0, 50.0, -1.0, 1.0);
  }
}

void RenderString(){  
	int i,l = 5;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();  
	
	glColor3f(1, 1, 1); // Green
    glTranslatef(18,25,0);
	glScalef(.025,.025,0);
	
	for (i=0; i<l; ++i)
	{
	    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, "Score"[i]);
	}
    glTranslatef(-524,-200,0);
    sprintf(buffer,"%05d",score);
    for (i=0; i<l; ++i)
	{
	    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, buffer[i]);
	}
    glTranslatef(-524,-200,0);
    for (i=0; i<l; ++i)
	{
	    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, "Hold"[i]);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-15,-30,0);

    // board

    for(uint8_t i = 0;i <BOARDWIDTH;i++)
    {
        for(uint8_t j = 0;j <BOARDHEIGHT;j++)
        {   
            if(board[i][j] != 0)
            {
                glColor3f(.17, .18, .27);
            }
            else
            {
                glColor3f(1, 1, .50);
            }
            glRectf((BOXWIDTH  * i) + (GRIDGAP * i),
                    (BOXHEIGHT * (BOARDHEIGHT - j)) + (GRIDGAP * (BOARDHEIGHT - j)), 
                    (BOXWIDTH  * (i+1)) + (GRIDGAP * i),
                    (BOXHEIGHT * (BOARDHEIGHT - (j+1)) + (GRIDGAP * (BOARDHEIGHT - j))));
        }   
    }
    
    // piece

    glColor3f(.1,.59,.54);
    for(uint8_t i = 0;i <active_piece.size;i++)
    {
        for(uint8_t j = 0;j <active_piece.size;j++)
        {   
            //
            if(active_piece.arr[i][j] != 0){
                glRectf((BOXWIDTH  * (i + active_piece.x)) + (GRIDGAP * (i + active_piece.x)),
                        (BOXHEIGHT * (BOARDHEIGHT - (j + active_piece.y))) + (GRIDGAP * (BOARDHEIGHT - (j + active_piece.y))),
                        (BOXWIDTH  * (i + 1 + active_piece.x)) + (GRIDGAP * (i + active_piece.x)),
                        (BOXHEIGHT * (BOARDHEIGHT -(j + 1 + active_piece.y))) + (GRIDGAP * (BOARDHEIGHT - (j + active_piece.y))));
            }
        }   
    }

    for(uint8_t i = 0;i <hold_piece.size;i++)
    {
        for(uint8_t j = 0;j <hold_piece.size;j++)
        {   
            //
            if(hold_piece.arr[i][j] != 0){
                glRectf((BOXWIDTH  * (i + (20 - hold_piece.size))),
                        (BOXHEIGHT * (BOARDHEIGHT - (j - 2))),
                        (BOXWIDTH  * (i + 1 + (20 - hold_piece.size))),
                        (BOXHEIGHT * (BOARDHEIGHT -(j + 1 - 2 ))));
            }
        }   
    }

    // border
    glColor3f(0.17, 0.18, 0.27);
    glRecti(-3,60,-1,-1);
    glRecti(30,60,32,-1);
    glRecti(-3,0,32,-2);

    RenderString();

    glFlush();
    glutSwapBuffers();  
    update(); 
}

void timer(int v) {
    glutPostRedisplay();
    glutTimerFunc(1000/30, timer, v);
}

void init()
{
    glClearColor(.0,.0,.0,1.0);
    shuffleBag();
    SpawnPiece();
    hold_piece.size = 0;
    // clear board
    for(uint8_t x = 0 ; x < BOARDWIDTH; x++)
    {
        for(uint8_t y = 0 ; y < BOARDHEIGHT; y++)
        {
            board[x][y] = 0;
        }
    }

}

void keysUp(unsigned char key, int x, int y){
    if(key=='a'){Keys.a=0;}
    if(key=='d'){Keys.d=0;}
    if(key=='s'){Keys.s=0;}
    if(key=='q'){Keys.q=0;}
    if(key=='e'){Keys.e=0;}
    if(key=='w'){Keys.w=0;}
    if(key==32){Keys.space=0;}
}

void keysDown(unsigned char key, int x, int y){
    if(key=='a'){Keys.a=1;}
    if(key=='d'){Keys.d=1;}
    if(key=='s'){Keys.s=1;}
    if(key=='q'){Keys.q=1;}
    if(key=='e'){Keys.e=1;}
    if(key=='w'){Keys.w=1;}
    if(key==32){Keys.space=1;}
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    init();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(800, 500);
    glutCreateWindow("Tetric");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    //glutMouseFunc(mouse);
    glutKeyboardFunc(keysDown);
    glutKeyboardUpFunc(keysUp);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();

  return 0;
}