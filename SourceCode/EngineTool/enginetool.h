#ifndef ENGINETOOL_H
#define ENGINETOOL_H

#include <fstream>
#include <calculation.h>
#include <engineTypes.h>

class EngineTool
{
private:
    DrivingCycle Driving;
    VehicleData Vehicle;
    string SelectedGearFilePath;
    CycleResults Results;
    string GearProfileFile;
    bool LoadCycleOK;
    bool LoadEngineOK;
    bool LoadVehicleOK;
    bool ResultsOk;
    bool StartStopIsOn;
    bool CutOffIsOn;
    Calculation Calculate;

    bool _LoadFullLoadMap(string FileName, map<float,FullLoad>& FullLoadMap);
    bool _Load2DMap(string FileName, Map2D& MapLoad);
    float _Interpolate2DMap(float Rpm, float Bmep, float Power, float IdleValue, Map2D Map);
    bool _LoadGearProfileData(string GearProfilePath, vector<int>& GearProfile);
    bool _LoadCycleVehicleData(string FileName, string CycleType, CycleVehicleData& CycleVehData, vector<int>& GearProfile);
    float _CalculateBmep(float Rpm, float Power);

public:
    EngineTool();
    ~EngineTool();
    bool LoadCycle(string FilePath);
    bool LoadEngine(string FilePath);
    bool LoadVehicle(string FilePath);
    void SetStartStop(bool StartStop);
    void SetCutOff(bool CutOff);
    bool RunCycle(void);
    DrivingCycle getCycle(void);
    CycleResults getResults(void);
    string getVehicleModel(void);
    string getEngineModel(void);
    EngineData getEngineMap(void);
    bool getCutOff(void);
    bool getStartStop(void);
    void ResetTool(void);
    bool ExportResults(string FilePath);

};

#endif // ENGINETOOL_H
