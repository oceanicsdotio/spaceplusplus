#include <GL/glew.h> // for PFNGLTEXIMAGE3DPROC
#include <GL/glut.h>
#include <GL/glx.h> // linux

#define GetProcAddress glXGetProcAddress
#define	iHeight 80
#define iDepth 80
#define	iWidth 80


PFNGLTEXIMAGE3DPROC glTexImage3D;
GLubyte colorTable[256*4];
GLuint texName2D[80];


namespace texture
{
    class Texture
    {
        friend class BoundingBox;
    private:
        float scale = 1.0;

        int shape[3];

    public:

        GLuint handle;
        GLubyte ****rgba;

        float ***data;
        float alpha;

        struct clipStruct {
            float bound[6] = {
                    0.5, 0.5,
                    0.5,-0.5,
                    -0.5,-0.5
            };
            GLdouble eqn[24] = {
                    1, 0, 0, 1,
                    0, 1, 0, 1,
                    0, 0, 1, 1,
                    -1, 0, 0, 1,
                    0,-1, 0, 1,
                    0, 0,-1, 1
            };
            bool dim[3] = {true, true, true};
        } clip;

        struct metaStruct {
            float max;
            float min;
            float mean;
            float sum;
            float isoValue;
            int slices;

            struct vec3 {
                float x;
                float y;
                float z;
            } vec;
        } metadata;

        Texture(std::string filename, float alpha) {

            int width, depth, height;

            FILE *fp;
            fp = fopen(filename.c_str(), "r");

            fscanf(fp, "%f", &this->metadata.isoValue);
            fscanf(fp, "%f %f %f", &this->metadata.vec.x, &this->metadata.vec.y, &this->metadata.vec.z);
            fscanf(fp, "%d %d %d", &height, &depth, &width);

            this->alpha = alpha;
            this->shape[0] = width;
            this->shape[1] = depth;
            this->shape[2] = height;

            data = (float ***) malloc(sizeof(float **) * width);
            rgba = (GLubyte ****) malloc(sizeof(GLubyte ***) * width);
            for (int ii = 0; ii < width; ii++) {
                data[ii] = (float **) malloc(sizeof(float *) * height);
                rgba[ii] = (GLubyte ***) malloc(sizeof(GLubyte **) * height);
                for (int jj = 0; jj < depth; jj++) {
                    data[ii][jj] = (float *) malloc(sizeof(float) * depth);
                    rgba[ii][jj] = (GLubyte **) malloc(sizeof(GLubyte **) * depth);
                    for (int kk = 0; kk < height; kk++) {
                        rgba[ii][jj][kk] = (GLubyte *) malloc(sizeof(GLubyte *) * 4);
                        fscanf(fp, "%f", &data[ii][jj][kk]); // read density.dat value
                        const float value = data[ii][jj][kk];
                        if ((ii + jj + kk) == 0) {
                            this->metadata.min = value;
                            this->metadata.max = value;
                            this->metadata.sum = value;
                        } else {
                            this->metadata.max = std::max(this->metadata.max, value);
                            this->metadata.min = std::min(this->metadata.min, value);
                            this->metadata.sum += value;
                        }
                    }
                }
            }

            fclose(fp);

            for (int ss = 0; ss < width; ss++) {
                for (int tt = 0; tt < height; tt++) {
                    for (int rr = 0; rr < depth; rr++) {
                        float val = this->data[ss][tt][rr] / this->metadata.max;
                        this->rgba[ss][tt][rr][0] = (GLubyte) 255 * val;
                        this->rgba[ss][tt][rr][1] = (GLubyte) 255 * 0.0;
                        this->rgba[ss][tt][rr][2] = (GLubyte) 255 * (1.0 - val);
                        this->rgba[ss][tt][rr][3] = (GLubyte) 255 * this->alpha * 0.005 * val;
                    }
                }
            }

            this->metadata.mean = this->metadata.sum / float(height * depth * width);
            this->reportStatistics();
        };

        void reportStatistics() {
            printf("Data minimum:  %8.2f\n", this->metadata.min);
            printf("Data maximum:  %8.2f\n", this->metadata.max);
            printf("Data mean:     %8.2f\n", this->metadata.mean);
        };

        void bind() {
            glGenTextures(1, &this->handle);
            glBindTexture(GL_TEXTURE_3D, handle);
            const int wrapDimensions[3] = {GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R};
            for (int ii=0; ii<3; ii++) {
                glTexParameteri(GL_TEXTURE_3D, wrapDimensions[ii], GL_CLAMP);
            }
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear interp
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexEnvf(GL_TEXTURE_3D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            ((PFNGLTEXIMAGE3DPROC) GetProcAddress((const GLubyte*)"glTexImage3D"))(GL_TEXTURE_3D, 0, GL_RGBA, this->shape[1], this->shape[2], this->shape[0], 0, GL_RGBA, GL_UNSIGNED_BYTE, this->rgba);

        };
    };

    class BoundingBox
    {
    public:
        void render (Texture texture, float color[4]) {

            glPushMatrix();
            glColor4fv(color);
            glutWireCube(texture.scale);

            glTranslatef(
                    -0.5*(texture.clip.bound[0] + texture.clip.bound[3]),
                    -0.5*(texture.clip.bound[1] + texture.clip.bound[4]),
                    -0.5*(texture.clip.bound[2] + texture.clip.bound[5])
            );

            glScalef(
                    texture.clip.bound[0] - texture.clip.bound[3],
                    texture.clip.bound[1] - texture.clip.bound[4],
                    texture.clip.bound[2] - texture.clip.bound[5]
            );

            glutWireCube(texture.scale);
            glPopMatrix();

        };
    };

};