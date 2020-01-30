#define CLOSE                       1
#define OPEN                        0


void dock (slope, ringNum) {

    pointCount = 9;
    float D, C, H;
    float Y[pointCount], Z[pointCount];

    if (ringNum == 1)
    {
        Y = {0.50, 1.00, 1.00, 1.75, 2.25, 2.25, 1.50, 1.50, 0.50};
        Z = {1.25, 1.25, 1.50, 1.50, 0.50, 0.00, 0.00, 0.25, 0.75};
    }
    if (ringNum == 2)
    {
        D = 1.5 - slope*0.75;
        Y = {0.50, 1.00, 1.00, 1.75, 2.25, 2.25, 1.00, 1.00, 0.50};
        Z = {1.25, 1.25, 1.50,   D , 0.50, 0.00, 0.00, 0.25, 0.75};
    }

    if (ringNum ==3)
    {
        D = 1.5 - slope*0.75;
        C = 1.25;
        H = 0.75;
        Y = {0.50, 1.00, 1.00, 1.75, 2.25, 2.25, 1.00, 1.00, 0.50};
        Z = {1.25, 1.25, C   , D   , 0.50, 0.00, 0.00, H   , 0.75];
        }


    }
}



void build_body (sbModel *M) {

    sbModel *tempMod; tempMod = new sbModel;
    sbPrimitive *sbPolygon; sbPolygon = new sbPrimitive;
    
    //bridges
    float radius[] = {1.0, 1.0};
    float offset[] = {0.0, 0.75};
    
    rectangle(0.5, 0.25, sbPolygon);
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE);
    shift(0.0, 1.5, 0.0, tempMod);
    append(tempMod,M);
    
    parallelogram(0.5,0.25,0.0, -0.25, sbPolygon);
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE);
    shift(0.5, 1.5, 0.0, tempMod);
    append(tempMod,M);

    copy(M,tempMod);
    shift(0.0, 0.0, -2.0, tempMod);
    append(tempMod,M);
    
    //fore block
    offset[1] = 2.75;
    rectangle(0.25,0.75, sbPolygon);
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE);
    shift(2.0, 0.0, -2.0, tempMod);
    append(tempMod,M);
    
    
    //forward shell
    offset[1] = 2.75;
    shell(8, 90.0, 90.0, 1.25, 0.75, 0.25, 0.25, sbPolygon);
    extrude(2, radius, offset, sbPolygon, tempMod, OPEN);
    shift(1.0, 0.75, -2.0, tempMod);
    append(tempMod,M);
    
    radius[0]=0.75; radius[1]=1.0;
    offset[0]=0.0; offset[1]=0.0;
    arc (8, 90, 90, 1.0, sbPolygon);
    extrude_planar(2,radius,offset,sbPolygon,tempMod);
    scale(1.25,0.75,1.0,tempMod);
    shift(1.0,0.75,-2.0,tempMod);
    append(tempMod,M);
    
}


void build_arm (sbModel *M) {

    sbModel *tempMod; tempMod = new sbModel;
    sbPrimitive *sbPolygon; sbPolygon = new sbPrimitive;
    float offset[] = {0.0, 1.0};
    float radius[] = {1.0, 1.0};
    
    // ARM+SHIELD
    //main panels
    parallelogram(1.25, 0.25, 0.0, -1.0, sbPolygon);
    offset[1] = 1.75;
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE);
    shift(1.0, 1.25, 0.75, tempMod);
    append(tempMod, M);
    
    
    offset[1] = 2.25;
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE);
    shift(1.0, 1.25, 3.0, tempMod);
    append(tempMod, M);
    
    parallelogram(1.0, 0.25, 0.0, -0.8, sbPolygon);
    offset[1] = 0.5;
    extrude(2, radius, offset, sbPolygon, tempMod, OPEN);
    shift(1.0, 1.25, 2.5, tempMod);
    append(tempMod, M);

    //lower joiner
    rectangle (0.25, 0.5, sbPolygon);
    offset[1] = 1.75;
    extrude (2, radius, offset, sbPolygon, tempMod, CLOSE); //close
    shift(2.0, 0.0, 0.75, tempMod);
    append(tempMod, M);

    //upper overhangs
    rectangle(0.5, 0.25, sbPolygon);
    bevel_half(10, 0.1, sbPolygon);
    offset[1] = 2.5;
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE); //close
    shift(0.5, 1.25, 0.0, tempMod);
    append(tempMod, M);

    offset[1] = 0.5;
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE); //close
    shift(0.5, 1.25, 3.0, tempMod);
    append(tempMod, M);

    offset[1] = 0.5;
    extrude(2, radius, offset, sbPolygon, tempMod, CLOSE); //close
    shift(0.5, 1.25, 3.75, tempMod);
    append(tempMod, M);
    
    

    //pipe
    regular_polygon(10, sbPolygon);
    radius[0] = 0.1; radius[1] = 0.1;
    offset[1] = 3.5;
    extrude(2, radius, offset, sbPolygon, tempMod, OPEN);
    shift(0.65, 1.35, 0.25, tempMod);
    append(tempMod, M);
    
    extrude(2, radius, offset, sbPolygon, tempMod, OPEN);
    shift(0.85, 1.35, 0.25, tempMod);
    append(tempMod, M);
}


