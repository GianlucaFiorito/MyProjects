#include "flywheeltool.h"
#include <fstream>
#include <math.h>

FlyWheelTool::FlyWheelTool()
{

}

FlyWheelTool::~FlyWheelTool()
{

}
void FlyWheelTool::_SaveVector(ofstream& File, string DataTitle, vector<float> Data)
{
    File<<DataTitle<<endl;
    for(auto ptr = Data.begin(); ptr != Data.end(); ptr++)
    {
        File<<*ptr<<endl;
    }
}

bool FlyWheelTool::_CollectedDataLoad(string PathFile)
{
    ifstream DataFile(PathFile);
    float Angle;
    float Pressure;
    bool FileOk = DataFile.good();
    CollectedData = {};

    while(!DataFile.eof() && FileOk)
    {
        DataFile>>Angle;
        CollectedData.CrankAngle.push_back(Angle);
        DataFile>>Pressure;
        CollectedData.Pressure.push_back(Pressure);
    }

    DataFile.close();
    return FileOk;
}

bool FlyWheelTool::LoadData(string PathFile)
{
    ifstream DataFile(PathFile);
    bool FileOk = DataFile.good();
    bool FileDataOK = false;
    string FileCheck;
    string CollData;

    TestCond = {};
    Engine = {};

    if(FileOk)
    {
        DataFile>>FileCheck;
        if(FileCheck != "TestEngine")
        {
            DataFile.close();
            return false;
        }

        DataFile>>TestCond.AmbientPress;
        DataFile>>TestCond.AmbientTemp;
        DataFile>>TestCond.EngineSpeed;
        DataFile>>TestCond.EngineTorque;
        DataFile>>TestCond.RelDosage;
        DataFile>>TestCond.VolumeFuel;
        DataFile>>TestCond.TimeTest;

        DataFile>>Engine.CylicersNumber;
        DataFile>>Engine.TotalDisplacement;
        DataFile>>Engine.CompRatio;
        DataFile>>Engine.Bore;
        DataFile>>Engine.Stroke;
        DataFile>>Engine.ConnRod;
        DataFile>>Engine.SpecAltMass;
        DataFile>>Engine.SpecRotMass;
        DataFile>>Engine.IntakeOpenAdv;
        DataFile>>Engine.IntakeClosDelay;
        DataFile>>Engine.ExaustOpenAdv;
        DataFile>>Engine.ExaustClosDelay;
        DataFile>>Engine.Fuel.Density;
        DataFile>>Engine.Fuel.NetCalValue;
        DataFile>>Engine.Fuel.StDosage;

        while(*(PathFile.end()-1) != '/')
        {
            PathFile.erase(PathFile.end()-1);
        }

        DataFile>>CollData;
        PathFile += CollData;

        FileDataOK = _CollectedDataLoad(PathFile);

    }

    DataFile.close();

    LoadOk = FileOk && FileDataOK;

    return LoadOk;
}

