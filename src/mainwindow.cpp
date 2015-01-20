#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rtoutput.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::vector<std::string> ports = RtOutput::Instance().listPorts();
    for (std::vector<std::string>::iterator s = ports.begin(); s != ports.end(); ++s)
        this->ui->cmbPort->addItem(QString::fromStdString(*s));

    for (int i = 0; i < 16; i++)
        this->ui->cmbChannel->addItem(QString("Channel ") + QString::number(i+1));

    for (int i = 0; i < 32; i++)
        this->ui->cmbBank->addItem(QString("Bank ") + QString::number(i+1));

    for (int i = 0; i < 161; i++)
        this->ui->cmbProgram->addItem(QString("Program ") + QString::number(i+1));

    connect(this->ui->cmbPort, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPortChanged(int)));
    connect(this->ui->cmbChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChannelChanged(int)));
    connect(this->ui->cmbBank, SIGNAL(currentIndexChanged(int)), this, SLOT(OnBankChanged(int)));
    connect(this->ui->cmbProgram, SIGNAL(currentIndexChanged(int)), this, SLOT(OnProgramChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnPortChanged(int index)
{
    RtOutput::Instance().changePort(index);
}

void MainWindow::OnChannelChanged(int index)
{
    RtOutput::Instance().changeChannel(index);
}

void MainWindow::OnBankChanged(int index)
{
    RtOutput::Instance().changeBank(index);
}

void MainWindow::OnProgramChanged(int index)
{
    RtOutput::Instance().changeProgram(index);
}
