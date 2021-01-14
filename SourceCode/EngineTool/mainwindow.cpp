#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    RunSim1 = false;
    RunSim2 = false;
    VehSpeed = new QChartView;
    EngSpeed = new QChartView;
    OperPoints = new QChartView;
    FuelCons = new QChartView;
    NOxEmis = new QChartView;
    Energy = new QChartView;

    RunFly = false;
    ui->DensLine->setText("7700");
    ui->IrrLine->setText("0.01");
    EngPress = new QChartView;
    IndCycle = new QChartView;
    EffPress = new QChartView;
    CylTorque = new QChartView;
    EngTorque = new QChartView;
    EngWork = new QChartView;
    ESpeed = new QChartView;
    FouSpect = new QChartView;
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::_CreateChartSeries( bool show, QString ChartTitle, QChartView *ChartView,
                                    vector<float> Xdata,  QString XAxisTitle,
                                    vector<float> Ydata,  QString YAxisTitle, QString DataLabe,
                                    vector<float> Xdata2, vector<float> Ydata2, QString DataLabe2)
{
    if(show)
    {
        int XMax = 0;
        int YMax = 0;

        ChartTitle+= " on ";

        if(Ydata.size() > 0 && Xdata.size() > 0 && Ydata2.size() > 0 && Xdata2.size() > 0)
        {
            ChartTitle+= QString::fromStdString(Simulation1.getCycle().Type) + " and " + QString::fromStdString(Simulation2.getCycle().Type) + " Cycles";
        }
        else if(Ydata.size() > 0 && Xdata.size() > 0)
        {
             ChartTitle+= QString::fromStdString(Simulation1.getCycle().Type) + " Cycle";
        }
        else if (Ydata2.size() > 0 && Xdata2.size() > 0)
        {
            ChartTitle+= QString::fromStdString(Simulation2.getCycle().Type) + " Cycle";
        }

        QLineSeries *Series = NULL;
        if(Ydata.size() > 0 && Xdata.size() > 0)
        {
            Series = new QLineSeries;

            if(Xdata.size() > 0 && XMax == 0)
            {
                XMax = Xdata[0];
            }

            if(Ydata.size() > 0 && YMax == 0)
            {
                YMax = Ydata[0];
            }

            for(unsigned int step = 0; step < Xdata.size(); step++)
            {
                Series->append(Xdata[step], Ydata[step]);

                if(XMax < Xdata[step])
                {
                    XMax = Xdata[step];
                }

                if(YMax < Ydata[step])
                {
                    YMax = Ydata[step];
                }
            }

            Series->setName(DataLabe);

        }

        QLineSeries *Series2 = NULL;

        if(Ydata2.size() > 0 && Xdata2.size() > 0)
        {
            Series2 = new QLineSeries;

            if(Xdata2.size() > 0 && XMax == 0)
            {
                XMax = Xdata2[0];
            }

            if(Ydata2.size() > 0 && YMax == 0)
            {
                YMax = Ydata2[0];
            }

            for(unsigned int step = 0; step < Xdata2.size(); step++)
            {
                Series2->append(Xdata2[step], Ydata2[step]);

                if(XMax < Xdata2[step])
                {
                    XMax = Xdata2[step];
                }

                if(YMax < Ydata2[step])
                {
                    YMax = Ydata2[step];
                }
            }

            Series2->setName(DataLabe2);

            QPen pen;
            pen.setColor(Qt::red);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            Series2->setPen(pen);
        }


        QChart *Chart = new QChart();
        if(Series != NULL)
        {
            Chart->addSeries(Series);
        }

        if(Series2 != NULL)
        {
            Chart->addSeries(Series2);
        }

        Chart->setTitle(ChartTitle);
        QFont font;
        font.setPixelSize(20);
        Chart->setTitleFont(font);
        Chart->legend()->setVisible(true);
        Chart->legend()->setAlignment(Qt::AlignRight);
        Chart->setAnimationOptions(QChart::SeriesAnimations);

        if(XMax%200 != 0)
        {
            XMax = (XMax/200 + 1) * 200;
        }

        QValueAxis *Xaxis = new QValueAxis;
        Xaxis->setRange(0, XMax);
        Xaxis->setTitleText(XAxisTitle);
        Xaxis->setTickCount(XMax/200 + 1);
        Chart->addAxis(Xaxis, Qt::AlignBottom);

        if(Series != NULL)
        {
            Series->attachAxis(Xaxis);
        }

        if(Series2 != NULL)
        {
            Series2->attachAxis(Xaxis);
        }

        QValueAxis *Yaxis = new QValueAxis;
        Yaxis->setTitleText(YAxisTitle);
        if(YMax < 10)
        {
            Yaxis->setTickCount(YMax + 1);
        }
        else if(YMax < 200)
        {
            YMax = (YMax/10 +1) * 10;
            Yaxis->setTickCount(YMax/10 + 1);
        }
        else if(YMax < 300)
        {
            YMax = (YMax/50 +1) * 50;
            Yaxis->setTickCount(YMax/200 + 1);
        }
        else
        {
            YMax = (YMax/200 +1) * 200;
            Yaxis->setTickCount(YMax/200 + 1);
        }
        Yaxis->setRange(0, YMax);
        Chart->addAxis(Yaxis, Qt::AlignLeft);

        if(Series != NULL)
        {
            Series->attachAxis(Yaxis);
        }

        if(Series2 != NULL)
        {
            Series2->attachAxis(Yaxis);
        }

        ChartView->setChart(Chart);
        ChartView->setGeometry(0, 0, 800, 600);
        ChartView->setRenderHint(QPainter::Antialiasing);

    }

    ChartView->setVisible(show);
}

