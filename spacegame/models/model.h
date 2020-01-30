#define CAPACITY 100000

float vecMag (float *vec) {
    return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}

class sbModel
{
public:

    int nv, nf;
    float vert[CAPACITY][3];
    float norm[CAPACITY][3];
    float norf[CAPACITY][3];
    int face[CAPACITY][3];
    int flag[CAPACITY];

    GLfloat glLightMat[3][4], shine;

    sbModel() {
        nv = 0;
        nf = 0;
        shine = 100;
        for (int ii = 0; ii < CAPACITY; ii++) {
            flag[ii] = 0;
            for (int jj = 0; jj < 3; jj++) {
                face[ii][jj] = 0;
                vert[ii][jj] = 0.0;
                norm[ii][jj] = 0.0;
                norf[ii][jj] = 0.0;
            }
        }
        for (int ii = 0; ii < 4; ii++) {
            if (ii < 3) {
                glLightMat[0][ii] = 0.1;
                glLightMat[1][ii] = 0.5;
                glLightMat[2][ii] = 1.0;
            } else {
                glLightMat[0][ii] = 1.0;
                glLightMat[1][ii] = 1.0;
                glLightMat[2][ii] = 1.0;
            }
        }
    }


    void calc_normals () {
        // calculate and draw vertex normals on the fly
        float normal[3], U[3], V[3];
        int norm_count[CAPACITY];
        for (int ii = 0; ii < nv; ii++) norm_count[ii]=0;


        for (int ii = 0; ii < nf; ii++) {
            for (int jj = 0; jj < 3; jj++) {
                if (jj==0) {
                    subtract(vert[face[ii][2]], vert[face[ii][0]], U);
                    subtract(vert[face[ii][1]], vert[face[ii][0]], V);  }

                if (jj==1) {
                    subtract(vert[face[ii][0]], vert[face[ii][1]], U);
                    subtract(vert[face[ii][2]], vert[face[ii][1]], V);  }

                if (jj==2) {
                    subtract(vert[face[ii][1]], vert[face[ii][2]], U);
                    subtract(vert[face[ii][0]], vert[face[ii][2]], V);  }

                crossProduct(V, U, normal);
                normalize(normal);

                if (jj==0) copyVertex(normal,norf[ii]);
                add(normal,norm[face[ii][jj]],norm[face[ii][jj]]);
                norm_count[face[ii][jj]]++;
            }
        }

        for (int ii = 0; ii < nv; ii++) {
            for (int dim = 0; dim < 3; dim++) {norm[ii][dim] /= float(norm_count[ii]);}
        }
    }

    void subdivide (void) {
        int vi, facesAtStart = nf;
        for (int ii = 0; ii < facesAtStart; ii++) { // for each face
            for (int jj = 0; jj < 3; jj++) { // and every vertex index

                vi = (jj < 2) ? jj+1 : 0;
                for (int dim = 0; dim < 3; dim++) {
                    vert[nv][dim] = 0.5*(vert[face[ii][jj]][dim] + vert[face[ii][vi]][dim] ); // create new intermediate vertex
                }
                for (int dim = 0; dim < 3; dim++) {
                    vert[nv][dim] *= 0.5 * (vecMag(vert[face[ii][jj]]) + vecMag(vert[face[ii][vi]]) ) / vecMag(vert[nv]);
                }

                face[nf][0] = face[ii][jj];
                face[nf][1] = nv;
                face[nf][2] = (jj < 1) ? nv+2 : nv-1;
                nf++;
                nv++;
            }
            face[ii][0] = nv-3;
            face[ii][1] = nv-2;
            face[ii][2] = nv-1;
        }
    };

    void dump (char filename[]) {
        // exports 3D sbModel to data file
        FILE *fp;
        fp = fopen(filename, "w");
        fprintf(fp, "%d %d \n", nv, nf);
        for (int ii = 0; ii < nv; ii++) {
            fprintf(fp,"%f %f %f \n", vert[ii][0], vert[ii][1], vert[ii][2]); // read vertex positions
        }
        for (int ii = 0; ii < nf; ii++) {
            fprintf(fp,"%d %d %d \n", face[ii][0], face[ii][1], face[ii][2]); // read vertex indices for faces
        }
        fclose(fp);
    }

