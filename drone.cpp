#define CLOSE                       1
#define OPEN                        0

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


////////////////////////////////////////////////////////////////////////////
void build_drone () {
    sbModel *tempModel; tempModel = new sbModel;
    sbPolygon = new sbPrimitive;
    int mn = 0, mm;    
    for (int ii=0; ii<100; ii++) {
        sbPoly[ii] = new sbPrimitive;
        M[ii] = new sbModel;
    }  
    
    int res = 24;
    float radius[100];
    float offset[100];
    float slope;
    
    for (int ii=0; ii<res; ii++) {
        radius[ii] = sin(ii*90.0/res*radians);
        offset[ii] = cos(ii*90.0/res*radians);
    }
    for (int ii=res; ii<res+6; ii++)    {
        radius[ii] = sin(ii*90.0/res*radians);
        offset[ii] = cos(ii*90.0/res*radians);
        if (ii==res+5) {slope = (radius[ii]-radius[ii-1])/(offset[ii]-offset[ii-1]);}
    }
    for (int ii=res+6; ii<2*res; ii++) {
        offset[ii] = offset[ii-1] - 1.0/(float(res)-6);
        radius[ii] = radius[ii-1] - slope*1.0/(float(res)-6);
    }
    

    
    int nPts = 49;
    float rotIncrement = -1.0 * 120.0 / float(nPts-1) * radians; // rotation increment
    float vertPosition[] = {1.0, 0.0, 0.0}; // initial point
    float srad = 1.0;
    sbPolygon->np = nPts; // point count
    for (int ii=0; ii<nPts; ii++) {
        srad = 1.0 + 0.3*sin(3.0*float(ii)*rotIncrement/2.0)*sin(3.0*float(ii)*rotIncrement/2.0)
                        *sin(3.0*float(ii)*rotIncrement/2.0)*sin(3.0*float(ii)*rotIncrement/2.0)
                        *sin(3.0*float(ii)*rotIncrement/2.0)*sin(3.0*float(ii)*rotIncrement/2.0)
                        *sin(3.0*float(ii)*rotIncrement/2.0)*sin(3.0*float(ii)*rotIncrement/2.0);
        for (int dim=0; dim<2; dim++) sbPolygon->vert[ii][dim] = vertPosition[dim]*srad; // copy point to polygon
        rotateVector(rotIncrement, ZAXIS, vertPosition); // use quarternion rotation
    }
    
    extrude_planar(2*res, radius, offset, sbPolygon, tempModel);
    removeDuplicates(tempModel);
    char filename[] = "tetra";
    exportModel(filename, tempModel);
}
