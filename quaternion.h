float XAXIS[]  = {1.0, 0.0, 0.0};
float YAXIS[]  = {0.0, 1.0, 0.0};
float ZAXIS[]  = {0.0, 0.0, 1.0};
float ORIGIN[] = {0.0, 0.0, 0.0};


void quatProduct (float *U, float *V, float *R) {
    //Quaternion Multiplication
    R[0] = U[0]*V[0] - U[1]*V[1] - U[2]*V[2] - U[3]*V[3];   // A*B - dotProduct(U,V)
    R[1] = U[2]*V[3] - U[3]*V[2] + U[0]*V[1] + V[0]*U[1];   // crossProduct(U,V) + A*V + B*U;
    R[2] = U[3]*V[1] - U[1]*V[3] + U[0]*V[2] + V[0]*U[2];
    R[3] = U[1]*V[2] - U[2]*V[1] + U[0]*V[3] + V[0]*U[3];
}


void normalize (float *V) {
    float M = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
    V[0] /= M; V[1] /= M; V[2] /= M;
}

void rotateVector (float rotAngle, float *inAxis, float *vertPosition) {

    //normalize rotation axis
    float rotAxis[3];
    for (int ii=0; ii<3; ii++) {rotAxis[ii] = inAxis[ii];}
    normalize(rotAxis);
    
    // initialize quaternions
    float rotQuaternion[]  = { cos(rotAngle/2.0),  rotAxis[0]*sin(rotAngle/2.0),  rotAxis[1]*sin(rotAngle/2.0),  rotAxis[2]*sin(rotAngle/2.0) };
    float conjQuaternion[] = { cos(rotAngle/2.0), -rotAxis[0]*sin(rotAngle/2.0), -rotAxis[1]*sin(rotAngle/2.0), -rotAxis[2]*sin(rotAngle/2.0) };
    float posQuaternion[]  = { 0.0              ,  vertPosition[0]             , vertPosition[1]              ,  vertPosition[2]              };
    float newQuaternion[]  = { 0.0              ,  0.0                         , 0.0                          ,  0.0                          };

    //newPosition = rotQuaternion * prevPosition * conjQuaternion
    quatProduct( rotQuaternion, posQuaternion , newQuaternion );
    quatProduct( newQuaternion, conjQuaternion, posQuaternion );
    
    vertPosition[0] = posQuaternion[1];
    vertPosition[1] = posQuaternion[2];
    vertPosition[2] = posQuaternion[3];

}

void rotateModel (float rotAngle, float *rotAxis, sbModel *M) {
    for (int ii=0; ii<M->nv; ii++) {
        rotateVector(rotAngle*radians, rotAxis, M->vert[ii]);
    }

}




