#ifndef MOMDP_H
#define MOMDP_H

#include "simulator.h"

class MOMDPState : public STATE {
public:
    unsigned long visiblestate, invisiblestate;
};

class MOMDP : public SIMULATOR {

public:
    MOMDP(int numActions, int numVisibleObservations, int numHiddenObservations, double discount);

    MOMDP(const MOMDP& other);

    virtual ~MOMDP();
};

#endif //MOMDP_H
