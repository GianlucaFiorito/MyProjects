#include <enginetool.h>
#include <vector>
#include <sstream>
#include <math.h>

EngineTool::EngineTool():
    LoadCycleOK(false),
    LoadEngineOK(false),
    LoadVehicleOK(false),
    ResultsOk(false),
    StartStopIsOn(false),
    CutOffIsOn(false)
{

}

EngineTool::~EngineTool()
{
    ResetTool();
}

bool EngineTool::_LoadFullLoadMap(string FileName, map<float,FullLoad>& FullLoadMap)
{
    float SpeedKey;
    FullLoad FullLoadData;

    ifstream FullLoadFile(FileName);

    bool FileOk = FullLoadFile.good();

    if(FileOk)
    {
        while(!FullLoadFile.eof())
        {
            FullLoadFile>>SpeedKey;
            FullLoadFile>>FullLoadData.Bmep;
            FullLoadFile>>FullLoadData.Torque;
            FullLoadFile>>FullLoadData.Power;

            FullLoadMap[SpeedKey] = FullLoadData;
        }
    }

    FullLoadFile.close();

    return FileOk;
}

bool EngineTool::_Load2DMap(string FileName, Map2D& MapLoad)
{
    ifstream DFile(FileName);
    string str;
    float Speed;
    float Bmep;
    float MapValue;
    vector<float> MapValueArray;
    bool FileOk = DFile.good();

    if(FileOk)
    {
        getline(DFile, str);
        istringstream ss(str);
        while(ss >> Speed)
        {
            MapLoad.RpmAxis.push_back(Speed);
        }


        while(!DFile.eof())
        {
            DFile>>Bmep;
            MapLoad.BmepAxis.push_back(Bmep);
            for(unsigned int speed = 0; speed < MapLoad.RpmAxis.size(); speed++)
            {
                DFile>>MapValue;
                MapValueArray.push_back(MapValue);
            }
            MapLoad.Values.push_back(MapValueArray);
            MapValueArray.clear();
        }
    }

    DFile.close();
    return FileOk;
}

float EngineTool::_Interpolate2DMap(float Rpm, float Bmep, float Power, float IdleValue, Map2D Map)
{
    float Value = -1;

    if(Bmep < Map.BmepAxis[0] ||
            Rpm < Map.RpmAxis[0])
    {
        if((StartStopIsOn || CutOffIsOn) &&
                ((Power < 0 && Rpm > 1000) || Rpm == 0))
        {
            Value = 0;
        }
        else
        {
            Value = IdleValue;
        }
    }
    else
    {
        float Value1, Value2;
        unsigned int RpmPos;
        unsigned int BmepPos;
        bool RpmOK = false;
        bool BmeoOK = false;

        for(RpmPos = 0; RpmPos < (Map.RpmAxis.size() - 1); RpmPos++)
        {
            if(Rpm >= Map.RpmAxis[RpmPos] && Rpm <= Map.RpmAxis[RpmPos+1])
            {
                RpmOK = true;
                break;
            }
        }

        for(BmepPos = 0; BmepPos < (Map.BmepAxis.size() - 1); BmepPos++)
        {
            if(Bmep >= Map.BmepAxis[BmepPos] && Bmep <= Map.BmepAxis[BmepPos+1])
            {
                BmeoOK = true;
                break;
            }
        }

        if(RpmOK && BmeoOK)
        {
            //interpolate in x
            Value1 = (Map.Values[BmepPos][RpmPos+1] - Map.Values[BmepPos][RpmPos]) *
                    (Rpm - Map.RpmAxis[RpmPos]) / (Map.RpmAxis[RpmPos+1] - Map.RpmAxis[RpmPos])
                    + Map.Values[BmepPos][RpmPos];

            Value2 = (Map.Values[BmepPos+1][RpmPos+1] - Map.Values[BmepPos+1][RpmPos]) *
                    (Rpm - Map.RpmAxis[RpmPos]) / (Map.RpmAxis[RpmPos+1] - Map.RpmAxis[RpmPos])
                    + Map.Values[BmepPos+1][RpmPos];

            //interpolat in y
            Value = (Value2 - Value1) *
                    (Bmep - Map.BmepAxis[BmepPos]) / (Map.BmepAxis[BmepPos+1] - Map.BmepAxis[BmepPos]) + Value1;
        }
    }
    return Value;
}