void build_engine (float S, float A, float B, float C, sbModel *M) {
    sbModel *tempMod; tempMod = new sbModel;
    sbPrimitive *sbPolygon; sbPolygon = new sbPrimitive;

    float w_radius[] = {  0.3, 0.3, 1.0, 1.0, 3.0, 3.0};
    float w_offset[] = {-12.0, 2.0, 3.0, 4.0, 6.0, 7.0};
    float e_offset[] =  {-12, -12, -11, -4, -3, 1, 2, 5, 5}; //{5.0, 5.0, 2.0, 1.0,-3.0, -4.0, -11.0, -12.0, -12.0};
    float e_radius[] =  {0.3, 2.0, 3.0, 3.0 ,2.5, 2.5, 3.0,3.0,2.0}; //{2.0, 3.0, 3.0, 2.5, 2.5,  3.0,   3.0,   2.0,   0.3};

    regular_polygon(72, sbPolygon); 
    extrude(9, e_radius, e_offset, sbPolygon, tempMod, OPEN);
    append(tempMod, M);
    extrude (6, w_radius, w_offset, sbPolygon, tempMod, OPEN);
    append(tempMod, M);
    shift(0.0, 0.0, 12.0, M);
    scale(S, S, S, M);
    shift(A, B, C, M);
}



void build_thruster (float A, float B, float C, sbModel *M) {
    sbModel *tempMod;
    tempMod = new sbModel;
    sbPrimitive *sbPolygon;
    sbPolygon = new sbPrimitive;
    float t_radius[] = {0.6, 0.75, 1.0, 1.0, 0.75, 0.6};
    float t_offset[] = {-0.75, -0.75, -0.5, 0.5, 0.75, 0.75};
    rectangle(0.5,0.8, sbPolygon);
    bevel(10, 0.15, sbPolygon);
    extrude(6, t_radius, t_offset, sbPolygon, tempMod, OPEN);
    shift(-0.5, 0, 0, tempMod);
    //reflect(2, tempMod);
    append(tempMod,M);
    reflect(1, tempMod);
    append(tempMod,M);
    shift(A, B, C, M);
}



void build_tube (float S, float A, float B, float C, sbModel *M) {
    sbModel *tempMod;
    tempMod = new sbModel;
    sbPrimitive *sbPoly[2];
    sbPoly[0] = new sbPrimitive;
    sbPoly[1] = new sbPrimitive;
    float tubeLength = 3.0;
    float radius[] = {1.0, 1.0};
    float offset[] = {0.0, tubeLength};
    
    regular_polygon(6, sbPoly[0]); // hexagon
    scale_p(2.0/sqrt(3), 2.0/sqrt(3), 1.0, sbPoly[0]);
    regular_polygon(72, sbPoly[1]); // tube poly
    scale_p(0.75, 0.75, 1.0, sbPoly[1]);
    shift_p(0.0,0.0,0.5,sbPoly[1]); // depth of funnel, shift tube
    stitch (sbPoly[0], sbPoly[1], tempMod); // create front funnel
    append(tempMod,M); // add to model
    reflect(2, tempMod); // create back
    shift(0.0, 0.0, tubeLength+0.5, tempMod); 
    append(tempMod,M); // add to model
    

    extrude (2, radius, offset, sbPoly[1], tempMod, OPEN); // inner tube
    append(tempMod,M);
    
    offset[0] = 0.0; offset[1] = 3.5;
    extrude (2, radius, offset, sbPoly[0], tempMod, OPEN); // inner tube
    append(tempMod,M);
    
    
    
    scale(S, S, S, M);
    shift(A, B, C, M);
 
}
 