void MainWindow::_CreateChartSeries( bool show, QString ChartTitle, QChartView *ChartView,
                                    vector<float> Xdata,  QString XAxisTitle,
                                    vector<vector<float>> Ydata,  QString YAxisTitle, QStringList DataLabe,
                                    vector<float> Xdata2, vector<float> Ydata2, QString DataLabe2, QStringList DataLabelList)
{
    if(show)
    {
        int XMax = 0;
        int YMax = 0;
        int YMin = 0;

        QLineSeries *Series = NULL;
        QScatterSeries *Series2 = NULL;
        QChart *Chart = new QChart();
        QValueAxis *Xaxis = new QValueAxis;
        QValueAxis *Yaxis = new QValueAxis;

        if(Ydata.size() > 0 && Xdata.size() > 0)
        {
           if(Xdata.size() > 0 && XMax == 0)
           {
               XMax = Xdata[0];
           }

           if(Ydata.size() > 0 && YMax == 0)
           {
               YMax = Ydata[0][0];
           }

           if(Ydata.size() > 0 && YMin == 0)
           {
               YMin = Ydata[0][0];
           }

           for(unsigned int ser = 0; ser <Ydata.size(); ser++)
           {
               Series = new QLineSeries;

               for(unsigned int step = 0; step < Ydata[ser].size(); step++)
               {
                   Series->append(Xdata[step], Ydata[ser][step]);

                   if(XMax < Xdata[step])
                   {
                       XMax = Xdata[step];
                   }

                   if(YMax < Ydata[ser][step])
                   {
                       YMax = Ydata[ser][step];
                   }

                   if(YMin > Ydata[ser][step])
                   {
                       YMin = Ydata[ser][step];
                   }
               }

               if(DataLabe.size() > 0)
               {
                   Series->setName(DataLabe[ser]);
               }
               Chart->addSeries(Series);
            }

           Xaxis->setTitleText(XAxisTitle);
           Xaxis->setTickCount(9);
           Xaxis->setRange(0, XMax);
           Chart->addAxis(Xaxis, Qt::AlignBottom);

           Yaxis->setTitleText(YAxisTitle);
           if((YMax - YMin) < 20)
           {
               if(YMin%2 == 0)
               {
                   YMin -= 2;
               }
               else
               {
                   YMin -= 3;
               }

               if(YMax%2 == 0)
               {
                   YMax += 2;
               }
               else
               {
                   YMax += 3;
               }

               Yaxis->setTickCount((YMax - YMin)/2 + 1);
           }
           else if((YMax - YMin) < 120)
           {
               YMin = (YMin/10 - 1) * 10;
               YMax = (YMax/10 +1) * 10;
               Yaxis->setTickCount((YMax - YMin)/10 + 1);
           }
           else
           {
               YMin = (YMin/50 - 1) * 50;
               YMax = (YMax/50 + 1) * 50;
               Yaxis->setTickCount((YMax - YMin)/50 + 1);
           }

           Chart->addAxis(Yaxis, Qt::AlignLeft);
           Yaxis->setRange(YMin, YMax);

           QList<QAbstractSeries *> SeriesList = Chart->series();

           for (int ser = 0; ser < SeriesList.size(); ser++)
           {
               SeriesList[ser]->attachAxis(Xaxis);
               SeriesList[ser]->attachAxis(Yaxis);
           }
        }

        if(Ydata2.size() > 0 && Xdata2.size() > 0)
        {
            for(unsigned int step = 0; step < Xdata2.size(); step++)
            {
                Series2 = new QScatterSeries;
                Series2->append(Xdata2[step], Ydata2[step]);

                Series2->setName(DataLabelList[step]);
                Chart->addSeries(Series2);
                Series2->attachAxis(Xaxis);
                Series2->attachAxis(Yaxis);

            }
        }

        Chart->setTitle(ChartTitle);
        QFont font;
        font.setPixelSize(20);
        Chart->setTitleFont(font);
        if(DataLabe.size() > 0 || DataLabe2 != "" || DataLabelList.size() > 0)
        {
            Chart->legend()->setVisible(true);
            Chart->legend()->setAlignment(Qt::AlignRight);
        }
        else
        {
            Chart->legend()->setVisible(false);
        }
        Chart->setAnimationOptions(QChart::SeriesAnimations);
        ChartView->setChart(Chart);
        ChartView->setGeometry(0, 0, 800, 600);
        ChartView->setRenderHint(QPainter::Antialiasing);

    }

    ChartView->setVisible(show);
}

void MainWindow::on_CycleButton_clicked()
{
    QFileDialog* dialog = new QFileDialog(this, "Select a file", "*.dat");
    dialog->exec();
    bool success = false;

    QStringList fileSelected = dialog->selectedFiles();

    if(fileSelected.size() > 0)
    {
        success = Simulation1.LoadCycle(fileSelected.at(0).toStdString());
    }

    if(success)
    {
        QString Label = QString::fromStdString(Simulation1.getCycle().Type);
        ui->CycleLine->setText(Label);
        ui->statusbar->showMessage("Load New Driving Cycle", 4000);
        ui->VehicleLine->clear();
        ui->EngineLine->clear();
    }
    else
    {
        ui->statusbar->showMessage("ERROR!!! File not opened: damanged or wrong", 4000);
    }
}

void MainWindow::on_VehicleButton_clicked()
{
    QFileDialog* dialog = new QFileDialog(this, "Select a file", "*.dat");
    dialog->exec();
    bool success = false;

    QStringList fileSelected = dialog->selectedFiles();

    if(fileSelected.size() > 0)
    {
        success = Simulation1.LoadVehicle(fileSelected.at(0).toStdString());
    }

    if(success)
    {
        QString Label = QString::fromStdString(Simulation1.getVehicleModel());
        ui->VehicleLine->setText(Label);
        ui->statusbar->showMessage("Load New Vehicle", 4000);
        ui->EngineLine->clear();
    }
    else
    {
        ui->statusbar->showMessage("ERROR!!! Try to load driving cycle or wrong file", 4000);
    }
}

