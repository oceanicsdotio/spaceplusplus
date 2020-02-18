class emplacement
{
public:

    int state, timer, cooldown;
    emplacement () {
        state = 0;
        timer = 50;
        cooldown = 50;
    }

    void fire_wpn() {
        if (state == 0) state = 1;
        cooldown = 50;
    }

    void draw_beam (obj *agent) {
        // Draw beam and decrement timer
        if (state) {
            glBegin(GL_LINES);
            glVertex3fv(ORIGIN);
            glVertex3fv(agent->heading);
            glEnd();
            if (timer == 0) {
                state = 0;
                timer = 50;
            }
            else {
                timer--;
            }
        }
    }
};
