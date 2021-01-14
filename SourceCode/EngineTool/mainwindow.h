#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QChartView>
#include <QBarSet>
#include <QBarSeries>
#include <QLegend>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QFileDialog>
#include <QLineSeries>
#include <QScatterSeries>
#include <QColorDialog>

#include <enginetool.h>
#include <flywheeltool.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_CycleButton_clicked();
    void on_VehicleButton_clicked();
    void on_EngineButton_clicked();
    void on_ResetButton_clicked();
    void on_RunButton_clicked();
    void on_CutOffButton_clicked(bool checked);
    void on_StartStopButton_clicked(bool checked);
    void on_CycleButton_2_clicked();
    void on_VehicleButton_2_clicked();
    void on_EngineButton_2_clicked();
    void on_CutOffButton_2_toggled(bool checked);
    void on_StartStopButton_2_toggled(bool checked);
    void on_ResetButton_2_clicked();
    void on_VeSpeedCheckB_toggled(bool checked);
    void on_EngSpeedCheckB_toggled(bool checked);
    void on_OperCheckB_toggled(bool checked);
    void on_FuelConsCheckB_toggled(bool checked);
    void on_NOxCheckB_toggled(bool checked);
    void on_EnergyCheckB_toggled(bool checked);
    void on_ExportButton_clicked();

    void on_LoadButton_clicked();
    void on_CalcButton_clicked();
    void on_EPressCheckB_toggled(bool checked);
    void on_IndCycleCheckB_toggled(bool checked);
    void on_EffPressCheckB_toggled(bool checked);
    void on_CTorqueCheckB_toggled(bool checked);
    void on_TTorqueCheckB_toggled(bool checked);
    void on_EngWorkCheckB_toggled(bool checked);
    void on_EngSpCheckB_toggled(bool checked);
    void on_FouSpecCheckB_toggled(bool checked);

    void on_ExpButton_clicked();

private:
    Ui::MainWindow *ui;
    EngineTool Simulation1;
    EngineTool Simulation2;
    QChartView *VehSpeed;
    QChartView *EngSpeed;
    QChartView *OperPoints;
    QChartView *FuelCons;
    QChartView *NOxEmis;
    QChartView *Energy;
    bool RunSim1;
    bool RunSim2;

    FlyWheelTool FlyWheel;
    QChartView *EngPress;
    QChartView *IndCycle;
    QChartView *EffPress;
    QChartView *CylTorque;
    QChartView *EngTorque;
    QChartView *EngWork;
    QChartView *ESpeed;
    QChartView *FouSpect;
    bool RunFly;

    void _CreateChartSeries(bool show, QString ChartTitle, QChartView *ChartView,
                                                 vector<float> Xdata,  QString XAxisTitle,
                                                 vector<float> Ydata,  QString YAxisTitle, QString DataLabel,
                                                 vector<float> Xdata2, vector<float> Ydata2, QString DataLabel2);
    void _CreateChartSeries( bool show, QString ChartTitle, QChartView *ChartView,
                             vector<float> Xdata,  QString XAxisTitle,
                             vector<vector<float>> Ydata,  QString YAxisTitle, QStringList DataLabe = {},
                             vector<float> Xdata2 = {}, vector<float> Ydata2 = {}, QString DataLabe2 = "", QStringList DataLabelList = {});
};
#endif // MAINWINDOW_H
