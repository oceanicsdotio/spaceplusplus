#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <stdarg.h> 
#include <time.h>
#define radians  3.1415926/180.0
#define pi       3.1415926
#include "model.h"     // order important
#include "quaternion.h"
#include "primitives.h"
#include "asteroid.cpp"
#include "drone.cpp"
#define RED                         1.0,0.0,0.0
#define WHITE                       1.0,1.0,1.0
#define GREEN                       0.0,1.0,0.0
#define BLUE                        0.0,0.0,1.0
#define YELLOW                      1.0,1.0,0.0
#define MAGENTA                     1.0,0.0,1.0
#define CYAN                        0.0,1.0,1.0
#define SHIP                        0
#define DRONE                       1
#define MISSILE                     2
#define SOLID                       true
#define WIRE                        false
#define LEFT_MOUSE_BUTTON_BIT       1
#define MIDDLE_MOUSE_BUTTON_BIT     2
#define RIGHT_MOUSE_BUTTON_BIT      4
#define PARENT_TYPE                 agent[parentID]->type

static float model_mat[16];
static int selected = 0;
static int nObj = 0;
static int TOTAL_TRIANGLES=0;
static int tubeState = 0;
static float domain = 2.0;
static bool lightState   = false;
static bool SHOW_EDGES   = true;
static bool SHOW_NORMALS = false;
static bool SHOW_BACK    = true;
static bool SHOW_FRONT   = true;
static bool RECALCULATE_MODEL = true;
static float fps = 0.0;
static clock_t fpsCount=0, fpsTimeNow=0, fpsTimePrevious=clock(), fpsTimeElapsed=0;

    GLfloat light_diffuse[] = {1.0,1.0,1.0,1.0};
    GLfloat light_position[] = {0.0,3.0,3.0,1.0};
    GLfloat light_specular[] = {1.0,1.0,1.0,1.0};
    GLfloat light_ambient[] = {1.0,1.0,1.0,1.0};
    GLfloat diffuse[] = {0.3,0.3,0.3,1.0};
    GLfloat ambient[] = {0.07,0.05,0.05,1.0};
    GLfloat specular[] = {0.6,0.6,0.6,1.0};
    GLfloat shininess = 40;




// SECTION: DRAW FUNCTIONS
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void draw_polygon (int npts) {
    // Draw regular polygon outline in (X, Y) plane
    float vec[] = {1.0, 0.0, 0.0};
    glBegin (GL_LINE_LOOP);
        for (int ii = 0; ii < npts; ii++) {
            glVertex3fv(vec);
            rotateVector(360.0/npts*radians, ZAXIS, vec);
        }
    glEnd ();
}



////////////////////////////////////////////////////////////////////////////
void draw_model (sbModel *M, int frame) {
    // Takes pointer to model instance and draws triangles from face and vertex data
    if (lightState)     {glEnable(GL_LIGHTING);                                 draw_triangles(M, SOLID); glDisable(GL_LIGHTING);} 
    if (SHOW_BACK)      {glCullFace(GL_FRONT); glColor4f(0.5, 0.3, 0.35, 0.75); draw_triangles(M, SOLID);}
    if (SHOW_FRONT)     {glCullFace(GL_BACK);  glColor4f(0.5, 0.5, 0.5, 0.75); draw_triangles(M, SOLID);}
    if (SHOW_EDGES)     {                      glColor4f(1.0, 1.0, 1.0, 0.05);  draw_triangles(M, WIRE);}
    if (SHOW_NORMALS)   {                      glColor4f(1.0, 0.0, 0.0, 0.1);   draw_avg_normals(M);}
}


////////////////////////////////////////////////////////////////////////////
void idle() {        
    for (int ii=0; ii<nObj; ii++) {
        for (int dim=0; dim<3; dim++) agent[ii]->position[dim] += agent[ii]->veloc[dim];
        
        if (agent[ii]->position[0] >= domain) agent[ii]->position[0] -= 2.0*domain;
        if (agent[ii]->position[0] <= -domain) agent[ii]->position[0] += 2.0*domain;
        if (agent[ii]->position[1] >= domain) agent[ii]->position[1] -= 2.0*domain;
        if (agent[ii]->position[1] <= -domain) agent[ii]->position[1] += 2.0*domain;
        if (agent[ii]->position[2] >= domain) agent[ii]->position[2] -= 2.0*domain;
        if (agent[ii]->position[2] <= -domain) agent[ii]->position[2] += 2.0*domain;
        rotateVector(agent[ii]->rot_v*radians, agent[ii]->rotaxis, agent[ii]->heading);
        rotateModel(agent[ii]->rot_v, agent[ii]->rotaxis, agent[ii]->objModel);
        
    } 

            
     if (fpsCounter()) glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////
void display () {
                               
    
    
    
    //glMatrixMode(GL_MODELVIEW); glLoadIdentity(); 
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    
    TOTAL_TRIANGLES = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    glMatrixMode(GL_MODELVIEW); glLoadIdentity(); glMultMatrixf(model_state->rot_matrix); // apply stored rotation
    
    
    for (int ii=0; ii<nObj; ii++) {
    glPushMatrix ();
        //glScalef(1.0,1.0,1.0);
        glTranslatef(agent[ii]->position[0],agent[ii]->position[1],agent[ii]->position[2]); // translate to position
        draw_model (agent[ii]->objModel, SOLID); // draw model
    glPopMatrix ();
    }


////////////////////////////////////////////////////////////////////////////
void initLighting () {

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); // material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

}


    
    if (RECALCULATE_MODEL) build_drone();
    char fname1[] = "tetra";
    agent[0] = new obj();
    importModel(fname1, agent[0]->objModel);
    calc_normals(agent[0]->objModel);