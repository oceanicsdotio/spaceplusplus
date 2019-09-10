////////////////////////////////////////////////////////////////////////////
void importModel (char filename[], sbModel *M) {
    // imports 3D sbModel from data file
    FILE *fp;
    fp = fopen(filename, "r");
    fscanf(fp, "%d %d", &M->nv, &M->nf); // read number of vertices and faces
    for (int ii = 0; ii < M->nv; ii++) {
        for (int jj = 0; jj < 3; jj++) {
            fscanf(fp,"%f", &M->vert[ii][jj]); // read vertex positions
        }
    }
    for (int ii = 0; ii < M->nf; ii++) {
        for (int jj = 0; jj < 3; jj++) {
            fscanf(fp,"%d", &M->face[ii][jj]); // read vertex indices for faces
        }
    }
    fclose(fp);
}

////////////////////////////////////////////////////////////////////////////

void exportModel (char filename[], sbModel *M) {
    // exports 3D sbModel to data file
    FILE *fp;
    fp = fopen(filename, "w");
    fprintf(fp, "%d %d \n", M->nv, M->nf);
    for (int ii = 0; ii < M->nv; ii++) {
            fprintf(fp,"%f %f %f \n", M->vert[ii][0],M->vert[ii][1],M->vert[ii][2]); // read vertex positions
    }
    for (int ii = 0; ii < M->nf; ii++) {
            fprintf(fp,"%d %d %d \n", M->face[ii][0], M->face[ii][1], M->face[ii][2]); // read vertex indices for faces
    }
    fclose(fp);
}


////////////////////////////////////////////////////////////////////////////
void append (sbModel *Min, sbModel *Mout) {
    // appends input sbModel to current object
    for (int ii = 0; ii < Min->nv; ii++) {
        for (int dim = 0; dim < 3; dim++) {
            Mout->vert[Mout->nv+ii][dim] = Min->vert[ii][dim];
        }
    }


    for (int ii = 0; ii < Min->nf; ii++) {
        for (int dim = 0; dim < 3; dim++) {
            Mout->face[Mout->nf+ii][dim] = Min->face[ii][dim] + Mout->nv;
        }
    }

    Mout->nf += Min->nf;
    Mout->nv += Min->nv;
}
////////////////////////////////////////////////////////////////////////////
void removeDuplicates (sbModel *M) {

    printf("Removing duplicates... ");

    int in_nv = M->nv, in_nf = M->nf;

    float duplicateThreshold = 0.001;
    float dx;
    float dy;
    float dz;
    float offset;

    for (int ii = 0; ii < M->nv-1; ii++) { // loop all vertices except last
        for (int jj = ii+1; jj < M->nv; jj++) { // loop through remaining vertices
            dx = M->vert[ii][0] - M->vert[jj][0];
            dy = M->vert[ii][1] - M->vert[jj][1];
            dz = M->vert[ii][2] - M->vert[jj][2];
            offset = sqrt(dx*dx + dy*dy + dz*dz);
            if (offset < duplicateThreshold) { // if close
                for (int kk = jj; kk < M->nv-1; kk++) {// loop remainder
                    for (int dim = 0; dim < 3; dim++) {// loop array dimensions
                        M->vert[kk][dim] = M->vert[kk+1][dim]; // shift vertex data
                    }
                }
                M->nv--; //decrement nv
                for (int kk = 0; kk < M->nf; kk++) { // loop faces
                    for (int dim = 0; dim < 3; dim++) { // loop face dimension
                        if (M->face[kk][dim] == jj) { // if duplicate vertex
                            M->face[kk][dim] = ii; // change
                        }
                        else if (M->face[kk][dim] > jj) {
                            M->face[kk][dim]--;// shift face data
                        }
                    }
                }
            }
        }
    }

    for (int ii = 0; ii < M->nf-1; ii++) {
        for (int jj = ii+1; jj < M->nf; jj++) {
            if ( ((M->face[ii][0]==M->face[jj][0]) && (M->face[ii][1]==M->face[jj][1]) && (M->face[ii][2]==M->face[jj][2])) ||
                 ((M->face[ii][0]==M->face[jj][0]) && (M->face[ii][1]==M->face[jj][2]) && (M->face[ii][2]==M->face[jj][1])) ||

                 ((M->face[ii][0]==M->face[jj][1]) && (M->face[ii][1]==M->face[jj][2]) && (M->face[ii][2]==M->face[jj][0])) ||
                 ((M->face[ii][0]==M->face[jj][1]) && (M->face[ii][1]==M->face[jj][0]) && (M->face[ii][2]==M->face[jj][2])) ||

                 ((M->face[ii][0]==M->face[jj][2]) && (M->face[ii][1]==M->face[jj][1]) && (M->face[ii][2]==M->face[jj][0])) ||
                 ((M->face[ii][0]==M->face[jj][2]) && (M->face[ii][1]==M->face[jj][0]) && (M->face[ii][2]==M->face[jj][1]))  )



            {
                for (int kk = jj; kk < M->nf-1; kk++) {
                    for (int dim = 0; dim < 3; dim++) {
                        M->face[kk][dim] = M->face[kk+1][dim];
                    }
                }
                M->nf--;
            }
        }
    }

    printf("%6d Vertices, %6d Faces\n", in_nv-M->nv, in_nf-M->nf);

}

////////////////////////////////////////////////////////////////////////////
void copy (sbModel *Min, sbModel *Mout) {
    // copy inout sbModel to current sbModel
    Mout->nv = Min->nv;
    for (int ii = 0; ii < Min->nv; ii++) {
        Mout->vert[ii][0] = Min->vert[ii][0];
        Mout->vert[ii][1] = Min->vert[ii][1];
        Mout->vert[ii][2] = Min->vert[ii][2];
    }

    Mout->nf = Min->nf;
    for (int ii = 0; ii < Min->nf; ii++) {
        Mout->face[ii][0] = Min->face[ii][0];
        Mout->face[ii][1] = Min->face[ii][1];
        Mout->face[ii][2] = Min->face[ii][2];
    }
}

void copyVertex (float *A, float *B) {
    for (int ii = 0; ii < 3; ii++) B[ii] = A[ii];
}

void copyFace (int *A, int *B) {
    for (int ii = 0; ii < 3; ii++) B[ii] = A[ii];
}
////////////////////////////////////////////////////////////////////////////
void shift (float dx, float dy, float dz, sbModel *M) {
    // shift all vertices by given offsets
    for (int ii = 0; ii < M->nv; ii++) {
        M->vert[ii][0] += dx;
        M->vert[ii][1] += dy;
        M->vert[ii][2] += dz;
    }
}
////////////////////////////////////////////////////////////////////////////
void reflect (int dim, sbModel *M) {
    // reflect all vertices over given axis
    int index;
    for (int ii = 0; ii < M->nv; ii++) {
        M->vert[ii][dim] *= -1.0;
    }
    // if (dim==1) {
    for (int ii = 0; ii < M->nf; ii++) {
        index = M->face[ii][0];
        M->face[ii][0] = M->face[ii][2];
        M->face[ii][2] = index;
    }
    //}
}
////////////////////////////////////////////////////////////////////////////
void scale (float sx, float sy, float sz, sbModel *M) {
    // scale all vertex coordinates
    for (int ii = 0; ii < M->nv; ii++) {
        M->vert[ii][0] *= sx;
        M->vert[ii][1] *= sy;
        M->vert[ii][2] *= sz;
    }
}

////////////////////////////////////////////////////////////////////////////

