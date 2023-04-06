#include "Simulation.h"
#include "constants.h"

Simulation::Simulation(int argc, char **argv)
{
    commandParse(argc,argv);
    char choice(_DEFAULT_CHOICE_);

    if (argc < 13) { // argc = 13 when the user entered all needed values
        std::cout << "\n You did not enter all the required values, the missing values will be initialized to their default values. If you don't know how to use the program, you can type ./Neurons -h in the command line, an explanation on how to use it will be provided. A typical command is : ./Neurons -N 100 -t 10 -C 2 -P 0.2 -I 0.2 -O test10000. Is this what you want to do ? Press y (yes, continue with default values) or n (no, enter new values). " << std::endl;
        std::cin>>choice;
    }

    for(size_t i(0); i<3; ++i) {
        if(i==2 and (choice!='y') and (choice!='n')) {
            std::cerr<< "choice: "<<choice<<std::endl;
            std::cerr << "You didn't press the right lettre. You have reached the maximum number of attempts. The program will be launched as if you had chosen y (with default values)." << std::endl;
            choice = 'y';
        }
        if((choice!='y') and (choice!='n')) {
            std::cerr<< "choice: "<<choice<<std::endl;
            std::cout << "You didn't press the right lettre. Did you mean y (yes) or n (no) ?" << std::endl;
            std::cin>>choice;
        }
    }

    if(choice=='n') initializeRemainingAttributs(); // if we get 'y' we keep the defaults values for the attributs that were not mentionned by the user.

    checkValues(); //check the validity of all the values to be sure we can run the program properly

    if(proportions.empty()) {
        network = new Network(size, excitatoryProportion, meanConnectivity, meanIntensity, delta, networkModel);
    } else {
        loadConfiguration();
        network = new Network(neuronsProportions, meanConnectivity, meanIntensity, delta, networkModel);
    }
}

Simulation::Simulation() : network(new Network(_NEURON_NUMBER_, _PROPORTION_EXCITATOR_, _MEAN_CONNECTIVITY_, _MEAN_INTENSITY_, _DELTA_, _NETWORK_MODEL_)), simulationDuration(_SIMULATION_TIME_), size(_NEURON_NUMBER_), excitatoryProportion(_PROPORTION_EXCITATOR_), meanIntensity(_MEAN_INTENSITY_), meanConnectivity(_MEAN_CONNECTIVITY_), delta(_DELTA_), outfileName(_OUTFILE_NAME_), networkModel(_NETWORK_MODEL_) // by default, additional fonctionalities are not used
{} // Default values initialization

Simulation::~Simulation()
{
    delete network;
    network=nullptr;
    std::cout.flush();
}

void Simulation::commandParse(int argc, char **argv)
{
    TCLAP::CmdLine cmd(_PRGRM_TEXT_);
    TCLAP::ValueArg<size_t> neuron_number("N", "neuron_number", _NEURON_NUMBER_TEXT_, false, _NEURON_NUMBER_, "size_t");
    cmd.add(neuron_number);

    TCLAP::ValueArg<double> proportion_excitator("P", "proportion_excitator", _PROPORTION_EXCITATOR_TEXT_, false, _PROPORTION_EXCITATOR_, "double");
    cmd.add(proportion_excitator);

    TCLAP::ValueArg<size_t> simulation_time("t", "simulation_time", _SIMULATION_TIME_TEXT_, false, _SIMULATION_TIME_, "size_t");
    cmd.add(simulation_time);

    TCLAP::ValueArg<double> mean_connectivity("C", "mean_connectivity", _MEAN_CONNECTIVITY_TEXT_, false, _MEAN_CONNECTIVITY_, "double");
    cmd.add(mean_connectivity);

    TCLAP::ValueArg<double> mean_intensity("I", "mean_intensity", _MEAN_INTENSITY_TEXT_, false, _MEAN_INTENSITY_, "double");
    cmd.add(mean_intensity);

    TCLAP::ValueArg<std::string> types_proportions("T", "neurontypes", _TYPES_TEXT_,  false, "", "string");
    cmd.add(types_proportions);

    TCLAP::ValueArg<std::string> output("O", "output", _OUTPUT_TEXT_,  false, _OUTFILE_NAME_, "string");
    cmd.add(output);

    TCLAP::ValueArg<double> delta_("d", "delta", _DELTA_TEXT_,  false, _DELTA_, "double");
    cmd.add(delta_);

    std::vector<char> allowed{'B','C','O'} ; // define the constraints on the values that the networkModel can take
    TCLAP::ValuesConstraint<char> allowedVals( allowed );
    TCLAP::ValueArg<char> network_model("M", "network_model", _NETWORK_MODEL_TEXT_, false, _NETWORK_MODEL_, &allowedVals);
    cmd.add(network_model);

    cmd.parse(argc, argv);

    size=neuron_number.getValue();
    simulationDuration=simulation_time.getValue();
    excitatoryProportion=proportion_excitator.getValue();
    meanConnectivity=mean_connectivity.getValue();
    meanIntensity=mean_intensity.getValue();
    proportions=types_proportions.getValue();
    delta=delta_.getValue();
    networkModel= network_model.getValue();
    outfileName=output.getValue();
}

