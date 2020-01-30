#ifndef SPACEGAME_PRIMITIVE_H
#define SPACEGAME_PRIMITIVE_H
#endif

class Primitive
{
// Describes a geometric primitive, either a polygon or an arc.
public:
    int np; // number of vertices
    float vert[CAPACITY][3]; // vertex array
    sbPrimitive () { // constructor
        np = 0;
        for (int ii = 0; ii < CAPACITY; ii++) {
            for (int dim = 0; dim < 3; dim++) {
                vert[ii][dim] = 0.0;
            }
        }
    }

    void shift (float dx, float dy, float dz) {
        // shift all vertices by given offsets
        for (int ii = 0; ii < np; ii++) {
            vert[ii][0] += dx;
            vert[ii][1] += dy;
            vert[ii][2] += dz;
        }
    }

    void scale (float dx, float dy, float dz) {
        // scale all vertices by given offsets
        for (int ii = 0; ii < np; ii++) {
            vert[ii][0] *= dx;
            vert[ii][1] *= dy;
            vert[ii][2] *= dz;
        }
    }


    void bevel (int nPts, float radius) {
        sbPrimitive *tempPoly, *outPoly;
        tempPoly = new sbPrimitive;
        outPoly = new sbPrimitive;
        float back[2], fore[2], theta, baseAngle, startAngle, sweepAngle, offset_x, offset_y, nextAngle;
        int length = np + 2;
        int indexVec[length];

        for (int ii = 0; ii < length; ii++) {
            if (ii == 0) {indexVec[ii] = np-1;}
            else if (ii == length-1) {indexVec[ii] = 0;}
            else {indexVec[ii] = ii-1;}
        }

        // forward and backward vectors for arbitrary angle calc
        for (int ii=1; ii < np+1; ii++) {
            for (int dim=0; dim<2; dim++) {
                back[dim] = vert[indexVec[ii-1]][dim] - vert[indexVec[ii]][dim];
                fore[dim] = vert[indexVec[ii+1]][dim] - vert[indexVec[ii]][dim];
            }

            theta = vecAngle(back,fore); // angle between segments, radians
            baseAngle = atan2(back[1], back[0]); // start angle derived from angle of back segment, radians
            nextAngle = atan2(fore[1], fore[0]);
            startAngle = (baseAngle - pi/2.0);
            sweepAngle = pi - theta;
            offset_x = vert[indexVec[ii]][0] + radius / sin(theta/2.0) * cos(nextAngle - theta/2.0);
            offset_y = vert[indexVec[ii]][1] + radius / sin(theta/2.0) * sin(nextAngle - theta/2.0);

            arc(nPts, startAngle*180.0/pi, sweepAngle*180.0/pi, radius, tempPoly); //create arc
            shift_p( offset_x, offset_y, 0.0, tempPoly );

            for (int jj=0; jj<nPts; jj++) { // append to new
                for (int dim=0; dim<3; dim++) {
                    outPoly->vert[outPoly->np][dim] = tempPoly->vert[jj][dim];
                }
                outPoly->np++;
            }
        }

        np = outPoly->np; // copy back to input
        for (int ii=0; ii<outPoly->np; ii++) {
            for (int dim=0; dim<3; dim++) {
                vert[ii][dim] = outPoly->vert[ii][dim];
            }
        }
    }

    void bevel_half (int nPts, float radius) {
        sbPrimitive *tempPoly, *outPoly;
        tempPoly = new sbPrimitive;
        outPoly = new sbPrimitive;
        float back[2], fore[2], theta, baseAngle, startAngle, sweepAngle, offset_x, offset_y, nextAngle;
        int length = np + 2;
        int indexVec[length];

        for (int ii = 0; ii < length; ii++) {
            if (ii == 0) {indexVec[ii] = np-1;}
            else if (ii == length-1) {indexVec[ii] = 0;}
            else {indexVec[ii] = ii-1;}
        }

        // forward and backward vectors for arbitrary angle calc
        for (int ii=1; ii < (np)+1; ii++) {
            if (ii<3) {
                for (int dim=0; dim<2; dim++) {
                    back[dim] = vert[indexVec[ii-1]][dim] - vert[indexVec[ii]][dim];
                    fore[dim] = vert[indexVec[ii+1]][dim] - vert[indexVec[ii]][dim];
                }

                theta = vecAngle(back,fore); // angle between segments, radians
                baseAngle = atan2(back[1], back[0]); // start angle derived from angle of back segment, radians
                nextAngle = atan2(fore[1], fore[0]);
                startAngle = (baseAngle - pi/2.0);
                sweepAngle = pi - theta;
                offset_x = vert[indexVec[ii]][0] + radius / sin(theta/2.0) * cos(nextAngle - theta/2.0);
                offset_y = vert[indexVec[ii]][1] + radius / sin(theta/2.0) * sin(nextAngle - theta/2.0);

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
                    outPoly->vert[outPoly->np][dim] = vert[indexVec[ii]][dim];
                }
                outPoly->np++;

            }
        }

        np = outPoly->np; // copy back to input
        for (int ii=0; ii<outPoly->np; ii++) {
            for (int dim=0; dim<3; dim++) {
                vert[ii][dim] = outPoly->vert[ii][dim];
            }
        }
    }


};



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


void rectangle (float ww, float hh, sbPrimitive *sbPolygon) {
    // special case of parallelogram
    parallelogram (ww, hh, 0.0, 0.0, sbPolygon);
}




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


