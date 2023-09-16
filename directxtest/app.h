#pragma once
#include "win_class.h"
#include "timer.h"

class App
{
public:
    App();
    int Run();
private:
    void DoFrame();
private:
    Window MainWindow;
    Timer MyTimer;
};