#pragma once

#define CAPACITY 10000
#define radians  3.1415926/180.0
#define pi       3.1415926

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <string>
#include <vector>


class Vec3
{
public:
    std::vector<float> value;

    Vec3 (std::initializer_list<float> data) {
        value = data;
    };

    Vec3 operator + (Vec3 data){
        auto newVec = Vec3({0,0,0});
        for (int ii=0; ii<3; ii++) {
            newVec.value[ii] = this->value[ii] + data[ii];
        }
        return newVec;
    };

    Vec3 operator - (Vec3 data){
        auto newVec = Vec3({0,0,0});
        for (int ii=0; ii<3; ii++) {
            newVec.value[ii] = this->value[ii] - data[ii];
        }
        return newVec;
    };

    Vec3 operator * (Vec3 data){
        auto newVec = Vec3({0,0,0});
        for (int ii=0; ii<3; ii++) {
            newVec.value[ii] = this->value[ii] * data[ii];
        }
        return newVec;
    }

    Vec3 operator / (float val){
        auto newVec = Vec3({0,0,0});
        for (int ii=0; ii<3; ii++) {
            newVec.value[ii] = this->value[ii] / val;
        }
        return newVec;
    }

    Vec3 operator * (float val){
        auto newVec = Vec3({0,0,0});
        for (int ii=0; ii<3; ii++) {
            newVec.value[ii] = this->value[ii] * val;
        }
        return newVec;
    }

    Vec3 operator + (float val){
        auto newVec = Vec3({0,0,0});
        for (int ii=0; ii<3; ii++) {
            newVec.value[ii] = value[ii] + val;
        }
        return newVec;
    }

    void operator += (float val){
        for (int ii=0; ii<3; ii++) {
            value[ii] += val;
        }
    }

    void operator += (Vec3 val){
        for (int ii=0; ii<3; ii++) {
            value[ii] += val[ii];
        }
    }

    void operator -= (float val){
        for (int ii=0; ii<3; ii++) {
            value[ii] -= val;
        }
    }

    void operator *= (float val)
    {
        for (int ii=0; ii<3; ii++) {
            value[ii] *= val;
        }
    }

    void operator /= (float val){
        for (int ii=0; ii<3; ii++) {
            value[ii] /= val;
        }
    }


    float operator [] (int ind) {
        return this->value[ind];
    }


    float length() {
        auto newVec = Vec3({0,0,0});
        for (int ii=0; ii<3; ii++) {
            newVec.value[ii] = this->value[ii];
        }
        return sqrt((*this * newVec).sum());
    }

    float sum() {
        float runningSum = 0.0;
        for (int ii=0; ii<3; ii++) {
            runningSum += this->value[ii];
        }
        return runningSum;
    }

    Vec3 normalized() {
        return *this / length();
    }



    float static dotProduct (Vec3 U, Vec3 V) {
        float runningSum = 0.0;
        auto newVec = U * V;
        for (int ii=0; ii<3; ii++) {
            runningSum += newVec[ii];
        }
        return runningSum;
    }

    float vecAngle (Vec3 U, Vec3 V) {
        float yy = dotProduct(U, V) / (U.length() * V.length());
        return (yy <= 1.0 and yy >=-1.0) ? acos(yy) : 0.0;
    }

    Vec3 rotate(float angle, Vec3 axis) {

        const auto quaternion = [](float *U, float *V, float *R) {
            //Quaternion Multiplication
            R[0] = U[0]*V[0] - U[1]*V[1] - U[2]*V[2] - U[3]*V[3];   // A*B - dotProduct(U,V)
            R[1] = U[2]*V[3] - U[3]*V[2] + U[0]*V[1] + V[0]*U[1];   // crossProduct(U,V) + A*V + B*U;
            R[2] = U[3]*V[1] - U[1]*V[3] + U[0]*V[2] + V[0]*U[2];
            R[3] = U[1]*V[2] - U[2]*V[1] + U[0]*V[3] + V[0]*U[3];
        };

        //normalize rotation axis
        Vec3 rotAxis = axis.normalized();
        float memo = sin(angle/2.0);

        float rotQuaternion[]  = { cos(angle/2.0),  rotAxis[0]*memo,  rotAxis[1]*memo,  rotAxis[2]*memo };
        float conjQuaternion[] = { cos(angle/2.0), -rotAxis[0]*memo, -rotAxis[1]*memo, -rotAxis[2]*memo };
        float posQuaternion[]  = { 0.0           ,  this->value[0],   this->value[1],   this->value[2]   };
        float newQuaternion[]  = { 0.0           ,  0.0            , 0.0             ,  0.0             };

        quaternion( rotQuaternion, posQuaternion , newQuaternion );
        quaternion( newQuaternion, conjQuaternion, posQuaternion );

        return Vec3({posQuaternion[1], posQuaternion[2], posQuaternion[3]});
    }

};

