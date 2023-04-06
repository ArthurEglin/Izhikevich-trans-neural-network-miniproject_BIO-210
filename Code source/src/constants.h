#pragma once
#include <tclap/CmdLine.h>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <numeric>
#include <cmath>


/*! @brief NeuronValues
 \p a, \p b, \p c, \p d, \p w, and a boolean \p exci for the excitatory state.
*/
struct NeuronValues {
    double a,b,c,d,w;
    bool exci;
};

/*! @brief NeuronParam contains the values of each constant parameter ( \p a, \p b, \p c, \p d, \p w, \p exci) for each \ref Neurone type.
*/
const std::map<std::string, NeuronValues> NeuronParam{
    {"RS",  {0.02, 0.2,  -65., 8., 5.,  true}},
    {"IB",  {0.02, 0.2,  -55., 4., 5.,  true}},
    {"CH",  {0.02, 0.2,  -50., 2., 5.,  true}},
    {"FS",  {0.1,  0.2,  -65., 2., 2.,  false}},
    {"LTS", {0.02, 0.25, -65., 2., 2.,  false}},
};

/*!
  A base class for TCLAP errors and output files error  thrown in this program and for the general constants used throughout the program. Other error types (std::invalid_argument) are handled directly in the program.
  Each error type has a specific exit code.
  Error messages will be passed by the exception caller.
*/
class SimulError : public std::runtime_error
{
public:
    SimulError(const char *c, int v=0) : std::runtime_error(c), code(v) {}
    SimulError(const std::string &s, int v=0) : std::runtime_error(s), code(v) {}
    int value() const
    {
        return code;
    }
protected:
    const int code;
};

#define _SIMULERR_(_N, _id) class _N : public SimulError { \
    public: _N(const char *c) : SimulError(c,_id) {} \
            _N(const std::string &s) : SimulError(s,_id) {} };
/// *Specific error codes*
_SIMULERR_(TCLAP_ERROR, 10)
_SIMULERR_(OUTPUT_ERROR, 30)

#undef _SIMULERR_

/// *texts for user input *
#define _PRGRM_TEXT_ "This program simulate a Neuronal Network based on Eugene Izhikevich simplified model."
#define _NEURON_NUMBER_TEXT_ "Total number of neurons in the simulation."
#define _PROPORTION_EXCITATOR_TEXT_ "Proportion of excitator neurons in the simulation (excitatory = RS, inhibitory = FS). This parameter is mutualy exclusive with 'types proportion'. With this parameter, only FS and RS neurons will be created so if you want to create other types of neurons, you need to precise all types proportions using the parameter 'types proportions'. By default, excitatory proportions is used instead of types proportions.But f your enter both, type proportions will be used."
#define _SIMULATION_TIME_TEXT_ "Number of time-steps (simulation duration)."
#define _MEAN_CONNECTIVITY_TEXT_ "Average number of connections between neurons."
#define _MEAN_INTENSITY_TEXT_ "Average strength of a connection."
#define _DELTA_TEXT_ "Noise variable parameter. This parameter needs to be between 0 and 1. This parameter is necessary if you want to use the additional fonctionnality of the program which model a more rational model. If you want to use the program in its easier way, don't give a value to this argument. "
#define _TYPES_TEXT_ "Proportions of each type of neurons (excitator : RS, IB, CH; inhibitor : FS, LTS). For exemple you can write : ’IB:0.2,FS:0.3,CH:0.2’. If the sum of the proportions is less than 1, the remaining neurons to be created will be RS. If the sum is more than 1, the proportions will be adapted in order to have the number of neurons requested."
#define _OUTPUT_TEXT_ "Name of the file to print the results of the simulation (it will contains the spikes of each neurons during all the simulation, the parameters of each neurons and time dependant variables of a neuron sample)."
#define _NETWORK_MODEL_TEXT_ "Model of the network that the user wish to simulate, either basic (B), constant (C) or overdispersed (O). These differents model influence how links between neurons are created. This program will not be launched if something else than B, C or O is specified. By default, the basic (Izhikevich) model is used."

/// * default parameters values in the program *
#define _T_ 30 // discharge threshold T = 30 [mV] corresponds to the potential value at which the neuron transmits a pulse along its axon.
#define _DT_ 1 // time elapsed between each time step

/// *default values for user input *
#define _DEFAULT_CHOICE_ 'y'
#define _NEURON_NUMBER_ 100
#define _PROPORTION_EXCITATOR_ 0.2
#define _SIMULATION_TIME_ 10
#define _MEAN_CONNECTIVITY_ 5
#define _MEAN_INTENSITY_ 0.5
#define _DELTA_ -1.0
#define _NETWORK_MODEL_ 'B'
#define _OUTFILE_NAME_ "test100"
#define _PROPORTIONS_ "IB:0.1,LTS:0.2,FS:0.3,CH:0.2"
