#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
