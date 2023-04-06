#include "constants.h"
#include "Random.h"
#include "Simulation.h"

/*!
 \mainpage From biology to informatic modelisation or how to model a neuron network ?
 \image html brain.jpg "modelisation of neuronales connections" width=600cm
 This program is an implementation of the simplified model of spiking neurons of Eugene Izhikevich. It models neurons interacting together in their network and try to show how they influence each other depending on their cellular features, electric current...
\n The implementation is based on this ressource : EM Izhikevich, IEE Trans. Neural Net, 2003 - https://www.izhikevich.org/publications/spikes.pdf

 Here is a typical command to run the program  :

 \verbatim
 ./Neurons -N 10000 -t 1000 -C 40 -T FS:0.2,IB:0.15,CH:0.15,RS:0.5 -I 7 -O test10000
 \endverbatim

 This command will creat a \ref Network composed of 10000 \ref Neurone which will be distributed as follow : 15% IB, 20% FS, 15% CH and 50% RS. They will have on average 40 connections each (about 400 000 connections in the whole network) with an intensity uniformly distributed between 0 and 14 (2*7).

 The results will be printed in 3 output files : test10000_spikes.txt,  test10000_parameters.txt,  test10000_sample_neurons.txt. These output files can be used to draw graphs thanks to the script RasterPlots.R (this file can be found on the git repository).


 The command to get the graphs based on the output files of the command line above is :

 \verbatim
Rscript RasterPlots.R test10000_spikes.txt test10000_sample_neurons.txt test10000_parameters.txt
 \endverbatim
 \n
 \image html graphs.png "exemples of graphs you will get when you run the program with the command above" width=1100cm

 In addition to being able to use the program as above, it offers two additional functionalities. The first  allows to simulate a more rational version of the neuron's cellular parameters' value. The second one allows to choose how to create the links between neurons (change the connectivity pattern).

 To have more informations on how to use the program, you can write the command bellow on the command line (this will provide help) :

  \verbatim
 ./Neurons -h
 \endverbatim

 You can play with the program to generate different simulation. Here are two examples of observable behaviour. 1) The more inhibitor neurons you have, the less your neurons will be firing (and vice versa). 2) By increasing the strength of the connections, you will increase the firing rate as well. All these behaviours can be seen thanks to the graphs.

 \authors Chekireb-Pack Antoine
 \authors Eglin Arthur
 \authors Rivet Victoria
 \authors Rodriguez Valentin

 \date fall semester 2020

 \n *This program was carried out as part of the EPFL course CS-116 software project.*
 \n  *The code is written in C++ and is formatted with astyle.*

 _____________________________________________________________________________________________
 images sources :
 \n brain : https://towardsdatascience.com/understanding-neural-networks-22b29755abd9
 \n EPFL logo : https://www.epfl.ch/about/overview/fr/identite/
 \n neuron logo (modified) : https://www.shutterstock.com/fr/search/neuron+logo

*/

RandomNumbers *_RNG = new RandomNumbers(857298564279165);

int main(int argc, char **argv)
{
    try {

        Simulation s(argc, argv);
        s.run();

    } catch(SimulError &e) {
        std::cerr << e.what() << std::endl;
        return e.value();
    } catch(TCLAP::ArgException &e) {
        throw(TCLAP_ERROR("Error: " + e.error() + " " + e.argId()));
    } catch (std::invalid_argument &ee) {
        std::cerr<<ee.what()<<std::endl;
    }

    if (_RNG) delete _RNG;

    return 0;
}