bool EngineTool::_LoadGearProfileData(string GearProfilePath, vector<int>& GearProfile)
{
    ifstream GearProfileFile(GearProfilePath);
    int Gear;
    bool FileOk = GearProfileFile.good();

    while(!GearProfileFile.eof() && FileOk)
    {
        GearProfileFile>>Gear;
        GearProfile.push_back(Gear);
    }

    GearProfileFile.close();
    return FileOk;
}

bool EngineTool::_LoadCycleVehicleData(string FileName, string CycleType, CycleVehicleData& CycleVehData, vector<int>& GearProfile)
{
    ifstream CycleVehicleFile;
    float CoastDown;
    bool GearProfileOk;
    string GearProfileFile;

    if(CycleType != "")
    {
        while(*(FileName.end()-1) != '.')
        {
            FileName.erase(FileName.end()-1);
        }
        FileName.erase(FileName.end()-1);

        FileName = FileName + CycleType + ".dat";

        CycleVehicleFile.open(FileName);

        bool FileOk = CycleVehicleFile.good();

        if(FileOk)
        {
            CycleVehicleFile>>CycleVehData.Mass;

            for(int i = 0; i < 3; i++)
            {
                CycleVehicleFile>>CoastDown;
                CycleVehData.CoastDownCoeff.push_back(CoastDown);
            }

            if(CycleType == "WLTC")
            {
                CycleVehicleFile>>GearProfileFile;
                GearProfileOk = _LoadGearProfileData(SelectedGearFilePath + GearProfileFile, GearProfile);
                return FileOk && GearProfileOk;
            }
        }
        CycleVehicleFile.close();

        return FileOk;
    }
    return false;
}

float EngineTool::_CalculateBmep(float Rpm, float Power)
{
    float Bmep;

    if(Rpm != 0 && Power > 0)
    {
        map<float, FullLoad>::iterator succ = ++Vehicle.Engine.FullLoadMap.begin();
        float BmepFullLoad = 0;
        bool RpmIsInMap = false;

        Bmep = 1200 * Power / (Vehicle.Engine.Displacement * Rpm);

        for(auto map = Vehicle.Engine.FullLoadMap.begin(); map != --Vehicle.Engine.FullLoadMap.end(); map++)
        {
            if(Rpm >= map->first && Rpm <= succ->first)
            {
                BmepFullLoad = (succ->second.Bmep - map->second.Bmep) *
                        (Rpm - map->first) / (succ->first - map->first) + map->second.Bmep;
                RpmIsInMap = true;
                break;
            }

            succ++;
        }

        if((Bmep > BmepFullLoad) && RpmIsInMap)
        {
            Bmep = BmepFullLoad;
        }
    }
    else
    {
        Bmep = 0; // the engine doesn't transform mechanical energy, but it dissipates it
    }

    return Bmep;
}

bool EngineTool::LoadCycle(string FilePath)
{
    ifstream CycleFile(FilePath);
    float Time;
    float Speed;
    int SelectedGear;
    bool FileOk = CycleFile.good();
    string FileCheck;

    if(FileOk)
    {
        CycleFile>>FileCheck;

        if(FileCheck != "Cycle")
        {
            CycleFile.close();
            return false;
        }

        Driving = {};
        Vehicle = {};
        LoadEngineOK = false;
        LoadVehicleOK = false;

        CycleFile>>Driving.Type;

        if(Driving.Type == "WLTC")
        {
            while(*(FilePath.end()-1) != '/')
            {
                FilePath.erase(FilePath.end()-1);
            }
            SelectedGearFilePath = FilePath;
        }
        while(!CycleFile.eof())
        {
            CycleFile>>Time;
            Driving.Time.push_back(Time);
            CycleFile>>Speed;
            Driving.Speed.push_back(Speed);

            if(Driving.Type != "WLTC")
            {
                CycleFile>>SelectedGear;
                Driving.SelectedGear.push_back(SelectedGear);
            }
        }
    }

    CycleFile.close();

    LoadCycleOK = FileOk;

    return FileOk;
}

