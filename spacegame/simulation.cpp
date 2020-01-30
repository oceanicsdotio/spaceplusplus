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


float find_angle (int id1, int id0, obj *agent[100]) {
    float dx = agent[id1]->position[0] - agent[id0]->position[0];
    float dy = agent[id1]->position[1] - agent[id0]->position[1];
    float angle = atan2(dy,dx)*180.0/3.14159;
    if (angle < 0.0) angle += 360.0;
    return angle;
}


void openTubes(sbModel *M, int id, float dTheta) {
    rotateModel(90, ZAXIS, M);
    rotateModel(-90, XAXIS, M);
    shift(-1.2, 0.0, 2.0, M);
    rotateModel(float(-id)*120.0-180.0, ZAXIS, M);
    shift(0.0, -0.25, 0.0, M);
    rotateModel(dTheta, XAXIS, M);
    shift(0.0, 0.25, 0.0, M);
    rotateModel(float(id)*120.0+180.0, ZAXIS, M);
    shift(1.2, 0.0, -2.0, M);
    rotateModel(90, XAXIS, M);
    rotateModel(-90, ZAXIS, M);
}


void closeTubes(sbModel *M, int id, float dTheta) {
    rotateModel(90, ZAXIS, M);
    rotateModel(-90, XAXIS, M);
    shift(-1.2, 0.0, 2.0, M);
    rotateModel(float(-id)*120.0-180.0, ZAXIS, M);
    shift(0.0, -0.25, 0.0, M);
    rotateModel(-dTheta, XAXIS, M);
    shift(0.0, 0.25, 0.0, M);
    rotateModel(float(id)*120.0+180.0, ZAXIS, M);
    shift(1.2, 0.0, -2.0, M);
    rotateModel(90, XAXIS, M);
    rotateModel(-90, ZAXIS, M);
}


void weigh (float W, float *U) {
    U[0] *= W;
    U[1] *= W;
    U[2] *= W;
}


float sign (float number) {
    return number >= 0.0 ? 1.0 : -1.0;
}


float* align (obj *agent, group *formation, float *alignment) {
    for (int dim = 0; dim < 3; dim++) {
        alignment[dim] = formation->heading[dim];
    }
    normalize(alignment);
    return alignment;
}


float* condense (obj *agent, group *formation, float *attractor) {
    subtract(formation->center, agent->position, attractor);
    normalize(attractor);
    return attractor;
}


float* repel (obj *agent, group *formation, float *repulsor) {
    repulsor[0] = repulsor[1] = repulsor[2] = 0.0;
    float offset[3] = {agent->position[0]-formation->center[0], agent->position[1]-formation->center[1], agent->position[2]-formation->center[2]};
    if (vecMag(offset) < 0.5) {
        subtract(agent->position, formation->center, repulsor);
        normalize(repulsor);
    }
    return repulsor;
}


void steerOne (obj *agent, group *formation) {
    float temp[3], final[3], alignment[3], attractor[3], repulsor[3], normv[3], normal[3], offset[3], angleOffset;
    weigh(1.0, align(agent, formation, alignment));
    weigh(1.0, condense(agent, formation, attractor));
    weigh(1.0, repel(agent, formation, repulsor));
    add(alignment, attractor, temp);
    add(temp, repulsor, final);

    normv[0] = agent->veloc[0];
    normv[1] = agent->veloc[1];
    normv[2] = agent->veloc[2];
    normalize(normv);
    normalize(agent->heading);

    glBegin(GL_LINES);
        glColor3f(BLUE);    glVertex3fv(ORIGIN);    glVertex3fv(alignment);
        glColor3f(GREEN);   glVertex3fv(ORIGIN);    glVertex3fv(attractor);
        glColor3f(RED);     glVertex3fv(ORIGIN);    glVertex3fv(repulsor);
        glColor3f(YELLOW);  glVertex3fv(ORIGIN);    glVertex3fv(agent->heading);
        glColor3f(MAGENTA); glVertex3fv(ORIGIN);    glVertex3fv(normv);
        glColor3f(CYAN);    glVertex3fv(ORIGIN);    glVertex3fv(final);
    glEnd();

    brake(agent);

    angleOffset  = vecAngle(agent->heading, final); // angleOffset reaches NaN
    crossProduct(agent->heading, final, normal);
    turn(-1.0*agent->rot_v + sign(normal[2])*0.5*angleOffset, agent); // critically damped oscillator

    rotateVector(agent->rot_v*radians, agent->rotaxis, agent->heading);
    rotateModel(agent->rot_v, agent->rotaxis, agent->objModel);

    impulse(agent);
}


void steerGroup (obj *agent[1000], group *formation) {
    formation->heading[0] = formation->heading[1] = formation->heading[2] = 0.0;
    for (int ii = 0; ii < formation->size; ii++) {
        for (int dim = 0; dim < 3; dim++) {
            formation->heading[dim] += agent[formation->list[ii]]->heading[dim];
        }
    }

    formation->center[0] = formation->center[1] = formation->center[2] = 0.0;
    for (int ii = 0; ii < formation->size; ii++) {
        for (int dim = 0; dim < 3; dim++) {
            formation->center[dim] += agent[formation->list[ii]]->position[dim] / formation->size;
        }
    }

    for (int ii = 0; ii < formation->size; ii++) {
        steerOne (agent[formation->list[ii]], formation);
    }
}