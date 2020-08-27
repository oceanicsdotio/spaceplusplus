#define PI 3.1415926
#define SOLID                       true
#define WIRE                        false
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <string>
#include <vector>



static bool lightState   = true;
static bool SHOW_EDGES   = false;
static bool SHOW_NORMALS = false;
static bool SHOW_BACK    = true;
static bool SHOW_FRONT   = true;
static bool SMOOTH_MODEL = true;


struct Palette{
    const GLfloat white[4] = {1.0, 1.0, 1.0, 0.5};
    const GLfloat red[4] = {1.0, 0.0, 0.0, 0.1};
    const GLfloat grey[4] = {0.5, 0.5, 0.5, 1.0};
} palette;

struct state {
    struct {
        bool button;
        struct {
            int x;
            int y;
        } coordinates;
    } mouse;
    struct {
        float increment[3];
        float current[3];
        float matrix[16];
    } rotation;
};


class Model
{
private:

    std::vector<Vec3>
            norf,
            norm,
            vert;

    std::vector<std::vector<int>>
            face,
            neighbors;

    struct Material{
        GLfloat shine=100;
        GLfloat light[3][4] = {
                {0.1, 0.1, 0.1, 1.0},
                {0.5, 0.5, 0.5, 1.0},
                {1.0, 1.0, 1.0, 1.0}
        };
    } material;

public:

    Model operator + (Model M) {
        Model C = copy();
        for (int ii = 0; ii < M.vert.size(); ii++) C.vert.push_back(M.vert[ii]);
        for (int ii = 0; ii < M.face.size(); ii++) C.face.push_back(M.face[ii]);
        return C;
    };

    void operator += (Model M) {
        for (int ii = 0; ii < M.vert.size(); ii++) vert.push_back(M.vert[ii]);
        for (int ii = 0; ii < M.face.size(); ii++) face.push_back(M.face[ii]);
    };

    void operator /= (int divisions) {
        /*
         Subdivide the cells of the topology
         */
        const int facesAtStart = face.size();
        for (int ii = 0; ii < facesAtStart; ii++) {
            for (int jj = 0; jj < 3; jj++) {
                const int a = face[ii][jj];
                const int b = face[ii][(jj < 2) ? jj+1 : 0];
                Vec3 midpoint = (vert[a] + vert[b]) * 0.5;
                vert.push_back(midpoint * (0.5 * (vert[a].length() + vert[b].length()) / midpoint.length()));
                const int nv = vert.size();
                face.push_back({a, nv, jj<1 ? nv+2 : nv-1});
            }
            const int nv = vert.size();
            face.push_back({nv-3, nv-2, nv-1});
        }
    };

    std::vector<Vec3> Normals () {

        std::vector<Vec3> normals;
        std::vector<int> count(vert.size(), 0);

        for (int ii = 0; ii < face.size(); ii++) {
            for (int jj = 0; jj < 3; jj++) {
                const int fid = face[ii][jj];
                Vec3 V = vert[face[ii][(jj + 1) % 3]] - vert[fid];
                Vec3 U = vert[face[ii][(jj + 2) % 3]] - vert[fid];
                Vec3 normal = crossProduct(V, U).normalized();
                if (jj==0) norf[ii] = normal;
                normals[fid] = (normals[fid] * count[fid] + normal) / ++count[fid];
            }
        }

        return normals;
    };

    void reflect (int dim) {
        // reflect all vertices over given axis
        for (int ii = 0; ii < vert.size(); ii++) {
            vert[ii] = vert[ii] * -1.0;
        }
        for (int ii = 0; ii < face.size(); ii++) {
            std::swap(face[ii][0], face[ii][2]);
        }
    };

    Model static tetrahedron () {

        Model M = Model();
        const float root = sqrt(1.0/3.0);

        M.vert.push_back( Vec3({sqrt(2.0/3.0), 0.0, root}));
        M.vert.push_back( Vec3({sqrt(2.0/3.0), 0.0, root}));
        M.vert.push_back( Vec3({0.0, -sqrt(2.0/3.0), -root}));
        M.vert.push_back( Vec3({0.0, sqrt(2.0/3.0), -root}));

        M.face.push_back({0, 1, 3});
        M.face.push_back({0, 3, 2});
        M.face.push_back({0, 2, 1});
        M.face.push_back({1, 2, 3});
        return M;
    };


    Model Sphere (int R = 24) {
        Model M = Model();

        const int nv = R * (R / 2 - 1) + 2;
        const int nf = R * R / 2;
        const int step = 2 * PI / ((float) R);

        M.vert.push_back(Vec3({0, 0, 1}));
        for (int ii = 1; ii < R-2; ii++) {
            for (int jj = 0; jj < R; jj++) {

                const float azimuth = PI / 2.0 - step * ii;
                const float theta = jj * step;

                M.vert.push_back(Vec3({
                                              cos(theta) * abs(cos(azimuth)),
                                              sin(theta) * abs(cos(azimuth)),
                                              sin(azimuth)
                                      }));
            }
        }
        M.vert.push_back(Vec3({0, 0, -1}));

        // top
        for (int ii = 0; ii < R; ii++) {
            M.face.push_back({0, ii+1, ii<R ? ii+2 : 1, -1});
        }

        // middle
        for (int ii = 0; ii < R/2-1; ii++) {
            for (int jj = 0; jj < R; jj++) {
                const int ind = (ii - 1) * R + jj;
                M.face.push_back({
                                         ind,
                                         ind + 1 - (jj==R ? R : 0),
                                         ind + 1 + (jj==R ? 0 : R),
                                         ind + R
                                 });
            }
        }


        return M;
    };

