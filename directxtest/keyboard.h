#pragma once

#include <queue>
#include <bitset>

class Keyboard
{
    friend class Window;
public:
    class Event
    {
    public:
        enum class Type
        {
            Press,
            Release,
            Invalid
        };
    private:
        Type type;
        unsigned char Code;
    public:
        Event() : type (Type::Invalid), Code(0u) {}
        Event(Type type, unsigned char Code) noexcept : type(type), Code(Code) {}
        bool IsPressed() const noexcept
        {
            return type == Type::Press;
        }
        bool IsReleased() const noexcept
        {
            return type == Type::Release;
        }
        bool IsValid() const noexcept
        {
            return type != Type::Invalid;
        }
        unsigned char GetCode() const noexcept
        {
            return Code;
        }   
    };
public:
    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;

    // Key events
    bool KeyIsPressed(unsigned char KeyCode) const noexcept;
    Event ReadKey() noexcept;
    bool KeyIsEmpty() const noexcept;
    void FlushKey() noexcept;

    // Char events
    char ReadChar() noexcept;
    bool CharIsEmpty() const noexcept;
    void FlushChar() noexcept;
    void Flush() noexcept;

    // Autorepeat control
    void EnableAutorepeat() noexcept;
    void DisableAutorepeat() noexcept;
    bool AutorepeatIsEnabled() const noexcept;
private:
    void OnKeyPressed(unsigned char KeyCode) noexcept;
    void OnKeyReleased(unsigned char KeyCode) noexcept;
    void OnChar(char Character) noexcept;
    void ClearState() noexcept;
    template<typename T>
    static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
    static constexpr unsigned int Keys = 256u;
    static constexpr unsigned int BufferSize = 16u;
    bool AutorepeatEnabled = false;
    std::bitset<Keys> KeyStates;
    std::queue<Event> KeyBuffer;
    std::queue<char> CharBuffer;
};