////////////////////////////////////////////////////////////////////////////
void subdivide (sbModel *M) {
    int vi, NF = M->nf;
    float scalar;
    for (int ii = 0; ii < NF; ii++) { // for each face
        for (int jj = 0; jj < 3; jj++) { // and every vertex index
            if (jj < 2) {vi=jj+1;} else {vi=0;}
            for (int dim = 0; dim < 3; dim++) { 
                M->vert[M->nv][dim] = 0.5*(M->vert[M->face[ii][jj]][dim] + M->vert[M->face[ii][vi]][dim] ); // create new intermediate vertex
            }
            for (int dim = 0; dim < 3; dim++) { 
                M->vert[M->nv][dim] *= 0.5 * ( vecMag(M->vert[M->face[ii][jj]]) + vecMag(M->vert[M->face[ii][vi]]) ) / vecMag(M->vert[M->nv]);
            }
            
            M->face[M->nf][0] = M->face[ii][jj];
            M->face[M->nf][1] = M->nv;
            if (jj < 1) {M->face[M->nf][2] = M->nv+2;} 
            else {M->face[M->nf][2] = M->nv-1;}
            M->nf++;
            M->nv++;
        }
        M->face[ii][0] = M->nv-3;
        M->face[ii][1] = M->nv-2;
        M->face[ii][2] = M->nv-1; 
    }

}

////////////////////////////////////////////////////////////////////////////
void roughen (sbModel *M) {
    float randAdj;
    for (int ii = 0; ii < M->nv; ii++) {
        randAdj = float(rand()%100-45)/4000.0 ;
        for (int dim = 0; dim < 3; dim++) {
            M->vert[ii][dim] *= 1.0 + randAdj;
        }
    }
}


////////////////////////////////////////////////////////////////////////////
void adjacency (sbModel *M, int adjVert[100000][5]) {
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

////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
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
    sbModel *tempMod; tempMod = new sbModel;
    float phi = 0.5*(1.0 + sqrt(5.0)), next[3];
    M->vert[M->nv][0] = 0.0; M->vert[M->nv][1] = 1.0/phi; M->vert[M->nv][2] = 1.0; M->nv++;
    next[0] = 0.0; next[1] = -1.0/phi; next[2] = 1.0;
    
    for (int ii=0; ii<5; ii++) {
        if (ii<4) {M->face[M->nf][0] = 0; M->face[M->nf][1] = M->nv; M->face[M->nf][2] = M->nv+1;   M->nf++;}
        else      {M->face[M->nf][0] = 0; M->face[M->nf][1] = M->nv; M->face[M->nf][2] = 1;         M->nf++;}
        copyVertex(next, M->vert[M->nv++]);
        rotateVector(72.0*radians, M->vert[0], next);
    }
    
    copy(M,tempMod);
    rotateModel(180.0, XAXIS, tempMod); rotateModel(-72.0, tempMod->vert[0], tempMod);
    append(tempMod,M);
    
    int TI=1, BI=11;
    for (int ii=0; ii<5; ii++) {
        M->face[M->nf][0] = TI; M->face[M->nf][1] = BI--; if (BI<7) BI=11; M->face[M->nf][2] = BI; M->nf++;
        M->face[M->nf][0] = TI++; if (TI>5) TI=1; M->face[M->nf][1] = BI; M->face[M->nf][2] = TI; M->nf++;
    }

}


////////////////////////////////////////////////////////////////////////////
void calc_asteroid (int nLevels) {
    
    sbModel *M; M = new sbModel;
    int adjVert[100000][5];

    icosahedron(M);
    for (int ii=0; ii<nLevels; ii++) {printf("Subdivision %d\n", ii); subdivide(M); removeDuplicates(M);}
    
    adjacency(M, adjVert);
    
    for (int ii=0; ii<100; ii++) {
        if (rand()%100 > 20) impact(M, adjVert);
        if (rand()%100 > 47) roughen(M);
        if (rand()%100 > 50) smooth(M, adjVert);
    }
    smooth(M, adjVert);
    
    char filename6[] = "aroid8";
    exportModel(filename6, M);
    
    
    
}