bool EngineTool::LoadEngine(string FilePath)
{
    ifstream EngineFile(FilePath);
    string FullLoadPath;
    string FuelConsumptionPath;
    string NOxEmissionsPath;
    bool FullLoadOk;
    bool FuelConsumptionOk;
    bool NOxEmissionOk;
    bool FileOk = EngineFile.good();
    string FileCheck;

    if(FileOk)
    {
        EngineFile>>FileCheck;

        if(FileCheck != "Engine")
        {
            EngineFile.close();
            return false;
        }

        EngineFile>>Vehicle.Engine.Model;
        EngineFile>>Vehicle.Engine.Displacement;
        EngineFile>>Vehicle.Engine.Inertia;
        EngineFile>>Vehicle.Engine.IdleSpeed;
        EngineFile>>Vehicle.Engine.IdleConsumption;
        EngineFile>>Vehicle.Engine.FuelDensity;

        EngineFile>>FullLoadPath;
        EngineFile>>FuelConsumptionPath;
        EngineFile>>NOxEmissionsPath;

        while(*(FilePath.end()-1) != '/')
        {
            FilePath.erase(FilePath.end()-1);
        }

        FullLoadPath = FilePath + FullLoadPath;
        FuelConsumptionPath = FilePath + FuelConsumptionPath;
        NOxEmissionsPath = FilePath + NOxEmissionsPath;

        FullLoadOk = _LoadFullLoadMap(FullLoadPath, Vehicle.Engine.FullLoadMap);
        FuelConsumptionOk = _Load2DMap(FuelConsumptionPath, Vehicle.Engine.FuelConsumptionMap);
        NOxEmissionOk = _Load2DMap(NOxEmissionsPath, Vehicle.Engine.NOxEmissionMap);
    }

    EngineFile.close();

    LoadEngineOK = (FileOk && FullLoadOk && FuelConsumptionOk && NOxEmissionOk);

    return (FileOk && FullLoadOk && FuelConsumptionOk && NOxEmissionOk);

}

bool EngineTool::LoadVehicle(string FilePath)
{
    ifstream VehicleFile(FilePath);
    int NumberGear;
    float GearRatio;
    float GearEff;
    bool VehicleCycleOk;
    string GearProfileFile;
    bool FileOk = VehicleFile.good();
    string FileCheck;

    if(FileOk)
    {
        VehicleFile>>FileCheck;

        if(FileCheck != "Vehicle")
        {
            VehicleFile.close();
            return false;
        }

        Vehicle = {};
        LoadEngineOK = false;

        VehicleFile>>Vehicle.Model;
        VehicleFile>>NumberGear;
        VehicleFile>>Vehicle.GearBoxDifferential.DifferentalRatio;

        Vehicle.GearBoxDifferential.GearShiftRatio.push_back(0); //neutral
        for(int gear = 0; gear < NumberGear; gear++)
        {
            VehicleFile>>GearRatio;
            Vehicle.GearBoxDifferential.GearShiftRatio.push_back(GearRatio);
        }

        VehicleFile>>Vehicle.GearBoxDifferential.DifferentalEfficiency;

        Vehicle.GearBoxDifferential.GearShiftEfficiency.push_back(1); //neutral

        for(int gear = 0; gear < NumberGear; gear++)
        {
            VehicleFile>>GearEff;
            Vehicle.GearBoxDifferential.GearShiftEfficiency.push_back(GearEff);
        }

        VehicleFile>>Vehicle.Wheel.Inertia;
        VehicleFile>>Vehicle.Wheel.Radius;

        VehicleCycleOk = _LoadCycleVehicleData(FilePath, Driving.Type, Vehicle.CycleVehicleData, Driving.SelectedGear);
    }

    VehicleFile.close();

    LoadVehicleOK = FileOk && VehicleCycleOk;
    return FileOk && VehicleCycleOk;
}

