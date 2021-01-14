#ifndef CALCULATION_H
#define CALCULATION_H

#include <vector>

using namespace std;

class Calculation
{
public:
    Calculation();
    ~Calculation();
    float deg2rad(float deg);
    vector<float> deg2rad(vector<float> deg);
    float Integrate(vector<float> x, vector<float> y);
    vector<float> CumIntegrate(vector<float> x, vector<float> y);
    void Fourier(vector<float> x, vector<float> y, vector<float>& Amp, vector<float>& Freq, float speed);
};

#endif // CALCULATION_H
