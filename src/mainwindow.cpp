#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), _midiout(0)
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress("192.168.178.23"), 5555);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    this->_midiout = new RtMidiOut();
    if (this->_midiout->getPortCount() > 0)
        this->_midiout->openPort(0);

    ui->setupUi(this);

    for (unsigned int i = 0; i < this->_midiout->getPortCount(); i++)
        this->ui->cmbPort->addItem(QString::fromStdString(this->_midiout->getPortName(i)));

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

    connect(this->ui->piano, SIGNAL(noteOn(char,char)), this, SLOT(noteOn(char,char)));
    connect(this->ui->piano, SIGNAL(noteOff(char)), this, SLOT(noteOff(char)));
}

void MainWindow::readPendingDatagrams()
{
    std::cout << "recv" << std::endl;
    while (udpSocket->hasPendingDatagrams())  {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        char note = datagram.data()[1];
        std::cout << "recv " << int(note) << std::endl;
        if (datagram.data()[0] == '1')
            this->noteOn(note, datagram.data()[2]);
        else if (datagram.data()[0] == '0')
            this->noteOff(note);
    }
}

MainWindow::~MainWindow()
{
    if (this->_midiout != 0)
    {
        delete this->_midiout;
        this->_midiout = 0;
    }
    delete ui;
}

void MainWindow::noteOn(char note, char velocity)
{
    std::vector<unsigned char> message;

    message.push_back(144);
    message.push_back(note);
    message.push_back(velocity);

    this->_midiout->sendMessage(&message);
}

void MainWindow::noteOff(char note)
{
    std::vector<unsigned char> message;

    message.push_back(128);
    message.push_back(note);
    message.push_back(0);

    this->_midiout->sendMessage(&message);
}

void MainWindow::OnPortChanged(int index)
{
    this->_midiout->closePort();
    this->_midiout->openPort(index);
}

void MainWindow::OnChannelChanged(int index)
{ }

void MainWindow::OnBankChanged(int index)
{ }

void MainWindow::OnProgramChanged(int index)
{ }