void EngineTool::SetStartStop(bool StartStop)
{
    StartStopIsOn = StartStop;
}

void EngineTool::SetCutOff(bool CutOff)
{
    CutOffIsOn = CutOff;
}

bool EngineTool::RunCycle(void)
{
    if(LoadCycleOK && LoadEngineOK && LoadVehicleOK)
    {
        float EngineRpm;
        float SelectedeGearRatio;
        float SelectedeGearEfficiency;
        float CoastDownForce;
        float SpeedStep;
        float ApparentMass;
        float Acceleration;
        float RequiredPower;
        float EnginePower;
        float Bmep;
        float Kmeters;
        float FuelConsTot;
        vector<float> RequiredPowerArray;
        vector<float> EnginePowerArray;

        Results = {};

        for(unsigned int SimStep = 0; SimStep < Driving.Time.size(); SimStep++)
        {
            SelectedeGearRatio = Vehicle.GearBoxDifferential.GearShiftRatio[Driving.SelectedGear[SimStep]];
            SpeedStep = Driving.Speed[SimStep];

            EngineRpm = SpeedStep / 3.6 / (Vehicle.Wheel.Radius / 1000) * 30 / M_PI
                    * Vehicle.GearBoxDifferential.DifferentalRatio *SelectedeGearRatio; // to convert in rpm

            if((EngineRpm < Vehicle.Engine.IdleSpeed)
                    && !(StartStopIsOn && EngineRpm == 0 && Driving.Time[SimStep] > 12))
            {
                EngineRpm = Vehicle.Engine.IdleSpeed;
            }

            Results.EngineSpeed.push_back(EngineRpm);

            CoastDownForce = Vehicle.CycleVehicleData.CoastDownCoeff[0] +
                    Vehicle.CycleVehicleData.CoastDownCoeff[1] * SpeedStep +
                    Vehicle.CycleVehicleData.CoastDownCoeff[2] * pow(SpeedStep,2);

            ApparentMass = Vehicle.CycleVehicleData.Mass +
                    Vehicle.Wheel.Inertia / pow(Vehicle.Wheel.Radius / 1000, 2) +
                    Vehicle.Engine.Inertia / pow(Vehicle.Wheel.Radius / 1000, 2) *
                    pow(SelectedeGearRatio * Vehicle.GearBoxDifferential.DifferentalRatio, 2);

            if(SimStep == 0) //for the first step it's impossible to calculate the derivative
            {
                Acceleration = 0;
            }
            else
            {
                Acceleration = (Driving.Speed[SimStep] - Driving.Speed[SimStep-1]) / 3.6 /
                        (Driving.Time[SimStep] - Driving.Time[SimStep-1]);
            }

            RequiredPower = (CoastDownForce + ApparentMass * Acceleration) * SpeedStep / 3600;
            // 3600 to convert km/h in m/s and W in kW

            SelectedeGearEfficiency = Vehicle.GearBoxDifferential.GearShiftEfficiency[Driving.SelectedGear[SimStep]];
            EnginePower = RequiredPower / (SelectedeGearEfficiency * Vehicle.GearBoxDifferential.DifferentalEfficiency);

            Bmep = _CalculateBmep(EngineRpm, EnginePower);

            Results.Bmep.push_back(Bmep);

            Results.FuelCons.push_back(_Interpolate2DMap(EngineRpm, Bmep, EnginePower, Vehicle.Engine.IdleConsumption/1000,
                                                 Vehicle.Engine.FuelConsumptionMap));
            Results.NOxEmission.push_back(_Interpolate2DMap(EngineRpm, Bmep, EnginePower, Vehicle.Engine.NOxEmissionMap.Values[0][0],
                                                    Vehicle.Engine.NOxEmissionMap));
            if(RequiredPower < 0)
            {
                RequiredPowerArray.push_back(- RequiredPower);
            }
            else
            {
                RequiredPowerArray.push_back(0);
            }

            if(EnginePower > 0)
            {
                EnginePowerArray.push_back(EnginePower);
            }
            else
            {
                EnginePowerArray.push_back(0);
            }
        }

        Kmeters = Calculate.Integrate(Driving.Time, Driving.Speed) / 3600; //km
        Results.BreakEnergySpec = Calculate.Integrate(Driving.Time, RequiredPowerArray) / Kmeters; //kJ
        Results.EngineEnergySpec = Calculate.Integrate(Driving.Time, EnginePowerArray) / Kmeters; //kJ
        FuelConsTot = Calculate.Integrate(Driving.Time, Results.FuelCons) / 3.6; //g
        Results.ConsFor100Km = FuelConsTot / Vehicle.Engine.FuelDensity / Kmeters * 100; //[l/100 km]
        // 31.5 comes from balacing the fuel combustion reaction
        Results.CO2EmissionSpec = FuelConsTot / Kmeters * 100 / 31.5; //[g/km]
        Results.NOxEmissionSpec = Calculate.Integrate(Driving.Time, Results.NOxEmission) / Kmeters / 3600; //[g/km]
    }

    ResultsOk = LoadCycleOK && LoadEngineOK && LoadVehicleOK;

    return ResultsOk;
}

