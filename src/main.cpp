#include "mcts.h"
#include "experiment.h"
#include <boost/program_options.hpp>
#include <APS.h>

using namespace std;
using namespace boost::program_options;

void disableBufferedIO(void) {
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    setbuf(stderr, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int main(int argc, char *argv[]) {
    MCTS::PARAMS searchParams;
    EXPERIMENT::PARAMS expParams;
    SIMULATOR::KNOWLEDGE knowledge;
    string problem, outputfile, policy;
    int size, number, treeknowledge = 1, rolloutknowledge = 1, smarttreecount = 10;
    double smarttreevalue = 1.0;

    options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")
            ("test", "run unit tests")
            ("problem", value<string>(&problem), "problem to run")
            ("outputfile", value<string>(&outputfile)->default_value("output.txt"), "summary output file")
            ("policy", value<string>(&policy), "policy file (explicit POMDPs only)")
            ("size", value<int>(&size), "size of problem (problem specific)")
            ("number", value<int>(&number), "number of elements in problem (problem specific)")
            ("timeout", value<double>(&expParams.TimeOut), "timeout (seconds)")
            ("mindoubles", value<int>(&expParams.MinDoubles), "minimum power of two simulations")
            ("maxdoubles", value<int>(&expParams.MaxDoubles), "maximum power of two simulations")
            ("runs", value<int>(&expParams.NumRuns), "number of runs")
            ("accuracy", value<double>(
                    &expParams.Accuracy), "accuracy level used to determine horizon")
            ("horizon", value<int>(
                    &expParams.UndiscountedHorizon), "horizon to use when not discounting")
            ("num steps", value<int>(&expParams.NumSteps), "number of steps to run when using average reward")
            ("verbose", value<int>(&searchParams.Verbose), "verbosity level")
            ("autoexploration", value<bool>(&expParams.AutoExploration),
             "Automatically assign UCB exploration constant")
            ("exploration", value<double>(&searchParams.ExplorationConstant),
             "Manual value for UCB exploration constant")
            ("usetransforms", value<bool>(&searchParams.UseTransforms), "Use transforms")
            ("transformdoubles", value<int>(&expParams.TransformDoubles),
             "Relative power of two for transforms compared to simulations")
            ("transformattempts", value<int>(&expParams.TransformAttempts), "Number of attempts for each transform")
            ("userave", value<bool>(&searchParams.UseRave), "RAVE")
            ("ravediscount", value<double>(&searchParams.RaveDiscount), "RAVE discount factor")
            ("raveconstant", value<double>(&searchParams.RaveConstant), "RAVE bias constant")
            ("treeknowledge", value<int>(&knowledge.TreeLevel), "Knowledge level in tree (0=Pure, 1=Legal, 2=Smart)")
            ("rolloutknowledge", value<int>(&knowledge.RolloutLevel),
             "Knowledge level in rollouts (0=Pure, 1=Legal, 2=Smart)")
            ("smarttreecount", value<int>(&knowledge.SmartTreeCount),
             "Prior count for preferred actions during smart tree search")
            ("smarttreevalue", value<double>(&knowledge.SmartTreeValue),
             "Prior value for preferred actions during smart tree search")
            ("disabletree", value<bool>(&searchParams.DisableTree), "Use 1-ply rollout action selection");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    SIMULATOR *real = 0;
    SIMULATOR *simulator = 0;

    unsigned int numOfArgs = 8, numOfAtks = 8, numOfActions = 5;

    Rule *r1_1 = new Rule(numOfArgs, numOfAtks, {0}, {{0}}, {1});
    Rule *r1_2 = new Rule(numOfArgs, numOfAtks, {1, 5+8, 2}, {{1, 2+8}}, {1});
    Rule *r1_3 = new Rule(numOfArgs, numOfAtks, {1, 5+8, 2}, {{2, 3+8}}, {1});
    Rule *r1_4 = new Rule(numOfArgs, numOfAtks, {3, 6+8, 4}, {{4, 7+8}}, {1});
    Rule *r1_5 = new Rule(numOfArgs, numOfAtks, {3, 6+8, 4}, {{3, 6+8}}, {1});

    Rule *r2_1 = new Rule(numOfArgs, numOfAtks, {7, 1+8}, {{7, 6+8}}, {1});
    Rule *r2_2 = new Rule(numOfArgs, numOfAtks, {6, 2+8}, {{6, 5+8}}, {1});
    Rule *r2_3 = new Rule(numOfArgs, numOfAtks, {0+8, 5, 6}, {{5, 0+8}, {6, 1+8}}, {0.8, 0.2});

    boost::dynamic_bitset<> private1(numOfArgs), private2(numOfArgs), publicArgs(numOfArgs+numOfAtks);
    private1[0] = 1;
    private1[1] = 1;
    private1[2] = 1;
    private1[3] = 1;
    private1[4] = 1;
    private2[5] = 1;
    private2[6] = 1;
    private2[7] = 1;

    boost::dynamic_bitset<> goal(numOfArgs);
    goal[0] = 1;

    std::vector<std::pair<unsigned int, unsigned int>*> attacks;
    attacks.push_back(new std::pair<unsigned int, unsigned int>(5,0));
    attacks.push_back(new std::pair<unsigned int, unsigned int>(6,0));
    attacks.push_back(new std::pair<unsigned int, unsigned int>(1,5));
    attacks.push_back(new std::pair<unsigned int, unsigned int>(2,5));
    attacks.push_back(new std::pair<unsigned int, unsigned int>(7,1));
    attacks.push_back(new std::pair<unsigned int, unsigned int>(6,2));
    attacks.push_back(new std::pair<unsigned int, unsigned int>(3,6));
    attacks.push_back(new std::pair<unsigned int, unsigned int>(4,6));

    APS* aps = new APS(numOfArgs, numOfAtks, numOfActions, private1, private2, publicArgs,
                       {r1_1, r1_2, r1_3, r1_4, r1_5}, {r2_1, r2_2, r2_3}, goal, attacks);
    real = aps;
    simulator = new APS(*aps);

    simulator->SetKnowledge(knowledge);
    EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
    experiment.DiscountedReturn();

    delete real;
    delete simulator;
    return 0;
}