Vec3 crossProduct (Vec3 U, Vec3 V) {
    return Vec3({
        U[1]*V[2] - U[2]*V[1],
        U[2]*V[0] - U[0]*V[2],
        U[0]*V[1] - U[1]*V[0]
    });
}


auto XAXIS = Vec3({1.0, 0.0, 0.0});
auto YAXIS = Vec3({0.0, 1.0, 0.0});
auto ZAXIS = Vec3({0.0, 0.0, 1.0});
auto ORIGIN = Vec3({0.0, 0.0, 0.0});


class Primitive
{
    friend class Vec3;
public:
    std::vector<Vec3> vert;

    Primitive (int size=0) {
        for (int ii = 0; ii < size; ii++) {
            vert.push_back(Vec3({0,0,0}));
        }
    };

    void shift (float dx, float dy, float dz) {
        auto vertexArray = Primitive();
        const int np = this->vert.size();
        for (int ii = 0; ii < np; ii++) {
            this->vert[ii] = vert[ii] + Vec3({dx, dy, dz});
        }
    }

    void scale (float dx, float dy, float dz) {
        const int np = this->vert.size();
        for (int ii = 0; ii < np; ii++) {
            this->vert[ii] = this->vert[ii] * Vec3({dx, dy, dz});
        }
    }

    void copyFace (int *A, int *B) {
        for (int ii = 0; ii < 3; ii++) B[ii] = A[ii];
    }

    Primitive static regular_polygon(int n, Vec3 axis = ZAXIS) {
        auto shape = Primitive(n);
        float rotIncrement = -1.0 * 360.0 / (float) n * radians; // rotation increment
        auto vertPosition = Vec3({1.0, 0.0, 0.0}); // initial point
        for (int ii = 0; ii < n; ii++) {
            shape.vert[ii] = vertPosition.rotate(rotIncrement*ii, axis);
        }
        return shape;
    }


    Primitive static parallelogram(float ww, float hh, float dw, float dh) {
        // panel with lower left corner at origin, clockwise
        auto shape = Primitive(4);
        shape.vert[0] = ORIGIN;
        shape.vert[1] = Vec3({dw, hh, 0.0});
        shape.vert[2] = Vec3({ww+dw, hh+dh, 0.0});
        shape.vert[3] = Vec3({ww+dw, dh, 0.0});
        return shape;
    }

    Primitive static rectangle(float ww, float hh) {
        // special case of parallelogram
        return parallelogram (ww, hh, 0.0, 0.0);
    }


    Primitive static wedge(int np, float startAngle, float sweepAngle, float radius) {
        // arc of regular sbPolygon approximating a circle, clockwise
        auto shape = arc(np, startAngle, sweepAngle, radius);
        shape.vert.push_back(ORIGIN);
        return shape;
    }

    Primitive static arc(int np, float startAngle, float sweepAngle, float radius) {
        float rotIncrement = -1.0 * sweepAngle / (float) (np-1) * radians; // rotation increment
        auto vertPosition = Vec3({radius, 0.0, 0.0}); // initial point
        auto shape = Primitive(np);
        for (int ii = 0; ii < np; ii++) {
            shape.vert[ii] = vertPosition.rotate(startAngle*radians + ii*rotIncrement, Vec3({0,0,1}));
        }
    }
};


