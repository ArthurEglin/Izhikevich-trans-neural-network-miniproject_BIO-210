#include "constants.h"


/*! @class Neurone
 The Neurone class creates a Neurone with its different parameters.
 A \ref Neurone is defined by different parameters :
 - a,  b,  c,  d : these values are specific to the neuron's type, they are the cellular parameters,
 - v : the membrane potential. Depending on the membrane potential value, the \ref Neurone can become "activated" (firing),
 - u : a relaxation variable (it doesn't exist in real life but it necessary for a realistic modelisation),
 - a type :  it tells if it is excitator or inhibitor. There is 5 different \ref Neurone types :  *RS*, *IB*, *CH* (excitators), *FS*, *LTS* (inhibitors).
 Each neuron is connected with others. Via this links, the neuron receive an electric current that can modifiy it's activation state.
*/

class Neurone;

/// * structure to assemble the neurone pointer and the strength of its bond *
struct NeuroneInteraction {
    Neurone* neurone;
    double bondStrength;
};

class Neurone
{

public:

    /*! @name Neurone construction and destruction
     A \ref Neurone can be constructed in two different ways. The first one constructs the neuron according to Izhikevich model and the second one implement a more rational model for all the cellular parameters (a, b, c and d). Each neuron is linked to other neurons and knows to which it is connected.
     \param chosen_type (string) :  the type of the neuron to construct
     \param delta (double) : noise parameter for the more rational model.
     \param strength (double) : strength of the connection to create.
    */
///@{
    Neurone (std::string chosen_type);
    Neurone (std::string chosen_type, double delta);
    ~Neurone();
    void initialize(std::string type);
    double deltaFactor(double delta);
    void addLink(Neurone* neurone, double strength);
///@}

    /*! @brief Update the membrane potential and relaxation variable of a Neurone each "dt" interval of time (the time is counted in milliseconds). The value of its membrane potential determines the state of the neuron. The neuron is updated according to its activation state : if it is firing, it transmittes an impulse along its axone; if not, it receives the impulsions of the firing neurons it is connected to. Its membrane potential is updated twice a millisecond and it's relaxation variable is updated only once a milliseconde.
    */
    void update();

    /*! @name Compute the synaptic current
          This methods compute the current \b *this receives thanks to the strength of all the connections it does with others.
         \n *getSumExcitator()* sums the strength of all the connections the neuron receives from firing excitator neurons (types *RS*, *IB*, *CH*).
         \n *getSumInhibitor()* sums  the strength of all the connections the neuron receives from firing inhibitor neurons (types *FS*, *LTS*).
         \n *getValence()* sums the the strength of all the connections the neuron receives. It adds half of the strength if the connected neuron is excitator and it substract the strength if the connected neuron is inhibitor. (This method is usefull for the parameter output file).
    */
///@{
    void computeI();
    double getSumExcitator()const;
    double getSumInhibitor()const;
    double getValence() const;
///@}

    /*! @name Display all the results in different output file.
        The method *printParams()* writes all the \ref Neurone parameters : type, a , b, c, d, excitator, degree (number of connections), valence (\ref getValence()). *printSample()* writes the \ref Neurone parameters : v, u, I.
        \param outfile (ostream&) : the name of the output file to write the results on.
    */
///@{
    void printParams(std::ofstream& outfile) const;
    void printSample(std::ofstream& outfile) const;
///@}

    /*!
       @name Utiliy methods
    */
///@{
    bool isFiring() const;
    size_t getSizeNeighborhood() const;
    bool getExcitator() const;
    void setFiringState(bool state);
    bool isType(std::string typeCheck) const;
    bool inNeighborhood(Neurone* neuron) const;
///@}

private:
    ///list of all the interactions of *this with the other neurones
    std::vector<NeuroneInteraction> neighborhood;
    ///membrane potential
    double v;
    ///relaxation variable
    double u;
    double a;
    double b;
    double c;
    double d;
    ///external noise parameter
    double w;
    ///electric current received by *this
    double I;
    ///true = firing, false = not firing
    bool firing;
    std::string type;
    ///true = excitator, false = inhibitor
    bool excitator;
};
