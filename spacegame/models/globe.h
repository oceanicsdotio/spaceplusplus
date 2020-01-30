float vertexColor[0];
//float vertexColor[0] = {1.0, 1.0, 1.0};
int R = 24; // NOTE: R=24 creates reasonable mesh
float PI = 3.1415926;
int ii, jj;
int vertexCount = R*(R/2-1)+2;
float vertexArray[vertexCount][3];
int polyCount = R*R/2;
int faces[polyCount][0];

// top vertex
vertexArray[0][0] = {0, 0, 1};

// bottom vertex
vertexArray[vertexCount-1][0] = {0, 0, -1};

// ring vertices
step = 2*PI/((float)R);
count = 2;
for (ii=1; ii<(R-2); ii++) { // cycle azimuth
    for (jj=0; jj<R; jj++) { // cycle around Z
        az = PI/2.0-step*ii;
        theta = jj*step;

        X = cos(theta) * abs(cos(az));
        Y = sin(theta) * abs(cos(az));
        Z = sin(az);
    }
}

// top faces
for (ii=0; ii<R; ii++) {
    if (ii < R) {
        faces[ii][0] = {0, ii+1, ii+2, -1};
    else
        faces[ii][0] = {0, ii+1, 1, -1];
    }
}

// middle faces
for (ii=0; ii<(R/2-1); ii++) {
    for (jj=0;jj<R;jj++) {
        if (j==R) {
            faces[ii*R+jj][0] = {(ii-1)*R+jj, (ii-1)*R+jj-R+1, (ii-1)*R+jj+1, (ii)*R+jj};
        else
            faces[ii*R+jj][0] = {(ii-1)*R+jj, (ii-1)*R+jj+1, (ii-1)*R+jj+1+R, (ii-1)*R+jj+R};
        }
    }
}

// bottom faces
for (ii=0; ii<R; ii++) {
    if (ii < R) {
        faces[polyCount-ii+1][0] = {vertexCount, vertexCount-ii, vertexCount-ii-1, 0};
    else
        faces[polyCount-ii+1][0] = {vertexCount, vertexCount-ii, vertexCount-1, 0};
    }
}
