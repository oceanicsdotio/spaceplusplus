class sbPrimitive {
    // Describes a geometric primitive, either a polygon or an arc.
    public:
        int np; // number of vertices
        float vert[1000][3]; // vertex array
        sbPrimitive () { // constructor
            np = 0;
            for (int ii = 0; ii < 1000; ii++) {
                for (int dim = 0; dim < 3; dim++) {
                    vert[ii][dim] = 0.0;
                }
            }
        }
}; sbPrimitive *sbPolygon, *sbPoly[100];





////////////////////////////////////////////////////////////////////////////
void parallelogram(float ww, float hh, float dw, float dh, sbPrimitive *sbPolygon) { 
    // panel with lower left corner at origin, clockwise
    sbPolygon->np = 4;
    
    sbPolygon->vert[0][0] =  0.0;
    sbPolygon->vert[0][1] =  0.0;
    sbPolygon->vert[0][2] =  0.0;
    
    sbPolygon->vert[1][0] =  dw;
    sbPolygon->vert[1][1] =  hh;
    sbPolygon->vert[1][2] =  0.0;
    
    sbPolygon->vert[2][0] =  ww+dw;
    sbPolygon->vert[2][1] =  hh+dh;
    sbPolygon->vert[2][2] =  0.0;
    
    sbPolygon->vert[3][0] =  ww+dw;
    sbPolygon->vert[3][1] =  dh;
    sbPolygon->vert[3][2] =  0.0;
}
////////////////////////////////////////////////////////////////////////////
void rectangle (float ww, float hh, sbPrimitive *sbPolygon) { 
    // special case of parallelogram
    parallelogram (ww, hh, 0.0, 0.0, sbPolygon);
}


////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
void wedge (int nPts, float startAngle, float sweepAngle, float radius, sbPrimitive *sbPolygon) {
    // arc of regular sbPolygon approximating a circle, clockwise
    arc(nPts, startAngle, sweepAngle, radius, sbPolygon);
    sbPolygon->np++;
    sbPolygon->vert[nPts][0] = 0.0; 
    sbPolygon->vert[nPts][1] = 0.0; 
    sbPolygon->vert[nPts][2] = 0.0;
}


////////////////////////////////////////////////////////////////////////////
void shift_p (float dx, float dy, float dz, sbPrimitive *Poly) {
    // shift all vertices by given offsets
    for (int ii = 0; ii < Poly->np; ii++) {
        Poly->vert[ii][0] += dx;
        Poly->vert[ii][1] += dy;
        Poly->vert[ii][2] += dz;
    }
}

////////////////////////////////////////////////////////////////////////////
void scale_p (float dx, float dy, float dz, sbPrimitive *Poly) {
    // scale all vertices by given offsets
    for (int ii = 0; ii < Poly->np; ii++) {
        Poly->vert[ii][0] *= dx;
        Poly->vert[ii][1] *= dy;
        Poly->vert[ii][2] *= dz;
    }
}
////////////////////////////////////////////////////////////////////////////
void shell (int nPts, float start, float sweep, float w, float h, float dw, float dh, sbPrimitive *Poly) {
    sbPrimitive *tempPoly;
    tempPoly = new sbPrimitive;
    arc(nPts, start, sweep, 1.0, tempPoly);
    scale_p(w,h,0,tempPoly);
    for (int ii=0; ii<nPts; ii++) {
        for (int dim=0; dim<3; dim++) {
            Poly->vert[ii][dim] = tempPoly->vert[ii][dim];
        }
    }
    Poly->np = nPts;
    arc(nPts, start, sweep, 1.0, tempPoly);
    scale_p(w-dw,h-dh,0,tempPoly);
    for (int ii=0; ii<nPts; ii++) {
        for (int dim=0; dim<3; dim++) {
            Poly->vert[nPts+ii][dim] = tempPoly->vert[nPts-ii-1][dim];
        }
    }
    Poly->np += nPts;


}
////////////////////////////////////////////////////////////////////////////
void crossProduct (float *U, float *V, float *R) {
    R[0] = U[1]*V[2] - U[2]*V[1];
    R[1] = U[2]*V[0] - U[0]*V[2];
    R[2] = U[0]*V[1] - U[1]*V[0];
}

////////////////////////////////////////////////////////////////////////////
void add (float *U, float *V, float *R) {
    R[0] = U[0] + V[0];
    R[1] = U[1] + V[1];
    R[2] = U[2] + V[2];
}
////////////////////////////////////////////////////////////////////////////
void subtract (float *U, float *V, float *R) {
    R[0] = U[0] - V[0];
    R[1] = U[1] - V[1];
    R[2] = U[2] - V[2];
}

////////////////////////////////////////////////////////////////////////////
float dotProduct (float *a, float *b) {
    int length = 3;
    float runningSum = 0.0;
    for (int ii=0; ii<length; ii++) {
        runningSum += a[ii] * b[ii];
    }
    return runningSum;
}
////////////////////////////////////////////////////////////////////////////
float vecMag (float *vec) {
    return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}
