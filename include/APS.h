#ifndef APS_H
#define APS_H

#include "MOMDP.h"
#include "memorypool.h
#include <boost/dynamic_bitset.hpp>

class APSState : public MOMDPState {
public:
    boost::dynamic_bitset<> private1, private2, publicArg;

    void fill_momdpstate() {
        invisiblestate = private2.to_ulong();
        boost::dynamic_bitset<> priv_pub(private1);
        priv_pub <<= publicArg.size();
        priv_pub |= publicArg;
        visiblestate   = priv_pub.to_ulong();
    }

    void fill_apsstate() {
        private2 = boost::dynamic_bitset<>(private2.size(), invisiblestate);
        boost::dynamic_bitset<> priv_pub(private1.size()+publicArg.size(), visiblestate);
        unsigned int size = private1.size();
        for( unsigned int i = 0; i < size; i++ )
            private1[i] = priv_pub[i];
        for( unsigned int i = 0; i < publicArg.size(); i++ )
            publicArg[i] = priv_pub[i + size];
    }
};

class APS : public MOMDP {
public:
    APS(unsigned int args, unsigned int atk, boost::dynamic_bitset<> private1,
        boost::dynamic_bitset<> private2, boost::dynamic_bitset<> publicArg);

    APSState *CreateStartState() const;
    void FreeState(STATE *state) const;
    bool Step(STATE &state, int action, int &observation, double &reward) const;
    APSState * Copy(const STATE &state) const;

private:
    mutable MEMORY_POOL<APSState> MemoryPool;
    unsigned int _numOfArguments, _numOfAttacks;
    boost::dynamic_bitset<> _private1, _private2, _publicArg;
};

#endif //APS_H
