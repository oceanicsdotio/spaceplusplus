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
#include "classes.h"

#include "model.h"     // order important !!!
#include "quaternion.h"
#include "primitives.h"
#include "tug.cpp"
#include "asteroid.cpp"
#include "utility.h"
#include "draw.h"
#include "simulation.h"
#include "interface.h"


// INITIALIZATION AND PRIMARY PROGRAM
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void reshape(int w, int h) {
    //float scale = domain;
    float aspect = (float) w / (float) h; // calc window aspect ratio
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-domain*aspect, domain*aspect, -domain, domain, -domain, domain);
    glMatrixMode(GL_MODELVIEW);
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
        //rotateVector(agent[ii]->rot_v*radians, agent[ii]->rotaxis, agent[ii]->heading);
        //rotateModel(agent[ii]->rot_v, agent[ii]->rotaxis, agent[ii]->objModel);

    }
    if (fpsCounter()) glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////
void display () {


    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    glLightfv (GL_LIGHT1, GL_POSITION, light1_position);

    TOTAL_TRIANGLES = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); glMultMatrixf(model_state->rot_matrix); // apply stored rotation


    //findGroupHeading(agent, flock);
    //findGroupCenter(agent, flock);



    for (int ii=0; ii<nObj; ii++) {
    glPushMatrix ();
        if (ii==selected)   glColor4f(RED,   0.5); // set color for target indicator
        else                glColor4f(WHITE, 0.5);
        glTranslatef(agent[ii]->position[0],agent[ii]->position[1],agent[ii]->position[2]); // translate to position
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

    glFlush(); glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////

void initLighting () {

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

}

////////////////////////////////////////////////////////////////////////////
void init () {
    // set up GL environment
    glEnable(GL_FLAT);
    glEnable(GL_SMOOTH);


    if (SMOOTH_MODEL) glShadeModel(GL_SMOOTH);
    else              glShadeModel(GL_FLAT);


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

    //if (RECALCULATE_MODEL) calc_asteroid(6);
    char fname1[] = "aroid6";
    agent[0] = new obj();
    agent[0]->position[0] = 0.0;
    importModel(fname1, agent[0]->objModel);
    calc_normals(agent[0]->objModel);

}

////////////////////////////////////////////////////////////////////////////
int main (int argc, char** argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    int x_pix = glutGet (GLUT_SCREEN_WIDTH);
    int y_pix = glutGet (GLUT_SCREEN_HEIGHT);
    glutInitWindowSize (x_pix, y_pix);
    glutInitWindowPosition (0, 0);
    glutCreateWindow ("Space");


    initLighting();
    init();


    glutDisplayFunc (display);
    glutKeyboardFunc (KeyboardFunc);
    glutSpecialFunc (Special_Keys);
    glutMotionFunc(MotionFunc);
    glutMouseFunc(MouseFunc);
    glutIdleFunc (idle);
    glutReshapeFunc (reshape);
    glutMainLoop ();
}


