#include "app.h"
#include <sstream>
#include <iomanip>

App::App() : MainWindow(800, 600, L"D3DEngine") {}

int App::Run()
{
    while (true)
    {
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }
        
        DoFrame();
    }
}

void App::DoFrame()
{
    const float c = sin(MyTimer.Peek()) / 2.0f + 0.5f;
    MainWindow.GetGFX().ClearBuffer(c, c, 1.0f);
    MainWindow.GetGFX().DrawTestTriangle();
    MainWindow.GetGFX().EndFrame();
}