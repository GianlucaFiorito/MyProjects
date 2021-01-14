#ifndef ENGINETYPE_H
#define ENGINETYPE_H

#include <iostream>
#include <vector>
#include <map>

using namespace std;

struct DrivingCycle
{
    string Type;
    vector<float> Time; //[s]
    vector<float> Speed; //[km/h]
    vector<int> SelectedGear; //[-]
};

struct FullLoad
{
    float Bmep; //[bar]
    float Torque; //[Nm]
    float Power; //[kW]
};
struct Map2D
{
    vector<vector<float>> Values;
    vector<float> RpmAxis;
    vector<float> BmepAxis;
};

struct EngineData
{
    string Model;
    map<float,FullLoad> FullLoadMap;
    Map2D FuelConsumptionMap; // [kg/h]
    Map2D NOxEmissionMap; //[g/h]
    float Displacement; //[dm^3]
    float Inertia; //[kg*m^2]
    float IdleSpeed; //[rpm]
    float IdleConsumption; //[g/h]
    float FuelDensity; //[g/l]
};

struct GearBoxDifferential
{
    float DifferentalRatio;
    vector<float> GearShiftRatio;
    float DifferentalEfficiency;
    vector<float> GearShiftEfficiency;
};

struct WheelData
{
    float Inertia; //[kg*m^2]
    float Radius; //[mm]
};

struct CycleVehicleData
{
    float Mass; //[kg]
    vector<float> CoastDownCoeff; //[N, N/(km/h), N/(km/h)^2]
};

struct VehicleData
{
    string Model;
    GearBoxDifferential GearBoxDifferential;
    WheelData Wheel;
    CycleVehicleData CycleVehicleData;
    EngineData Engine;
};

struct CycleResults
{
    vector<float> EngineSpeed; //[rpm]
    vector<float> Bmep; //[rpm]
    float EngineEnergySpec; //[kJ]
    float BreakEnergySpec; //[kJ]
    vector<float> FuelCons; //[g/h]
    float ConsFor100Km; //[l/100km]
    float CO2EmissionSpec; //[g/km]
    vector<float> NOxEmission; //[g/h]
    float NOxEmissionSpec; //[g/km]
};

#endif // ENGINETYPE_H