    void import (char filename[]) {
        /*
         Import 3-D model from XYZ
         */
        FILE *fp;
        fp = fopen(filename, "r");
        fscanf(fp, "%d %d", &nv, &nf); // read number of vertices and faces
        for (int ii = 0; ii < nv; ii++) {
            for (int jj = 0; jj < 3; jj++) {
                fscanf(fp,"%f", &vert[ii][jj]); // read vertex positions
            }
        }
        for (int ii = 0; ii < nf; ii++) {
            for (int jj = 0; jj < 3; jj++) {
                fscanf(fp,"%d", &face[ii][jj]); // read vertex indices for faces
            }
        }
        fclose(fp);
    }

    void reflect (int dim) {
        // reflect all vertices over given axis
        int index;
        for (int ii = 0; ii < nv; ii++) {
            vert[ii][dim] *= -1.0;
        }
        for (int ii = 0; ii < nf; ii++) {
            std::swap(face[ii][0], face[ii][2]);
        }
    }

    void shift (float dx, float dy, float dz) {
        // shift all vertices by given offsets
        for (int ii = 0; ii < nv; ii++) {
            vert[ii][0] += dx;
            vert[ii][1] += dy;
            vert[ii][2] += dz;
        }
    }

    void scale (float sx, float sy, float sz) {
        // scale all vertex coordinates
        for (int ii = 0; ii < nv; ii++) {
            vert[ii][0] *= sx;
            vert[ii][1] *= sy;
            vert[ii][2] *= sz;
        }
    }

