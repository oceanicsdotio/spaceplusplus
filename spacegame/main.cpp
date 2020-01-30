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
#include "space.h"
#include "../models/model.h"

#include "../models/model.h"     // order important !!!
#include "quaternion.h"
#include "primitives.cpp"
#include "../models/shipyard.cpp"
#include "models/asteroid.cpp"
#include "utility.h"
#include "draw.cpp"
#include "simulation.cpp"

#include "../models/interface.h"

#define radians  3.1415926/180.0
#define pi       3.1415926
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


#include "models/model.h"
#include "models/interface.h"
#include "primitives.cpp"
#include "simulation.cpp"
#include "draw.cpp"


int main (int argc, char** argv) {

    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    int x_pix = glutGet (GLUT_SCREEN_WIDTH);
    int y_pix = glutGet (GLUT_SCREEN_HEIGHT);
    glutInitWindowSize (x_pix, y_pix);
    glutInitWindowPosition (0, 0);
    glutCreateWindow ("Space");
    std::cout << "Starting...";
    // set up GL environment

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); // material properties
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
    model_state = new StateType;

    /*
    build_ship();
    for (int ii=0; ii<50; ii++) calc_normals(M[ii]);


    dm = new sbModel;
    char filename1[] = "drone";
    importModel(filename1, dm);
    calc_normals(dm);

    tm = new sbModel;
    char filename2[] = "hull";
    importModel(filename2, tm);
    calc_normals(tm);
    */


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


    model_state = new StateType;
    sbModel *Model;
    Model = new sbModel;
    int adjVert[100000][5];
    int nLevels = 4;

    icosahedron(Model);
    for (int ii=0; ii<nLevels; ii++) {
        printf("Subdivision %d\n", ii);
        Model->subdivide();
        removeDuplicates(Model);
    }

    adjacency(Model, adjVert);

    for (int ii=0; ii<100; ii++) {
        if (rand()%100 > 20) impact(Model, adjVert);
        if (rand()%100 > 47) roughen(Model);
        if (rand()%100 > 50) smooth(Model, adjVert);
    }
    smooth(Model, adjVert);

    Model->dump("bin/asteroid.dat");
    agent[0] = new obj();
    agent[0]->position[0] = 0.0;
    agent[0]->objModel = Model;
    calc_normals(agent[0]->objModel);

//    glutKeyboardFunc(KeyboardFunc);
    glutKeyboardFunc([](unsigned char key, int x, int y) {
        switch (key) {
            case 27: exit(0); break;
            case 'n':
                SHOW_NORMALS = !SHOW_NORMALS; break;
            case 'l':
                lightState = !lightState; break;
            case 'e':
                SHOW_EDGES = !SHOW_EDGES; break;
        }
    });


    glutSpecialFunc(SpecialKeys);
    glutMotionFunc(MotionFunc);
    glutMouseFunc(MouseFunc);
    glutIdleFunc(IdleFunc);
    glutReshapeFunc(ReshapeFunc);
    glutMainLoop();


//    glutDisplayFunc([]() {
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        glLightfv (GL_LIGHT0, GL_POSITION, light_position);
//        float rotvec[] = {1.0, -1.0, 0.0};
//        rotateVector(0.01, rotvec, light1_position);
//        glLightfv (GL_LIGHT1, GL_POSITION, light1_position);
//
//        TOTAL_TRIANGLES = 0;
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        glMultMatrixf(model_state->rot_matrix); // apply stored rotation
//
//        for (int ii=0; ii<nObj; ii++) {
//            glPushMatrix();
//            glScalef(1.0,1.0,1.0);
//            glTranslatef(agent[ii]->position[0], agent[ii]->position[1], agent[ii]->position[2]);
//            draw_model(agent[ii]->objModel);
//            glPopMatrix();
//        }
//
//        glFlush();
//        glutSwapBuffers();
//    });

    glutDisplayFunc([]() {

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLightfv (GL_LIGHT0, GL_POSITION, light_position);
        glLightfv (GL_LIGHT1, GL_POSITION, light1_position);

        TOTAL_TRIANGLES = 0;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glMultMatrixf(model_state->rot_matrix); // apply stored rotation

        //findGroupHeading(agent, flock);
        //findGroupCenter(agent, flock);

        for (int ii=0; ii<nObj; ii++) {
            glPushMatrix ();
            glColor4f(ii == selected ? RED : WHITE, 0.5);
            glTranslatef(agent[ii]->position[0], agent[ii]->position[1], agent[ii]->position[2]);
            draw_model(agent[ii]->objModel);

            glScalef(2.0,2.0,2.0); // scale circle
            draw_polygon (50); // draw circle around agent
            glPopMatrix ();
        }

        /*
        glPushMatrix ();
           glTranslatef(agent[0]->position[0],agent[0]->position[1],agent[0]->position[2]);
           for (int ii=0; ii<50; ii++) {
              draw_model (M[ii],SOLID);
           }
           draw_model(tm, SOLID);
        glPopMatrix ();
        */

        glFlush();
        glutSwapBuffers();
        );
    }
}


