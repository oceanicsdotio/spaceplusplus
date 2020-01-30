#define CAPACITY 1000

float XAXIS[]  = {1.0, 0.0, 0.0};
float YAXIS[]  = {0.0, 1.0, 0.0};
float ZAXIS[]  = {0.0, 0.0, 1.0};
float ORIGIN[] = {0.0, 0.0, 0.0};



void normalize (float *V) {
    float M = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
    V[0] /= M; V[1] /= M; V[2] /= M;
}

void rotateVector (float rotAngle, float *inAxis, float *vertPosition) {

    void quatProduct (float *U, float *V, float *R) {
        //Quaternion Multiplication
        R[0] = U[0]*V[0] - U[1]*V[1] - U[2]*V[2] - U[3]*V[3];   // A*B - dotProduct(U,V)
        R[1] = U[2]*V[3] - U[3]*V[2] + U[0]*V[1] + V[0]*U[1];   // crossProduct(U,V) + A*V + B*U;
        R[2] = U[3]*V[1] - U[1]*V[3] + U[0]*V[2] + V[0]*U[2];
        R[3] = U[1]*V[2] - U[2]*V[1] + U[0]*V[3] + V[0]*U[3];
    }

    //normalize rotation axis
    float rotAxis[3];
    for (int ii=0; ii<3; ii++) {rotAxis[ii] = inAxis[ii];}
    normalize(rotAxis);

    float memo = sin(rotAngle/2.0)
    // initialize quaternions
    float rotQuaternion[]  = { cos(rotAngle/2.0),  rotAxis[0]*memo,  rotAxis[1]*memo,  rotAxis[2]*memo };
    float conjQuaternion[] = { cos(rotAngle/2.0), -rotAxis[0]*memo, -rotAxis[1]*memo, -rotAxis[2]*memo };
    float posQuaternion[]  = { 0.0              ,  vertPosition[0], vertPosition[1] ,  vertPosition[2] };
    float newQuaternion[]  = { 0.0              ,  0.0            , 0.0             ,  0.0             };

    quatProduct( rotQuaternion, posQuaternion , newQuaternion );
    quatProduct( newQuaternion, conjQuaternion, posQuaternion );

    vertPosition[0] = posQuaternion[1];
    vertPosition[1] = posQuaternion[2];
    vertPosition[2] = posQuaternion[3];

}


void arc (int nPts, float startAngle, float sweepAngle, float radius, sbPrimitive *sbPolygon) {
    float rotIncrement = -1.0 * sweepAngle / (float) (nPts-1) * radians; // rotation increment
    float vertPosition[] = {radius, 0.0, 0.0}; // initial point
    rotateVector(startAngle*radians, ZAXIS, vertPosition); // rotate to starting angle
    sbPolygon->np = nPts;
    for (int ii = 0; ii < nPts; ii++) {
        for (int dim=0; dim<2; dim++) sbPolygon->vert[ii][dim] = vertPosition[dim]; // copy point to polygon
        rotateVector(rotIncrement, ZAXIS, vertPosition); // use quarternion rotation
    }
}


void regular_polygon (int nPts, sbPrimitive *sbPolygon) {
    // regular sbPolygon approximating a circle
    float rotIncrement = -1.0 * 360.0 / (float) nPts * radians; // rotation increment
    float vertPosition[] = {1.0, 0.0, 0.0}; // initial point
    sbPolygon->np = nPts; // point count
    for (int ii=0; ii<nPts; ii++) {
        for (int dim=0; dim<2; dim++) sbPolygon->vert[ii][dim] = vertPosition[dim]; // copy point to polygon
        rotateVector(rotIncrement, ZAXIS, vertPosition); // use quarternion rotation
    }
}


void wedge (int nPts, float startAngle, float sweepAngle, float radius, sbPrimitive *sbPolygon) {
    // arc of regular sbPolygon approximating a circle, clockwise
    arc(nPts, startAngle, sweepAngle, radius, sbPolygon);
    sbPolygon->np++;
    sbPolygon->vert[nPts][0] = 0.0; 
    sbPolygon->vert[nPts][1] = 0.0; 
    sbPolygon->vert[nPts][2] = 0.0;
}



void crossProduct (float *U, float *V, float *R) {
    R[0] = U[1]*V[2] - U[2]*V[1];
    R[1] = U[2]*V[0] - U[0]*V[2];
    R[2] = U[0]*V[1] - U[1]*V[0];
}


void add (float *U, float *V, float *R) {
    R[0] = U[0] + V[0];
    R[1] = U[1] + V[1];
    R[2] = U[2] + V[2];
}


void subtract (float *U, float *V, float *R) {
    R[0] = U[0] - V[0];
    R[1] = U[1] - V[1];
    R[2] = U[2] - V[2];
}


float dotProduct (float *a, float *b) {
    int length = 3;
    float runningSum = 0.0;
    for (int ii=0; ii<length; ii++) {
        runningSum += a[ii] * b[ii];
    }
    return runningSum;
}


float vecAngle (float *a, float*b) {
    
    float yy = dotProduct(a, b) / (vecMag(a) * vecMag(b));
    if (yy <= 1.0 and yy >=-1.0) return acos(yy);
    else return 0.0;
}


