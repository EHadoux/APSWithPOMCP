#include "MOMDP.h"

MOMDP::MOMDP() : SIMULATOR() {}

MOMDP::MOMDP(int numActions, int numVisibleObservations, int numHiddenObservations, double discount) :
        SIMULATOR(numActions, numVisibleObservations * numHiddenObservations, discount) {}


MOMDP::~MOMDP() { }
