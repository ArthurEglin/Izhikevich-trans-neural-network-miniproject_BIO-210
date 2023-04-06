#include "Random.h"

RandomNumbers::RandomNumbers(unsigned long int s) : seed(s)
{
    if (seed == 0) {
        std::random_device rd;
        seed = rd();
    }
    rng = std::mt19937(seed);
}

double RandomNumbers::uniform_double(double lower, double upper)
{
    std::uniform_real_distribution<> unif(lower, upper) ;
    return unif(rng) ;
}

void RandomNumbers::uniform_double(std::vector<double> &res, double lower, double upper)
{
    std::uniform_real_distribution<> unif(lower, upper) ;
    for (auto I=res.begin(); I!=res.end(); I++) *I = unif(rng) ;
}

double RandomNumbers::normal(double mean, double sd)
{
    std::normal_distribution<> norm(mean,sd) ;
    return norm(rng) ;
}

int RandomNumbers::poisson(double mean)
{
    std::poisson_distribution<> pois(mean) ;
    return pois(rng) ;
}

double RandomNumbers::exponential(const double rate)
{
    std::exponential_distribution<> expo(rate) ;
    return expo(rng) ;
}

void RandomNumbers::shuffle(std::vector<size_t> &res)
{
    std::shuffle(res.begin(), res.end(), rng);
}
