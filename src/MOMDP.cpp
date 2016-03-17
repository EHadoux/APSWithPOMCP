#include "MOMDP.h"

MOMDP::MOMDP(int numActions, int numVisibleObservations, int numHiddenObservations, double discount) :
        SIMULATOR(numActions, numVisibleObservations * numHiddenObservations, discount) {}


MOMDP::MOMDP(const MOMDP &other) : SIMULATOR(other) {}

MOMDP::~MOMDP() { }
