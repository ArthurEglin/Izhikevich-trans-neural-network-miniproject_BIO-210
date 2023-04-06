#include "Neurone.h"
#include "Random.h"

Neurone::Neurone(std::string chosen_type) : type(chosen_type)
{
    double r(_RNG->uniform_double(0.0,1.0));
    initialize(type);
    if (chosen_type == "FS") {
        a *= (1-0.8*r);
        b *= (1+0.25*r);
    }
    if (chosen_type == "RS") {
        c *= (1-(3.0/13.0)*r*r);
        d *= (1-0.75*r*r);
    }
    v = -65.0;
    u = b*v;
    firing = false;
    I = w*_RNG->normal(0.0,1.0);
}

Neurone::Neurone(std::string chosen_type, double delta) : type(chosen_type)
{
    v = -65.0;
    initialize(type);
    a*= deltaFactor(delta);
    b*= deltaFactor(delta);
    c*= deltaFactor(delta);
    d*= deltaFactor(delta);
    u = b*v;
    firing = false;
    I = w*_RNG->normal(0.0,1.0);
}

Neurone::~Neurone() {}

void Neurone::initialize(std::string type)
{
    auto paire = NeuronParam.find(type);
    NeuronValues parameters = paire->second;
    a=parameters.a;
    b=parameters.b;
    c=parameters.c;
    d=parameters.d;
    w=parameters.w;
    excitator=parameters.exci;
}

double Neurone::deltaFactor(double delta)
{
    return _RNG->uniform_double(1-delta,1+delta);
}

void Neurone::addLink(Neurone* neurone, double strength)
{
    NeuroneInteraction new_link = {neurone, strength};
    neighborhood.push_back(new_link);
}

void Neurone::update()
{
    if (v > _T_) {
        firing = true;
    } else {
        firing = false;
    }
    if (firing) {
        v = c;
        u += d;
    } else {
        v += 0.5*(0.04*v*v + 5.0*v + 140.0 - u + I);
        v += 0.5*(0.04*v*v + 5.0*v + 140.0 - u + I);
        u += a*(b*v-u);
    }
}

void Neurone::computeI()
{
    I = w*_RNG->normal(0.0,1.0) + 0.5*getSumExcitator() - getSumInhibitor();
}

double Neurone::getSumExcitator() const
{
    double value(0.0);
    for (size_t i(0); i<neighborhood.size(); ++i) {
        if ((neighborhood[i].neurone->isFiring()) and (neighborhood[i].neurone->getExcitator())) {
            value += neighborhood[i].bondStrength;
        }
    }
    return value;
}

double Neurone::getSumInhibitor() const
{
    double value(0.0);
    for (size_t i(0); i<neighborhood.size(); ++i) {
        if ((neighborhood[i].neurone->isFiring()) and !(neighborhood[i].neurone->getExcitator())) {
            value += neighborhood[i].bondStrength;
        }
    }
    return value;
}

double Neurone::getValence() const
{
    double valence(0.0);
    for(auto interaction : neighborhood) {
        if(interaction.neurone->getExcitator()) valence+=0.5*interaction.bondStrength; //excitator
        if(!interaction.neurone->getExcitator()) valence-=interaction.bondStrength; //inhibitor
    }
    return valence;
}

void Neurone::printSample(std::ofstream& outfile) const
{
    outfile << "\t" << v << "\t" << u << "\t" << I;
}

void Neurone::printParams(std::ofstream& outfile) const
{
    double valence(getValence());
    outfile << type << "\t" << a << "\t" << b << "\t" << c << "\t" << d << "\t" << !excitator << "\t" << neighborhood.size() << "\t" << valence;
    outfile << std::endl;
}

bool Neurone::isFiring() const
{
    return firing;
}

size_t Neurone::getSizeNeighborhood() const
{
    return neighborhood.size();
}

bool Neurone::getExcitator() const
{
    return excitator;
}

void Neurone::setFiringState(bool state)
{
    firing = state;
}

bool Neurone::isType(std::string typeCheck) const
{
    return (type==typeCheck);
}

bool Neurone::inNeighborhood(Neurone* neuron) const
{
    for(auto interaction : neighborhood) {
        if(interaction.neurone==neuron) return true;
    }

    return false;
}

