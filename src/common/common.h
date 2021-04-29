#ifndef COMMON_HH_INCLUDED
#define COMMON_HH_INCLUDED

#include <cmath>
#include <limits>
#include <memory>
#include <random>


using std::unique_ptr;
using std::make_unique;
using std::sqrt;


const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees)
{
  return degrees * pi / 180.0;
}

inline double clamp(double x, double min, double max)
{
  if(x < min) return min;
  if(x > max) return max;

  return x;
}

thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
thread_local std::mt19937 generator;

inline double random_double()
{
  return distribution(generator);
}

inline double random_double(double min, double max)
{
  return min + (max-min)*random_double();
}

#endif // COMMON_HH_INCLUDED
