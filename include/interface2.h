

#define LEFT_MOUSE_BUTTON_BIT   1
#define RIGHT_MOUSE_BUTTON_BIT  4

void ReshapeFunc(int w, int h) {

    const float scale = 0.5*sqrt(3.0);
    const float aspect = (float) w / (float) h; // calc window aspect ratio

    glViewport(0.0, 0.0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-scale*aspect, scale*aspect, -scale, scale, -scale, scale);
    glMatrixMode(GL_MODELVIEW);

}


static std::string gui_str = "\
&Left drag: Rotate volume\
&Right drag: move light\
&c/C: Box clipping\
&a/A: Alpha scale\
&s/S : Slices\
&r: Reset\
&Esc: Exit"; // string for bitmap display
