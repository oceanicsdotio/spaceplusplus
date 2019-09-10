// SECTION: GLUT USER CONTROL FUNCTIONS
////////////////////////////////////////////////////////////////////////////
void KeyboardFunc (unsigned char key, int x, int y) {
    switch (key) {
        case 27: exit(0); break;
        case 's':
            if (agent[selected]->shield_state == 0) agent[selected]->shield_state = 1;
            else {agent[selected]->shield_state = 0; agent[selected]->shield_timer = 50;}
            break;
        case ' ':
            fire_wpn(weapon[selected]);
            break;
        case 'O':
            if (coverAngle<180.0) {openTubes(M[4],0,1.0); openTubes(M[7],1,1.0); openTubes(M[10],2,1.0); coverAngle+=1.0;}
            break;
        case 'o':
            if (coverAngle>0.0)   {closeTubes(M[4],0,1.0); closeTubes(M[7],1,1.0); closeTubes(M[10],2,1.0); coverAngle-=1.0;}
            break;

        case 'q':
            stabilize(agent[selected]);
            break;

        case 'N': SHOW_NORMALS = !SHOW_NORMALS; break;
        case 'E': SHOW_EDGES   = !SHOW_EDGES;   break;
        case 'L': lightState   = !lightState;   break;
        case 'S':
            SMOOTH_MODEL = !SMOOTH_MODEL;
            if (SMOOTH_MODEL) glShadeModel(GL_SMOOTH);
            else              glShadeModel(GL_FLAT);  break;

        case '1': selected = 0; break;
        case '2': selected = 1; break;
        case '3': selected = 2; break;
        case '4': selected = 3; break;
    }
}

////////////////////////////////////////////////////////////////////////////
void Special_Keys (int key, int x, int y) {

    switch (key) {
       case GLUT_KEY_LEFT: turn(1, agent[selected]); break;
       case GLUT_KEY_RIGHT: turn(-1, agent[selected]); break;
       case GLUT_KEY_UP: {impulse(agent[selected]);} break;
       case GLUT_KEY_DOWN: {brake(agent[selected]);} break;
    }
}

////////////////////////////////////////////////////////////////////////////
void MouseFunc (int button, int buttonstate, int x, int y) {
    model_state->mouse_prev_x = x; model_state->mouse_prev_y = y;
    if (button == GLUT_LEFT_BUTTON) {
        if (buttonstate == GLUT_DOWN) model_state->mbs |= LEFT_MOUSE_BUTTON_BIT;
        else model_state->mbs &= ~LEFT_MOUSE_BUTTON_BIT;}
    else if (button == GLUT_MIDDLE_BUTTON) {
        if (buttonstate == GLUT_DOWN) model_state->mbs |= MIDDLE_MOUSE_BUTTON_BIT;
        else  model_state->mbs &= ~MIDDLE_MOUSE_BUTTON_BIT; }
    else if (button == GLUT_RIGHT_BUTTON){
        if (buttonstate == GLUT_DOWN) model_state->mbs |= RIGHT_MOUSE_BUTTON_BIT;
        else model_state->mbs &= ~RIGHT_MOUSE_BUTTON_BIT;
    }
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////
void MotionFunc (int x, int y) { // when mouse moves
    int dx = x - model_state->mouse_prev_x; model_state->mouse_prev_x = x;
    int dy = y - model_state->mouse_prev_y; model_state->mouse_prev_y = y;
    if (model_state->mbs & LEFT_MOUSE_BUTTON_BIT) { // if left button held
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        model_state->rot_inc[1] = .5*dx; model_state->rot[1]+=model_state->rot_inc[1]; glRotatef (model_state->rot_inc[1], 0, 1, 0);
        model_state->rot_inc[0] = .5*dy; model_state->rot[0]+=model_state->rot_inc[0]; glRotatef (model_state->rot_inc[0], 1, 0, 0);
        glMultMatrixf(model_state->rot_matrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, model_state->rot_matrix); // store current to state
    }
}