void MainWindow::on_EngineButton_clicked()
{
    QFileDialog* dialog = new QFileDialog(this, "Select a file", "*.dat");
    dialog->exec();
    bool success = false;

    QStringList fileSelected = dialog->selectedFiles();

    if(fileSelected.size() > 0)
    {
        success = Simulation1.LoadEngine(fileSelected.at(0).toStdString());
    }

    if(success)
    {
        QString Label = QString::fromStdString(Simulation1.getEngineModel());
        ui->EngineLine->setText(Label);
        ui->statusbar->showMessage("Load New Engine", 4000);
    }
    else
    {
        ui->statusbar->showMessage("ERROR!!! Try to load driving cycle or wrong file", 4000);
    }
}

void MainWindow::on_ResetButton_clicked()
{
    Simulation1.ResetTool();
    ui->CycleLine->clear();
    ui->VehicleLine->clear();
    ui->EngineLine->clear();
    ui->CutOffButton->setStyleSheet("color: black");
    ui->CutOffButton->setChecked(false);
    ui->StartStopButton->setStyleSheet("color: black");
    ui->StartStopButton->setChecked(false);
    ui->statusbar->showMessage("Vehicle 1 cleared", 4000);
}

void MainWindow::on_CycleButton_2_clicked()
{
    QFileDialog* dialog = new QFileDialog(this, "Select a file", "*.dat");
    dialog->exec();
    bool success = false;

    QStringList fileSelected = dialog->selectedFiles();

    if(fileSelected.size() > 0)
    {
        success = Simulation2.LoadCycle(fileSelected.at(0).toStdString());
    }

    if(success)
    {
        QString Label = QString::fromStdString(Simulation2.getCycle().Type);
        ui->CycleLine_2->setText(Label);
        ui->statusbar->showMessage("Load New Driving Cycle", 4000);
        ui->VehicleLine_2->clear();
        ui->EngineLine_2->clear();
    }
    else
    {
        ui->statusbar->showMessage("ERROR!!! File not opened: damanged or wrong", 4000);
    }
}

void MainWindow::on_VehicleButton_2_clicked()
{
    QFileDialog* dialog = new QFileDialog(this, "Select a file", "*.dat");
    dialog->exec();
    bool success = false;

    QStringList fileSelected = dialog->selectedFiles();

    if(fileSelected.size() > 0)
    {
        success = Simulation2.LoadVehicle(fileSelected.at(0).toStdString());
    }

    if(success)
    {
        QString Label = QString::fromStdString(Simulation2.getVehicleModel());
        ui->VehicleLine_2->setText(Label);
        ui->statusbar->showMessage("Load New Vehicle", 4000);
        ui->EngineLine_2->clear();
    }
    else
    {
        ui->statusbar->showMessage("ERROR!!! Try to load driving cycle or wrong file", 4000);
    }
}

void MainWindow::on_EngineButton_2_clicked()
{
    QFileDialog* dialog = new QFileDialog(this, "Select a file", "*.dat");
    dialog->exec();
    bool success = false;

    QStringList fileSelected = dialog->selectedFiles();

    if(fileSelected.size() > 0)
    {
        success = Simulation2.LoadEngine(fileSelected.at(0).toStdString());
    }

    if(success)
    {
        QString Label = QString::fromStdString(Simulation2.getEngineModel());
        ui->EngineLine_2->setText(Label);
        ui->statusbar->showMessage("Load New Engine", 4000);
    }
    else
    {
        ui->statusbar->showMessage("ERROR!!! Try to load driving cycle or wrong file", 4000);
    }
}

void MainWindow::on_ResetButton_2_clicked()
{
    Simulation2.ResetTool();
    ui->CycleLine_2->clear();
    ui->VehicleLine_2->clear();
    ui->EngineLine_2->clear();
    ui->CutOffButton_2->setStyleSheet("color: black");
    ui->CutOffButton_2->setChecked(false);
    ui->StartStopButton_2->setStyleSheet("color: black");
    ui->StartStopButton_2->setChecked(false);
    ui->statusbar->showMessage("Vehicle 2 cleared", 4000);
}

void MainWindow::on_RunButton_clicked()
{
    RunSim1 = Simulation1.RunCycle();
    RunSim2 = Simulation2.RunCycle();
    ui->FuelConsLine->setText("");
    ui->CO2EmissLine->setText("");
    ui->NOxEmissionLine->setText("");
    ui->FuelConsLine_2->setText("");
    ui->CO2EmissLine_2->setText("");
    ui->NOxEmissionLine_2->setText("");

    if(RunSim1)
    {
        ui->statusbar->showMessage("Data Calculated", 4000);
        VehSpeed->hide();
        ui->VeSpeedCheckB->setChecked(false);
        EngSpeed->hide();
        ui->EngSpeedCheckB->setChecked(false);
        OperPoints->hide();
        ui->OperCheckB->setChecked(false);
        FuelCons->hide();
        ui->FuelConsCheckB->setChecked(false);
        NOxEmis->hide();
        ui->NOxCheckB->setChecked(false);

        ui->FuelConsLine->setText(QString::number(Simulation1.getResults().ConsFor100Km));
        ui->CO2EmissLine->setText(QString::number(Simulation1.getResults().CO2EmissionSpec));
        ui->NOxEmissionLine->setText(QString::number(Simulation1.getResults().NOxEmissionSpec));
    }

    if(RunSim2)
    {
        ui->statusbar->showMessage("Data Calculated", 4000);
        VehSpeed->hide();
        ui->VeSpeedCheckB->setChecked(false);
        EngSpeed->hide();
        ui->EngSpeedCheckB->setChecked(false);
        OperPoints->hide();
        ui->OperCheckB->setChecked(false);
        FuelCons->hide();
        ui->FuelConsCheckB->setChecked(false);
        NOxEmis->hide();
        ui->NOxCheckB->setChecked(false);

        ui->FuelConsLine_2->setText(QString::number(Simulation2.getResults().ConsFor100Km));
        ui->CO2EmissLine_2->setText(QString::number(Simulation2.getResults().CO2EmissionSpec));
        ui->NOxEmissionLine_2->setText(QString::number(Simulation2.getResults().NOxEmissionSpec));
    }

    if(!RunSim1 && !RunSim2)
    {
        ui->statusbar->showMessage("ERROR!!! Missing Data", 4000);
    }
}

