#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <string>
#include <time.h>
#include <math.h>
#include <vector>

#include "../include/texture.h"
#include "../include/light.h"
#include "../include/interface2.h"

using namespace texture;
using namespace lighting;


struct colorDict{
    GLfloat black[4] =   {0, 0, 0, 1};
    GLfloat white[4] =   {0, 0, 0, 1};
    GLfloat red[4] =     {1, 0, 0, 1};
    GLfloat magenta[4] = {1, 0, 1, 1};
} colors;


struct stateStruct {

    bool enableLight = true;
    int sliceAxis = 0;
    int nSlices = 80;
    int compositeOrder = -1;

    float origin[3] = {0, 0, 0};

    struct rotationStruct {
        float inc[3];
        float vec[3];
        float mat[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    } rotation, light;

    struct mouse {
        bool button;
        struct coordinates {
            int x;
            int y;
        } coordinates;
    } mouse;

    struct renderLoopStruct{
        float fps = 0.0;
        clock_t 
            frames=0,
            now=0,
            previous=clock(),
            ticks=0,
            report=200;
    } render;

} state;


auto testTexture = Texture("bin/density.dat", 50);
auto lightingData = LightData();


void print_string(void* font, std::string s, float x, float y, float z) {
    int lines = 0;
    for (int ii=0; ii<s.length(); ii++) {
        if (s[ii]=='&') {
            glRasterPos3f(x, y-0.05*(float)lines++, z);
        }
        else {
            glutBitmapCharacter(font, s[ii]);
        }
    }
}



class RotationIndicator
{
private:

    const float scale = 0.25;

    const GLfloat position[3] = {1.00, -0.5, 0.0};

    const float vertexArray[6][3] = {
            {*state.origin},
            {1.0, 0.0, 0.0},
            {*state.origin},
            {0.0, 1.0, 0.0},
            {*state.origin},
            {0.0, 0.0, 1.0}
    };

    struct styleStruct{
        GLfloat axes[4];
        GLfloat light[4];
    };

public:

