#pragma once

#include <queue>

class Mouse
{
    friend class Window;
public:
    class Event
    {
    public:
        enum class Type
        {
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,
            Enter,
            Leave,
            Invalid
        };
    private:
        Type type;
        bool LeftIsPressed;
        bool RightIsPressed;
        int X;
        int Y;
    public:
        Event() noexcept : type (Type::Invalid), LeftIsPressed(false), RightIsPressed(false), X(0), Y(0) {}
        Event(Type type, const Mouse& Parent) noexcept : 
        type(type), LeftIsPressed(Parent.LeftIsPressed), RightIsPressed(Parent.RightIsPressed), X(Parent.X), Y(Parent.Y) {}
        bool IsValid() const noexcept
        {
            return type != Type::Invalid;
        }
        Type GetType() const noexcept
        {
            return type;
        }   
        std::pair<int, int> GetPos() const noexcept
        {
            return{X, Y};
        }
        int GetPosX() const noexcept
        {
            return X;
        }
        int GetPosY() const noexcept
        {
            return Y;
        }
        bool LPressed()
        {
            return LeftIsPressed;
        }
        bool RPressed()
        {
            return RightIsPressed;
        }
    };
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    std::pair<int, int> GetPos() const noexcept;
    int GetPosX() const noexcept;
    int GetPosY() const noexcept;
    bool IsInWindow() const noexcept;
    bool LPressed() const noexcept;
    bool RPressed() const noexcept;
    Mouse::Event Read() noexcept;
    bool IsEmpty() const noexcept
    {
        return Buffer.empty();
    }

    void Flush() noexcept;
private:
    void OnMouseMove(int X, int Y) noexcept;
    void OnMouseLeave() noexcept;
    void OnMouseEnter() noexcept;
    void OnLeftPressed(int X, int Y) noexcept;
    void OnLeftReleased(int X, int Y) noexcept;
    void OnRightPressed(int X, int Y) noexcept;
    void OnRightReleased(int X, int Y) noexcept;
    void OnWheelUp(int X, int Y) noexcept;
    void OnWheelDown(int X, int Y) noexcept;
    void OnWheelDelta(int X, int Y, int Delta) noexcept;

    void TrimBuffer() noexcept;
private:
    static constexpr unsigned int BufferSize = 16u;
    int X;
    int Y;
    bool LeftIsPressed = false;
    bool RightIsPressed = false;
    bool InWindow = false;
    int WheelDeltaCarry = 0;
    std::queue<Event> Buffer;
};