#include "Neurone.h"

/*! @class Network

 A Network represents the environment in which each  \ref Neurone evolve.
 A Network is a set of \ref Neuron which models how they interact and how the connections between them influence their behaviour. Each neuron is connected to other neurons by a link with a certain intensity. The network makes it possible to generate these links randomly when constructing its neurons. The number of links depends on the mode chosen by the user : constant (each neurons has the same number of connections), random near a mean or overdispersed. The links a neuron has with others are found in the class \ref Neurone. Each neuron knows all the neurons it is connected with and these links are unidirectional and unique (if neuron 1 is linked to neuron 2, neuron 2 is not necessary linked to neuron 1 and neuron 1 can not have more than 1 links with neuron 2).
 Each neuron in the network is associated with an index in the network's neuron set and can thus be found in the network thanks to it.
 The dynamic of the network is such that it updates each of its neurons at each time step.
 The neuron's state (firing or not) and parameters can be printed on three output files to be studied.
 */

/// \typedef Neurons represents the set of neurons of the Network.
typedef std::vector<Neurone*> Neurons;

class Network
{

public:

    /*! @name Construct the Network
        Create all the \ref Network's neurons and add them to its neurons set. Then, create random links for each neurons. \ref Neurone are constructed depending on the user input : the proportions of excitatory neurons or the proprtions of precise neuron types. By default, the constructor uses the excitatory proportion.
        \param neuronNumber (size_t) : number of neurons that make up the network.
        \param excitatoryProportion_ (double) : proportion of excitatory neurons making up the network. The number of excitatory neurons is excitatoryProportion x neuronNumber (all other neurons are inhibitory).
        \param meanConnectivity_  (double) : average number of connections received by a neuron.
        \param meanStrength_ (double) : average strength of a link between two neurons.
        \param neuronsProportions_ (map<string,size_t>) : number of each neuron type (5 possible types : RS, FS, CH, IB, LTS)
        \param delta_ (double) : parameter to compute the noise in one additional fonctionality of the program.
        \param networkModel_ (char) : specify the distribution of the number of links (constant, random near a mean or overdispersed).
     */
///@{
    Network(size_t neuronNumber, double excitatoryProportion_, double meanConnectivity_, double meanStrength_, double delta_, char networkModel_);
    Network(std::map< std::string, size_t > neuronsProportions_, double meanConnectivity_, double meanStrength_, double delta_, char networkModel_);
    void createNeurons(size_t neuronNumber, std::string type, double delta);
    ~Network();
///@}

    /*! @name Creating link between neurons
        Randomly create the links that a given neuron has with the other neurons in the network. These links are then added to the neuron via  \ref Neurone:: addLink().

        There is three ways of linking neurons.

        - In the original model, each neuron is connected to \b n other neurons in the network (n is determined by a Poisson law \ref RandomNumbers::poisson() of mean meanConncectivity). The \b n neurons are chosen randomly from the set of neurons using the *RandomIndices()* method which determines the index of the \ b n neurons to be linked.  Each link has an intensity (determined by a uniform law \ref RandomNumbers::uniform_double() with minimum 0 and maximum 2 * meanStrength). The strength of each link is calculated by the \ref *RandomStrength()* method. Each neuron that is to be linked is then affiliated with the strength of the link. Then, these associations are added to the link set of the given neuron. These links are created at the begining of the simulation and do not change during it. We make sure that each link is only created once and that a neuron cannot make a link with itself. The number \b n of connections is always reached. *findNeurons()* allows to find out if a given neuron exists in the network's set of neurons to avoid a neuron to be linked with itself.

        - In the constant model, every neuron have the same number of links (their intensity are still randomly chosen) determined by the user. The connections are then built as the original model.

        - In the overdispersed model, every neuron has a different mean of connections (chosen by an exponential distribution). The connections are then built as the original model.

       \param neuron (Neurone*) : neuron whose links we want to create.
       \param avoidIndice (size_t) : index of the neuron whose links are created in the network's neuron set. This index can't be added to the link table because a neuron can't be linked to itself.
       \param numberLinks (int) : number of connections received by a neurons.
    */
///@{
    std::vector<size_t> RandomIndices(size_t avoidIndice, size_t numberLinks);
    std::vector<double> RandomStrength (int numberLinks) const;
    void createRandomLinks(Neurone* neuron);

    /*!
       \param neuron (Neurone*) : neuron sought in the whole network.
       \return if the neuron is found, returns its index in the set. If not, return the size of the set (no neuron has the size of the set as an index since indexes go from 0 to size-1).
    */
    size_t findNeuron(Neurone* neuron) const;
///@}

    /*!
       @brief Computes each neuron current via \ref Neurone::computeI() and updates them with it at each time step using \ref Neurone::update().
    */
    void update();


    /*!@name Display all the results in different output file.
       \param outfile (ostream&) : the name of the output file to write the spikes results on.
       \param outfile (ofstream&) : the name of the output file to write the parameters or the neuron sample on.
       \param time (size_t) : the moment of the simulation to consider
    */
///@{
    /*!
       @brief Writes the header of the sample file  in order to make it easier to understand to what the values in the files correspond to.
    */
    void headerSample(std::ofstream& outfile) const;
    /*!
       @brief Writes the header of the parameters file  in order to make it easier to understand to what the values in the files correspond to.
    */
    void headerParameters(std::ofstream& outfile) const;
    /*!
       @brief Writes  the state of each neuron as spikes (firing=1 or not fring=0) in the output file which name has the suffix _spikes.
    */
    void printSpikes(std::ostream& outfile, size_t time) const;
    /*!
       @brief Calls \ref Neurone::printParams() to write all the \ref Neurone parameters : type, a , b, c, d, excitator, degree (number of connections), valence (\ref Neurone::getValence()) in one output file which name has the suffix _parameters.
    */
    void printParameters(std::ofstream& outfile) const;
    /*!
       @brief Chose one neurone of each type present in the simulation and calls \ref Neurone::printSample() to write the \ref Neurone parameters : v, u, I in the output file which name  has the suffix _sample_neurons).
    */
    void printSample(std::ofstream& outfile, size_t time) const;
///@}

    /*!
       @name Utility methods (getters)
    */
///@{
    size_t getNumberNeurons() const;
    double getMeanStrength()const;
    double getMeanConnectivity()const;
    double getExcitatoryProportion()const;
    Neurons getNeurons() const;
///@}

private :
    Neurons neurons;
    double meanStrength;
    double meanConnectivity;
    double excitatoryProportion;
    std::map< std::string, size_t > neuronsProportions;
    char networkModel;
};
