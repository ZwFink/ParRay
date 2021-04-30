#ifndef COMMON_HH_INCLUDED
#define COMMON_HH_INCLUDED

#include <cmath>
#include <limits>
#include <memory>
#include <random>


using std::unique_ptr;
using std::make_unique;
using std::shared_ptr;
using std::make_shared;
using std::sqrt;


constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

double degrees_to_radians(double degrees);

double clamp(double x, double min, double max);

extern thread_local std::uniform_real_distribution<double> distribution;
extern thread_local std::mt19937 generator;

double random_double();

double random_double(double min, double max);

#endif // COMMON_HH_INCLUDED