    void removeDuplicates() {

        int in_nv = nv, in_nf = nf;

        float duplicateThreshold = 0.001;
        float dx;
        float dy;
        float dz;
        float offset;

        for (int ii = 0; ii < nv-1; ii++) { // loop all vertices except last
            for (int jj = ii+1; jj < nv; jj++) { // loop through remaining vertices
                dx = vert[ii][0] - vert[jj][0];
                dy = vert[ii][1] - vert[jj][1];
                dz = vert[ii][2] - vert[jj][2];
                offset = sqrt(dx*dx + dy*dy + dz*dz);
                if (offset < duplicateThreshold) { // if close
                    for (int kk = jj; kk < nv-1; kk++) {// loop remainder
                        for (int dim = 0; dim < 3; dim++) {// loop array dimensions
                            vert[kk][dim] = vert[kk+1][dim]; // shift vertex data
                        }
                    }
                    nv--; //decrement nv
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
        printf("%6d Vertices, %6d Faces\n", in_nv-nv, in_nf-nf);
    }

    void append (sbModel *attach) {
        // appends input sbModel to current object
        for (int ii = 0; ii < attach->nv; ii++) {
            for (int dim = 0; dim < 3; dim++) {
                vert[nv+ii][dim] = attach->vert[ii][dim];
            }
        }

        for (int ii = 0; ii < attach->nf; ii++) {
            for (int dim = 0; dim < 3; dim++) {
                face[nf+ii][dim] = attach->face[ii][dim] + nv;
            }
        }
        nf += Min->nf;
        nv += Min->nv;
    }

    void fuzz () {
        float randAdj;
        for (int ii = 0; ii < nv; ii++) {
            randAdj = float(rand()%100-45)/4000.0 ;
            for (int dim = 0; dim < 3; dim++) {
                vert[ii][dim] *= 1.0 + randAdj;
            }
        }
    }

    void copy(sbModel *attach) {
        // copy inout sbModel to current sbModel
        Mout->nv = attach->nv;
        for (int ii = 0; ii < attach->nv; ii++) {
            vert[ii][0] = attach->vert[ii][0];
            vert[ii][1] = attach->vert[ii][1];
            vert[ii][2] = attach->vert[ii][2];
        }

        nf = Min->nf;
        for (int ii = 0; ii < Min->nf; ii++) {
            face[ii][0] = attach->face[ii][0];
            face[ii][1] = attach->face[ii][1];
            face[ii][2] = attach->face[ii][2];
        }
    }


    void rotate(float rotAngle, float *rotAxis) {
        for (int ii=0; ii<nv; ii++) {
            rotateVector(rotAngle*radians, rotAxis, vert[ii]);
        }

    }

    void draw_triangles (bool fill) {
        // takes pointer to model instance and draws either filled or outlined triangles
        if (fill) glBegin(GL_TRIANGLES);
        for (int ii = 0; ii < nf; ii++) {
            TOTAL_TRIANGLES++;
            if (!fill) glBegin(GL_LINE_LOOP);
            for (int jj = 0; jj < 3; jj++) {
                if (lightState) glNormal3fv(norm[face[ii][jj]]);
                glVertex3fv(vert[face[ii][jj]]);
            }
            if (!fill) glEnd();
        }
        if (fill) glEnd();
    }

    void draw_model () {
        // Takes pointer to model instance and draws triangles from face and vertex data
        if (lightState)     {
            glEnable(GL_LIGHTING);
            glMaterialfv(GL_FRONT, GL_AMBIENT,   glLightMat[0]);
            glMaterialfv(GL_FRONT, GL_DIFFUSE,   glLightMat[1]);
            glMaterialfv(GL_FRONT, GL_SPECULAR,  glLightMat[2]);
            glMaterialf (GL_FRONT, GL_SHININESS, shine);
            draw_triangles(SOLID);
        }
        else {
            if (SHOW_BACK)  {
                glCullFace(GL_FRONT);
                glColor4f(0.5, 0.5, 0.5, 1.0);
                draw_triangles(SOLID);
            }
            if (SHOW_FRONT) {
                glCullFace(GL_BACK);
                glColor4f(0.5, 0.5, 0.5, 1.0);
                draw_triangles(SOLID);
            }
        }
        glDisable(GL_LIGHTING);
        if (SHOW_EDGES) {
            glColor4f(1.0, 1.0, 1.0, 0.5);
            draw_triangles(WIRE);
        }
        if (SHOW_NORMALS) {
            glColor4f(1.0, 0.0, 0.0, 0.1);

            float normScale=0.1, normal[3];

            for (int ii = 0; ii < nv; ii++) {
                for (int dim=0; dim<3; dim++) normal[dim] = vert[ii][dim] + normScale*vert[ii][dim];
                glBegin(GL_LINES);
                glVertex3fv(vert[ii]);
                glVertex3fv(normal);
                glEnd();
            }
        }
    }


    void draw_multi_normals () {
        // calculate and draw vertex normals on the fly
        float normal[3], U[3], V[3], normScale=0.25;

        for (int ii = 0; ii < nf; ii++) {
            for (int jj = 0; jj < 3; jj++) {
                if (jj==0) {
                    subtract(vert[face[ii][2]], vert[face[ii][0]], U);
                    subtract(vert[face[ii][1]], vert[face[ii][0]], V);  }

                if (jj==1) {
                    subtract(vert[face[ii][0]], vert[face[ii][1]], U);
                    subtract(vert[face[ii][2]], vert[face[ii][1]], V);  }

                if (jj==2) {
                    subtract(vert[face[ii][1]], vert[face[ii][2]], U);
                    subtract(vert[face[ii][0]], vert[face[ii][2]], V);  }

                crossProduct(V,U,normal);
                normalize(normal);
                for (int dim=0; dim<3; dim++) normal[dim] = vert[face[ii][jj]][dim] + normScale*normal[dim];

                glBegin(GL_LINES);
                glVertex3fv(vert[face[ii][jj]]);
                glVertex3fv(normal);
                glEnd();
            }
        }
    }
};


class StateType
{
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
};


void copyVertex (float *A, float *B) {
    for (int ii = 0; ii < 3; ii++) B[ii] = A[ii];
}

void copyFace (int *A, int *B) {
    for (int ii = 0; ii < 3; ii++) B[ii] = A[ii];
}


void tetrahedron (sbModel *M) {
    M->nv = 4;
    M->vert[0][0] = -sqrt(2.0/3.0); M->vert[0][1] = 0.0;            M->vert[0][2] = sqrt(1.0/3.0);
    M->vert[1][0] = sqrt(2.0/3.0);  M->vert[1][1] = 0.0;            M->vert[1][2] = sqrt(1.0/3.0);
    M->vert[2][0] = 0.0;            M->vert[2][1] = -sqrt(2.0/3.0); M->vert[2][2] = -sqrt(1.0/3.0);
    M->vert[3][0] = 0.0;            M->vert[3][1] = sqrt(2.0/3.0);  M->vert[3][2] = -sqrt(1.0/3.0);

    M->nf = 4;
    M->face[0][0] = 0; M->face[0][1] = 1; M->face[0][2] = 3;
    M->face[1][0] = 0; M->face[1][1] = 3; M->face[1][2] = 2;
    M->face[2][0] = 0; M->face[2][1] = 2; M->face[2][2] = 1;
    M->face[3][0] = 1; M->face[3][1] = 2; M->face[3][2] = 3;
}



void adjacency (sbModel *M, int adjVert[CAPACITY][5]) {
    int adjFound, flag;
    for (int ii=0; ii<M->nv; ii++) { // for each vertex
        adjFound = 0; // there are at first no adjacents
        for (int jj=0; jj<M->nf; jj++) { // so search faces
            for (int kk=0; kk<3; kk++) { // by each member vertex
                if (ii == M->face[jj][kk]) { // to find the current index
                    for (int mm=0; mm<3; mm++) { // then loop again
                        if (mm != kk) { // through other two
                            flag = 0;
                            for (int nn=0; nn<adjFound; nn++) { // and check against already found
                                if (M->face[jj][mm] == adjVert[ii][nn]) flag = 1;
                            }
                            if (flag == 0) adjVert[ii][adjFound++] = M->face[jj][mm];
                        }
                    }
                }
            }
        }
    }


}


void smooth (sbModel *M, int adjVert[100000][5] ) {
    float runningSum;
    for (int ii = 0; ii < M->nv; ii++) {
        runningSum = 4.0*vecMag( M->vert[ii] );
        for (int jj = 0; jj < 5; jj++) {
            runningSum += vecMag( M->vert[ adjVert[ii][jj] ] );
        }
        runningSum /= (9.0*vecMag(M->vert[ii]));

        for (int dim=0; dim<3; dim++) {
            M->vert[ii][dim] *= runningSum;
        }
    }

}


void impact (sbModel *M, int adjVert[100000][5]) {
    int vi = rand()%M->nv;
    int nextVert;
    for (int dim = 0; dim < 3; dim++) {
        M->vert[vi][dim] *= 0.95;
    }
    for (int ii=0; ii<5; ii++) {
        nextVert = adjVert[vi][ii];
        for (int dim = 0; dim < 3; dim++) {M->vert[nextVert][dim] *= 0.95;}
        for (int jj=0; jj<5; jj++) {
            for (int dim = 0; dim < 3; dim++) {M->vert[adjVert[nextVert][jj]][dim] *= 0.95;}
        }
    }
}


void icosahedron (sbModel *M) {
    sbModel *tempMod;
    tempMod = new sbModel;
    float phi = 0.5*(1.0 + sqrt(5.0)), next[3];

    M->vert[M->nv][0] = 0.0;
    M->vert[M->nv][1] = 1.0/phi;
    M->vert[M->nv][2] = 1.0;

    M->nv++;
    next[0] = 0.0; next[1] = -1.0/phi; next[2] = 1.0;

    for (int ii=0; ii<5; ii++) {
        if (ii<4) {M->face[M->nf][0] = 0; M->face[M->nf][1] = M->nv; M->face[M->nf][2] = M->nv+1;   M->nf++;}
        else      {M->face[M->nf][0] = 0; M->face[M->nf][1] = M->nv; M->face[M->nf][2] = 1;         M->nf++;}
        copyVertex(next, M->vert[M->nv++]);
        rotateVector(72.0*radians, M->vert[0], next);
    }

    copy(M,tempMod);
    rotateModel(180.0, XAXIS, tempMod);
    rotateModel(-72.0, tempMod->vert[0], tempMod);
    append(tempMod,M);

    int TI=1, BI=11;
    for (int ii=0; ii<5; ii++) {
        M->face[M->nf][0] = TI; M->face[M->nf][1] = BI--; if (BI<7) BI=11; M->face[M->nf][2] = BI; M->nf++;
        M->face[M->nf][0] = TI++; if (TI>5) TI=1; M->face[M->nf][1] = BI; M->face[M->nf][2] = TI; M->nf++;
    }
}

