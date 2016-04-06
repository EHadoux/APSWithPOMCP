#ifndef APS_H
#define APS_H

#include "simulator.h"
#include "memorypool.h"
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <initializer_list>
#include <utility>

enum PRED_STATE {
    POS, NEG, UNDEF
};

enum PRED_MODIF {
    ADD, REMOVE, NONE
};

struct Rule {
    //std::vector<PRED_STATE> premises;
    //std::vector<std::vector<PRED_MODIF>*> acts;
    std::vector<unsigned int> _premises;
    std::vector<std::vector<unsigned int> *> _acts;
    std::vector<double> _probas;

    Rule(unsigned int nbArgs, unsigned int nbAtks, std::initializer_list<int> prem,
         std::initializer_list<std::initializer_list<unsigned int> > acts, std::initializer_list<double> probas) :
            _premises(nbArgs * 2, 0), _probas(probas) {
        for(auto i : prem)
            _premises[i] = 1;

        for(auto a : acts) {
            auto vect = new std::vector<unsigned int>(nbArgs + nbAtks);
            for(auto opt : a)
                vect->at(opt) = 1;
            _acts.push_back(vect);
        }
    }

    Rule(const Rule &orig) : _premises(orig._premises), _probas(orig._probas) {
        for(auto a : orig._acts)
            _acts.push_back(new std::vector<unsigned int>(*a));
    }

    ~Rule() {
        for(auto a : _acts) delete a;
    }

    std::string to_s() const {
        std::stringstream s;
        s << "[";
        for(auto p : _premises)
            s << p << ", ";
        s << "] => ";
        for(auto a : _acts) {
            s << "[";
            for(auto p : *a)
                s << p << ", ";
            s << "] & ";
        }
        return s.str();
    }
};

class APSState : public STATE {
public:
    boost::dynamic_bitset<> private1, private2, publicArg;
    int invisiblestate, visiblestate;

    void fill_momdpstate() {
        invisiblestate = (int)private2.to_ulong();
        boost::dynamic_bitset<> priv_pub(private1);
        priv_pub.append(publicArg.to_ulong());
        visiblestate = (int)priv_pub.to_ulong();
    }

    /*void fill_apsstate() {
        private2 = boost::dynamic_bitset<>(private2.size(), invisiblestate);
        boost::dynamic_bitset<> priv_pub(private1.size() + publicArg.size(), visiblestate);
        unsigned int size = (unsigned int) private1.size();
        for(unsigned int i = 0; i < size; i++)
            private1[i] = priv_pub[i];
        for(unsigned int i = 0; i < publicArg.size(); i++)
            publicArg[i] = priv_pub[i + size];
    }*/
};

class APS : public SIMULATOR {
public:
    APS(unsigned int args, unsigned int atk, unsigned int actions, const boost::dynamic_bitset<> &private1,
        const boost::dynamic_bitset<> &private2, const boost::dynamic_bitset<> &publicArg, std::initializer_list<Rule *> rules1,
        std::initializer_list<Rule *> rules2, const boost::dynamic_bitset<> &goal,
        const std::vector<std::pair<unsigned int, unsigned int>*> &attacks);

    APS(const APS &other);

    virtual ~APS();

    APSState *CreateStartState() const;

    void FreeState(STATE *state) const;

    bool Step(STATE &state, int action, int &observation, double &reward) const;

    APSState *Copy(const STATE &state) const;

private:
    bool Accepted(boost::dynamic_bitset<> &current, unsigned int argument) const;

    mutable MEMORY_POOL<APSState> MemoryPool;
    unsigned int _numOfArguments, _numOfAttacks;
    boost::dynamic_bitset<> _private1, _private2, _publicArg, _goal;
    std::vector<Rule *> _rules1, _rules2;
    std::vector<std::pair<unsigned int, unsigned int>*> _attacks;
    bool _copy;
};

#endif //APS_H