void MainWindow::on_CutOffButton_clicked(bool checked)
{
    Simulation1.SetCutOff(checked);

    if(checked)
    {
        ui->CutOffButton->setStyleSheet("color: red");
    }
    else
    {
        ui->CutOffButton->setStyleSheet("color: black");
    }
}

void MainWindow::on_StartStopButton_clicked(bool checked)
{
    Simulation1.SetStartStop(checked);

    if(checked)
    {
        ui->StartStopButton->setStyleSheet("color: red");
    }
    else
    {
        ui->StartStopButton->setStyleSheet("color: black");
    }
}

void MainWindow::on_CutOffButton_2_toggled(bool checked)
{
    Simulation2.SetCutOff(checked);

    if(checked)
    {
        ui->CutOffButton_2->setStyleSheet("color: red");
    }
    else
    {
        ui->CutOffButton_2->setStyleSheet("color: black");
    }
}

void MainWindow::on_StartStopButton_2_toggled(bool checked)
{
    Simulation2.SetStartStop(checked);

    if(checked)
    {
        ui->StartStopButton_2->setStyleSheet("color: red");
    }
    else
    {
        ui->StartStopButton_2->setStyleSheet("color: black");
    }
}

void MainWindow::on_VeSpeedCheckB_toggled(bool checked)
{
    _CreateChartSeries(checked, "Vehicle Speed", VehSpeed,
                       Simulation1.getCycle().Time, "Time [s]",
                       Simulation1.getCycle().Speed, "Speed [km/h]", "Vehicle 1",
                       Simulation2.getCycle().Time, Simulation2.getCycle().Speed, "Vehicle 2");
}

void MainWindow::on_EngSpeedCheckB_toggled(bool checked)
{
    _CreateChartSeries(checked, "Engine Speed", EngSpeed,
                       Simulation1.getCycle().Time, "Time [s]",
                       Simulation1.getResults().EngineSpeed, "Speed [rpm]", "Vehicle 1",
                       Simulation2.getCycle().Time, Simulation2.getResults().EngineSpeed,  "Vehicle 2");
}

