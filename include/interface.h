//void KeyboardFunc (unsigned char key, int x, int y) {
//    switch (key) {
//        case 27:
//            exit(0);
//            break;
//        case 's':
//            if (agent[selected]->shield_state == 0) agent[selected]->shield_state = 1;
//            else {agent[selected]->shield_state = 0; agent[selected]->shield_timer = 50;}
//            break;
//        case ' ':
//            fire_wpn(weapon[selected]);
//            break;
//        case 'O':
//            if (coverAngle<180.0) {openTubes(M[4],0,1.0); openTubes(M[7],1,1.0); openTubes(M[10],2,1.0); coverAngle+=1.0;}
//            break;
//        case 'o':
//            if (coverAngle>0.0)   {closeTubes(M[4],0,1.0); closeTubes(M[7],1,1.0); closeTubes(M[10],2,1.0); coverAngle-=1.0;}
//            break;
//
//        case 'q':
//            stabilize(agent[selected]);
//            break;
//
//        case 'N': SHOW_NORMALS = !SHOW_NORMALS; break;
//        case 'E': SHOW_EDGES   = !SHOW_EDGES;   break;
//        case 'L': lightState   = !lightState;   break;
//        case 'S':
//            SMOOTH_MODEL = !SMOOTH_MODEL;
//            if (SMOOTH_MODEL) glShadeModel(GL_SMOOTH);
//            else              glShadeModel(GL_FLAT);  break;
//
//        case '1': selected = 0; break;
//        case '2': selected = 1; break;
//        case '3': selected = 2; break;
//        case '4': selected = 3; break;
//    }
//}

void SpecialKeys (int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_LEFT: turn(1, agent[selected]); break;
        case GLUT_KEY_RIGHT: turn(-1, agent[selected]); break;
        case GLUT_KEY_UP: {impulse(agent[selected]);} break;
        case GLUT_KEY_DOWN: {brake(agent[selected]);} break;
    }
}

void MouseFunc (int button, int buttonstate, int x, int y) {
    model_state->mouse_prev_x = x; model_state->mouse_prev_y = y;
    if (button == GLUT_LEFT_BUTTON) {
        if (buttonstate == GLUT_DOWN) model_state->mbs |= LEFT_MOUSE_BUTTON_BIT;
        else model_state->mbs &= ~LEFT_MOUSE_BUTTON_BIT;}
    else if (button == GLUT_RIGHT_BUTTON){
        if (buttonstate == GLUT_DOWN) model_state->mbs |= RIGHT_MOUSE_BUTTON_BIT;
        else model_state->mbs &= ~RIGHT_MOUSE_BUTTON_BIT;
    }
    glutPostRedisplay();
}


void MotionFunc (int x, int y) { // when mouse moves
    int dx = x - model_state->mouse_prev_x; model_state->mouse_prev_x = x;
    int dy = y - model_state->mouse_prev_y; model_state->mouse_prev_y = y;
    if (model_state->mbs & LEFT_MOUSE_BUTTON_BIT) { // if left button held
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        model_state->rot_inc[1] = .5*dx;
        model_state->rot[1]+=model_state->rot_inc[1];
        glRotatef (model_state->rot_inc[1], 0, 1, 0);
        model_state->rot_inc[0] = .5*dy;
        model_state->rot[0]+=model_state->rot_inc[0];
        glRotatef (model_state->rot_inc[0], 1, 0, 0);
        glMultMatrixf(model_state->rot_matrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, model_state->rot_matrix); // store current to state
    }
}


void ReshapeFunc(int w, int h) {
    float scale = domain;
    float aspect = (float) w / (float) h; // calc window aspect ratio
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-scale*aspect, scale*aspect, -scale, scale, -scale, scale);
    glMatrixMode(GL_MODELVIEW);
}

void IdleFunc() {
    for (int ii = 0; ii < nObj; ii++) {
        for (int dim = 0; dim < 3; dim++) agent[ii]->position[dim] += agent[ii]->veloc[dim];
        if (agent[ii]->position[0] >= domain) agent[ii]->position[0] -= 2.0 * domain;
        if (agent[ii]->position[0] <= -domain) agent[ii]->position[0] += 2.0 * domain;
        if (agent[ii]->position[1] >= domain) agent[ii]->position[1] -= 2.0 * domain;
        if (agent[ii]->position[1] <= -domain) agent[ii]->position[1] += 2.0 * domain;
        if (agent[ii]->position[2] >= domain) agent[ii]->position[2] -= 2.0 * domain;
        if (agent[ii]->position[2] <= -domain) agent[ii]->position[2] += 2.0 * domain;
        rotateVector(agent[ii]->rot_v * radians, agent[ii]->rotaxis, agent[ii]->heading);
        rotateModel(agent[ii]->rot_v, agent[ii]->rotaxis, agent[ii]->objModel);
    }
    if ([]() {
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
    }()) glutPostRedisplay();
}

SimpleKeyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0); break;
        case 'n':
            SHOW_NORMALS = !SHOW_NORMALS; break;
        case 'l':
            lightState = !lightState; break;
        case 'e':
            SHOW_EDGES = !SHOW_EDGES; break;
    }
}

DisplayFunc() {

//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        glLightfv (GL_LIGHT0, GL_POSITION, light_position);
//        float rotvec[] = {1.0, -1.0, 0.0};
//        rotateVector(0.01, rotvec, light1_position);
//        glLightfv (GL_LIGHT1, GL_POSITION, light1_position);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    TOTAL_TRIANGLES = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glMultMatrixf(model_state->rot_matrix); // apply stored rotation

//findGroupHeading(agent, flock);
//findGroupCenter(agent, flock);

    for (int ii = 0; ii < nObj; ii++) {
        glPushMatrix();
        glColor4f(ii == selected ? RED : WHITE, 0.5);
        glTranslatef(agent[ii]->position[0], agent[ii]->position[1], agent[ii]->position[2]);
        draw_model(agent[ii]->objModel);

        glScalef(2.0, 2.0, 2.0); // scale circle

        // Draw regular polygon outline in (X, Y) plane
        float vec[] = {1.0, 0.0, 0.0};
        glBegin (GL_LINE_LOOP);
        for (int ii = 0; ii < 50; ii++) {
            glVertex3fv(vec);
            rotateVector(360.0/npts*radians, ZAXIS, vec);
        }
        glEnd ();

        glPopMatrix();
    }

/*
glPushMatrix ();
   glTranslatef(agent[0]->position[0],agent[0]->position[1],agent[0]->position[2]);
   for (int ii=0; ii<50; ii++) {
      draw_model (M[ii],SOLID);
   }
   draw_model(tm, SOLID);
glPopMatrix ();
*/

    glFlush();
    glutSwapBuffers();
}