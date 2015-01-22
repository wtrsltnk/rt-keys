#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "RtMidi.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void OnPortChanged(int index);
    void OnChannelChanged(int index);
    void OnBankChanged(int index);
    void OnProgramChanged(int index);
    void noteOn(char note, char velocity);
    void noteOff(char note);

private:
    Ui::MainWindow *ui;
    RtMidiOut* _midiout;

};

#endif // MAINWINDOW_H