    void render() const {

        const std::string labels[3] = {"&X", "&Y", "&Z"};

        glColor4fv(colors.black);
        glDisable(GL_DEPTH_TEST);
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(position[0], position[1], position[2]);
        glScalef(scale, scale, scale);

        glMultMatrixf(state.rotation.mat); // apply stored value

        glBegin(GL_LINES);
        for (int ii = 0; ii < 3; ii++) {
            glVertex3fv(this->vertexArray[2*ii]);
            glVertex3fv(this->vertexArray[2*ii+1]);
        }
        glEnd();

        for (int ii = 0; ii < 3; ii++){
            print_string(
                GLUT_BITMAP_HELVETICA_18,
                labels[ii],
                (ii == 0) ? 1.1 : 0.0,
                (ii == 1) ? 1.1 : 0.0,
                (ii == 2) ? 1.1 : 0.0
            );
        }


        glColor4fv(colors.magenta);
        glLoadIdentity();
        glTranslatef(1.00, -0.5, 0.0);
        glScalef(0.25/sqrt(3), 0.25/sqrt(3), 0.25/sqrt(3));

        glMultMatrixf(state.light.mat);

        glBegin(GL_LINES);
        glVertex3fv(state.origin);
        glVertex3fv(lightingData.light.position);
        glEnd();

        glPopMatrix();
    };
};



int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Texture Visualizer");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glClearColor(1, 1, 1, 1);
    glLineWidth(2);

    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightingData.light.diffuse);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightingData.light.ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, lightingData.light.position);

    glEnable(GL_BLEND);
    glEnable(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    testTexture.bind();

    glutReshapeFunc(ReshapeFunc);
    glutIdleFunc(glutPostRedisplay);

    glutDisplayFunc([]() {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear desktop from buffer
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glMultMatrixf(state.rotation.mat);
        glEnable(GL_TEXTURE_3D);

        for (int ii=0; ii<6; ii++) {
            testTexture.clip.eqn[ii*4+3] = (testTexture.clip.bound[ii]>0.0) ? testTexture.clip.bound[ii] : -testTexture.clip.bound[ii];
            glClipPlane(GL_CLIP_PLANE0+ii, &testTexture.clip.eqn[ii*4]);
            glEnable(GL_CLIP_PLANE0+ii);
        }

        glPushMatrix();
        glLoadIdentity();
        glMultMatrixf(state.light.mat);
        glLightfv(GL_LIGHT0, GL_POSITION, lightingData.light.position);
        glPopMatrix();

        for (int ii = 0; ii < state.nSlices; ii++) { // slice loop

            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lightingData.material.diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lightingData.material.ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lightingData.material.specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, lightingData.material.shininess);

            glBegin(GL_POLYGON);

            float iStep = ii / (float) state.nSlices;

            if (state.sliceAxis==0) {
                glTexCoord3f(0.0, 0.0, iStep);
                glTexCoord3f(0.0, 1.0, iStep);
                glTexCoord3f(1.0, 1.0, iStep);
                glTexCoord3f(1.0, 0.0, iStep);

                glVertex3f(-0.5, -0.5, -0.5+iStep);
                glVertex3f(-0.5,  0.5, -0.5+iStep);
                glVertex3f( 0.5,  0.5, -0.5+iStep);
                glVertex3f( 0.5, -0.5, -0.5+iStep);
            }
            if (state.sliceAxis==1) {
                glTexCoord3f(0.0, iStep, 0.0);
                glTexCoord3f(1.0, iStep, 0.0);
                glTexCoord3f(1.0, iStep, 1.0);
                glTexCoord3f(0.0, iStep, 1.0);

                glVertex3f(-0.5, -0.5+iStep, -0.5);
                glVertex3f( 0.5, -0.5+iStep, -0.5);
                glVertex3f( 0.5, -0.5+iStep,  0.5);
                glVertex3f(-0.5, -0.5+iStep,  0.5);
            }
            if (state.sliceAxis==2) {
                glTexCoord3f(iStep, 0.0, 0.0);
                glTexCoord3f(iStep, 0.0, 1.0);
                glTexCoord3f(iStep, 1.0, 1.0);
                glTexCoord3f(iStep, 1.0, 0.0);

                glVertex3f(-0.5+iStep, -0.5, -0.5);
                glVertex3f(-0.5+iStep, -0.5,  0.5);
                glVertex3f(-0.5+iStep,  0.5,  0.5);
                glVertex3f(-0.5+iStep,  0.5, -0.5);
            }
            glEnd();
        }
        glDisable(GL_TEXTURE_3D);


        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);

        for (int ii=0; ii<6; ii++) {
            glDisable(GL_CLIP_PLANE0 + ii);
        }

        BoundingBox().render(testTexture, colors.black);
        RotationIndicator().render();

        {
            glColor4fv(colors.black);
            glPushMatrix();
            glLoadIdentity();

            print_string(GLUT_BITMAP_HELVETICA_18, gui_str, 0.75, 0.75, 0.0);

            glPopMatrix();
            glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            glutSwapBuffers();
        }

        if (++state.render.frames >= state.render.report) {
            state.render.now = clock();
            state.render.ticks = state.render.now - state.render.previous;
            float seconds = ((float) state.render.ticks) / CLOCKS_PER_SEC;
            std::cout << "Average framerate:" << state.render.report / seconds << std::endl;
            state.render.previous = state.render.now;
            state.render.frames = 0;
        }

    });

    glutKeyboardFunc([](unsigned char key, int x, int y) {

        switch (key) {
            case 27:
                exit(0);
                break;
            case 'l':
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, state.enableLight ? GL_REPLACE : GL_MODULATE);
                state.enableLight = !state.enableLight;
                break;

            case 'A':
                testTexture.alpha += (testTexture.alpha < 100) ? 5 : 0;
                break;

            case 'a':
                testTexture.alpha -= (testTexture.alpha > 5) ? 5 : 0;
                break;

            case 'r':
                state = stateStruct();
                break;

            case 'c':
                for (int ii=0; ii<6; ii++) {
                    if (testTexture.clip.dim[ii]&&(testTexture.clip.bound[ii] > 0.1)) {
                        testTexture.clip.bound[ii] -= 0.01;
                    }
                    if (testTexture.clip.dim[ii]&&(testTexture.clip.bound[ii] < -0.1)) {
                        testTexture.clip.bound[ii] += 0.01;
                    }
                }
                break;

            case 'C':
                for (int ii=0; ii<6; ii++) {
                    if (testTexture.clip.dim[ii]&&(testTexture.clip.bound[ii]>0.00)&&(testTexture.clip.bound[ii] < 0.5)) {
                        testTexture.clip.bound[ii] += 0.01;
                    }
                    if (testTexture.clip.dim[ii]&&(testTexture.clip.bound[ii]<0.00)&&(testTexture.clip.bound[ii] > -0.5)) {
                        testTexture.clip.bound[ii] -= 0.01;
                    }
                }
                break;

            case 's':
                if (state.nSlices>4) {
                    for (int ii=(state.nSlices-1); ii>=4; ii--) {
                        if (iWidth % ii == 0) {
                            state.nSlices = ii;
                            break;
                        }
                    }
                }
                break;

            case 'S':
                if (state.nSlices < iWidth) {
                    for (int ii=(state.nSlices+1); ii<=iWidth; ii++) {
                        if (iWidth % ii == 0) {
                            state.nSlices=ii;
                            break;
                        }
                    }
                }
                break;

            case 'd':
                state.sliceAxis = (state.sliceAxis + 1) % 3;
                break;
        }
    });

    glutMotionFunc([] (int x, int y) {


        if (state.mouse.button) {

            int dx = x - state.mouse.coordinates.x;
            state.mouse.coordinates.x = x;
            int dy = y - state.mouse.coordinates.y;
            state.mouse.coordinates.y = y;

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            state.rotation.inc[1] = .5*dx;
            state.rotation.vec[1] += state.rotation.inc[1];
            glRotatef (state.rotation.inc[1], 0, 1, 0);

            state.rotation.inc[0] = .5*dy;
            state.rotation.vec[0] += state.rotation.inc[0];
            glRotatef (state.rotation.inc[0], 1, 0, 0);

            glMultMatrixf(state.rotation.mat);
            glGetFloatv(GL_MODELVIEW_MATRIX, state.rotation.mat); // store current to state
        }

    });


    glutMouseFunc([] (int button, int buttonstate, int x, int y) {

        state.mouse.coordinates.x = x;
        state.mouse.coordinates.y = y;

        if (button == GLUT_LEFT_BUTTON) {
            if (buttonstate == GLUT_DOWN) state.mouse.button |= LEFT_MOUSE_BUTTON_BIT;
            else state.mouse.button &= ~LEFT_MOUSE_BUTTON_BIT;}
        if (button == GLUT_RIGHT_BUTTON){
            if (buttonstate == GLUT_DOWN) state.mouse.button |= RIGHT_MOUSE_BUTTON_BIT;
            else state.mouse.button &= ~RIGHT_MOUSE_BUTTON_BIT;
        }
        glutPostRedisplay();
    });


    glutMainLoop();
}