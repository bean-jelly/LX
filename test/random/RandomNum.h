#ifndef LCRANDOM_HPP_
#define LCRANDOM_HPP_
#include <ctime>

class lcrandom
{
public:
explicit lcrandom(size_t s = 0) : seed(s)
{
    if (0 == seed) seed = std::time(0);
    randomize();
}

void reset(size_t s)
{
    seed = s;
    if (0 == seed) seed = std::time(0);
    randomize();
}

size_t rand()
{
//returns a random integer in the range [0, -1UL)  
    randomize();
    return seed;
}

double real()
{
//returns a random real number in the range [0.0, 1.0)  
    randomize();
    return (double)(seed) / -1UL;
}

private:
size_t seed;
void randomize() { seed = 1103515245UL * seed + 12345UL; }
};

class lcrand_help
{
static lcrandom r;
public:
lcrand_help() {}
void operator()(size_t s) { r.reset(s); }
size_t operator()() const { return r.rand(); }
double operator()(double) { return r.real(); }
};
lcrandom lcrand_help:: r;

extern void lcsrand(size_t s) { lcrand_help()(s); }
extern size_t lcirand() { return lcrand_help()(); }
extern double lcdrand() { return lcrand_help()(1.0); }

#endif