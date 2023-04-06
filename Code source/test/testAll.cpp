#include <gtest/gtest.h>
#include "constants.h"
#include "Simulation.h"
#include "Random.h"

RandomNumbers *_RNG = new RandomNumbers(23948710923);

//tests for class Random
TEST(Random, distributions)
{
    double mean = 0;
    double input_mean(1.35), input_sd(2.8);
    std::vector<double> res;
    res.resize(10000);
    double delta = input_sd*sqrt(3.0);
    double lower = input_mean-delta, upper = input_mean+delta;
    _RNG->uniform_double(res, lower, upper);
    for (auto I : res) {
        EXPECT_GE(I, lower);
        EXPECT_LT(I, upper);
        mean += I*1e-4;
    }
    EXPECT_NEAR(input_mean, mean, 3e-2*input_sd);

    _RNG->exponential(input_mean);
    mean = 0;
    for (auto I : res) mean += I*1e-4;
    EXPECT_NEAR(input_mean, mean, 2e-2*input_mean);
}

TEST(Random, Shuffle) //check if the table is well mixed otherwise all neurons will be linked to the same ones
{
    std::vector<size_t> toShuffle(_NEURON_NUMBER_);
    std::iota(toShuffle.begin(), toShuffle.end(),0);
    std::vector<size_t> control = toShuffle;
    _RNG->shuffle(toShuffle);
    bool isSame(toShuffle==control);
    EXPECT_FALSE(isSame);
}

//tests for class Simulation
TEST(Simulation, NeuronsCount)
{
    Simulation simulation;
    Network* network = simulation.getNetwork();
    size_t nb_neurons = network->getNumberNeurons();
    EXPECT_EQ(nb_neurons, size_t(_NEURON_NUMBER_));
}

TEST(Simulation, TimeCheck) //check that the simulation lasted the time we wanted
{
    Simulation simulation;
    size_t runtime(0);
    runtime = simulation.run();
    EXPECT_EQ(runtime, size_t(_SIMULATION_TIME_));
}

TEST(Simulation, DataCheck) //check that all the attributs are initialized
{
    Simulation simulation;
    Network* network = simulation.getNetwork();
    EXPECT_EQ(simulation.getSimulationDuration(), size_t(_SIMULATION_TIME_));
    EXPECT_EQ(network->getMeanStrength(), _MEAN_INTENSITY_);
    EXPECT_EQ(network->getMeanConnectivity(), _MEAN_CONNECTIVITY_);
    EXPECT_EQ(network->getExcitatoryProportion(), _PROPORTION_EXCITATOR_);
}

TEST(Simulation, unwantedCharacters)
{
    Simulation simulation;
    std::string bad("IB:0.2,F/S?:0.3+0.4"), good("IB:0.1,LTS:0.2,FS:0.3,CH:0.2");
    bool containsUnwChar(false);
    for(size_t i(0); i<bad.size(); ++i) {
        if(simulation.unwantedCharacters(bad[i])) {
            containsUnwChar=true;
            break;
        }
    }
    EXPECT_TRUE(containsUnwChar);

    containsUnwChar=false;
    for(size_t i(0); i<good.size(); ++i) {
        if(simulation.unwantedCharacters(good[i])) {
            containsUnwChar=true;
            break;
        }
    }
    EXPECT_FALSE(containsUnwChar);
}

TEST(Simulation, TypeExist)
{
    Simulation simulation;
    bool answer = simulation.typeExists("RS");
    EXPECT_TRUE(answer);
    answer = simulation.typeExists("IB");
    EXPECT_TRUE(answer);
    answer = simulation.typeExists("CH");
    EXPECT_TRUE(answer);
    answer = simulation.typeExists("FS");
    EXPECT_TRUE(answer);
    answer = simulation.typeExists("LTS");
    EXPECT_TRUE(answer);
    answer = simulation.typeExists("TEST");
    EXPECT_FALSE(answer);
}

