#ifndef FLYWHEELTOOL_H
#define FLYWHEELTOOL_H

#include <string>
#include <calculation.h>
#include <flywheelType.h>

using namespace std;

class FlyWheelTool
{
private:
    EngineFlyData Engine;
    TestCondition TestCond;
    MesuredData CollectedData;
    FlyWheelResults Results;
    bool LoadOk;
    Calculation Calculate;
    void _SaveVector(ofstream& File, string DataTitle, vector<float> Data);
    bool _CollectedDataLoad(string PathFile);

public:
    FlyWheelTool();
    ~FlyWheelTool();
    bool LoadData(string PathFile);
    bool RunCycle(float FWdensity, float FWirreg);
    bool ExportResults(string FilePath);
    void ResetTool(void);
    void getEngineData(unsigned int& CylindersNum, float& Displacement);
    void getTestCond(float& EngineSpeed, float& EngineTorque, float& RelDosag);
    FlyWheelResults getResults(void);
    MesuredData getMesuredData(void);
};

#endif // FLYWHEELTOOL_H