void MainWindow::on_OperCheckB_toggled(bool checked)
{
    if(checked)
    {
        EngineData Engine1 = Simulation1.getEngineMap();
        EngineData Engine2 = Simulation2.getEngineMap();
        CycleResults Results1 = Simulation1.getResults();
        CycleResults Results2 = Simulation2.getResults();

        float XMax = 0;
        float YMax = 0;

        QLineSeries *LineSeries = NULL;
        QScatterSeries *ScatterSeries = NULL;
        if(Results1.EngineSpeed.size() > 0 && Results1.Bmep.size() > 0)
        {
            if(Engine1.FullLoadMap.size() > 0 && XMax == 0 && YMax == 0)
            {
                XMax = Engine1.FullLoadMap.begin()->first;
                YMax = Engine1.FullLoadMap.begin()->second.Bmep;
            }

            LineSeries = new QLineSeries;
            ScatterSeries = new QScatterSeries;

            for(auto step = Engine1.FullLoadMap.begin(); step != Engine1.FullLoadMap.end(); step++)
            {
                LineSeries->append(step->first, step->second.Bmep);

                if(XMax < step->first)
                {
                    XMax = step->first;
                }

                if(YMax < step->second.Bmep)
                {
                    YMax = step->second.Bmep;
                }
            }

            LineSeries->setName("Full Load Vehicle 1");
            QPen pen;
            pen.setColor(Qt::blue);
            pen.setWidth(2);
            pen.setStyle(Qt::SolidLine);
            LineSeries->setPen(pen);


            for(unsigned int  step = 0; step < Results1.EngineSpeed.size(); step++)
            {
                ScatterSeries->append(Results1.EngineSpeed[step], Results1.Bmep[step]);

                if(XMax < Results1.EngineSpeed[step])
                {
                    XMax = Results1.EngineSpeed[step];
                }

                if(YMax < Results1.Bmep[step])
                {
                    YMax = Results1.Bmep[step];
                }
            }

            ScatterSeries->setName("Operative Points Vehicle 1");

            pen.setColor(Qt::darkMagenta);
            pen.setWidth(2);
            pen.setStyle(Qt::SolidLine);
            ScatterSeries->setPen(pen);
        }

        QLineSeries *LineSeries2 = NULL;
        QScatterSeries *ScatterSeries2 = NULL;
        if(Results2.EngineSpeed.size() > 0 && Results2.Bmep.size() > 0)
        {
            if(Engine2.FullLoadMap.size() > 0 && XMax == 0 && YMax == 0)
            {
                XMax = Engine2.FullLoadMap.begin()->first;
                YMax = Engine2.FullLoadMap.begin()->second.Bmep;
            }

            LineSeries2 = new QLineSeries;
            ScatterSeries2 = new QScatterSeries;

            for(auto step = Engine2.FullLoadMap.begin(); step != Engine2.FullLoadMap.end(); step++)
            {
                LineSeries2->append(step->first, step->second.Bmep);

                if(XMax < step->first)
                {
                    XMax = step->first;
                }

                if(YMax < step->second.Bmep)
                {
                    YMax = step->second.Bmep;
                }
            }

            LineSeries2->setName("Full Load Vehicle 2");

            QPen pen;
            pen.setColor(Qt::red);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            LineSeries2->setPen(pen);

            for(unsigned int  step = 0; step < Results2.EngineSpeed.size(); step++)
            {
                ScatterSeries2->append(Results2.EngineSpeed[step], Results2.Bmep[step]);

                if(XMax < Results2.EngineSpeed[step])
                {
                    XMax = Results2.EngineSpeed[step];
                }

                if(YMax < Results2.Bmep[step])
                {
                    YMax = Results2.Bmep[step];
                }
            }

            ScatterSeries2->setName("Operative Points Vehicle 2");

            pen.setColor(Qt::darkGreen);
            pen.setWidth(2);
            pen.setStyle(Qt::SolidLine);
            ScatterSeries2->setPen(pen);
        }

        QChart *Chart = new QChart();

        if(LineSeries != NULL && ScatterSeries != NULL)
        {
            Chart->addSeries(LineSeries);
            Chart->addSeries(ScatterSeries);
        }

        if(LineSeries2 != NULL && ScatterSeries2 != NULL)
        {
            Chart->addSeries(LineSeries2);
            Chart->addSeries(ScatterSeries2);
        }

        QString ChartTitle = "Operative points on ";
        if(Results1.EngineSpeed.size() > 0 && Results1.Bmep.size() > 0
                && Results2.EngineSpeed.size() > 0 && Results2.Bmep.size() > 0)
        {
            ChartTitle+= QString::fromStdString(Simulation1.getCycle().Type) + " and " + QString::fromStdString(Simulation2.getCycle().Type) + " Cycles";
        }
        else if(Results1.EngineSpeed.size() > 0 && Results1.Bmep.size() > 0)
        {
             ChartTitle+= QString::fromStdString(Simulation1.getCycle().Type) + " Cycle";
        }
        else if (Results2.EngineSpeed.size() > 0 && Results2.Bmep.size() > 0)
        {
            ChartTitle+= QString::fromStdString(Simulation2.getCycle().Type) + " Cycle";
        }

        Chart->setTitle(ChartTitle);
        QFont font;
        font.setPixelSize(20);
        Chart->setTitleFont(font);
        Chart->legend()->setVisible(true);
        Chart->legend()->setAlignment(Qt::AlignRight);
        Chart->setAnimationOptions(QChart::SeriesAnimations);

        QValueAxis *Xaxis = new QValueAxis;

        if(Engine1.IdleSpeed <= Engine2.IdleSpeed)
        {
            if(Results1.EngineSpeed.size() > 0 && Results1.Bmep.size() > 0)
            {
                Xaxis->setRange(Engine1.IdleSpeed, XMax);
            }
            else
            {
                Xaxis->setRange(Engine2.IdleSpeed, XMax);
            }
        }
        else
        {
            if(Results2.EngineSpeed.size() > 0 && Results2.Bmep.size() > 0)
            {
                Xaxis->setRange(Engine2.IdleSpeed, XMax);
            }
            else
            {
                Xaxis->setRange(Engine1.IdleSpeed, XMax);
            }
        }

        Xaxis->setTitleText("Engine Speed [rpm]");
        Xaxis->setTickCount(10);
        Chart->addAxis(Xaxis, Qt::AlignBottom);

        if(LineSeries != NULL && ScatterSeries != NULL)
        {
            LineSeries->attachAxis(Xaxis);
            ScatterSeries->attachAxis(Xaxis);
        }

        if(LineSeries2 != NULL && ScatterSeries2 != NULL)
        {
            LineSeries2->attachAxis(Xaxis);
            ScatterSeries2->attachAxis(Xaxis);
        }

        QValueAxis *Yaxis = new QValueAxis;
        Yaxis->setRange(0, 1.01*YMax);
        Yaxis->setTitleText("Bmep [bar]");
        Yaxis->setTickCount(10);
        Chart->addAxis(Yaxis, Qt::AlignLeft);

        if(LineSeries != NULL && ScatterSeries != NULL)
        {
            LineSeries->attachAxis(Yaxis);
            ScatterSeries->attachAxis(Yaxis);
        }

        if(LineSeries2 != NULL && ScatterSeries2 != NULL)
        {
            LineSeries2->attachAxis(Yaxis);
            ScatterSeries2->attachAxis(Yaxis);
        }

        OperPoints->setChart(Chart);
        OperPoints->setGeometry(0, 0, 800, 600);
        OperPoints->setRenderHint(QPainter::Antialiasing);
    }

    OperPoints->setVisible(checked);
}

void MainWindow::on_FuelConsCheckB_toggled(bool checked)
{    
    _CreateChartSeries(checked, "Fuel Consumption", FuelCons,
                       Simulation1.getCycle().Time, "Time [s]",
                       Simulation1.getResults().FuelCons, "Fuel [kg/h]", "Vehicle 1",
                       Simulation2.getCycle().Time, Simulation2.getResults().FuelCons, "Vehicle 2");
}

void MainWindow::on_NOxCheckB_toggled(bool checked)
{
    _CreateChartSeries(checked, "NOx Emission", NOxEmis,
                       Simulation1.getCycle().Time, "Time [s]",
                       Simulation1.getResults().NOxEmission, "NOx [g/h]", "Vehicle 1",
                       Simulation2.getCycle().Time, Simulation2.getResults().NOxEmission, "Vehicle 2");
}