TEST (Simulation, loadConfiguration)
{
    Simulation simulation;
    std::string prop = "IB:0.2, CH:0.2, LTS:0.2, FS:0.2";
    simulation.setProportions(prop);
    simulation.loadConfiguration();
    std::map<std::string, size_t> neurons_proportions = simulation.getNeuronsProportions();

    //check if the map contains the right proportions
    size_t number(0.2*_NEURON_NUMBER_);
    EXPECT_EQ(number, neurons_proportions.find("IB")->second);
    EXPECT_EQ(number, neurons_proportions.find("CH")->second);
    EXPECT_EQ(number, neurons_proportions.find("LTS")->second);
    EXPECT_EQ(number, neurons_proportions.find("FS")->second);
    EXPECT_EQ(_NEURON_NUMBER_-4*number, neurons_proportions.find("RS")->second);

    //check if the right number of neurons were created according to the proportions
    size_t total_proportions(0.0);
    total_proportions = neurons_proportions["RS"] + neurons_proportions["IB"] + neurons_proportions["FS"] + neurons_proportions["CH"] + neurons_proportions["LTS"];
    EXPECT_EQ(total_proportions, size_t(_NEURON_NUMBER_));
}

//tests for class Network
TEST(Network, RandomIndices)
{
    size_t numberLinks(_NEURON_NUMBER_*0.6);
    size_t avoidIndice(_NEURON_NUMBER_*0.03);
    Network network(_NEURON_NUMBER_,_PROPORTION_EXCITATOR_,_MEAN_CONNECTIVITY_,_MEAN_INTENSITY_, _DELTA_, _NETWORK_MODEL_);
    std::vector<size_t> links=network.RandomIndices(avoidIndice,numberLinks);
    for(const auto& element : links) EXPECT_NE(element,avoidIndice);
    EXPECT_EQ(links.size(),numberLinks); // checks that each link has an intensity

    // checks that the links are unique (not linked twice to the same neuron)
    std::sort(links.begin(), links.end());
    bool unique (true);
    for(size_t i(0); i < links.size() - 1; i++) {
        if (links[i] == links[i + 1]) {
            unique=false;
        }
    }
    EXPECT_TRUE(unique);
}

TEST(Network, RandomStrength)
{
    size_t numberLinks(_NEURON_NUMBER_*0.6);
    Network network(_NEURON_NUMBER_,_PROPORTION_EXCITATOR_,_MEAN_CONNECTIVITY_,_MEAN_INTENSITY_, _DELTA_, _NETWORK_MODEL_);
    std::vector<double> strength=network.RandomStrength(numberLinks);
    for(const auto& element : strength) EXPECT_TRUE(element>=0 and element<=2*_MEAN_INTENSITY_);
    EXPECT_EQ(strength.size(),numberLinks);
}

TEST(Network, findNeuron)
{
    Network network(_NEURON_NUMBER_,_PROPORTION_EXCITATOR_,_MEAN_CONNECTIVITY_,_MEAN_INTENSITY_, _DELTA_, _NETWORK_MODEL_);
    Neurone neuron("FS");
    size_t find = network.findNeuron(&neuron);
    EXPECT_EQ(find,size_t(_NEURON_NUMBER_));
}

TEST (Network, InhibExciProportion)
{
    Network network(_NEURON_NUMBER_,_PROPORTION_EXCITATOR_,_MEAN_CONNECTIVITY_,_MEAN_INTENSITY_, _DELTA_, _NETWORK_MODEL_);

    size_t inhibitory(_NEURON_NUMBER_*(1.0-_PROPORTION_EXCITATOR_));
    size_t excitatory(_NEURON_NUMBER_-inhibitory);

    size_t counter(0);
    for (const auto& neuron : network.getNeurons()) {
        if (neuron->getExcitator()) {
            ++counter;
        }
    }
    EXPECT_EQ(excitatory, counter);

    counter=0;
    for (const auto& neuron : network.getNeurons()) {
        if (!neuron->getExcitator()) {
            ++counter;
        }
    }
    EXPECT_EQ(inhibitory, counter);
}

//tests for class Neurone
TEST(Neurone, update_case_excitation)
{
    Neurone neurone_FS("FS");
    Neurone neurone_newlink_RS("RS");
    Neurone neurone_newlink_FS("FS");
    neurone_newlink_RS.setFiringState(true);
    neurone_newlink_FS.setFiringState(true);
    neurone_FS.addLink(&neurone_newlink_RS, 30);
    neurone_FS.addLink(&neurone_newlink_FS, 1);
    bool test(false);
    for(int i(0); i<10; ++i) { //since the strength with the excitatory neuron is so high, the neuron will fire at least one time
        neurone_FS.computeI();
        neurone_FS.update();
        if(neurone_FS.isFiring()) {
            test = true;
            break;
        }
    }
    EXPECT_TRUE(test);
}

