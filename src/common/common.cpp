#include "common.h"

thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
thread_local std::mt19937 generator;
 
double degrees_to_radians(double degrees)
{
  return degrees * pi / 180.0;
}

double clamp(double x, double min, double max)
{
  if(x < min) return min;
  if(x > max) return max;

  return x;
}

double random_double()
{
 return distribution(generator);
}

double random_double(double min, double max)
{
  return min + (max-min)*random_double();
}