bool FlyWheelTool::RunCycle(float FWdensity, float FWirreg)
{
    if(LoadOk)
    {
        Results = {};
        int phase = CollectedData.CrankAngle.size() / Engine.CylicersNumber;
        float CrankRodRatio = Engine.Stroke / 2 /Engine.ConnRod;
        float SpecDisplacement = Engine.TotalDisplacement / Engine.CylicersNumber / 1000; //[dm^3]
        float MinVolume = SpecDisplacement / (Engine.CompRatio - 1); //[dm^3]
        float AvgEngSpeed = TestCond.EngineSpeed * M_PI / 30; // [rad/s]
        vector<float> ResTorque; //[Nm]
        vector<float> CylTorque; //[Nm]
        vector<vector<float>> EngineTorque(Engine.CylicersNumber, vector<float>(CollectedData.CrankAngle.size() , 0));//[Nm]
        float EnginePower;
        float IndWork;
        float Imep;
        float AirFlow;
        float AirDensity;
        float DWmax;
        float DWmin;
        float TotJerk;
        float FWJerk;
        float SpdDiff;
        float MaxSpeed;
        float MinSpeed;

        //Torque Calculation
        for(unsigned int crkstep = 0; crkstep < CollectedData.CrankAngle.size(); crkstep++)
        {
            float CosBeta; //[-]
            float Beta; //[rad]
            float PistonPos; //[m/m]
            float PistonAccel; //[m/s^2]
            float InPressure; //[bar]
            float EffPressure; //[bar]
            float Torque; //[Nm]

            CosBeta = sqrt(1 - pow(CrankRodRatio, 2) * pow(sin(Calculate.deg2rad(CollectedData.CrankAngle[crkstep])), 2));
            PistonPos = (1 / CrankRodRatio * (1 - CosBeta) + (1 - cos(Calculate.deg2rad(CollectedData.CrankAngle[crkstep])))) / 2;

            Results.ChamberVolume.push_back(MinVolume + SpecDisplacement * PistonPos);

            if(CollectedData.CrankAngle[crkstep] == (180 + Engine.IntakeClosDelay) ||
                    CollectedData.CrankAngle[crkstep] == (540 - Engine.ExaustOpenAdv) ||
                    CollectedData.CrankAngle[crkstep] == (720 - Engine.IntakeOpenAdv) ||
                    CollectedData.CrankAngle[crkstep] == Engine.ExaustClosDelay)
            {
                Results.ValveTiming.ChambVolume.push_back(Results.ChamberVolume[crkstep]);
                Results.ValveTiming.Pressure.push_back(CollectedData.Pressure[crkstep]);

                if(CollectedData.CrankAngle[crkstep] == (180 + Engine.IntakeClosDelay))
                {
                    Results.ValveTiming.Type.push_back("Intake Close Delay");
                }
                else if(CollectedData.CrankAngle[crkstep] == (540 - Engine.ExaustOpenAdv))
                {
                    Results.ValveTiming.Type.push_back("Exaust Open Advance");
                }
                else if(CollectedData.CrankAngle[crkstep] == (720 - Engine.IntakeOpenAdv))
                {
                    Results.ValveTiming.Type.push_back("Intake Open Advance");
                }
                else
                {
                     Results.ValveTiming.Type.push_back("Exaust Close Delay");
                }
            }

            PistonAccel = pow(AvgEngSpeed, 2) * Engine.Stroke / 2  * (cos(Calculate.deg2rad(CollectedData.CrankAngle[crkstep])) +
                                                                            CrankRodRatio * cos(Calculate.deg2rad(2 * CollectedData.CrankAngle[crkstep])) /
                                                                            CosBeta) / 1000;
            InPressure  = - Engine.SpecAltMass * SpecDisplacement * PistonAccel / (M_PI * pow(Engine.Bore/1000/2, 2)) / pow(10,5);
            Results.InertiaPressure.push_back(InPressure);

            EffPressure = CollectedData.Pressure[crkstep] - TestCond.AmbientPress / 760 * 1.01325 +InPressure;
            Results.EffectivePressure.push_back(EffPressure);

            if(CollectedData.CrankAngle[crkstep] <= 180 ||
                    (CollectedData.CrankAngle[crkstep] >= 360 && CollectedData.CrankAngle[crkstep] < 540))
            {
                Beta = acos(CosBeta);
            }
            else
            {
                Beta = -acos(CosBeta);
            }

            Torque = EffPressure * pow(10, 5) * SpecDisplacement / 2 / 1000 * sin(Calculate.deg2rad(CollectedData.CrankAngle[crkstep]) + Beta) / CosBeta;
            CylTorque.push_back(Torque);
            int a;
            for(unsigned int cylinder = 0; cylinder < Engine.CylicersNumber; cylinder++)
            {
                a = (cylinder * phase + crkstep) % (CollectedData.CrankAngle.size());
                EngineTorque[cylinder][a] = Torque;
            }
        }

        Results.AmbientPressure = (TestCond.AmbientPress / 760 * 1.01325);
        Results.CylinderEngineTorque = EngineTorque;

        for(unsigned int crkstep = 0; crkstep < CollectedData.CrankAngle.size(); crkstep++)
        {
            float TotalTorque = 0;
            for(unsigned int cylinder = 0; cylinder < Engine.CylicersNumber; cylinder++)
            {
                TotalTorque += EngineTorque[cylinder][crkstep];
            }
            Results.EngineTorque.push_back(TotalTorque);
        }

        Results.ResistentTorque = Calculate.Integrate(CollectedData.CrankAngle, Results.EngineTorque)
                / (CollectedData.CrankAngle[CollectedData.CrankAngle.size() - 1] - CollectedData.CrankAngle[0]);


        for(unsigned int crkstep = 0; crkstep < CollectedData.CrankAngle.size(); crkstep++)
        {
            ResTorque.push_back(Results.ResistentTorque);
        }

        //Engine Paramiters Calculation
        Results.AvgFuelFlow = TestCond.VolumeFuel / 1000 * Engine.Fuel.Density / TestCond.TimeTest;
        EnginePower = TestCond.EngineTorque * AvgEngSpeed / 1000; //[kW]
        Results.Bmep = 1200 * EnginePower / (Engine.TotalDisplacement / 1000) / TestCond.EngineSpeed;
        Results.UsEfficiency = EnginePower / (Results.AvgFuelFlow * Engine.Fuel.NetCalValue * 1000);
        Results.MaxEfficiency = (1 - 1 / pow(Engine.CompRatio, 0.35)) * TestCond.RelDosage; //k-1 = 1-1.35=0.35
        IndWork = Calculate.Integrate(Results.ChamberVolume, CollectedData.Pressure) *100; //[J]
        Imep = IndWork / SpecDisplacement / 100; //[bar]
        Results.OrgEfficiency = Results.Bmep / Imep;
        Results.TerFluEfficiency = Results.UsEfficiency / Results.MaxEfficiency / Results.OrgEfficiency;
        AirFlow = Results.AvgFuelFlow * Engine.Fuel.StDosage * TestCond.RelDosage;
        AirDensity = TestCond.AmbientPress / 760 * 101325 / 289 / (TestCond.AmbientTemp + 273.15); //[kg/m^3]
        Results.FillCoeff = AirFlow / (AirDensity * Engine.TotalDisplacement / pow(10, 6) * TestCond.EngineSpeed / 60 / 2);

        //FlyWheel Calculation
        Results.EngineWork = Calculate.CumIntegrate(Calculate.deg2rad(CollectedData.CrankAngle), Results.EngineTorque);
        Results.ResistentWork = Calculate.CumIntegrate(Calculate.deg2rad(CollectedData.CrankAngle), ResTorque);
        DWmax = Results.EngineWork[0] - Results.ResistentWork[0];
        DWmin = Results.EngineWork[0] - Results.ResistentWork[0];
        for(unsigned int crkstep = 0; crkstep < Results.EngineWork.size(); crkstep++)
        {
            float Diff = Results.EngineWork[crkstep] - Results.ResistentWork[crkstep];
            if(DWmax < Diff)
            {
                DWmax = Diff;
            }
            else if (DWmin > Diff)
            {
                DWmin = Diff;
            }
        }

        TotJerk = (DWmax - DWmin) / FWirreg / pow(AvgEngSpeed, 2);

        FWJerk = TotJerk - Engine.SpecRotMass * Engine.TotalDisplacement / 1000 * pow(Engine.Stroke/2/1000, 2);
        Results.FlyWDiameter = pow(FWJerk * 320 / M_PI / FWdensity, .2) * 1000;
        Results.FlyWDepth = Results.FlyWDiameter / 10;

        for(unsigned int crkstep = 0; crkstep < Results.EngineWork.size(); crkstep++)
        {
            Results.EngineSpeed.push_back(sqrt(pow(AvgEngSpeed, 2) + 2 / TotJerk
                                               * (Results.EngineWork[crkstep] - Results.ResistentWork[crkstep])));
            Results.EngineWork[crkstep] = Results.EngineWork[crkstep]/Engine.TotalDisplacement *10;
            Results.ResistentWork[crkstep] = Results.ResistentWork[crkstep]/Engine.TotalDisplacement * 10;
        }

        SpdDiff = Calculate.Integrate(CollectedData.CrankAngle, Results.EngineSpeed)
                / (CollectedData.CrankAngle[CollectedData.CrankAngle.size() - 1] - CollectedData.CrankAngle[0])
                - AvgEngSpeed;

        MaxSpeed = (Results.EngineSpeed[0] - SpdDiff) * 30 / M_PI;
        MinSpeed = (Results.EngineSpeed[0] - SpdDiff) * 30 / M_PI;
        for(unsigned int crkstep = 0; crkstep < Results.EngineSpeed.size(); crkstep++)
        {
            Results.EngineSpeed[crkstep] = (Results.EngineSpeed[crkstep] - SpdDiff) * 30 / M_PI;

            if(MaxSpeed < Results.EngineSpeed[crkstep])
            {
                MaxSpeed = Results.EngineSpeed[crkstep];
            }
            else if (MinSpeed > Results.EngineSpeed[crkstep])
            {
                MinSpeed = Results.EngineSpeed[crkstep];
            }
        }

        Results.IntAvgSpeed = Calculate.Integrate(CollectedData.CrankAngle, Results.EngineSpeed)
                / (CollectedData.CrankAngle[CollectedData.CrankAngle.size() - 1] - CollectedData.CrankAngle[0]);

        Results.AritAvgSpeed = (MaxSpeed + MinSpeed) / 2;

        Calculate.Fourier(CollectedData.CrankAngle, Results.EngineTorque,
                          Results.FreqSpectrum.Amplitude, Results.FreqSpectrum.Frequence, TestCond.EngineSpeed * 6);
    }

    return LoadOk;
}

