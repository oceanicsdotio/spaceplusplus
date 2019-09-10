////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
bool fpsCounter(void) {
    fpsCount++;
    float seconds;
    if (fpsCount>=1000) {
        fpsTimeNow = clock();
        fpsTimeElapsed = fpsTimeNow - fpsTimePrevious;
        seconds = 0.001 * ((float) fpsTimeElapsed) / CLOCKS_PER_SEC;
        fps = 1.0 / seconds;
        printf("Average framerate... %5.2f \n", fps);
        printf("  Total triangles... %d \n", TOTAL_TRIANGLES);
        fpsTimePrevious = fpsTimeNow;
        fpsCount = 0;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////
float find_angle (int id1, int id0, obj *agent[100]) {
        float dx = agent[id1]->position[0] - agent[id0]->position[0];
        float dy = agent[id1]->position[1] - agent[id0]->position[1];
        float angle = atan2(dy,dx)*180.0/3.14159;
        if (angle < 0.0) angle += 360.0;
        return angle;
}
////////////////////////////////////////////////////////////////////////////
void calc_normals (sbModel *M) {
    // calculate and draw vertex normals on the fly
    float normal[3], U[3], V[3];
    int norm_count[100000]; for (int ii = 0; ii < M->nv; ii++) norm_count[ii]=0;


    for (int ii = 0; ii < M->nf; ii++) {
        for (int jj = 0; jj < 3; jj++) {
            if (jj==0) {
                subtract(M->vert[M->face[ii][2]], M->vert[M->face[ii][0]], U);
                subtract(M->vert[M->face[ii][1]], M->vert[M->face[ii][0]], V);  }

            if (jj==1) {
                subtract(M->vert[M->face[ii][0]], M->vert[M->face[ii][1]], U);
                subtract(M->vert[M->face[ii][2]], M->vert[M->face[ii][1]], V);  }

            if (jj==2) {
                subtract(M->vert[M->face[ii][1]], M->vert[M->face[ii][2]], U);
                subtract(M->vert[M->face[ii][0]], M->vert[M->face[ii][2]], V);  }

            crossProduct(V,U,normal);
            normalize(normal);

            if (jj==0) copyVertex(normal,M->norf[ii]);
            add(normal,M->norm[M->face[ii][jj]],M->norm[M->face[ii][jj]]);
            norm_count[M->face[ii][jj]]++;
        }
    }

    for (int ii = 0; ii < M->nv; ii++) {
        for (int dim = 0; dim < 3; dim++) {M->norm[ii][dim] /= float(norm_count[ii]);}
    }
}