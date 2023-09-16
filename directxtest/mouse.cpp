#include "mouse.h"
#include "win_include.h"

std::pair<int,int> Mouse::GetPos() const noexcept
{
    return {X, Y};
}

int Mouse::GetPosX() const noexcept
{
    return X;
}

int Mouse::GetPosY() const noexcept
{
    return Y;
}

bool Mouse::LPressed() const noexcept
{
    return LeftIsPressed;
}

bool Mouse::RPressed() const noexcept
{
    return RightIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
    if (Buffer.size() > 0u)
    {
        Mouse::Event e = Buffer.front();
        Buffer.pop();
        return e;
    }
    else
    {
        return Mouse::Event();
    }
}

void Mouse::Flush() noexcept
{
    Buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int NewX, int NewY) noexcept
{
    X = NewX;
    Y = NewY;

    Buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
    TrimBuffer();
}

void Mouse::OnLeftPressed(int X, int Y) noexcept
{
    LeftIsPressed = true;

    Buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
    TrimBuffer();
}

void Mouse::OnLeftReleased(int X, int Y) noexcept
{
    LeftIsPressed = false;

    Buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
    TrimBuffer();
}

void Mouse::OnRightPressed(int X, int Y) noexcept
{
    RightIsPressed = true;

    Buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
    TrimBuffer();
}

void Mouse::OnRightReleased(int X, int Y) noexcept
{
    RightIsPressed = false;

    Buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
    TrimBuffer();
}

void Mouse::OnWheelUp(int X, int Y) noexcept
{
    Buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
    TrimBuffer();
}

void Mouse::OnWheelDown(int X, int Y) noexcept
{
    Buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
    TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
    while (Buffer.size() > BufferSize)
    {
        Buffer.pop();
    }
}

bool Mouse::IsInWindow() const noexcept
{
    return InWindow;
}

void Mouse::OnMouseLeave() noexcept
{
    InWindow = false;
    Buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
    TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
    InWindow = true;
    Buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
    TrimBuffer();
}

void Mouse::OnWheelDelta(int X, int Y, int Delta) noexcept
{
    WheelDeltaCarry += Delta;
    // Generate events for every 120
    while (WheelDeltaCarry >= WHEEL_DELTA)
    {
        WheelDeltaCarry -= WHEEL_DELTA;
        OnWheelUp(X, Y);
    }
    while (WheelDeltaCarry <= -WHEEL_DELTA)
    {
        WheelDeltaCarry += WHEEL_DELTA;
        OnWheelDown(X, Y);
    }
}
