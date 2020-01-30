//
// Created by keeney on 1/29/20.
//

#ifndef SPACEGAME_EMPLACEMENT_H
#define SPACEGAME_EMPLACEMENT_H

#endif //SPACEGAME_EMPLACEMENT_H


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


};
