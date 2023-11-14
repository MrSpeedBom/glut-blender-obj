#include<windows.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include "mainMemory.h"

Drawable x;
void updatePosition();
point position(-3,0,0);
float angle1=0,angle2=0;
float speed=0.1;
point getVector(float x,float y,int b=0)
{
    if(b==1)
    {
        x-=3.141592653589/2;
        return point{cos(x),sin(x),0};
    }
    if(b==2)
        y+=3.141592653589/2;
    return point{cos(x)*cos(y),sin(x)*cos(y),sin(y)};
}

void MyInit()
{
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1,1,-1,1,2,10);
    glMatrixMode(GL_MODELVIEW);

}

void initGL()
{
    glClearColor(0.0f,0.0f,0.0f,1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);  // Nice perspective corrections
}
void Draw()
{
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    updatePosition();
    point cen=getVector(angle1,angle2,0)+position;
    point up=getVector(angle1,angle2,2);
    glLoadIdentity();
    gluLookAt(position.x,position.y,position.z,cen.x,cen.y,cen.z,up.x,up.y,up.z);
    x.draw();
    glFlush();
    glutSwapBuffers();

}
static void specialKey(int key,int xx,int yy)
{
    switch(key)
    {

    case GLUT_KEY_LEFT:
    {
        angle1+=speed;
        break;
    }
    case GLUT_KEY_UP:
    {
        angle2+=speed;
        break;
    }
    case GLUT_KEY_RIGHT:
    {
        
        angle1-=speed;
        break;
    }
    case GLUT_KEY_DOWN:
    {
        angle2-=speed;
        break;
    }
    }
    glutPostRedisplay();
}
point vecrl,vecfb;
bool mov[6]={0,0,0,0};// 0 1 2 3  4 5// right front left back up down
void updatePosition() {
    vecrl=getVector(angle1,angle2,1)*speed;
    vecfb=getVector(angle1+acos(-1)/2,angle2,1)*speed;
    if(mov[0])
    {
        position=position+vecrl;
    }
    if(mov[1])
    {
        position=position+vecfb;
    }
    if(mov[2])
    {
        position=position-vecrl;
    }
    if(mov[3])
    {
        position=position-vecfb;
    }
    if(mov[4])
        position.z+=speed;
    if(mov[5])
        position.z-=speed;
}
void Key(unsigned char ch,int x,int y)
{
    switch(ch)
    {
    case 's': { mov[3]=1; break; }
    case 'w': { mov[1]=1; break; }


    case 'a': { mov[2]=1; break; }
    case 'd': { mov[0]=1; break; }

    case 'z': { mov[4]=1; break; }
    case 'x': { mov[5]=1; break; }
    }
    glutPostRedisplay();
}
void KeyUp(unsigned char ch,int x,int y)
{
    switch(ch)
    {
    case 's': { mov[3]=0; break; }
    case 'w': { mov[1]=0; break; }
    case 'a': { mov[2]=0; break; }
    case 'd': { mov[0]=0; break; }
    case 'z': { mov[4]=0; break; }
    case 'x': { mov[5]=0; break; }
    }
    glutPostRedisplay();
}



int main(int argC,char* argV[ ])
{
    glutInit(&argC,argV);
    glutInitWindowSize(600,600);
    glutInitWindowPosition(100,150);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
    glutCreateWindow("Color Cube with Camera");
   MyInit();
    initGL();
    glutDisplayFunc(Draw);
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(specialKey);

    x.loadFile("world.obj");
    glutMainLoop();

    return 0;
}