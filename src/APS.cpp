#include "APS.h"

APS::APS(unsigned int args, unsigned int atk, boost::dynamic_bitset<> private1,
         boost::dynamic_bitset<> private2, boost::dynamic_bitset<> publicArg) :
    _numOfArguments(args), _numOfAttacks(atk), _private1(private1),
    _private2(private2), _publicArg(publicArg) {}

APSState *APS::CreateStartState() const {
    APSState* state  = MemoryPool.Allocate();

    state->private1  = _private1;
    state->private2  = _private2;
    state->publicArg = _publicArg;

    state->fill_momdpstate();
}

void APS::FreeState(STATE *state) const {
    APSState *s = safe_cast<APSState*>(state);
    MemoryPool.Free(s);
}


bool APS::Step(STATE &state, int action, int &observation, double &reward) const {
    return false;
}

APSState *APS::Copy(const STATE &state) const {
    APSState *s = MemoryPool.Allocate();
    const APSState &previous = safe_cast<const APSState&>(state);

    s->private1 = previous.private1;
    s->private2 = previous.private2;
    s->publicArg = previous.publicArg;
    s->invisiblestate = previous.invisiblestate;
    s->visiblestate = previous.visiblestate;
}