TEST(Neurone, update_case_inhibition)
{
    Neurone neurone_FS("FS");
    Neurone neurone_newlink_RS("RS");
    Neurone neurone_newlink_FS("FS");
    neurone_newlink_RS.setFiringState(true);
    neurone_newlink_FS.setFiringState(true);
    neurone_FS.addLink(&neurone_newlink_RS, 1);
    neurone_FS.addLink(&neurone_newlink_FS, 30);
    bool test(false);
    for(int i(0); i<10; ++i) { //since the strength with the inhibitory neuron is so high, the neuron will never fire
        neurone_FS.computeI();
        neurone_FS.update();
        if(neurone_FS.isFiring()) {
            test = true;
            break;
        }
    }
    EXPECT_FALSE(test);
}

TEST(Neurone, check_addLink)
{
    Neurone neurone_FS ("FS");
    Neurone neurone_newlink_RS("RS");
    Neurone neurone_newlink_FS("FS");
    Neurone neurone_notlinked_FS("FS");
    neurone_FS.addLink(&neurone_newlink_RS, 3);
    neurone_FS.addLink(&neurone_newlink_FS, 5);

    EXPECT_TRUE(neurone_FS.inNeighborhood(&neurone_newlink_RS));
    EXPECT_TRUE(neurone_FS.inNeighborhood(&neurone_newlink_FS));
    EXPECT_FALSE(neurone_FS.inNeighborhood(&neurone_notlinked_FS));

    EXPECT_EQ(size_t(2), neurone_FS.getSizeNeighborhood());
}

TEST(Neurone, check_neighborhood_sums)
{
    Neurone neurone_FS ("FS");
    Neurone neurone_newlink_RS("RS");
    Neurone neurone_newlink_FS("FS");
    Neurone neurone_newlink_RS2("RS");
    Neurone neurone_newlink_FS2("FS");
    Neurone neurone_newlink_RS3("RS");
    Neurone neurone_newlink_FS3("FS");

    neurone_newlink_RS.setFiringState(true);
    neurone_newlink_FS.setFiringState(true);
    neurone_newlink_RS2.setFiringState(false);
    neurone_newlink_FS2.setFiringState(false);
    neurone_newlink_RS3.setFiringState(true);
    neurone_newlink_FS3.setFiringState(true);

    neurone_FS.addLink(&neurone_newlink_RS, 3);
    neurone_FS.addLink(&neurone_newlink_FS, 5);
    neurone_FS.addLink(&neurone_newlink_RS2, 3);
    neurone_FS.addLink(&neurone_newlink_FS2, 5);
    neurone_FS.addLink(&neurone_newlink_RS3, 3);
    neurone_FS.addLink(&neurone_newlink_FS3, 5);

    EXPECT_NEAR(6, neurone_FS.getSumExcitator(), 1e-12);
    EXPECT_NEAR(10, neurone_FS.getSumInhibitor(), 1e-12);
}

//tests for spikes output file dimensions
TEST(OutputFile, DimensionCheck)
{
    Simulation simulation;
    simulation.run();
    std::string name(_OUTFILE_NAME_);
    name+="_spikes.txt";
    std::ifstream config_file(name);
    std::string item, line, key;
    std::vector<std::vector<int>> output_matrice;
    if (config_file.is_open()) {
        while (std::getline(config_file, line)) {
            std::stringstream line_outfile(line);
            std::getline(line_outfile,key, ' '); // key will contain the time the line corresponds to
            if(key.empty()) {
                continue;
            } else {
                std::vector<int> output_line;
                for (size_t n(0); (std::getline(line_outfile, item, ' '))&&(n<line.size()); n++) {
                    EXPECT_TRUE(item=="0" or item=="1");
                    output_line.push_back(stoi(item));
                }
                output_matrice.push_back(output_line);
            }
        }
        config_file.close();
    }
    EXPECT_EQ(output_matrice.size(),size_t(_SIMULATION_TIME_));
    for(size_t i(0); i<output_matrice.size(); ++i) {
        EXPECT_EQ(output_matrice[i].size(), size_t(_NEURON_NUMBER_));
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
