#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <vector>
#include <algorithm>

/*! @class RandomNumbers
 * Random class based on standard c++-11 generators
 */
class RandomNumbers
{

public:

    /*!@name Initialization
      The constructor initialiazes the generator \ref rng with the Mersenne twister *mt19937* engine from the standard library and seeds it.
      \param s (unsigned long int) : seed. If 0 (default) the seed is generated from a *random_device*.
      */
    RandomNumbers(unsigned long int s=0);

    /*!@name Distributions
      These functions either fill the vector(first argument) with random numbers distributed according to the specific distribution or they return a single random number distributed according the specified distribution.
      The additional arguments are the standard parameters of these distributions.
      */
///@{
    double uniform_double(double lower=0, double upper=1);
    void uniform_double(std::vector<double> &T, double lower=0, double upper=1);
    double normal(double mean=0, double sd=1);
    int poisson(double mean=1);
    double exponential(const double rate=1);
///@}

    /*!@name Vector manipulation using the mersenne twister mt19937
       Randomly rearrange elements of a given vector using a random number generator.
       The function use the std::shuffle function of the algorithm library (http://www.cplusplus.com/reference/algorithm/shuffle/)
       \param res (vector<size_t>&) : vector to shuffle.
    */
    void shuffle(std::vector<size_t> &res) ;

private:
    std::mt19937 rng;
    long int seed;
};

extern RandomNumbers *_RNG;

#endif //RANDOM_H