//
//    void bevel (int nPts, float radius) {
//        sbPrimitive *tempPoly, *outPoly;
//        tempPoly = Primitive;
//        outPoly = new sbPrimitive;
//        float back[2], fore[2], theta, baseAngle, startAngle, sweepAngle, offset_x, offset_y, nextAngle;
//        int length = np + 2;
//        int indexVec[length];
//
//        for (int ii = 0; ii < length; ii++) {
//            if (ii == 0) {indexVec[ii] = np-1;}
//            else if (ii == length-1) {indexVec[ii] = 0;}
//            else {indexVec[ii] = ii-1;}
//        }
//
//        // forward and backward vectors for arbitrary angle calc
//        for (int ii=1; ii < np+1; ii++) {
//            for (int dim=0; dim<2; dim++) {
//                back[dim] = vert[indexVec[ii-1]][dim] - vert[indexVec[ii]][dim];
//                fore[dim] = vert[indexVec[ii+1]][dim] - vert[indexVec[ii]][dim];
//            }
//
//            theta = vecAngle(back,fore); // angle between segments, radians
//            baseAngle = atan2(back[1], back[0]); // start angle derived from angle of back segment, radians
//            nextAngle = atan2(fore[1], fore[0]);
//            startAngle = (baseAngle - pi/2.0);
//            sweepAngle = pi - theta;
//            offset_x = vert[indexVec[ii]][0] + radius / sin(theta/2.0) * cos(nextAngle - theta/2.0);
//            offset_y = vert[indexVec[ii]][1] + radius / sin(theta/2.0) * sin(nextAngle - theta/2.0);
//
//            arc(nPts, startAngle*180.0/pi, sweepAngle*180.0/pi, radius, tempPoly); //create arc
//            shift_p( offset_x, offset_y, 0.0, tempPoly );
//
//            for (int jj=0; jj<nPts; jj++) { // append to new
//                for (int dim=0; dim<3; dim++) {
//                    outPoly->vert[outPoly->np][dim] = tempPoly->vert[jj][dim];
//                }
//                outPoly->np++;
//            }
//        }
//
//        np = outPoly->np; // copy back to input
//        for (int ii=0; ii<outPoly->np; ii++) {
//            for (int dim=0; dim<3; dim++) {
//                vert[ii][dim] = outPoly->vert[ii][dim];
//            }
//        }
//    }
//
//    void bevel_half (int nPts, float radius) {
//        sbPrimitive *tempPoly, *outPoly;
//        tempPoly = Primitive();
//        outPoly = Primitive();
//        float back[2], fore[2], theta, baseAngle, startAngle, sweepAngle, offset_x, offset_y, nextAngle;
//        int length = np + 2;
//        int indexVec[length];
//
//        for (int ii = 0; ii < length; ii++) {
//            if (ii == 0) {indexVec[ii] = np-1;}
//            else if (ii == length-1) {indexVec[ii] = 0;}
//            else {indexVec[ii] = ii-1;}
//        }
//
//        // forward and backward vectors for arbitrary angle calc
//        for (int ii=1; ii < (np)+1; ii++) {
//            if (ii<3) {
//                for (int dim=0; dim<2; dim++) {
//                    back[dim] = vert[indexVec[ii-1]][dim] - vert[indexVec[ii]][dim];
//                    fore[dim] = vert[indexVec[ii+1]][dim] - vert[indexVec[ii]][dim];
//                }
//
//                theta = vecAngle(back,fore); // angle between segments, radians
//                baseAngle = atan2(back[1], back[0]); // start angle derived from angle of back segment, radians
//                nextAngle = atan2(fore[1], fore[0]);
//                startAngle = (baseAngle - pi/2.0);
//                sweepAngle = pi - theta;
//                offset_x = vert[indexVec[ii]][0] + radius / sin(theta/2.0) * cos(nextAngle - theta/2.0);
//                offset_y = vert[indexVec[ii]][1] + radius / sin(theta/2.0) * sin(nextAngle - theta/2.0);
//
//                arc(nPts, startAngle*180.0/pi, sweepAngle*180.0/pi, radius, tempPoly); //create arc
//                shift_p( offset_x, offset_y, 0.0, tempPoly );
//
//                for (int jj=0; jj<nPts; jj++) { // append to new
//                    for (int dim=0; dim<3; dim++) {
//                        outPoly->vert[outPoly->np][dim] = tempPoly->vert[jj][dim];
//                    }
//                    outPoly->np++;
//                }
//            }
//            else {
//                for (int dim=0; dim<3; dim++) {
//                    outPoly->vert[outPoly->np][dim] = vert[indexVec[ii]][dim];
//                }
//                outPoly->np++;
//
//            }
//        }
//
//        np = outPoly->np; // copy back to input
//        for (int ii=0; ii<outPoly->np; ii++) {
//            for (int dim=0; dim<3; dim++) {
//                vert[ii][dim] = outPoly->vert[ii][dim];
//            }
//        }
//    }



