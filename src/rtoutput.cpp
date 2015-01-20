#include "rtoutput.h"

using namespace std;

RtOutput::RtOutput()
    : midiout(0)
{ }

RtOutput& RtOutput::Instance()
{
    static RtOutput instance;

    return instance;
}

RtOutput::~RtOutput()
{
    if (this->midiout != 0)
    {
        delete this->midiout;
        this->midiout = 0;
    }
}

int RtOutput::init()
{
    if (this->midiout != 0)
        return 1;

    this->midiout = new RtMidiOut();
    if (this->midiout->getPortCount() <= 0)
        return 0;

    this->midiout->openPort(0);

    for (int i = 0; i < this->midiout->getPortCount(); i++)
        this->ports.push_back(this->midiout->getPortName(i));

    return 1;
}

void RtOutput::changePort(char index)
{
    this->midiout->closePort();
    this->midiout->openPort(index);
}

const std::vector<std::string>& RtOutput::listPorts()
{
    return this->ports;
}

void RtOutput::noteOn(char note, char velocity)
{
    std::vector<unsigned char> message;

    message.push_back(144);
    message.push_back(note);
    message.push_back(velocity);

    this->midiout->sendMessage(&message);
}

void RtOutput::noteOff(char note)
{
    std::vector<unsigned char> message;

    message.push_back(128);
    message.push_back(note);
    message.push_back(0);

    midiout->sendMessage(&message);
}

void RtOutput::changeChannel(char index)
{ }

void RtOutput::changeBank(char index)
{
    std::vector<unsigned char> message;
}

void RtOutput::changeProgram(char index)
{
    std::vector<unsigned char> message;

    message.push_back(192);
    message.push_back(index);

    midiout->sendMessage(&message);
}
