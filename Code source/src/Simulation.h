#include "Network.h"

/*! @class Simulation

   The Simulation class is the main class in this program. It constructs the \ref Network according to user-specified parameters, and *run()* "runs" the simulation.

   Simulation results are printed to output files. Files are named according to what they contains (spikes, parameters or values of a sample of neurons).

   A Simulation is mainly made of a number of time steps \ref simulationDuration and a \ref Network, which contains pointers to \ref Neurone objects. These neurons are constructed and linked randomly. The simulation is either created according to user inputs or with default values.

   The map \ref neuronsProportions describes the neuron population (see \ref constants.h) :
   The key is the neuron type from \ref Neuron::NeuronTypes and the associated value is the number of this neuron type in the simulation.
*/

class Simulation
{

public:

    /*!
     * @name Construction and destruction
        Create the \ref Simulation according to values entered via the command line by the user or create it using the default values. If the user don't enter a value for each parameter, a message is deplayed on the terminal to explain how to use the program and help the user.
     *  The constructor uses *commandParse()* to process user inputs with TCLAP and construct the simulation with the right values.
     */
///@{
    Simulation(int argc, char **argv);
    Simulation();
    ~Simulation();
///@}

    /*!
     * @name Process user inputs
     * The following methods process user input and verify the validity of the values to be sure the program will work properly.
     */
///@{
    /*!
     * @brief This méthod uses TCLAP (http://tclap.sourceforge.net/manual.html) to process user inputs written on the command line and initialize the attributs with the corresponding values.
    */
    void commandParse(int argc, char **argv);

    /*!
     * @brief This method is called in case the user execute the program without all the parameters required by mistakes. Thus, it allows him to initialize the ones he forgot to initialize and let the others to their default values. If the user doesn't enter any values, by defaul this method will only permit him to use this program in its easiest way. Here are the possible values to chose : the duration of the simulation, the total number of neurons, the proportion of excitatory neurons, the mean intensity of the connections and the mean number of connections between each neurons.
    */
    void initializeRemainingAttributs();

    /*!
     * @brief Each neuron proportion can be specified by the user on the command line. If the user enter the proportions, the method will decompose the instruction to then allow *this to creat its network with the desired neuron proportions. The right way to enter neurons proportions is to write the type in capital letter, write ":" and then specify the proportion (between 0 and 1). Each proportions must be separated by a comma. For exemple \b IB:0.2,FS:0.3,CH:0.2 will create a network with 20% IB, 30% FS, 20% CH and 30% RS. By default, the remaining neurons needed to have the right number of neurons will be of type RS. If the proportions are not well entered, the default ones will be used (IB:0.1,LTS:0.2,FS:0.3,CH:0.2).
    */
    void loadConfiguration();

    /*!
     * @brief Check that the values entered bu the user are acceptable to run the programme properly. If not, an error message in desplayed on the terminal and the default value is used instead of the wrong one. Default values can be found in \ref constants.h .
    */
    void checkValues();

    /*!
     * @brief checks if the type given in parameter is one of the 5 possible neuron type implemented in this program (RS, FS, LTS, CH, IB).
     * @param type (string) : type to check.
     * @return true if type is found in the 5 possible types, false otherwise.
    */
    bool typeExists(std::string type) const;
///@}

    /*!@name Run the Simulation
     */
///@{
    /*! @brief This method is the most important of the \ref Simulation class. It runs the simulation with a loop until the requested simulation duration is reached. At each new time step, the Simulation updates its network, so updates indirectly each neurons of its \ref Network. Moreover, it prints the results on 3 output file (the spikes \ref Network::printSpikes(), the parameters of each neuron  \ref Network::printParameters(), and the membrane potential, recovery variable and current of one neurone of each type present in the simulation  \ref Network::printSample()).
     * @return the time the simulation lasted.
    */
    size_t run();
///@}

    /*!
       @name Utility methods (getters and setters)
    */
///@{
    Network* getNetwork() const;
    std::map<std::string, size_t>  getNeuronsProportions();
    size_t getSimulationDuration()const;
    void setProportions(std::string prop);
    static bool isApostrophe(char c);

    /*! @brief Check if the char in parameter is an unwanted character. The allowed characters in this program are : capital letters, numbers, comma, colon or full stop.
     * @param toTest (char) : character you want to know if it is an unwanted one or not.
     * @return true if the character in parameter is an unwanted character, false otherwise.
    */
    bool unwantedCharacters(char toTest) const;
///@}

private:

    Network* network;
    size_t simulationDuration, size;
    double excitatoryProportion, meanIntensity, meanConnectivity, delta;
    std::string outfileName, proportions;
    std::map< std::string, size_t > neuronsProportions;
    char networkModel ;
};
