GLfloat light_diffuse[] = {1.0,1.0,1.0,1.0};
GLfloat light_position[] = {0.0,3.0,3.0,1.0};
GLfloat light_specular[] = {1.0,1.0,1.0,1.0};
GLfloat light_ambient[] = {1.0,1.0,1.0,1.0};
GLfloat light1_diffuse[] = {0.0,0.0,1.0,1.0};
GLfloat light1_position[] = {1.0,1.5,0.0,1.0};
GLfloat light1_specular[] = {1.0,1.0,1.0,1.0};
GLfloat light1_ambient[] = {1.0,1.0,1.0,1.0};
GLfloat diffuse[] = {0.3,0.3,0.3,1.0};
GLfloat ambient[] = {0.06,0.05,0.05,1.0};
GLfloat specular[] = {0.6,0.6,0.6,1.0};
GLfloat shininess = 40;


namespace lighting {

    class Light
    {
    public:
        int id;
        GLfloat position[4], diffuse[4], specular[4], ambient[4];

        Light () {
            for (int ii=0; ii<4; ii++) {
                position[ii] = 0.0;
                diffuse[ii]  = 0.0;
                specular[ii] = 0.0;
                ambient[ii]  = 0.0;
            }
        }
    };

    struct LightData {
        struct {
            GLfloat shininess[1] = {100.0};
            GLfloat specular[4] = {1.0, 1.0, 1.0, 1.0};
            GLfloat diffuse[4] = {1.0, 0.0, 1.0, 1.0};
            GLfloat ambient[4] = {0.5, 0.0, 0.5, 1.0};
        } material;
        struct {
            GLfloat diffuse[4] = {1.0, 1.0, 1.0, 0.0};
            GLfloat ambient[4] = {1.0, 1.0, 1.0, 1.0};
            GLfloat position[4] = {1.0, 1.0, -1.0, 0.0};
        } light;
    };

}