void extrude (int nRings, float *radius, float *offset, sbPrimitive *P, sbModel *M, int close_state) {
    int index, v1i, v2i, v3i, v4i, start;
    int nVertices = P->np * nRings;
    int nTriangles = 2 * P->np * (nRings-1);
    
    M->nv = nVertices;
    M->nf = nTriangles;
    
    for (int ii = 0; ii < nRings; ii++) {
        for (int jj = 0; jj < P->np; jj++) {
            index = ii * P->np + jj;
            M->vert[index][0] = radius[ii] * P->vert[jj][0];
            M->vert[index][1] = radius[ii] * P->vert[jj][1];
            M->vert[index][2] =              P->vert[jj][2] + offset[ii];
            
            if (ii < (nRings-1)) {
                if (jj == 0) start = index;
                v1i = index;
                v4i = index + P->np;
                if (jj < (P->np-1)) {
                    v2i = index + 1;
                    v3i = index + P->np + 1;
                } 
                else {
                    v2i = start;
                    v3i = start + P->np;
                }
               
                M->face[index][2] = v1i;
                M->face[index][1] = v2i;
                M->face[index][0] = v3i;
                
                M->face[index+nTriangles/2][2] = v1i;
                M->face[index+nTriangles/2][1] = v3i;
                M->face[index+nTriangles/2][0] = v4i;
            }
        }
    }
    if (close_state == 1) {
    for (int ii=0; ii<(P->np-2); ii++) {
        M->face[nTriangles+ii][0] = 0;
        M->face[nTriangles+ii][1] = ii + 1;
        M->face[nTriangles+ii][2] = ii + 2;
        M->nf++;
        
        M->face[nTriangles+ii+(P->np-2)][2] = M->nv - P->np;
        M->face[nTriangles+ii+(P->np-2)][1] = M->nv - P->np + ii + 1;
        M->face[nTriangles+ii+(P->np-2)][0] = M->nv - P->np + ii + 2;
        M->nf++;
    }
    
    }
    
}


void extrude_planar (int nArcs, float *radius, float *offset, sbPrimitive *P, sbModel *M) {
    int index, v1i, v2i, v3i, v4i, start;
    int nVertices = P->np * nArcs;
    int nTriangles = 2 * (P->np-1) * (nArcs-1);
    
    M->nv = nVertices;
    M->nf = nTriangles;
    
    for (int ii = 0; ii < nArcs; ii++) {
        for (int jj = 0; jj < P->np; jj++) {
            index = ii * P->np + jj;
            M->vert[index][0] = radius[ii] * P->vert[jj][0];
            M->vert[index][1] = radius[ii] * P->vert[jj][1];
            M->vert[index][2] =              P->vert[jj][2] + offset[ii];
            
            if ((ii<(nArcs-1))&&(jj<(P->np-1))) {
                v1i = index;
                v2i = index + P->np;
                v3i = index + P->np + 1;
                v4i = index + 1;
                
                M->face[index][0] = v1i;
                M->face[index][1] = v2i;
                M->face[index][2] = v3i;
                
                M->face[index+nTriangles/2][0] = v1i;
                M->face[index+nTriangles/2][1] = v3i;
                M->face[index+nTriangles/2][2] = v4i;
            }
        }
    }    
}


void stitch (sbPrimitive *outer, sbPrimitive *inner, sbModel *M) {

    M->nf = outer->np + inner->np;
    M->nv = outer->np + inner->np;
    
    float dx, dy, distance, minimum;
    int inner_index=0, outer_index=0, start_index=0, nT=0, leftovers=0, index_offset=0;
    int linesPerVertex = (int) floor((float)inner->np/(float)outer->np);
    
    for (int ii = 0; ii < inner->np; ii++) {
        dx = outer->vert[0][0] - inner->vert[ii][0];
        dy = outer->vert[0][1] - inner->vert[ii][1];
        distance = sqrt(dx*dx + dy*dy);
        if ((ii == 0)||(distance < minimum)) {
            start_index = ii;
            minimum = distance;
        }
    }
    
    for (int ii = 0; ii < M->nv; ii++) {
        for (int dim = 0; dim < 3; dim++) {
            if (ii < outer->np)     M->vert[ii][dim] = outer->vert[ii            ][dim];
            else                    M->vert[ii][dim] = inner->vert[ii - outer->np][dim];
        }
    }
    
    
    inner_index = start_index - (int)floor((float)(linesPerVertex-1)/2.0);
    //printf("%d %d %d\n", start_index, inner_index, linesPerVertex);
    
    for (int ii = 0; ii < outer->np; ii++) {
        for (int jj = 0; jj < linesPerVertex+1; jj++) {
            if (inner_index >= inner->np)  {inner_index -= inner->np;}
            if (inner_index < 0)           {inner_index += inner->np;}
            //printf("%d %d %d\n", inner->np, inner_index, nT);
            if (jj == linesPerVertex) {
                M->face[nT][0] = ii;
                M->face[nT][1] = ii+1; if ((ii+1)>=outer->np) {M->face[nT][1] -= outer->np;}
                M->face[nT][2] = outer->np + inner_index;;
            }
            else {
                M->face[nT][0] = ii;
                M->face[nT][1] = outer->np + inner_index + 1; if ((inner_index+1)>=inner->np) {M->face[nT][1] -= inner->np;}
                M->face[nT][2] = outer->np + inner_index;
                inner_index++; 
            }
            nT++;
        }
    }
    
}




