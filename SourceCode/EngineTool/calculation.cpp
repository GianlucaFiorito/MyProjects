#include "calculation.h"
#include <math.h>

Calculation::Calculation()
{

}
Calculation::~Calculation()
{

}

float Calculation::deg2rad(float deg)
{
    return deg / 180.0 * M_PI;
}

vector<float> Calculation::deg2rad(vector<float> deg)
{
    for(auto ideg = deg.begin(); ideg != deg.end(); ideg++)
    {
        *ideg = *ideg/ 180.0 * M_PI;
    }
    return deg;
}

float Calculation::Integrate(vector<float> x, vector<float> y)
{
    float Integrate = 0;

    for(unsigned int step = 0; step < (x.size() -1); step++)
    {
        Integrate += (y[step+1] + y[step]) * (x[step+1] - x[step]) / 2;
    }
    return Integrate;
}

vector<float> Calculation::CumIntegrate(vector<float> x, vector<float> y)
{
    float Integrate = 0;
    vector<float> CumResult;
    CumResult.push_back(Integrate);

    for(unsigned int step = 0; step < (x.size() -1); step++)
    {
        Integrate += (y[step+1] + y[step]) * (x[step+1] - x[step]) / 2;
        CumResult.push_back(Integrate);
    }
    return CumResult;
}

void Calculation::Fourier(vector<float> x, vector<float> y, vector<float>& Amp, vector<float>& Freq, float speed)
{
    float a;
    float b;
    vector<float> a_integrate(y.size(),0);
    vector<float> b_integrate(y.size(),0);

    a = Integrate(x, y) / (x[x.size() - 1] - x[0]);

    Amp.push_back(a);
    Freq.push_back(0);

    for(int n = 1; n <= 40; n++)
    {
        for(unsigned int leng = 0; leng < x.size(); leng++)
        {
            a_integrate[leng] = y[leng] * cos(2*M_PI/(x[x.size() - 1] - x[0]) * n * x[leng]);
            b_integrate[leng] = y[leng] * sin(2*M_PI/(x[x.size() - 1] - x[0]) * n * x[leng]);
        }
        a = 2 * Integrate(x,a_integrate) / (x[x.size() - 1] - x[0]);
        b = 2 * Integrate(x,b_integrate) / (x[x.size() - 1] - x[0]);
        Amp.push_back(sqrt(pow(a,2) + pow(b,2)));
        Freq.push_back(n * speed / (x[x.size() - 1] - x[0]));
    }
}







