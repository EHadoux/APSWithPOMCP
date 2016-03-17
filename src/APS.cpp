#include "APS.h"
#include "utils.h"

APS::APS(unsigned int args, unsigned int atk, unsigned int actions, const boost::dynamic_bitset<> &private1,
         const boost::dynamic_bitset<> &private2, const boost::dynamic_bitset<> &publicArg,
         std::initializer_list<Rule *> rules1, std::initializer_list<Rule *> rules2, const boost::dynamic_bitset<> &goal,
         const std::vector<std::pair<unsigned int, unsigned int>*> &attacks) :
        SIMULATOR(actions, 4*((unsigned long)pow(2,2*args+atk)), 0.9), _numOfArguments(args), _numOfAttacks(atk), _private1(private1),
        _private2(private2), _publicArg(publicArg), _rules1(rules1), _rules2(rules2), _goal(goal), _attacks(attacks) {
    _copy = false;
}

APS::APS(const APS &other) :
        SIMULATOR(other),
        _numOfArguments(other._numOfArguments), _numOfAttacks(_numOfAttacks), _private1(other._private1),
        _private2(other._private2), _publicArg(other._publicArg), _rules1(other._rules1), _rules2(other._rules2),
        _goal(other._goal), _attacks(other._attacks) {
    _copy = true;
}

APS::~APS() {
    if(!_copy) {
        for(auto r : _rules1) delete r;
        for(auto r : _rules2) delete r;
        for(auto a : _attacks) delete a;
    }
}

APSState *APS::CreateStartState() const {
    APSState *state = MemoryPool.Allocate();

    state->private1 = _private1;
    state->private2 = _private2;
    state->publicArg = _publicArg;

    state->fill_momdpstate();

    return state;
}

void APS::FreeState(STATE *state) const {
    APSState *s = safe_cast<APSState *>(state);
    MemoryPool.Free(s);
}

bool APS::Step(STATE &state, int action, unsigned long &observation, double &reward) const {
    APSState s    = safe_cast<APSState&>(state);
    Rule *to_fire   = _rules1[action];

    // Agent 1 check action/rule compatibility
    bool compatible = true;
    for(unsigned int i = 0; i < to_fire->_premises.size(); i++) {
        if( to_fire->_premises[i] == 1 ) {
            if(i < _numOfArguments) {
                if(s.private1[i] != 1) {
                    compatible = false;
                    break;
                }
            } else {
                if(s.publicArg[i - _numOfArguments] != 1) {
                    compatible = false;
                    break;
                }
            }
        }
    }
    if(compatible) {
        //Agent 1's rule application
        for(unsigned int i = 0; i < to_fire->_acts[0]->size(); i++ ) {
            if( to_fire->_acts[0]->at(i) == 1 )
                s.publicArg[i] = 1;
        }
    }

    // Agent 2 compatible rules computation
    std::vector<Rule*> opp_to_fire;
    for(auto a : _rules2) {
        bool opp_compatible = true;
        for(unsigned int i = 0; i < a->_premises.size(); i++) {
            if( a->_premises[i] == 1 ) {
                if(i < _numOfArguments) {
                    if(s.private2[i] != 1) {
                        opp_compatible = false;
                        break;
                    }
                } else {
                    if(s.publicArg[i - _numOfArguments] != 1) {
                        opp_compatible;
                        break;
                    }
                }
            }
        }
        if(opp_compatible) opp_to_fire.push_back(a);
    }
    //Agent 2 compatible rule selection
    if( !opp_to_fire.empty() ) {
        int index = UTILS::Random((unsigned int)opp_to_fire.size());
        to_fire = opp_to_fire[index];
        unsigned int act = UTILS::discrete_distribution(_rules2[index]->_probas);

        //Agent 2 rule application
        for(unsigned int i = 0; i < to_fire->_acts[act]->size(); i++ ) {
            if( to_fire->_acts[act]->at(i) == 1 )
                s.publicArg[i] = 1;
        }
    }

    //Goal fulfilment
    bool fulfiled = true;
    for(unsigned int i = 0; i < _goal.size(); i++) {
        boost::dynamic_bitset<> current(s.publicArg);
        if( !Accepted(current, i) ) {
            fulfiled = false;
            break;
        }
    }
    reward = -1;
    if( fulfiled )
        reward = 10;

    s.fill_momdpstate();
    observation = s.visiblestate;

    return fulfiled;
}

bool APS::Accepted(boost::dynamic_bitset<> &current, unsigned int argument) const {
    bool attacked = false;
    for(unsigned int a = 0; a < _attacks.size(); a++) {
        auto atk = _attacks[a];
        if( current[a+_numOfArguments] == 1 && atk->second == argument && current[atk->first] == 1 )
            if( Accepted(current, atk->first) ) {
                current[argument] = 0;
                attacked = true;
                break;
            }
    }
    return !attacked;
}

APSState *APS::Copy(const STATE &state) const {
    APSState *s = MemoryPool.Allocate();
    const APSState &previous = safe_cast<const APSState&>(state);
    s->private1 = previous.private1;
    s->private2 = previous.private2;
    s->publicArg = previous.publicArg;
    s->invisiblestate = previous.invisiblestate;
    s->visiblestate = previous.visiblestate;

    return s;
}
