#define radians  3.1415926/180.0
#define pi       3.1415926
#define RED                         1.0,0.0,0.0
#define WHITE                       1.0,1.0,1.0
#define GREEN                       0.0,1.0,0.0
#define BLUE                        0.0,0.0,1.0
#define YELLOW                      1.0,1.0,0.0
#define MAGENTA                     1.0,0.0,1.0
#define CYAN                        0.0,1.0,1.0
#define SOLID                       true
#define WIRE                        false
#define LEFT_MOUSE_BUTTON_BIT       1
#define MIDDLE_MOUSE_BUTTON_BIT     2
#define RIGHT_MOUSE_BUTTON_BIT      4
#define PARENT_TYPE                 agent[parentID]->type

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <stdarg.h>
#include <time.h>


#include "../include/primitive.h"
#include "../include/model.h"
#include "../include/agent.h"
#include "../include/interface.h"


static float model_mat[16];
static int selected = -1;
static int nObj = 0, nWpn = 0;
static int TOTAL_TRIANGLES=0;
static int tubeState = 0;
static float coverAngle = 0.0;
static float domain = 4.0;
static bool lightState   = true;
static bool SHOW_EDGES   = false;
static bool SHOW_NORMALS = false;
static bool SHOW_BACK    = true;
static bool SHOW_FRONT   = true;
static bool SMOOTH_MODEL = true;
static bool RECALCULATE_MODEL = true;
static float fps = 0.0;
static clock_t fpsCount=0, fpsTimeNow=0, fpsTimePrevious=clock(), fpsTimeElapsed=0;


int main (int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(
            glutGet(GLUT_SCREEN_WIDTH),
            glutGet(GLUT_SCREEN_HEIGHT)
    );

    glutInitWindowPosition (0, 0);
    glutCreateWindow("spackle");

    std::cout << "Starting...";

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);

    glLightfv (GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv (GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv (GL_LIGHT1, GL_POSITION, light1_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_FLAT);
    glEnable(GL_SMOOTH);

    glShadeModel(SMOOTH_MODEL ? GL_SMOOTH : GL_FLAT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*
    build_ship();
    for (int ii=0; ii<50; ii++) calc_normals(M[ii]);
    /*
    int nDrones = 3;
    flock = new group();
    for (int ii=0; ii<nDrones; ii++) {
        agent[ii] = new obj();
        weapon[ii] = new emplacement();
        if (ii>0) {
            copy(dm, agent[ii]->objModel);
            rotateModel(vecAngle(XAXIS,agent[ii]->heading), ZAXIS, agent[ii]->objModel);
            agent[ii]->target = 0;
            agent[ii]->veloc[0] = 0.01;
            agent[ii]->position[0] = (rand()%100)/50.0;
            agent[ii]->position[1] = (rand()%100)/50.0;
            //agent[ii]->position[2] = (rand()%100)/50.0;
            flock->list[flock->size++] = ii;
        }
    }*/

    model_state = StateType();
    sbModel *Model;
    Model = new sbModel;
    int adjVert[100000][5];
    const nLevels = 4;
    icosahedron(Model);
    for (int ii=0; ii<nLevels; ii++) {
        Model->subdivide();
        removeDuplicates(Model);
    }

    adjacency(Model, adjVert);

//    for (int ii=0; ii<100; ii++) {
//        if (rand()%100 > 20) impact(Model, adjVert);
//        if (rand()%100 > 47) roughen(Model);
//        if (rand()%100 > 50) smooth(Model, adjVert);
//    }
//
//    smooth(Model, adjVert);

//    Model->dump("bin/asteroid.dat");

    agent[0] = new obj();
    agent[0]->position[0] = 0.0;
    agent[0]->objModel = Model;

    calc_normals(agent[0]->objModel);

    glutSpecialFunc(SpecialKeys);
    glutMotionFunc(MotionFunc);
    glutMouseFunc(MouseFunc);
    glutIdleFunc(IdleFunc);
    glutReshapeFunc(ReshapeFunc);
    glutMainLoop();
    glutKeyboardFunc(SimpleKeyboardFunc);
    glutDisplayFunc(DisplayFunc);

}


