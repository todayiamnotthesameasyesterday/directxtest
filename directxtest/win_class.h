#pragma once
#include "win_include.h"
#include "exceptions.h"
#include "keyboard.h"
#include "mouse.h"
#include "graphics.h"
#include <optional>
#include <memory>

class Window
{
public:
    class Exception : public MyException
    {
        using MyException::MyException;
    public:      
        static std::string TranslateErrorCode(HRESULT Result) noexcept;
        
    };
    class HrException : public Exception
    {
    public:
        HrException(int Line, const char* File, HRESULT Result) noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorDescription() const noexcept;
    private:
        HRESULT Result;
    };
    class NoGFXException : public Exception
    {
    public:
        using Exception::Exception;
        const char* GetType() const noexcept override;
    };
private:
    class WindowClass
    {
    public:
        static const wchar_t* GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;
    private:
        WindowClass() noexcept;
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=( const WindowClass& ) = delete;
        static constexpr const wchar_t* ClassName = L"Window Class";
        static WindowClass WinClass;
        HINSTANCE hInstance;
    };
public:
    Window(int Width, int Height, const wchar_t* Name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    void SetTitle(const std::string& Title); // NOTE:
    static std::optional<int> ProcessMessages() noexcept;
    Graphics& GetGFX();
private:
    static LRESULT CALLBACK HandleMsgSetup(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMessage(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam) noexcept;
public:
    Keyboard MainKeyboard;
    Mouse MainMouse;
private:
    int Width;
    int Height;
    HWND WindowHandle;
    std::unique_ptr<Graphics> GFX;
};

#define MYWND_EXCEPT(Result) Window::HrException(__LINE__,__FILE__,(Result))
#define MYWND_LAST_EXCEPT() Window::HrException(__LINE__,__FILE__,GetLastError())
#define MYWND_NOGFX_EXCEPT() Window::NoGFXException(__LINE__, __FILE__)