//    void shell (int nPts, float start, float sweep, float w, float h, float dw, float dh) {
//
//        auto tempPoly = Primitive();
//        tempPoly.arc(nPts, start, sweep, 1.0);
//        tempPoly.scale(w,h,0);
//        for (int ii=0; ii<nPts; ii++) {
//            for (int dim=0; dim<3; dim++) {
//                vert[ii][dim] = tempPoly.vert[ii][dim];
//            }
//        }
//        np = nPts;
//        tempPoly.arc(nPts, start, sweep, 1.0);
//        tempPoly.scale(w-dw, h-dh, 0);
//        for (int ii=0; ii<nPts; ii++) {
//            for (int dim=0; dim<3; dim++) {
//                vert[nPts+ii][dim] = tempPoly.vert[nPts-ii-1][dim];
//            }
//        }
//        np += nPts;
//    }
//
//};

//
//
//void draw_ticks (float radius, int spacing) {
//    // Draw tick marks
//    const int nTicks = 360/spacing;
//    float tick_size = 0.05;
//    std::string labelString = "000";
//    float label_offset = 0.05;
//
//    for (int ii=0; ii<nTicks; ii++) {
//        glPushMatrix();
//        glRotatef (ii*spacing, 0.0, 0.0, 1.0);
//        glTranslatef (radius, 0.0, 0.0);
//        glBegin (GL_LINES);
//        glVertex2f (0.0,       0.0);
//        glVertex2f (tick_size, 0.0);
//        glEnd ();
//
//        glRasterPos3f (label_offset, 0.0, 0.0);
//        for (int ii=0; ii<labelString.length(); ii++) {
//            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, labelString[ii]);
//        }
//        glPopMatrix();
//    }
//}
//



