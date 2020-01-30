//
// Created by keeney on 1/29/20.
//

#ifndef SPACEGAME_AGENTS_H
#define SPACEGAME_AGENTS_H

#endif //SPACEGAME_AGENTS_H


class Agent
{
public:
    int id, target, type;
    int shield_state, shield_timer;
    float prevHeading[3], heading[3], rot_v, position[3], veloc[3], rotaxis[3];
    sbModel *model;

    Agent () {
        id = nObj++;
        target = id;
        type = 0;
        rot_v = 0.0;

        shield_state = 0;
        shield_timer = 50;
        prevHeading = {1.0, 0.0, 0.0};
        heading = {1.0, 0.0, 0.0};
        position = {0.0, 0.0, 0.0};
        veloc = {0.0, 0.0, 0.0};

        rotaxis = {
            float(rand()%100)/100,
            float(rand()%100)/100,
            float(rand()%100)/100
        };

        model = new sbModel;
    }

    void draw_shield () {
        // Draw shield
        if (shield_state) {
            float radius = (float) (50-agent->shield_timer)/50;
            glColor4f(BLUE, 0.25);
            glutSolidSphere(radius, 24, 24);
            if (shield_timer > 0) shield_timer--;
        }
    }

    void stabilize() {
        rot_v += -1.0*rot_v;
    }

    void impulse() {
        for (int dim=0; dim<3; dim++) veloc[dim] += 0.005*heading[dim];
    }

    void brake() {
        for (int dim=0; dim<3; dim++) veloc[dim] -= 0.005*heading[dim];
    }

    void turn(float direction) {
        rot_v += direction;
    }

};


class Group {
public:
    int size;
    int list[1000];
    float center[3];
    float heading[3];

    group() {
        size = 0;
        for (int ii=0; ii<1000; ii++) list[ii]=0;
        center[0] = center[1] = center[2] = 0.0;
        heading[0] = heading[1] = heading[2] = 0.0;
    }
};
