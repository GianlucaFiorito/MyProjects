#ifndef FLYWHEELTYPE_H
#define FLYWHEELTYPE_H

#endif // FLYWHEELTYPE_H

#include <vector>
#include <string>
using namespace std;

struct FuelData
{
    float Density; //[kg/dm^3]
    float NetCalValue; //[MJ/kg]
    float StDosage; //[-]
};

struct EngineFlyData
{
    unsigned int CylicersNumber;
    float TotalDisplacement; //[cm^3]
    float CompRatio; //[-]
    float Bore; //[mm]
    float Stroke; //[mm]
    float ConnRod; //[mm]
    float SpecAltMass; //[kg/dm^3]
    float SpecRotMass; //[kg/dm^3]
    float IntakeOpenAdv; //[°]
    float IntakeClosDelay; //[°]
    float ExaustOpenAdv; //[°]
    float ExaustClosDelay; //[°]
    FuelData Fuel;
};

struct TestCondition
{
    float AmbientPress; //[mmHg]
    float AmbientTemp; //[°C]
    float EngineSpeed; //[rpm]
    float EngineTorque; //[Nm]
    float RelDosage;
    float VolumeFuel; //[cm^3]
    float TimeTest; //[s]
};

struct MesuredData
{
    vector<float> CrankAngle; //[°]
    vector<float> Pressure; //[bar]
};

struct ValTiming
{
    vector<float> ChambVolume;
    vector<float> Pressure;
    vector<string> Type;
};

struct Fourier
{
    vector<float> Amplitude; //[Nm]
    vector<float> Frequence; //[Hz]
};

struct FlyWheelResults
{
    vector<float> ChamberVolume; //[dm^3]
    ValTiming ValveTiming;
    vector<float> InertiaPressure; //[bar]
    vector<float> EffectivePressure; //[bar]
    float AmbientPressure; //[bar]
    vector<vector<float>> CylinderEngineTorque;//[Nm]
    vector<float> EngineTorque;//[Nm]
    float ResistentTorque; //[Nm]
    float AvgFuelFlow; //[kg/s]
    float Bmep; //[bar]
    float UsEfficiency; //[-]
    float MaxEfficiency; //[-]
    float OrgEfficiency; //[-]
    float TerFluEfficiency; //[-]
    float FillCoeff;
    vector<float> EngineWork; //[bar]
    vector<float> ResistentWork; //[bar]
    float FlyWDiameter; //[mm];
    float FlyWDepth; //[mm]
    vector<float> EngineSpeed; //[rpm]
    float AritAvgSpeed; //[rpm]
    float IntAvgSpeed; //[rpm]
    Fourier FreqSpectrum;
};
