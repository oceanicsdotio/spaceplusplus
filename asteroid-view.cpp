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
#include "model.h"     // order important
#include "quaternion.h"
#include "primitives.h"
#include "asteroid.cpp"
#include "utility.h"
#include "draw.h"



// SECTION: GLUT USER CONTROL FUNCTIONS
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void KeyboardFunc (unsigned char key, int x, int y) {
    switch (key) {
        case 27: exit(0); break;
        case 'n':
            SHOW_NORMALS = !SHOW_NORMALS; break;
        case 'l':
            lightState = !lightState; break;
        case 'e':
            SHOW_EDGES = !SHOW_EDGES; break;
    }
}


////////////////////////////////////////////////////////////////////////////
void MouseFunc (int button, int buttonstate, int x, int y) {
    model_state->mouse_prev_x = x; model_state->mouse_prev_y = y;
    if (button == GLUT_LEFT_BUTTON) {
        if (buttonstate == GLUT_DOWN) model_state->mbs |= LEFT_MOUSE_BUTTON_BIT;
        else model_state->mbs &= ~LEFT_MOUSE_BUTTON_BIT;}
    else if (button == GLUT_MIDDLE_BUTTON) {
        if (buttonstate == GLUT_DOWN) model_state->mbs |= MIDDLE_MOUSE_BUTTON_BIT;
        else  model_state->mbs &= ~MIDDLE_MOUSE_BUTTON_BIT; }
    else if (button == GLUT_RIGHT_BUTTON){
        if (buttonstate == GLUT_DOWN) model_state->mbs |= RIGHT_MOUSE_BUTTON_BIT;
        else model_state->mbs &= ~RIGHT_MOUSE_BUTTON_BIT;
    }
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////
void MotionFunc (int x, int y) { // when mouse moves
    int dx = x - model_state->mouse_prev_x; model_state->mouse_prev_x = x;
    int dy = y - model_state->mouse_prev_y; model_state->mouse_prev_y = y;
    if (model_state->mbs & LEFT_MOUSE_BUTTON_BIT) { // if left button held
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        model_state->rot_inc[1] = .5*dx; model_state->rot[1]+=model_state->rot_inc[1]; glRotatef (model_state->rot_inc[1], 0, 1, 0);
        model_state->rot_inc[0] = .5*dy; model_state->rot[0]+=model_state->rot_inc[0]; glRotatef (model_state->rot_inc[0], 1, 0, 0);
        glMultMatrixf(model_state->rot_matrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, model_state->rot_matrix); // store current to state
    }
}

// SECTION: INITIALIZATION AND PRIMARY PROGRAM
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void reshape(int w, int h) {
    float scale = domain;
    float aspect = (float) w / (float) h; // calc window aspect ratio
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-scale*aspect, scale*aspect, -scale, scale, -scale, scale);
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
        rotateVector(agent[ii]->rot_v*radians, agent[ii]->rotaxis, agent[ii]->heading);
        rotateModel(agent[ii]->rot_v, agent[ii]->rotaxis, agent[ii]->objModel);

    }


     if (fpsCounter()) glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////
void display () {




    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    float rotvec[] = {1.0, -1.0, 0.0};
    rotateVector(0.01, rotvec, light1_position);
    glLightfv (GL_LIGHT1, GL_POSITION, light1_position);

    TOTAL_TRIANGLES = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity(); glMultMatrixf(model_state->rot_matrix); // apply stored rotation


    for (int ii=0; ii<nObj; ii++) {
    glPushMatrix ();
        glScalef(1.0,1.0,1.0);
        glTranslatef(agent[ii]->position[0],agent[ii]->position[1],agent[ii]->position[2]); // translate to position
        draw_model (agent[ii]->objModel, SOLID); // draw model
    glPopMatrix ();
    }

    glFlush();
    glutSwapBuffers();
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

    glLightfv (GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv (GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv (GL_LIGHT1, GL_POSITION, light1_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

}


void init () {

    // set up GL environment
    glShadeModel(GL_FLAT);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_SMOOTH);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    model_state = new StateType;

    //if (RECALCULATE_MODEL) calc_asteroid(8);
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
    glutCreateWindow ("Asteroid");
    initLighting();
    init();
    glutDisplayFunc (display);
    glutKeyboardFunc (KeyboardFunc);
    glutMotionFunc(MotionFunc);
    glutMouseFunc(MouseFunc);
    glutIdleFunc (idle);
    glutReshapeFunc (reshape);
    glutMainLoop ();
}

////////////////////////////////////////////////////////////////////////////

