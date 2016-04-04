#include "node.h"
#include "history.h"
#include "utils.h"

using namespace std;

//-----------------------------------------------------------------------------

int QNODE::NumChildren = 0;

void QNODE::Initialise() {
    assert(NumChildren);
    AlphaData.AlphaSum.clear();
}

void QNODE::DisplayValue(HISTORY &history, int maxDepth, ostream &ostr) const {
    history.Display(ostr);
    ostr << ": " << Value.GetValue() << " (" << Value.GetCount() << ")\n";
    if( history.Size() >= maxDepth )
        return;

    for( auto it = Children.begin(); it != Children.end(); ++it ) {
        if( it->second ) {
            history.Back().Observation = it->first;
            it->second->DisplayValue(history, maxDepth, ostr);
        }
    }
}

void QNODE::DisplayPolicy(HISTORY &history, int maxDepth, ostream &ostr) const {
    history.Display(ostr);
    ostr << ": " << Value.GetValue() << " (" << Value.GetCount() << ")\n";
    if( history.Size() >= maxDepth )
        return;

    for( auto it = Children.begin(); it != Children.end(); ++it ) {
        if( it->second ) {
            history.Back().Observation = it->first;
            it->second->DisplayPolicy(history, maxDepth, ostr);
        }
    }
}

//-----------------------------------------------------------------------------

MEMORY_POOL <VNODE> VNODE::VNodePool;

int VNODE::NumChildren = 0;

void VNODE::Initialise() {
    assert(NumChildren);
    Children.resize(VNODE::NumChildren);
    for( int action = 0; action < VNODE::NumChildren; action++ )
        Children[action].Initialise();
}

VNODE *VNODE::Create() {
    VNODE *vnode = VNodePool.Allocate();
    vnode->Initialise();
    return vnode;
}

void VNODE::Free(VNODE *vnode, const SIMULATOR &simulator) {
    vnode->BeliefState.Free(simulator);
    VNodePool.Free(vnode);
    for( int action = 0; action < VNODE::NumChildren; action++ )
        for( auto it = vnode->Child(action).begin(); it != vnode->Child(action).end(); ++it )
            if( it->second )
                Free(it->second, simulator);
}

void VNODE::FreeAll() {
    VNodePool.DeleteAll();
}

void VNODE::SetChildren(int count, double value) {
    for( int action = 0; action < NumChildren; action++ ) {
        QNODE &qnode = Children[action];
        qnode.Value.Set(count, value);
        qnode.AMAF.Set(count, value);
    }
}

void VNODE::DisplayValue(HISTORY &history, int maxDepth, ostream &ostr) const {
    if( history.Size() >= maxDepth )
        return;

    for( int action = 0; action < NumChildren; action++ ) {
        history.Add(action);
        Children[action].DisplayValue(history, maxDepth, ostr);
        history.Pop();
    }
}

void VNODE::DisplayPolicy(HISTORY &history, int maxDepth, ostream &ostr) const {
    if( history.Size() >= maxDepth )
        return;

    double bestq = -Infinity;
    int besta = -1;
    for( int action = 0; action < NumChildren; action++ ) {
        if( Children[action].Value.GetValue() > bestq ) {
            besta = action;
            bestq = Children[action].Value.GetValue();
        }
    }

    if( besta != -1 ) {
        history.Add(besta);
        Children[besta].DisplayPolicy(history, maxDepth, ostr);
        history.Pop();
    }
}

//-----------------------------------------------------------------------------