void build_tube_cover (float S, float A, float B, float C, sbModel *M) {
    sbModel *tempMod;
    tempMod = new sbModel;
    sbPrimitive *sbPoly[2];
    sbPoly[0] = new sbPrimitive;
    sbPoly[1] = new sbPrimitive;
    
    regular_polygon(6, sbPoly[0]);
    scale_p(2.0/sqrt(3), 2.0/sqrt(3), 1.0,sbPoly[0]);
    regular_polygon(72, sbPoly[1]);
    scale_p(0.75, 0.75, 1.0, sbPoly[1]);
    shift_p(0.0,0.0,0.5,sbPoly[1]);
    stitch (sbPoly[0], sbPoly[1], tempMod);
    append(tempMod,M);
    float radius[] = {1.0, 1.0};
    float offset[] = {0.0, 0.0};
    extrude (2, radius, offset, sbPoly[0], tempMod, CLOSE);
    append(tempMod,M);
    extrude (2, radius, offset, sbPoly[1], tempMod, CLOSE);
    append(tempMod,M);
    scale(S, S, S, M);
    shift(A, B, C, M);
 
}


void build_ship () {
    sbModel *tempModel; tempModel = new sbModel;
    int mn = 0, mm;    
    for (int ii=0; ii<100; ii++) {
        sbPoly[ii] = new sbPrimitive;
        M[ii] = new sbModel;
    }  
    sbPolygon = new sbPrimitive;
  
    float offset[] = {0.0, 1.0};
    float radius[] = {1.0, 1.0};
    
    build_body (M[mn++]);    
    build_arm  (M[mn++]);

    
    for (int ii=0; ii<mn; ii++) {
        copy(M[ii],tempModel);
        reflect(1, tempModel);
        append(tempModel,M[ii]);
        
        copy(M[ii], tempModel);
        reflect(0, tempModel);
        append(tempModel,M[ii]);
    }
    
    tempModel = new sbModel; 
    for (int ii=0; ii<mn; ii++) {
        append(M[ii], tempModel);
        M[ii] = new sbModel;
    }
    
    removeDuplicates(tempModel);
    shift(0.0,0.0,2.0-2.75/2.0,tempModel);
    rotateModel(90.0,XAXIS,tempModel);
    rotateModel(-90.0,ZAXIS,tempModel);
    char filename[] = "hull";
    exportModel(filename, tempModel);
    
    mn=0;
    
    // smaller engines
    build_engine    (0.1,   0.6,    0.70,   2.75/2.0,    M[mn++]);
    build_thruster  (       2.75,   0.0,    0.0,  M[mn++]); 
    build_engine    (0.2,   1.2,    0.0,    2.75/2.0,    M[mn++]); //large engines x2
    build_launcher  (                                      );
    
    float testOffset=0.0;
    float tubeScale = 0.25;
    float LRShift = tubeScale*2.0/sqrt(3);
    for (int ii=0; ii<3; ii++) {
        build_tube(tubeScale, LRShift, 0.0, 0.0, M[mn]); // missile tubes
        rotateModel(float(ii)*120.0 + 180.0, ZAXIS, M[mn]);
        shift(testOffset+1.2, 0.0, -2.0, M[mn]);
        mn++;
        
        build_tube_cover(tubeScale, LRShift, 0.0, 0.0, M[mn]); // tube cover
        shift(0.0, -tubeScale, 0.0, M[mn]);
        rotateModel(0.0, XAXIS, M[mn]);
        shift(0.0, tubeScale, 0.0, M[mn]);
        rotateModel(float(ii)*120.0 + 180.0, ZAXIS, M[mn]);
        shift(testOffset+1.2, 0.0, -2.0, M[mn]);
        printf("%d \n", mn);
        mn++;
        
        
        rectangle(0.5, 0.5, sbPolygon); // fill between
        offset[1] = 0.5;
        extrude(2, radius, offset, sbPolygon, M[mn], CLOSE);
        rotateModel(-45.0, ZAXIS, M[mn]);
        scale(LRShift*sqrt(2.0), sqrt(2.0)/2.0, sqrt(2.0)/2.07, M[mn]);
        shift(LRShift,0.0,0.0,M[mn]);
        rotateModel(float(ii)*120.0, ZAXIS, M[mn]);
    
    shift(testOffset+1.2, 0.0, -2.0, M[mn]);
    mn++;
        
    }
    //skirt
    regular_polygon(6, sbPolygon);
    radius[0] = 2.0*tubeScale*2.0/sqrt(3); radius[1] = 2.0*1.25*tubeScale*2.0/sqrt(3);
    offset[0] = 0.0; offset[1] = 0.1;
    extrude(2, radius, offset, sbPolygon, M[mn], OPEN);
    shift(testOffset+1.2, 0.0, -2.0, M[mn]);
    mn++;
    
    
    //pipe
    regular_polygon(6, sbPolygon);
    radius[0] = 0.025; radius[1] = 0.025;
    offset[0] = -2.0; offset[1] = 0.0;
    extrude(2, radius, offset, sbPolygon, M[mn], OPEN);
    shift(testOffset+1.2, 0.0, 0.0, M[mn]);
    mn++;
    
    //loader
    float thetaA = 30.0;

    
    shell(36, 90.0, 180.0, 1.0, 1.0, 0.25, 0.25, sbPolygon); 
    radius[0] = 1.0; radius[1] = 1.0;
    offset[0] = -1.0; offset[1] = -0.8;
    extrude(2, radius, offset, sbPolygon, M[mn], OPEN); // clamp
    scale(tubeScale, tubeScale, 1.0, M[mn]);
    
    copy(M[mn],M[mn+1]); // clamp
    rotateModel(thetaA, ZAXIS, M[mn]);
    shift(testOffset+1.2-tubeScale, 0.0, 0.0, M[mn]);
    mn++;

    copy(M[mn],M[mn+1]); // clamp
    rotateModel(-thetaA, ZAXIS, M[mn]);
    shift(testOffset+1.2-tubeScale, 0.0, 0.25, M[mn]);
    mn++;
    
    rotateModel(thetaA, ZAXIS, M[mn]);
    shift(testOffset+1.2-tubeScale, 0.0, 0.5, M[mn]);
    mn++;
    
    regular_polygon(72, sbPolygon); // plunger
    offset[0] = -0.25; offset[1] = -0.2;
    extrude(2, radius, offset, sbPolygon, M[mn], CLOSE);
    scale(0.75*tubeScale, 0.75*tubeScale, 1.0, M[mn]);
    shift(testOffset+1.2-tubeScale, 0.0, 0.0, M[mn]);
    mn++;
    
    
    regular_polygon(3, sbPolygon); // base
    bevel(10, 0.3, sbPolygon);
    offset[0] = -1.0; offset[1] = -0.25;
    extrude(2, radius, offset, sbPolygon, M[mn], OPEN);
    scale(0.75*tubeScale, 0.75*tubeScale, 1.0, M[mn]);
    shift(testOffset+1.2-tubeScale, 1.0, 0.0, M[mn]);
    mn++;

    for (int ii=0; ii<mn; ii++) {
       rotateModel(90.0,XAXIS,M[ii]);
       rotateModel(-90.0,ZAXIS,M[ii]);
       //scale(0.2,0.2,0.2,M[ii]);  
    }
}





void build_drone () {
    sbModel *tempModel;
    tempModel = new sbModel;
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

        // copy point to polygon
        for (int dim=0; dim<2; dim++) sbPolygon->vert[ii][dim] = vertPosition[dim]*srad;
        rotateVector(rotIncrement, ZAXIS, vertPosition);
    }

    extrude_planar(2*res, radius, offset, sbPolygon, tempModel);
    removeDuplicates(tempModel);
    exportModel("drone/drone.dat", tempModel);
}
