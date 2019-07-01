#ifndef _BOUNCE2_H
#define _BOUNCE2_H

class Bounce {
public:
    Bounce() {};

    bool read() { return false; };
    void attach(int) {};
    void interval(int) {};
    void update() {};
    bool fell() { return false; }
    bool rose() {return false; }
};

#endif
