#include "constants.h"
#include "Network.h"
#include "Random.h"

Network::Network(size_t neuronNumber, double excitatoryProportion_, double meanConnectivity_, double meanStrength_, double delta_, char networkModel_) : meanStrength(meanStrength_), meanConnectivity(meanConnectivity_), excitatoryProportion(excitatoryProportion_), networkModel(networkModel_)
{
    size_t inhibitory(neuronNumber*(1.0-excitatoryProportion));
    size_t excitatory(neuronNumber-inhibitory);
    createNeurons(inhibitory,"FS", delta_);
    if(inhibitory!=0)neuronsProportions["FS"] = inhibitory; //For the map to never be empty, because we need it for the prints. We add the type to the map only if it is not 0 otherwise we will get an empty graph
    createNeurons(excitatory,"RS",delta_);
    if(excitatory!=0)neuronsProportions["RS"] = excitatory;
    for(auto& neuron : neurons) createRandomLinks(neuron);
}

Network::Network(std::map< std::string, size_t > neuronsProportions_, double meanConnectivity_, double meanStrength_, double delta_, char networkModel_) :  meanStrength(meanStrength_), meanConnectivity(meanConnectivity_), neuronsProportions(neuronsProportions_), networkModel(networkModel_)
{
    std::map< std::string, size_t >::iterator p;
    for(p = neuronsProportions.begin(); p != neuronsProportions.end(); p++) {
        createNeurons(p->second, p->first, delta_);
    }

    for(auto& neuron : neurons) createRandomLinks(neuron);
}

void Network::createNeurons(size_t neuronNumber, std::string type, double delta)
{
    // neurons are created differently if the user use the basic model or the more rational one.
    if(delta==_DELTA_) { //we don't use the rational model
        for (size_t i(0); i<neuronNumber; ++i) {
            neurons.push_back(new Neurone(type));
        }
    } else {
        for (size_t i(0); i<neuronNumber; ++i) {
            neurons.push_back(new Neurone(type,delta));
        }
    }
}

Network::~Network()
{
    for(auto& neuron : neurons) {
        delete neuron;
        neuron=nullptr;
    }
}

std::vector<size_t> Network::RandomIndices(size_t avoidIndice, size_t numberLinks)
{
    std::vector<size_t> indicesLinks;
    std::vector<size_t> choseIndices(neurons.size());
    std::iota(choseIndices.begin(), choseIndices.end(),0); // see https://en.cppreference.com/w/cpp/algorithm/iota (the vector will be filled with integrers from 0 until neurons.size())
    _RNG->shuffle(choseIndices); // mix the vector to randomly chose neurons to link with a given neuron

    choseIndices.erase(std::remove(choseIndices.begin(), choseIndices.end(), avoidIndice), choseIndices.end()); // a neuron can not be linked with itself so we remove its index from the possible links

    for(size_t i(0); i<numberLinks; ++i) indicesLinks.push_back(choseIndices[i]);

    return indicesLinks; // indices of the neurons to link with the one with the index avoidIndice
}

std::vector<double> Network::RandomStrength (int numberLinks) const
{
    std::vector<double> strengthLinks(numberLinks);
    double min(0.0);
    double max(2.0*meanStrength);
    _RNG->uniform_double(strengthLinks,min, max);
    return strengthLinks;
}

void Network::createRandomLinks(Neurone* neuron)
{
    size_t neuronIndice(findNeuron(neuron));
    if(neuronIndice==neurons.size()) throw std::invalid_argument("The neuron for which you want to create links does not exist in the network.");

    int nbLinks;

    switch (networkModel) {
    case 'B' :
        nbLinks = _RNG->poisson(meanConnectivity);
        break ;
    case 'C' :
        nbLinks = meanConnectivity;
        break ;
    case 'O' :
        nbLinks = _RNG->poisson(_RNG->exponential(1.0/meanConnectivity));
        break ;
    }

    if(size_t(nbLinks)>=neurons.size()) nbLinks = neurons.size()-1; //if the link number exceed the number of neurons, we impose it to be the maximal possible value (the neuron will be link to each neuron in the network)

    if (nbLinks<0) throw std::invalid_argument("The number of connections received by a neuron must be positive");

    std::vector<size_t> indicesLinks=RandomIndices(neuronIndice,nbLinks);
    std::vector<double> strengthLinks=RandomStrength(indicesLinks.size());

    if(indicesLinks.size()!=strengthLinks.size()) throw std::invalid_argument("The number of links and number of strength are not the same so it is not possible to match a link with a strength. It is not possible to creat the links.");

    for(size_t i(0); i<indicesLinks.size(); ++i) {
        neuron->addLink(neurons[indicesLinks[i]],strengthLinks[i]);
    }
}

size_t Network::findNeuron(Neurone* neuron) const
{
    for(size_t i(0); i<neurons.size(); ++i) {
        if(neuron and neurons[i]==neuron ) return i;
    }

    return neurons.size(); //no neuron has the index neurons.size() (possibles indices are from 0 to neurons.size()-1)
}

void Network::update()
{
    for(auto& neuron : neurons) {
        neuron->computeI();
    }

    for(auto& neuron : neurons) {
        neuron->update();
    }
}

void Network::headerSample(std::ofstream& outfile) const
{
    for(const auto& proportions : neuronsProportions) {
        outfile << "\t" << proportions.first << ".v\t" << proportions.first << ".u\t" << proportions.first << ".I";
    }
    outfile << std::endl;
}

void Network::headerParameters(std::ofstream& outfile) const
{
    outfile << "Type\ta\tb\tc\td\tInhibitory\tdegree\tvalence" << std::endl;
}

void Network::printSpikes(std::ostream& outfile, size_t time) const
{
    outfile << time;
    for (const auto& neuron : neurons)
        outfile << " " << neuron->isFiring();
    outfile << std::endl;
}

void Network::printParameters (std::ofstream& outfile) const
{
    for(const auto& neuron : neurons) {
        neuron->printParams(outfile);
    }
}

void Network::printSample(std::ofstream& outfile, size_t time) const
{
    outfile << time;
    for (const auto& proportions : neuronsProportions) {
        for (const auto& neuron : neurons) {
            if (neuron->isType(proportions.first)) { //we print one neuron per type
                neuron->printSample(outfile); //this will always be the same neuron to be printed as soon as the neurons order in the neuron vector doesn't change
                break; //We exit the for as soon as we print a neuron because we only want to print one per type
            }
        }
    }
    outfile << std::endl;
}

size_t Network::getNumberNeurons() const
{
    return neurons.size();
}

double Network::getMeanStrength()const
{
    return meanStrength;
}

double Network::getMeanConnectivity() const
{
    return meanConnectivity;
}

double Network::getExcitatoryProportion() const
{
    return excitatoryProportion;
}

Neurons Network::getNeurons() const
{
    return neurons;
}
