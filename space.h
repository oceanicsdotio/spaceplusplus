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

    GLfloat light_diffuse[] = {1.0,1.0,1.0,1.0};
    GLfloat light_position[] = {0.0,3.0,3.0,1.0};
    GLfloat light_specular[] = {1.0,1.0,1.0,1.0};
    GLfloat light_ambient[] = {1.0,1.0,1.0,1.0};
    GLfloat light1_diffuse[] = {0.0,0.0,1.0,1.0};
    GLfloat light1_position[] = {1.0,1.5,0.0,1.0};
    GLfloat light1_specular[] = {1.0,1.0,1.0,1.0};
    GLfloat light1_ambient[] = {1.0,1.0,1.0,1.0};
    //GLfloat diffuse[] = {0.3,0.3,0.3,1.0};
    //GLfloat ambient[] = {0.06,0.05,0.05,1.0};
    //GLfloat specular[] = {0.6,0.6,0.6,1.0};
    //GLfloat shininess = 40;