//
//
//void extrude (int nRings, float *radius, float *offset, sbPrimitive *P, sbModel *M, int close_state) {
//    int index, v1i, v2i, v3i, v4i, start;
//    int nVertices = P->np * nRings;
//    int nTriangles = 2 * P->np * (nRings-1);
//
//    M->nv = nVertices;
//    M->nf = nTriangles;
//
//    for (int ii = 0; ii < nRings; ii++) {
//        for (int jj = 0; jj < P->np; jj++) {
//            index = ii * P->np + jj;
//            M->vert[index][0] = radius[ii] * P->vert[jj][0];
//            M->vert[index][1] = radius[ii] * P->vert[jj][1];
//            M->vert[index][2] =              P->vert[jj][2] + offset[ii];
//
//            if (ii < (nRings-1)) {
//                if (jj == 0) start = index;
//                v1i = index;
//                v4i = index + P->np;
//                if (jj < (P->np-1)) {
//                    v2i = index + 1;
//                    v3i = index + P->np + 1;
//                }
//                else {
//                    v2i = start;
//                    v3i = start + P->np;
//                }
//
//                M->face[index][2] = v1i;
//                M->face[index][1] = v2i;
//                M->face[index][0] = v3i;
//
//                M->face[index+nTriangles/2][2] = v1i;
//                M->face[index+nTriangles/2][1] = v3i;
//                M->face[index+nTriangles/2][0] = v4i;
//            }
//        }
//    }
//    if (close_state == 1) {
//        for (int ii=0; ii<(P->np-2); ii++) {
//            M->face[nTriangles+ii][0] = 0;
//            M->face[nTriangles+ii][1] = ii + 1;
//            M->face[nTriangles+ii][2] = ii + 2;
//            M->nf++;
//
//            M->face[nTriangles+ii+(P->np-2)][2] = M->nv - P->np;
//            M->face[nTriangles+ii+(P->np-2)][1] = M->nv - P->np + ii + 1;
//            M->face[nTriangles+ii+(P->np-2)][0] = M->nv - P->np + ii + 2;
//            M->nf++;
//        }
//
//    }
//
//}
//
//
//void extrude_planar (int nArcs, float *radius, float *offset, sbPrimitive *P, sbModel *M) {
//    int index, v1i, v2i, v3i, v4i, start;
//    int nVertices = P->np * nArcs;
//    int nTriangles = 2 * (P->np-1) * (nArcs-1);
//
//    M->nv = nVertices;
//    M->nf = nTriangles;
//
//    for (int ii = 0; ii < nArcs; ii++) {
//        for (int jj = 0; jj < P->np; jj++) {
//            index = ii * P->np + jj;
//            M->vert[index][0] = radius[ii] * P->vert[jj][0];
//            M->vert[index][1] = radius[ii] * P->vert[jj][1];
//            M->vert[index][2] =              P->vert[jj][2] + offset[ii];
//
//            if ((ii<(nArcs-1))&&(jj<(P->np-1))) {
//                v1i = index;
//                v2i = index + P->np;
//                v3i = index + P->np + 1;
//                v4i = index + 1;
//
//                M->face[index][0] = v1i;
//                M->face[index][1] = v2i;
//                M->face[index][2] = v3i;
//
//                M->face[index+nTriangles/2][0] = v1i;
//                M->face[index+nTriangles/2][1] = v3i;
//                M->face[index+nTriangles/2][2] = v4i;
//            }
//        }
//    }
//}
//
//
//void stitch (sbPrimitive *outer, sbPrimitive *inner, sbModel *M) {
//
//    M->nf = outer->np + inner->np;
//    M->nv = outer->np + inner->np;
//
//    float dx, dy, distance, minimum;
//    int inner_index=0, outer_index=0, start_index=0, nT=0, leftovers=0, index_offset=0;
//    int linesPerVertex = (int) floor((float)inner->np/(float)outer->np);
//
//    for (int ii = 0; ii < inner->np; ii++) {
//        dx = outer->vert[0][0] - inner->vert[ii][0];
//        dy = outer->vert[0][1] - inner->vert[ii][1];
//        distance = sqrt(dx*dx + dy*dy);
//        if ((ii == 0)||(distance < minimum)) {
//            start_index = ii;
//            minimum = distance;
//        }
//    }
//
//    for (int ii = 0; ii < M->nv; ii++) {
//        for (int dim = 0; dim < 3; dim++) {
//            if (ii < outer->np)     M->vert[ii][dim] = outer->vert[ii            ][dim];
//            else                    M->vert[ii][dim] = inner->vert[ii - outer->np][dim];
//        }
//    }
//
//    inner_index = start_index - (int)floor((float)(linesPerVertex-1)/2.0);
//    //printf("%d %d %d\n", start_index, inner_index, linesPerVertex);
//
//    for (int ii = 0; ii < outer->np; ii++) {
//        for (int jj = 0; jj < linesPerVertex+1; jj++) {
//            if (inner_index >= inner->np)  {inner_index -= inner->np;}
//            if (inner_index < 0)           {inner_index += inner->np;}
//            //printf("%d %d %d\n", inner->np, inner_index, nT);
//            if (jj == linesPerVertex) {
//                M->face[nT][0] = ii;
//                M->face[nT][1] = ii+1; if ((ii+1)>=outer->np) {M->face[nT][1] -= outer->np;}
//                M->face[nT][2] = outer->np + inner_index;;
//            }
//            else {
//                M->face[nT][0] = ii;
//                M->face[nT][1] = outer->np + inner_index + 1; if ((inner_index+1)>=inner->np) {M->face[nT][1] -= inner->np;}
//                M->face[nT][2] = outer->np + inner_index;
//                inner_index++;
//            }
//            nT++;
//        }
//    }
//}
