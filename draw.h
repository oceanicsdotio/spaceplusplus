// SECTION: DRAW FUNCTIONS
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void print_string(void* font, std::string label_string, float xpos, float ypos, float zpos) {
    // Prints string at specific position
    for (int ii=0; ii<label_string.length(); ii++) {
        glRasterPos3f(xpos,ypos,zpos);
        glutBitmapCharacter(font, label_string[ii]); // advances raster position on each call
    }
}

////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
void draw_label (std::string labelString) {
    // Draw label
    float label_offset = 0.05;
    glRasterPos3f (label_offset, 0.0, 0.0);
    for (int ii=0; ii<labelString.length(); ii++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, labelString[ii]);
    }
}

////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
void draw_avg_normals (sbModel *M) {
    // calculate and draw vertex normals on the fly
    float normScale=0.1, normal[3];


    for (int ii = 0; ii < M->nv; ii++) {
        for (int dim=0; dim<3; dim++) normal[dim] = M->vert[ii][dim] + normScale*M->vert[ii][dim];
        glBegin(GL_LINES);
                glVertex3fv(M->vert[ii]);
                glVertex3fv(normal);
        glEnd();
    }
}



void draw_multi_normals (sbModel *M) {
    // calculate and draw vertex normals on the fly
    float normal[3], U[3], V[3], normScale=0.25;


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
            for (int dim=0; dim<3; dim++) normal[dim] = M->vert[M->face[ii][jj]][dim] + normScale*normal[dim];

            glBegin(GL_LINES);
                glVertex3fv(M->vert[M->face[ii][jj]]);
                glVertex3fv(normal);
            glEnd();
        }
    }
}

////////////////////////////////////////////////////////////////////////////
void draw_triangles (sbModel *M, bool fill) {
    // takes pointer to model instance and draws either filled or outlined triangles
    if (fill) glBegin(GL_TRIANGLES);
    for (int ii = 0; ii < M->nf; ii++) {
        TOTAL_TRIANGLES++;
        if (!fill) glBegin(GL_LINE_LOOP);
        for (int jj = 0; jj < 3; jj++) {
            if (lightState) glNormal3fv(M->norm[M->face[ii][jj]]);
            glVertex3fv(M->vert[M->face[ii][jj]]);
        }
        if (!fill) glEnd();
    }
    if (fill) glEnd();
}

////////////////////////////////////////////////////////////////////////////
void draw_model (sbModel *M) {
    // Takes pointer to model instance and draws triangles from face and vertex data
    if (lightState)     {glEnable(GL_LIGHTING);
                         glMaterialfv(GL_FRONT, GL_AMBIENT,   M->glLightMat[0]);
                         glMaterialfv(GL_FRONT, GL_DIFFUSE,   M->glLightMat[1]);
                         glMaterialfv(GL_FRONT, GL_SPECULAR,  M->glLightMat[2]);
                         glMaterialf (GL_FRONT, GL_SHININESS, M->shine);
                                                                               draw_triangles(M, SOLID);}
    else                {
        if (SHOW_BACK)  {glCullFace(GL_FRONT); glColor4f(0.5, 0.5, 0.5, 1.0);  draw_triangles(M, SOLID);}
        if (SHOW_FRONT) {glCullFace(GL_BACK);  glColor4f(0.5, 0.5, 0.5, 1.0);  draw_triangles(M, SOLID);}}
                         glDisable(GL_LIGHTING);
    if (SHOW_EDGES)     {                      glColor4f(1.0, 1.0, 1.0, 0.5);  draw_triangles(M, WIRE);}
    if (SHOW_NORMALS)   {                      glColor4f(1.0, 0.0, 0.0, 0.1);  draw_avg_normals(M);}
}

// SECTION: DRAW FUNCTIONS
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void draw_beam (obj *agent, emplacement *weapon) {
    // Draw beam and decrement timer
    if (weapon->state) {
        glBegin(GL_LINES);
            glVertex3fv(ORIGIN); glVertex3fv(agent->heading);
        glEnd();
        if (weapon->timer == 0) {weapon->state = 0; weapon->timer = 50;}
        else weapon->timer--;
    }
}

////////////////////////////////////////////////////////////////////////////
void draw_shield (obj *agent) {
    // Draw shield
    if (agent->shield_state) {
       float radius = (float) (50-agent->shield_timer)/50;
       glColor4f(BLUE,0.25);
       glutSolidSphere(radius,24,24);
       if (agent->shield_timer > 0) agent->shield_timer--;
    }
}


////////////////////////////////////////////////////////////////////////////
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