bool FlyWheelTool::ExportResults(string FilePath)
{
    ofstream ExportFile(FilePath, ios::out);

    if(!ExportFile.is_open())
    {
        return false;
    }
    _SaveVector(ExportFile, "ChamberVolume", Results.ChamberVolume);
    _SaveVector(ExportFile, "InertiaPressure", Results.InertiaPressure);
    _SaveVector(ExportFile, "EffectivePressure", Results.EffectivePressure);
    _SaveVector(ExportFile, "EngineTorque", Results.EngineTorque);
    _SaveVector(ExportFile, "EngineWork", Results.EngineWork);
    _SaveVector(ExportFile, "ResistentWork", Results.ResistentWork);
    _SaveVector(ExportFile, "EngineSpeed", Results.EngineSpeed);

    ExportFile<<"CylinderEngineTorque"<<endl;
    for(unsigned int cyltor = 0; cyltor < Results.CylinderEngineTorque[0].size(); cyltor++)
    {
        for(unsigned int cyl = 0; cyl < Results.CylinderEngineTorque.size(); cyl++)
        {
            ExportFile<<Results.CylinderEngineTorque[cyl][cyltor]<<" ";
        }
        ExportFile<<endl;
    }

    ExportFile<<"AmbientPressure"<<endl;
    ExportFile<<Results.AmbientPressure<<endl;
    ExportFile<<"ResistentTorque"<<endl;
    ExportFile<<Results.ResistentTorque<<endl;
    ExportFile<<"AvgFuelFlow"<<endl;
    ExportFile<<Results.AvgFuelFlow<<endl;
    ExportFile<<"Bmep"<<endl;
    ExportFile<<Results.Bmep<<endl;
    ExportFile<<"UsEfficiency"<<endl;
    ExportFile<<Results.UsEfficiency<<endl;
    ExportFile<<"MaxEfficiency"<<endl;
    ExportFile<<Results.MaxEfficiency<<endl;
    ExportFile<<"OrgEfficiency"<<endl;
    ExportFile<<Results.OrgEfficiency<<endl;
    ExportFile<<"TerFluEfficiency"<<endl;
    ExportFile<<Results.TerFluEfficiency<<endl;
    ExportFile<<"FillCoeff"<<endl;
    ExportFile<<Results.FillCoeff<<endl;
    ExportFile<<"FlyWDiameter"<<endl;
    ExportFile<<Results.FlyWDiameter<<endl;
    ExportFile<<"FlyWDepth"<<endl;
    ExportFile<<Results.FlyWDepth<<endl;
    ExportFile<<"AritAvg"<<endl;
    ExportFile<<Results.AritAvgSpeed<<endl;
    ExportFile<<"IntAvg"<<endl;
    ExportFile<<Results.IntAvgSpeed<<endl;

    ExportFile<<"FreqSpectrum"<<endl;
    auto freq = Results.FreqSpectrum.Frequence.begin();
    for(auto amp = Results.FreqSpectrum.Amplitude.begin(); amp != Results.FreqSpectrum.Amplitude.end(); amp++)
    {
        ExportFile<<*freq<<" "<<*amp<<endl;
        freq++;
    }


    ExportFile.close();

    return true;

}

void FlyWheelTool::ResetTool(void)
{
    Engine = {};
    TestCond = {};
    CollectedData = {};
    Results = {};
    LoadOk = false;
    Calculate = {};
}

void FlyWheelTool::getEngineData(unsigned int& CylindersNum, float& Displacement)
{
    CylindersNum = Engine.CylicersNumber;
    Displacement = Engine.TotalDisplacement;
}

void FlyWheelTool::getTestCond(float& EngineSpeed, float& EngineTorque, float& RelDosag)
{
    EngineSpeed = TestCond.EngineSpeed;
    EngineTorque = TestCond.EngineTorque;
    RelDosag = TestCond.RelDosage;
}

FlyWheelResults FlyWheelTool::getResults(void)
{
    return Results;
}

MesuredData FlyWheelTool::getMesuredData(void)
{
    return CollectedData;
}
