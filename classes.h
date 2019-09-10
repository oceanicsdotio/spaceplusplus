// SECTION: CLASSES
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class sbModel {
    public:
        int nv, nf;
        float vert[100000][3];
        float norm[100000][3];
        float norf[100000][3];
        int face[100000][3];
        int flag[100000];
        GLfloat glLightMat[3][4], shine;
        sbModel() {
            nv = 0;
            nf = 0;
            shine = 100;
            for (int ii=0; ii<100000; ii++) {
                flag[ii] = 0;
                for (int jj=0; jj<3; jj++) {
                    face[ii][jj] = 0;
                    vert[ii][jj] = 0.0;
                    norm[ii][jj] = 0.0;
                    norf[ii][jj] = 0.0;
                }
            }
            for (int ii=0; ii<4; ii++) {
                if (ii<3) {glLightMat[0][ii] = 0.1;
                           glLightMat[1][ii] = 0.5;
                           glLightMat[2][ii] = 1.0;}
                else      {glLightMat[0][ii] = 1.0;
                           glLightMat[1][ii] = 1.0;
                           glLightMat[2][ii] = 1.0;}
            }
        }
}; sbModel *dm, *tm, *M[100], *am;

////////////////////////////////////////////////////////////////////////////
class obj {
    public:
    int id, target, type;
    int shield_state, shield_timer;
    float prevHeading[3], heading[3], rot_v, position[3], veloc[3], rotaxis[3];
    sbModel *objModel;

    obj () { // constructor
        id = nObj++;
        target = id;
        type = 0;
        rot_v = 0.0;

        shield_state = 0;   shield_timer = 50;
        prevHeading[0] = 1.0; prevHeading[1] = 0.0; prevHeading[2] = 0.0;
        heading[0] = 1.0;   heading[1] = 0.0;   heading[2] = 0.0;
        position[0] = 0.0;  position[1] = 0.0;  position[2] = 0.0;
        veloc[0] = 0.0;     veloc[1] = 0.0;     veloc[2] = 0.0;
        rotaxis[0] = float(rand()%100)/100;   rotaxis[1] = float(rand()%100)/100;   rotaxis[2] = float(rand()%100)/100;

        objModel = new sbModel;
    }
}; obj *agent[1000], *newObj;

////////////////////////////////////////////////////////////////////////////
class lightSource {
    public:
    int id;
    GLfloat position[4], diffuse[4], specular[4], ambient[4];

    lightSource () {
        for (int ii=0; ii<4; ii++) {
            position[ii] = 0.0;
            diffuse[ii]  = 0.0;
            specular[ii] = 0.0;
            ambient[ii]  = 0.0;
        }
    }
}; lightSource *light[8];

////////////////////////////////////////////////////////////////////////////
class emplacement {
    public:
    int state, timer, cooldown;
    emplacement () {
        state = 0;
        timer = 50;
        cooldown = 50;
    }
}; emplacement *weapon[1000];

////////////////////////////////////////////////////////////////////////////
class StateType {
    public:
        int mbs, mouse_prev_x, mouse_prev_y; // mouse states
        float rot[3], rot_inc[3]; //* Transformation states
        float rot_matrix[16]; // rotation matrix storage
        StateType () {
            mbs = 0; // mouse button state
            for (int ii=0; ii<16; ii++) {
                if (ii==0 || ii==5 || ii==10 || ii==15) {rot_matrix[ii] = 1;}
                else {rot_matrix[ii] = 0;} //initialize as identity matrix
            }
        }
}; StateType *model_state;



class group {
    public:
        int size;
        int list[1000];
        float center[3];
        float heading[3];

        group() {
            size = 0;
            for (int ii=0; ii<1000; ii++) list[ii]=0;
            center[0] = center[1] = center[2] = 0.0;
            heading[0] = heading[1] = heading[2] = 0.0;
        }
}; group *flock;