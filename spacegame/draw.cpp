void print_string(void* font, std::string label_string, float xpos, float ypos, float zpos) {
    // Prints string at specific position
    for (int ii=0; ii<label_string.length(); ii++) {
        glRasterPos3f(xpos,ypos,zpos);
        glutBitmapCharacter(font, label_string[ii]); // advances raster position on each call
    }
}


void draw_polygon (int npts) {
    // Draw regular polygon outline in (X, Y) plane
    float vec[] = {1.0, 0.0, 0.0};
    glBegin (GL_LINE_LOOP);
    for (int ii = 0; ii < npts; ii++) {
        glVertex3fv(vec);
        rotateVector(360.0/npts*radians, ZAXIS, vec);
    }
    glEnd ();
}


void draw_label (std::string labelString) {
    // Draw label
    float label_offset = 0.05;
    glRasterPos3f (label_offset, 0.0, 0.0);
    for (int ii=0; ii<labelString.length(); ii++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, labelString[ii]);
    }
}


void draw_ticks (float radius, int spacing) {
    // Draw tick marks
    int nTicks = 360/spacing;
    float tick_size = 0.05;

    for (int ii=0; ii<nTicks; ii++) {
        glPushMatrix();
        glRotatef (ii*spacing, 0.0, 0.0, 1.0);
        glTranslatef (radius, 0.0, 0.0);
        glBegin (GL_LINES);
        glVertex2f (0.0,       0.0);
        glVertex2f (tick_size, 0.0);
        glEnd ();
        draw_label ("000");
        glPopMatrix();
    }
}

void draw_beam (obj *agent, emplacement *weapon) {
    // Draw beam and decrement timer
    if (weapon->state) {
        glBegin(GL_LINES);
        glVertex3fv(ORIGIN);
        glVertex3fv(agent->heading);
        glEnd();
        if (weapon->timer == 0) {
            weapon->state = 0;
            weapon->timer = 50;
        }
        else {
            weapon->timer--;
        }
    }
}


void draw_target_indicator (int id, obj *agent[1000]) {
    if (id != agent[id]->target) {
        glPushMatrix();
        glRotatef(find_angle(agent[id]->target, id, agent), 0.0, 0.0, 1.0);

        glBegin(GL_LINES);
        glVertex3f(0.23, 0.0, 0.0);
        glVertex3f(0.27, 0.0, 0.0);
        glEnd();
        glPopMatrix();
    }
}