void MainWindow::on_EnergyCheckB_toggled(bool checked)
{
    if(checked)
    {
        QBarSet *EngineEnergy = new QBarSet("Specific Engine Energy");
        QBarSet *BrakeEnergy = new QBarSet("Specific Brake Energy");
        CycleResults Results1 = Simulation1.getResults();
        CycleResults Results2 = Simulation2.getResults();
        float YMax = 0;

        if(Results1.EngineEnergySpec > 0 && Results1.BreakEnergySpec > 0)
        {
            *EngineEnergy<<Results1.EngineEnergySpec;
            *BrakeEnergy<<Results1.BreakEnergySpec;
            if(YMax < Results1.EngineEnergySpec || YMax < Results1.BreakEnergySpec)
            {
                if(Results1.EngineEnergySpec >= Results1.BreakEnergySpec)
                {
                    YMax = Results1.EngineEnergySpec;
                }
                else
                {
                    YMax = Results1.BreakEnergySpec;
                }
            }
        }

        if(Results1.EngineEnergySpec > 0 && Results1.BreakEnergySpec > 0)
        {
            *EngineEnergy<<Results2.EngineEnergySpec;
            *BrakeEnergy<<Results2.BreakEnergySpec;

            if(YMax < Results2.EngineEnergySpec || YMax < Results2.BreakEnergySpec)
            {
                if(Results2.EngineEnergySpec >= Results2.BreakEnergySpec)
                {
                    YMax = Results2.EngineEnergySpec;
                }
                else
                {
                    YMax = Results2.BreakEnergySpec;
                }
            }
        }

        QBarSeries *Series = new QBarSeries;
        Series->append(EngineEnergy);
        Series->append(BrakeEnergy);
        Series->setLabelsPosition(QAbstractBarSeries::LabelsCenter);

        QString ChartTitle = "Energy on ";
        QStringList categories;
        if(Results1.EngineEnergySpec > 0 && Results1.BreakEnergySpec > 0
                && Results2.EngineEnergySpec > 0 && Results2.BreakEnergySpec > 0)
        {
            ChartTitle+= QString::fromStdString(Simulation1.getCycle().Type) + " and " + QString::fromStdString(Simulation2.getCycle().Type) + " Cycles";
            categories<<"Vehicle 1";
            categories<<"Vehicle 2";
        }
        else if(Results1.EngineEnergySpec > 0 && Results1.BreakEnergySpec > 0)
        {
             ChartTitle+= QString::fromStdString(Simulation1.getCycle().Type) + " Cycle";
             categories<<"Vehicle 1";
        }
        else if (Results2.EngineEnergySpec > 0 && Results2.BreakEnergySpec > 0)
        {
            ChartTitle+= QString::fromStdString(Simulation2.getCycle().Type) + " Cycle";
            categories<<"Vehicle 2";
        }

        QChart *Chart = new QChart;
        Chart->addSeries(Series);
        Chart->setTitle(ChartTitle);
        QFont font;
        font.setPixelSize(20);
        Chart->setTitleFont(font);



        QBarCategoryAxis *Xaxis = new QBarCategoryAxis;
        Xaxis->append(categories);
        Chart->addAxis(Xaxis,Qt::AlignBottom);
        Chart->legend()->setVisible(true);
        Chart->legend()->setAlignment(Qt::AlignBottom);
        Chart->setAnimationOptions(QChart::SeriesAnimations);

        QValueAxis *Yaxis = new QValueAxis;
        Yaxis->setRange(0, 1.01*YMax);
        Yaxis->setTitleText("Specific Energy [kJ/km]");
        Yaxis->setTickCount(10);
        Chart->addAxis(Yaxis, Qt::AlignLeft);
        Series->attachAxis(Yaxis);

        Energy->setChart(Chart);
        Energy->setGeometry(0, 0, 800, 600);
        Energy->setRenderHint(QPainter::Antialiasing);
    }

    Energy->setVisible(checked);
}

void MainWindow::on_ExportButton_clicked()
{
    if(RunSim1 || RunSim2)
    {
        QString Status = "Saved: ";
        string Path;

        if(RunSim1)
        {
            Path = QDir::currentPath().toStdString() + "/SavedData/DrivingCycle/"
                    + Simulation1.getVehicleModel() + Simulation1.getCycle().Type;
            if(Simulation1.getStartStop())
            {
                Path += "StartStop";
            }
            else if(Simulation1.getCutOff())
            {
                Path += "CutOff";
            }
            Simulation1.ExportResults(Path + ".dat");
            Status += "Vehicle1 ";
        }

        if(RunSim2)
        {
            Path = QDir::currentPath().toStdString() + "/SavedData/DrivingCycle/"
                    + Simulation2.getVehicleModel() + Simulation2.getCycle().Type;
            if(Simulation2.getStartStop())
            {
                Path += "StartStop";
            }
            else if(Simulation2.getCutOff())
            {
                Path += "CutOff";
            }
            Simulation2.ExportResults(Path + ".dat");
            Status += "Vehicle2";
        }
        ui->statusbar->showMessage(Status, 4000);
    }
    else
    {
        ui->statusbar->showMessage("ERROR!! No Data Available", 4000);
    }
}

void MainWindow::on_LoadButton_clicked()
{
    QFileDialog* dialog = new QFileDialog(this, "Select a file", "*.dat");
    dialog->exec();
    bool success = false;

    QStringList fileSelected = dialog->selectedFiles();

    if(fileSelected.size() > 0)
    {
        success = FlyWheel.LoadData(fileSelected.at(0).toStdString());
    }

    if(success)
    {
        unsigned int CylsNumbers;
        float Displace;
        float Speed;
        float Torque;
        float Dosage;

        FlyWheel.getEngineData(CylsNumbers, Displace);
        FlyWheel.getTestCond(Speed, Torque, Dosage);
        ui->LoadLine->setText("Load " + QString::number(CylsNumbers) + " Clylinders Engine");
        ui->DispLine->setText(QString::number(Displace));
        ui->SpeedLine->setText(QString::number(Speed));
        ui->TorqueLine->setText(QString::number(Torque));
        ui->DosageLine->setText(QString::number(Dosage));
        ui->FuelLine->setText("");
        ui->BmepLine->setText("");
        ui->UsEffLine->setText("");
        ui->LimEffLine->setText("");
        ui->OrgaEffLine->setText("");
        ui->TerMFluEffLine->setText("");
        ui->FillCoeffLine->setText("");
        ui->DiamLine->setText("");
        ui->DepthLine->setText("");

        ui->statusbar->showMessage("Data Calculated", 4000);
        EngPress->hide();
        ui->EPressCheckB->setChecked(false);
        IndCycle->hide();
        ui->IndCycleCheckB->setChecked(false);
        EffPress->hide();
        ui->EffPressCheckB->setChecked(false);
        CylTorque->hide();
        ui->CTorqueCheckB->setChecked(false);
        EngTorque->hide();
        ui->TTorqueCheckB->setChecked(false);
        EngWork->hide();
        ui->EngWorkCheckB->setChecked(false);
        ESpeed->hide();
        ui->EngSpCheckB->setChecked(false);
        FouSpect->hide();
        ui->FouSpecCheckB->setChecked(false);
    }
    else
    {
        ui->statusbar->showMessage("ERROR!!! File not opened: damanged or wrong", 4000);
    }
}