    Model Icosahedron () {

        Model M = Model();
        const float phi = 0.5*(1.0 + sqrt(5.0));
        Vec3 next = Vec3({0.0, (float) -1.0/phi, 1.0});
        for (int ii=0; ii<5; ii++) {
            M.face.push_back({0, ii, ii<4 ? ii+1 : 1});
            M.vert.push_back(next.rotate(72.0*radians*ii, M.vert[0]));
        }

        Model tempMod = M.copy().rotate(180.0, XAXIS);
        return M + tempMod.rotate(-72.0, tempMod.vert[0]);
    }


    Model smooth() {

        Model M = copy();

        for (int ii = 0; ii < M.vert.size(); ii++) {
            const float length = M.vert[ii].length();
            float runningSum = 4.0 * length;
            const std::vector<int> adj = neighbors[ii];
            for (int jj = 0; jj < adj.size(); jj++) {
                runningSum += M.vert[ adj[jj] ].length();
            }
            vert[ii] *= runningSum / length / 9.0;
        }
        return M;
    }

    Model rotate(float angle, Vec3 axis) {
        Model M = copy();
        for (int ii=0; ii < M.vert.size(); ii++) {
            M.vert[ii].rotate(angle, axis);
        }
        return M;
    }

    Model impact (int root, int distance, float coef=0.95) {
        // Create impact sites recursively
        Model M = copy();
        vert[root] *= coef;
        std::vector<int> adj = neighbors[root];
        if (distance > 0) {
            for (int ii = 0; ii < adj.size(); ii++) {
                impact(distance-1, adj[ii], coef);
            }
        }
        return M;
    }

    Model fuzz () {
        Model M = Model();
        float randAdj;
        for (int ii = 0; ii < M.vert.size(); ii++) {
            M.vert[ii] *= (float) (1.0 + (rand()%100-45)/4000.0);
        }
        return M;
    }

    Model copy() {
        // copy inout Model to current Model
        Model M = Model();
        for (int ii = 0; ii < vert.size(); ii++) {
            M.vert.push_back(vert[ii]);
        }
        for (int ii = 0; ii < face.size(); ii++) {
            M.face.push_back(face[ii]);
        }
        return M;
    }

    std::vector<std::vector<int>> Neighbors() {

        const int nv = vert.size();
        std::vector<std::vector<int>> value;

        for (int ii = 0; ii < nv; ii++) {
            int count;
            // search faces by each member vertex
            value.push_back({});
            for (int jj = 0; jj < face.size(); jj++) {
                for (int kk = 0; kk < 3; kk++) {
                    if (ii != face[jj][kk]) continue; // find the current index
                    for (int mm=0; mm<3; mm++) {
                        if (mm == kk) continue;
                        const int fid = face[jj][mm];
                        bool flag = false;
                        for (int nn = 0; nn < count; nn++) {
                            if (fid == value[ii][nn]) {
                                flag = true;
                                break;
                            }
                        }
                        if (flag) continue;
                        value[ii].push_back(fid);
                    }
                }
            }
        }

        return value;
    }


    Model deduplicate(float threshold = 0.001) {

        Model M = copy();

        int nf = M.face.size();
        int nv = M.vert.size();

        for (int ii = 0; ii < nv-1; ii++) { // loop all vertices except last
            for (int jj = ii+1; jj < nv; jj++) { // loop through remaining vertices
                if ((vert[ii] - vert[jj]).length() >= threshold) break;
                for (int kk = jj; kk < nv-1; kk++) {
                    vert[kk] = vert[kk+1]; // shift remaining vertex data
                }
                nv--;
                for (int kk = 0; kk < nf; kk++) { // loop faces
                    for (int dim = 0; dim < 3; dim++) { // loop face dimension
                        if (face[kk][dim] == jj) { // if duplicate vertex
                            face[kk][dim] = ii; // change
                        }
                        else if (face[kk][dim] > jj) {
                            face[kk][dim]--;// shift face data
                        }
                    }
                }
            }
        }


        for (int ii = 0; ii < nf-1; ii++) {
            for (int jj = ii+1; jj < nf; jj++) {
                if ( ((face[ii][0]==face[jj][0]) && (face[ii][1]==face[jj][1]) && (face[ii][2]==face[jj][2])) ||
                     ((face[ii][0]==face[jj][0]) && (face[ii][1]==face[jj][2]) && (face[ii][2]==face[jj][1])) ||

                     ((face[ii][0]==face[jj][1]) && (face[ii][1]==face[jj][2]) && (face[ii][2]==face[jj][0])) ||
                     ((face[ii][0]==face[jj][1]) && (face[ii][1]==face[jj][0]) && (face[ii][2]==face[jj][2])) ||

                     ((face[ii][0]==face[jj][2]) && (face[ii][1]==face[jj][1]) && (face[ii][2]==face[jj][0])) ||
                     ((face[ii][0]==face[jj][2]) && (face[ii][1]==face[jj][0]) && (face[ii][2]==face[jj][1]))  )

                {
                    for (int kk = jj; kk < nf-1; kk++) {
                        for (int dim = 0; dim < 3; dim++) {
                            face[kk][dim] = face[kk+1][dim];
                        }
                    }
                    nf--;
                }
            }
        }

        return M;

    }
};