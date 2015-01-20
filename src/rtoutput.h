#ifndef RTOUTPUT_H
#define RTOUTPUT_H

#include "RtMidi.h"

class RtOutput
{
    RtOutput();
public:
    static RtOutput& Instance();
    virtual ~RtOutput();

    int init();

    void noteOn(char note, char velocity);
    void noteOff(char note);
    void changePort(char index);
    void changeChannel(char index);
    void changeBank(char index);
    void changeProgram(char index);

    const std::vector<std::string>& listPorts();
private:
    RtMidiOut* midiout;
    std::vector<std::string> ports;

};

#endif // RTOUTPUT_H