void Simulation::initializeRemainingAttributs()
{
    if(simulationDuration==_SIMULATION_TIME_) {
        double time;
        std::cout << "How many time do you want to do the simulation (how many time step) ? (Value upper 0)"<< std::endl;
        std::cin>>time;
        simulationDuration=size_t(time); // make a rounding if the value is decimal and take the absolute value if it is a signed number
    }

    if(size==_NEURON_NUMBER_) {
        double number;
        std::cout << "How many neurons do you want in your network ? (value upper 0)"<< std::endl;
        std::cin>> number;
        size=size_t(number); // make a rounding if the value is decimal and take the absolute value if it is a signed number
    }

    if(excitatoryProportion==_PROPORTION_EXCITATOR_) {
        std::cout << "Which proportion of excitatory neurons do you want ? (value between 0 and 1)"<< std::endl;
        std::cin>> excitatoryProportion;
    }

    if(meanIntensity==_MEAN_INTENSITY_) {
        std::cout << "What is the mean intensity of a connection between two neurons ? (value upper 0)"<< std::endl;
        std::cin>> meanIntensity;
    }

    if(meanConnectivity==_MEAN_CONNECTIVITY_) {
        std::cout << "What is the mean number of connections each neuron receive ? (Value between 0 and the total number of neurons minus 1 (a neuron can not be linked with itself))"<< std::endl;
        std::cin>> meanConnectivity;
    }

    if(outfileName==_OUTFILE_NAME_) {
        std::cout << "What is the name of the output file ? (suffixes will be added to this name so do not enter extensions like .txt)"<< std::endl;
        std::cin>> outfileName;
    }

    std::cout << "\n" << "We will check the values you gave to be sure they are correct. If not, default values will be used. The simulation will then be ready to run. When it will have finished, you will be able to see results in files called " + outfileName + "_spikes.txt, " + outfileName + "_sample_neurons.txt and " + outfileName + "_parameters.txt." << "\n" << std::endl;
}

void Simulation::loadConfiguration() //Initiliaze the proportion of the different type of neurons and order them in a map according to the proportions entered by the user
{
    proportions.erase(std::remove_if(proportions.begin(), proportions.end(), isApostrophe), proportions.end()); // remove the first and last apostrophes if the user enter proportions like '...'
    proportions.erase(std::remove_if(proportions.begin(), proportions.end(), isspace), proportions.end());

    for(size_t i(0); i<proportions.size(); ++i) {
        if(unwantedCharacters(proportions[i])) {
            proportions=_PROPORTIONS_;
            std::cerr << "The proportions were not entered correclty. The program will continue with the following proportions: " + std::string(_PROPORTIONS_) << std::endl;;
        }
    }

    std::string type;
    std::stringstream substringtest(proportions);
    size_t total = 0;
    for (std::string paire; std::getline(substringtest, paire, ','); ) {
        size_t typeName = paire.find(':');
        if(typeName > 3) { //typeName can be 2 or 3 (depending on the neuron type), if bigger it means that ':' was not found and typeName is equal to std::string::npos, the biggest value for size_t
            proportions=_PROPORTIONS_;
            std::cerr << "The proportions were not entered correclty. The program will continue with the following proportions: " + std::string(_PROPORTIONS_) << std::endl;
        }
    }

    //from there we can be sure that the proportions are in the correct layout
    std::stringstream substring(proportions);
    for (std::string paire; total<size and std::getline(substring, paire, ','); ) {
        size_t typeName = paire.find(':');
        type = paire.substr(0, typeName);
        double value = std::min(std::stod(paire.substr(typeName+1)), 1.0); // a proportion is alawys <=1.0. If the user entered something >1, the program will use the value 1.
        if (value>0 and type!="RS" and typeExists(type)) { // all the remaining neurons will be initialize as RS to achieve the total number of neurons (i.e. if we have 10 neurons with proportions FS:0.3,IB:0.5 we will get 3 FS, 5 IB and 2 RS)
            size_t number(value*size);
            if (total+number>size) number = size-total;
            neuronsProportions[type] = number;
            total += number;
        }
    }
    if(total<size)neuronsProportions["RS"]=size-total; // we put the other neurons as RS (default type of a neuron)
}