void MainWindow::on_CalcButton_clicked()
{
    float Density = ui->DensLine->text().toFloat();
    float Irr = ui->IrrLine->text().toFloat();
    RunFly = FlyWheel.RunCycle(Density, Irr);

    if(RunFly)
    {
        ui->FuelLine->setText(QString::number(FlyWheel.getResults().AvgFuelFlow));
        ui->BmepLine->setText(QString::number(FlyWheel.getResults().Bmep));
        ui->UsEffLine->setText(QString::number(FlyWheel.getResults().UsEfficiency));
        ui->LimEffLine->setText(QString::number(FlyWheel.getResults().MaxEfficiency));
        ui->OrgaEffLine->setText(QString::number(FlyWheel.getResults().OrgEfficiency));
        ui->TerMFluEffLine->setText(QString::number(FlyWheel.getResults().TerFluEfficiency));
        ui->FillCoeffLine->setText(QString::number(FlyWheel.getResults().FillCoeff));
        ui->DiamLine->setText(QString::number(FlyWheel.getResults().FlyWDiameter));
        ui->DepthLine->setText(QString::number(FlyWheel.getResults().FlyWDepth));
    }
}

void MainWindow::on_EPressCheckB_toggled(bool checked)
{
    _CreateChartSeries(checked, "Engine Pressure", EngPress, FlyWheel.getMesuredData().CrankAngle, "Crank Angle [°]",
                       {FlyWheel.getMesuredData().Pressure}, "Pressure [bar]");
}

void MainWindow::on_IndCycleCheckB_toggled(bool checked)
{
    QStringList LabelList;
    if(checked)
    {
        vector<string> Label =  FlyWheel.getResults().ValveTiming.Type;
        for(auto label =Label.begin(); label != Label.end(); label++)
        {
            LabelList << QString::fromStdString(*label);
        }
    }

    _CreateChartSeries(checked, "Indicated Cycle", IndCycle, FlyWheel.getResults().ChamberVolume, "Chamber Volume [dm^3]",
                       {FlyWheel.getMesuredData().Pressure}, "Pressure [bar]", {"Indicated Cycle"}, FlyWheel.getResults().ValveTiming.ChambVolume,
                       FlyWheel.getResults().ValveTiming.Pressure, "", LabelList);
}

void MainWindow::on_EffPressCheckB_toggled(bool checked)
{
    vector<vector<float>> PressureData;
    QStringList Labels;
    if(checked)
    {
        vector<float> AmbientPress(FlyWheel.getMesuredData().CrankAngle.size(), FlyWheel.getResults().AmbientPressure);
        Labels<<"Ambient Pressure"<<"Gas Pressure"<<"Inertia Pressure"<<"Effective Pressure";
        PressureData.push_back(AmbientPress);
        PressureData.push_back(FlyWheel.getMesuredData().Pressure);
        PressureData.push_back(FlyWheel.getResults().InertiaPressure);
        PressureData.push_back(FlyWheel.getResults().EffectivePressure);
    }
    _CreateChartSeries(checked, "Effective Pressure", EffPress, FlyWheel.getMesuredData().CrankAngle, "Crank Angle [°]",
                       PressureData, "Pressure [bar]", Labels);
}

void MainWindow::on_CTorqueCheckB_toggled(bool checked)
{
    QStringList Labels;
    if(checked)
    {
        for(unsigned int cyl = 1; cyl <= FlyWheel.getResults().CylinderEngineTorque.size(); cyl++)
        {
            Labels << ("Cylinder " + QString::number(cyl));
        }
    }
    _CreateChartSeries(checked, "Cylinders' Torque", CylTorque, FlyWheel.getMesuredData().CrankAngle, "Cranck Angle [°]",
                       FlyWheel.getResults().CylinderEngineTorque, "Torque [Nm]", Labels);
}

void MainWindow::on_TTorqueCheckB_toggled(bool checked)
{
    vector<vector<float>> TorqueData;
    QStringList Labels;
    if(checked)
    {
        vector<float> ResistentTorque(FlyWheel.getMesuredData().CrankAngle.size(), FlyWheel.getResults().ResistentTorque);
        Labels<<"Resistent Torque"<<"Engine Torque";
        TorqueData.push_back(ResistentTorque);
        TorqueData.push_back(FlyWheel.getResults().EngineTorque);
    }
    _CreateChartSeries(checked, "Engine Torque", EngTorque, FlyWheel.getMesuredData().CrankAngle, "Crank Angle [°]",
                       TorqueData, "Torque [Nm]", Labels);
}