DrivingCycle EngineTool::getCycle(void)
{
    return Driving;
}

CycleResults EngineTool::getResults(void)
{
    return Results;
}

string EngineTool::getVehicleModel(void)
{
    return Vehicle.Model;
}

string EngineTool::getEngineModel(void)
{
    return Vehicle.Engine.Model;
}

EngineData EngineTool::getEngineMap(void)
{
    return Vehicle.Engine;
}
bool EngineTool::getCutOff(void)
{
    return CutOffIsOn;
}

bool EngineTool::getStartStop(void)
{
    return StartStopIsOn;
}

void EngineTool::ResetTool(void)
{
    Driving = {};
    Vehicle = {};
    LoadEngineOK = false;
    LoadVehicleOK = false;
    LoadCycleOK = false;
    ResultsOk = false;
    StartStopIsOn = false;
    CutOffIsOn = false;
}

bool EngineTool::ExportResults(string FilePath)
{
    ofstream ExportFile(FilePath, ios::out);

    if(!ExportFile.is_open())
    {
        return false;
    }
    ExportFile<<"EngineSpeed"<<endl;
    for(auto rpm = Results.EngineSpeed.begin(); rpm != Results.EngineSpeed.end(); rpm++)
    {
        ExportFile<<*rpm<<endl;
    }
    ExportFile<<"Bmep"<<endl;
    for(auto bmep = Results.Bmep.begin(); bmep != Results.Bmep.end(); bmep++)
    {
        ExportFile<<*bmep<<endl;
    }
    ExportFile<<"EngineEnergy"<<endl;
    ExportFile<<Results.EngineEnergySpec<<endl;
    ExportFile<<"BrakingEnergy"<<endl;
    ExportFile<<Results.BreakEnergySpec<<endl;
    ExportFile<<"FuelConsumption"<<endl;
    ExportFile<<Results.ConsFor100Km<<endl;
    ExportFile<<"CO2Emission"<<endl;
    ExportFile<<Results.CO2EmissionSpec<<endl;
    ExportFile<<"NOxEmission"<<endl;
    ExportFile<<Results.NOxEmissionSpec<<endl;

    ExportFile.close();

    return true;

}