void Simulation::checkValues() //since we don't want to stop the program when a value is wrong but just to use the default one, we display error messages to inform the user we will not use his values but we don't throw errors (we throw errors only for TCLAP errors, output files errors or link creation errors).
{
    if (excitatoryProportion>1.0 or excitatoryProportion<0.0) {
        excitatoryProportion=_PROPORTION_EXCITATOR_;
        std::cerr << "The proportion of excitator neurons must be between 0 and 1. The default value " + std::to_string(_PROPORTION_EXCITATOR_) + " will be used instead of the one you gave. \n" << std::endl;
    }

    if (size>100000) {
        size=_NEURON_NUMBER_;
        std::cerr << "The number of neurons is very big, this can be due to a negative number given. The default value " + std::to_string(_NEURON_NUMBER_) + " will be used instead of the one you gave. \n" << std::endl;
    }

    if (meanConnectivity<0.0 or meanConnectivity>size-1) {
        meanConnectivity=size*0.1;
        std::cerr << "The number of connections received by a neuron can not be negative and can not exceed the total number of neurons in the network. The value 0.1*" + std::to_string(size) + " (neuron number) will be used instead of the one you gave. \n" << std::endl;
    }

    if (meanIntensity<0) {
        meanIntensity=_MEAN_INTENSITY_;
        std::cerr << "The mean strength of a link between two neurons must be positive. The default value " + std::to_string( _MEAN_INTENSITY_) + " will be used instead of the one you gave. \n" << std::endl;
    }

    if ((delta<0.0 or delta>1.0) and delta!=_DELTA_) {
        delta=_DELTA_;
        std::cerr << "The delta parameter used in the additional functionality must be between 0 and 1 because the 'noise' that affect each parameter must be approximately 1. The default value " + std::to_string(_DELTA_) + " will be used instead of the one you gave (i.e. this fonctionnality will not be used). \n" << std::endl;
    }

    if (simulationDuration>10000) {
        simulationDuration=_SIMULATION_TIME_;
        std::cerr << "The time is very big, this can be due to a negative time given. The default value " + std::to_string(_SIMULATION_TIME_) + " will be used instead of the one you gave. \n" << std::endl;
    }
}

bool Simulation::typeExists(std::string type) const
{
    auto paire=NeuronParam.find(type);

    if(paire==NeuronParam.end()) {
        return false;
    }
    return true;
}

size_t Simulation::run()
{
    std::ofstream outfileSpikes,outfileParam, outfileSample;
    outfileSpikes.open(outfileName+"_spikes.txt", std::ios_base::out);
    if (!outfileSpikes.good()) throw(OUTPUT_ERROR(std::string("The spikes output file is not in good condition, it is impossible to write on it. The spikes results will be written on the terminal. \n")));
    std::ostream *outstream = &std::cout; //if the file is not open, we print the results in the terminal (we only print the spikes results on the terminal because it is the main result file)
    if (outfileSpikes.is_open()) outstream = &outfileSpikes;

    outfileParam.open(outfileName+"_parameters.txt");
    if (!outfileParam.good()) throw(OUTPUT_ERROR(std::string("The parameters output file is not in good condition, it is impossible to write on it. This results will not be displayed. \n")));

    outfileSample.open(outfileName+"_sample_neurons.txt");
    if (!outfileSample.good()) throw(OUTPUT_ERROR(std::string("The neurons sample output file is not in good condition, it is impossible to write on it. This results will not be displayed. \n")));

// fill the parameters files
    network->headerParameters(outfileParam);
    network->printParameters(outfileParam);
    if (outfileParam.is_open()) outfileParam.close();

// print the header for sample output file
    network->headerSample(outfileSample);

// print both the spikes and sample output files
    size_t current_time(1); // we start a t=1
    while(current_time <= simulationDuration) {
        network->update();
        network->printSpikes(*outstream, current_time);
        if (outfileSample.is_open()) network->printSample(outfileSample, current_time);
        current_time += _DT_;
    }
    if (outfileSpikes.is_open()) outfileSpikes.close();
    if (outfileSample.is_open()) outfileSample.close();

    return current_time-1; // the while loop increments the time counter 1 time too much, the real duration is thus current_time-1
}

Network* Simulation::getNetwork() const
{
    return network;
}

std::map<std::string, size_t>  Simulation::getNeuronsProportions()
{
    return neuronsProportions;
}

size_t Simulation::getSimulationDuration() const
{
    return simulationDuration;
}

void Simulation::setProportions(std::string prop)
{
    proportions = prop;
}

bool Simulation::isApostrophe(char c) // used to delete the apostrophe (''')
{
    return c == '\'';
}

bool Simulation::unwantedCharacters(char toTest) const
{
    int c(toTest);
    bool capitalLetter(c>=65 and c<=90);
    bool number(c>=48 and c<=57);
    bool comma(c==44); // ','
    bool colon(c==58); // ':'
    bool fullStop(c==46); // '.'
    return !(capitalLetter or number or comma or colon or fullStop);
}
