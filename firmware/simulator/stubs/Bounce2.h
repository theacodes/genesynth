#ifndef _BOUNCE2_H
#define _BOUNCE2_H

class Bounce {
public:
    Bounce() {};

    bool read() { return state; };
    void attach(int pin) { this->pin = pin; };
    void interval(int) {};
    void update();
    bool fell() { return state == false && last == true; }
    bool rose() { return state == true && last == false; }

private:
    int pin = 0;
    bool state = false;
    bool last = false;
};

#endif