////////////////////////////////////////////////////////////////////////////
float vecAngle (float *a, float*b) {
    
    float yy = dotProduct(a, b) / (vecMag(a) * vecMag(b));
    if (yy <= 1.0 and yy >=-1.0) return acos(yy);
    else return 0.0;


}
////////////////////////////////////////////////////////////////////////////
void bevel (int nPts, float radius, sbPrimitive *sbPolygon) {
    sbPrimitive *tempPoly, *outPoly;
    tempPoly = new sbPrimitive;
    outPoly = new sbPrimitive;
    float back[2], fore[2], theta, baseAngle, startAngle, sweepAngle, offset_x, offset_y, nextAngle;
    int length = sbPolygon->np + 2;
    int indexVec[length];
    
    for (int ii = 0; ii < length; ii++) {
        if (ii == 0) {indexVec[ii] = sbPolygon->np-1;}
        else if (ii == length-1) {indexVec[ii] = 0;}
        else {indexVec[ii] = ii-1;}
    }
    
    // forward and backward vectors for arbitrary angle calc
    for (int ii=1; ii < sbPolygon->np+1; ii++) {
        for (int dim=0; dim<2; dim++) {
            back[dim] = sbPolygon->vert[indexVec[ii-1]][dim] - sbPolygon->vert[indexVec[ii]][dim];
            fore[dim] = sbPolygon->vert[indexVec[ii+1]][dim] - sbPolygon->vert[indexVec[ii]][dim];
        }
        
        theta = vecAngle(back,fore); // angle between segments, radians
        baseAngle = atan2(back[1], back[0]); // start angle derived from angle of back segment, radians
        nextAngle = atan2(fore[1], fore[0]);
        startAngle = (baseAngle - pi/2.0);
        sweepAngle = pi - theta;
        offset_x = sbPolygon->vert[indexVec[ii]][0] + radius / sin(theta/2.0) * cos(nextAngle - theta/2.0);
        offset_y = sbPolygon->vert[indexVec[ii]][1] + radius / sin(theta/2.0) * sin(nextAngle - theta/2.0);
        
        arc(nPts, startAngle*180.0/pi, sweepAngle*180.0/pi, radius, tempPoly); //create arc
        shift_p( offset_x, offset_y, 0.0, tempPoly );
                    
        for (int jj=0; jj<nPts; jj++) { // append to new
            for (int dim=0; dim<3; dim++) {
                outPoly->vert[outPoly->np][dim] = tempPoly->vert[jj][dim];
            }
            outPoly->np++;
        }
    }
    
    sbPolygon->np = outPoly->np; // copy back to input
    for (int ii=0; ii<outPoly->np; ii++) {
        for (int dim=0; dim<3; dim++) {
            sbPolygon->vert[ii][dim] = outPoly->vert[ii][dim];
        }
    }


}

void bevel_half (int nPts, float radius, sbPrimitive *sbPolygon) {
    sbPrimitive *tempPoly, *outPoly;
    tempPoly = new sbPrimitive;
    outPoly = new sbPrimitive;
    float back[2], fore[2], theta, baseAngle, startAngle, sweepAngle, offset_x, offset_y, nextAngle;
    int length = sbPolygon->np + 2;
    int indexVec[length];
    
    for (int ii = 0; ii < length; ii++) {
        if (ii == 0) {indexVec[ii] = sbPolygon->np-1;}
        else if (ii == length-1) {indexVec[ii] = 0;}
        else {indexVec[ii] = ii-1;}
    }
    
    // forward and backward vectors for arbitrary angle calc
    for (int ii=1; ii < (sbPolygon->np)+1; ii++) {
        if (ii<3) {
        for (int dim=0; dim<2; dim++) {
            back[dim] = sbPolygon->vert[indexVec[ii-1]][dim] - sbPolygon->vert[indexVec[ii]][dim];
            fore[dim] = sbPolygon->vert[indexVec[ii+1]][dim] - sbPolygon->vert[indexVec[ii]][dim];
        }
        
        theta = vecAngle(back,fore); // angle between segments, radians
        baseAngle = atan2(back[1], back[0]); // start angle derived from angle of back segment, radians
        nextAngle = atan2(fore[1], fore[0]);
        startAngle = (baseAngle - pi/2.0);
        sweepAngle = pi - theta;
        offset_x = sbPolygon->vert[indexVec[ii]][0] + radius / sin(theta/2.0) * cos(nextAngle - theta/2.0);
        offset_y = sbPolygon->vert[indexVec[ii]][1] + radius / sin(theta/2.0) * sin(nextAngle - theta/2.0);
        
        arc(nPts, startAngle*180.0/pi, sweepAngle*180.0/pi, radius, tempPoly); //create arc
        shift_p( offset_x, offset_y, 0.0, tempPoly );
                    
        for (int jj=0; jj<nPts; jj++) { // append to new
            for (int dim=0; dim<3; dim++) {
                outPoly->vert[outPoly->np][dim] = tempPoly->vert[jj][dim];
            }
            outPoly->np++;
        }
        }
        else {
            for (int dim=0; dim<3; dim++) {
                outPoly->vert[outPoly->np][dim] = sbPolygon->vert[indexVec[ii]][dim];
            }
            outPoly->np++;
        
        }
    }
    
    sbPolygon->np = outPoly->np; // copy back to input
    for (int ii=0; ii<outPoly->np; ii++) {
        for (int dim=0; dim<3; dim++) {
            sbPolygon->vert[ii][dim] = outPoly->vert[ii][dim];
        }
    }


}
////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
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
