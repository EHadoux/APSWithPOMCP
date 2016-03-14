#ifndef MOMDP_H
#define MOMDP_H

#include "simulator.h"

class MOMDPState : public STATE {
public:
    unsigned long visiblestate, invisiblestate;
};

class MOMDP : public SIMULATOR {

public:
    MOMDP();

    MOMDP(int numActions, int numVisibleObservations, int numHiddenObservations, double discount);

    virtual ~MOMDP();
};

#endif //MOMDP_H