void MainWindow::on_EngWorkCheckB_toggled(bool checked)
{
    vector<vector<float>> WorkData;
    QStringList Labels;
    if(checked)
    {
        Labels<<"Resistent Work"<<"Engine Work";
        WorkData.push_back(FlyWheel.getResults().ResistentWork);
        WorkData.push_back(FlyWheel.getResults().EngineWork);
    }
    _CreateChartSeries(checked, "Engine Work", EngWork, FlyWheel.getMesuredData().CrankAngle, "Crank Angle [°]",
                       WorkData, "Work/Displacement [bar]", Labels);
}

void MainWindow::on_EngSpCheckB_toggled(bool checked)
{
    vector<vector<float>> SpeedData;
    QStringList Labels;
    if(checked)
    {
        vector<float> AritAvgSpeed(FlyWheel.getMesuredData().CrankAngle.size(), FlyWheel.getResults().AritAvgSpeed);
        vector<float> IntAvgSpeed(FlyWheel.getMesuredData().CrankAngle.size(), FlyWheel.getResults().IntAvgSpeed);
        Labels<<"Arithmetic Average"<<"Integral Average"<<"Engine Speed";
        SpeedData.push_back(AritAvgSpeed);
        SpeedData.push_back(IntAvgSpeed);
        SpeedData.push_back(FlyWheel.getResults().EngineSpeed);
    }
    _CreateChartSeries(checked, "Engine Speed", ESpeed , FlyWheel.getMesuredData().CrankAngle, "Crank Angle [°]",
                       SpeedData, "Speed [Rpm]", Labels);
}

void MainWindow::on_FouSpecCheckB_toggled(bool checked)
{
    if(checked)
    {
        int XMax = 0;
        int YMax = 0;
        int YMin = 0;

        QScatterSeries *Series = NULL;
        QLineSeries *Series2 = NULL;
        QChart *Chart = new QChart();
        QValueAxis *Xaxis = new QValueAxis;
        QValueAxis *Yaxis = new QValueAxis;
        Fourier FourierSpect = FlyWheel.getResults().FreqSpectrum;

        if(FourierSpect.Amplitude.size() > 0 && FourierSpect.Frequence.size() > 0)
        {
           if(FourierSpect.Frequence.size() > 0 && XMax == 0)
           {
               XMax = FourierSpect.Frequence[0];
           }

           if(FourierSpect.Amplitude.size() > 0 && YMax == 0)
           {
               YMax = FourierSpect.Amplitude[0];
           }

           if(FourierSpect.Amplitude.size() > 0 && YMin == 0)
           {
               YMin = FourierSpect.Amplitude[0];
           }
           Series = new QScatterSeries;

           for(unsigned int step = 0; step < FourierSpect.Amplitude.size(); step++)
           {
               Series->append(FourierSpect.Frequence[step],FourierSpect.Amplitude[step]);

               if(XMax < FourierSpect.Frequence[step])
               {
                   XMax = FourierSpect.Frequence[step];
               }

               if(YMax <  FourierSpect.Amplitude[step])
               {
                   YMax =  FourierSpect.Amplitude[step];
               }

               if(YMin >  FourierSpect.Amplitude[step])
               {
                   YMin =  FourierSpect.Amplitude[step];
               }
           }
           Series->setColor(Qt::blue);
           Chart->addSeries(Series);

           Xaxis->setTitleText("Frequencies [Hz]");
           Xaxis->setTickCount(XMax/100 + 1);
           Xaxis->setRange(0, XMax);
           Chart->addAxis(Xaxis, Qt::AlignBottom);

           Yaxis->setTitleText("Amplitude [Nm]");
           if((YMax - YMin) < 20)
           {
               YMin = YMin/10 * 10 - 2;
               YMax = YMax/10 * 10 + 2;
               Yaxis->setTickCount((YMax - YMin)/2 + 1);
           }
           else if((YMax - YMin) < 120)
           {
               YMin = (YMin/10 - 1) * 10;
               YMax = (YMax/10 +1) * 10;
               Yaxis->setTickCount((YMax - YMin)/10 + 1);
           }
           else
           {
               YMin = (YMin/50 - 1) * 50;
               YMax = (YMax/50 + 1) * 50;
               Yaxis->setTickCount((YMax - YMin)/50 + 1);
            }

            Chart->addAxis(Yaxis, Qt::AlignLeft);
            Yaxis->setRange(YMin, YMax);

            Series->attachAxis(Xaxis);
            Series->attachAxis(Yaxis);

            for(unsigned int step = 0; step < FourierSpect.Frequence.size(); step++)
            {
                Series2 = new QLineSeries;
                Series2->append(FourierSpect.Frequence[step], 0);
                Series2->append(FourierSpect.Frequence[step], FourierSpect.Amplitude[step]);
                Series2->setColor(Qt::blue);

                Chart->addSeries(Series2);
                Series2->attachAxis(Xaxis);
                Series2->attachAxis(Yaxis);

            }
        }

        Chart->setTitle("Frequencies Spectrum");
        QFont font;
        font.setPixelSize(20);
        Chart->setTitleFont(font);
        Chart->legend()->setVisible(false);
        Chart->setAnimationOptions(QChart::SeriesAnimations);
        FouSpect->setChart(Chart);
        FouSpect->setGeometry(0, 0, 800, 600);
        FouSpect->setRenderHint(QPainter::Antialiasing);

    }

    FouSpect->setVisible(checked);
}

void MainWindow::on_ExpButton_clicked()
{
    if(RunFly)
    {
        string Path;
        unsigned int CylinderN;
        float Disp;
        char str[1];
        FlyWheel.getEngineData(CylinderN, Disp);
        sprintf(str,"%d",CylinderN);
        Path = QDir::currentPath().toStdString() + "/SavedData/FlyWheel/"
                "Results" + str + "Cylinders";

        FlyWheel.ExportResults(Path + ".dat");;
        ui->statusbar->showMessage("Data " + QString::number(CylinderN) + " Cylinder Saved", 4000);
    }
    else
    {
        ui->statusbar->showMessage("ERROR!! No Data Available", 4000);
    }
}


















