#include "win_class.h"
#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::WinClass;

Window::WindowClass::WindowClass() noexcept : hInstance(GetModuleHandle(0))
{
    WNDCLASSEX WC = {};
    WC.cbSize = sizeof(WNDCLASSEX);
    WC.style = CS_OWNDC;
    WC.lpfnWndProc = HandleMsgSetup;
    WC.cbClsExtra = 0;
	WC.cbWndExtra = 0;
    WC.hInstance = GetInstance();
    WC.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
    WC.hCursor = nullptr;
	WC.hbrBackground = nullptr;
	WC.lpszMenuName = nullptr;
    WC.lpszClassName = GetName();
    WC.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));

    RegisterClassEx(&WC);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(ClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
    return ClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return WinClass.hInstance;
}

Window::Window(int Width, int Height, const wchar_t* Name) : Width(Width), Height(Height)
{
    RECT WindowRect;
    WindowRect.left = 100;
    WindowRect.right = Width + WindowRect.left;
    WindowRect.top = 100;
    WindowRect.bottom = Height + WindowRect.top;

    if (AdjustWindowRect(&WindowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
    {
        throw MYWND_LAST_EXCEPT();
    }

    WindowHandle = CreateWindowEx(0, WindowClass::GetName(), Name, 
                                 WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                                 CW_USEDEFAULT, CW_USEDEFAULT, 
                                 WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
                                 0, 0, WindowClass::GetInstance(), this);
    if (!WindowHandle)
    {
        throw MYWND_LAST_EXCEPT();
    }

    ShowWindow(WindowHandle, SW_SHOWDEFAULT);

    // Create Graphics object
    GFX = std::make_unique<Graphics>(WindowHandle);
}

Window::~Window()
{
    DestroyWindow(WindowHandle);
}

std::optional<int> Window::ProcessMessages() noexcept
{
    MSG Message;
    while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        if (Message.message == WM_QUIT)
        {
            return Message.wParam;
        }

        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return {};
}

Graphics& Window::GetGFX()
{
    if (!GFX)
    {
        throw MYWND_NOGFX_EXCEPT();
    }
    return *GFX;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam) noexcept
{
    if (Message == WM_NCCREATE)
    {
        const CREATESTRUCT* const Create = reinterpret_cast<CREATESTRUCT*>(lParam);
        Window* const Wnd = static_cast<Window*>(Create->lpCreateParams);

        SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Wnd));
        SetWindowLongPtr(WindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

        return Wnd->HandleMessage(WindowHandle, Message, wParam, lParam);

    }
    else
    {
        return DefWindowProc(WindowHandle, Message, wParam, lParam);
    }
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const Wnd = reinterpret_cast<Window*>(GetWindowLongPtr(WindowHandle, GWLP_USERDATA));

    return Wnd->HandleMessage(WindowHandle, Message, wParam, lParam);
}

void Window::SetTitle(const std::string& Title)
{
    if (SetWindowTextA(WindowHandle, Title.c_str()) == 0)
    {
        throw MYWND_LAST_EXCEPT();
    }
    
}

// NOTE: MAIN WINDOW MESSAGE HANDLER
LRESULT Window::HandleMessage(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (Message)
    {
        case WM_CLOSE:
        {   
            PostQuitMessage(0);
            return 0;
        } break;

        case WM_KILLFOCUS:
        {
            MainKeyboard.ClearState();
        } break;

        // NOTE: Keyboard messages
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {   
            if (!(lParam >> 30) || MainKeyboard.AutorepeatIsEnabled())
            {
                MainKeyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
            }
        } break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            MainKeyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
        } break;

        case WM_CHAR:
        {
            MainKeyboard.OnChar(static_cast<unsigned char>(wParam));
        } break;

        // NOTE: Mouse messages
        case WM_MOUSEMOVE:
        {
            const POINTS Point = MAKEPOINTS(lParam);
            // Check if mouse inside the client region
            if (Point.x >= 0 && Point.x < Width && Point.y >= 0 && Point.y < Height)
            {
                MainMouse.OnMouseMove(Point.x, Point.y);
                if (!MainMouse.IsInWindow())
                {
                    SetCapture(WindowHandle);
                    MainMouse.OnMouseEnter();
                }
            }
            else
            {
                if (wParam & (MK_LBUTTON | MK_RBUTTON)) // NOTE: Drag operation, capture the mouse even out of clieant area
                {
                    MainMouse.OnMouseMove(Point.x, Point.y);
                }
                else
                {
                    ReleaseCapture();
                    MainMouse.OnMouseLeave();
                }
            }
        } break;

        case WM_LBUTTONDOWN:
        {
            const POINTS Point = MAKEPOINTS(lParam);
            MainMouse.OnLeftPressed(Point.x, Point.y);
        } break;

        case WM_RBUTTONDOWN:
        {
            const POINTS Point = MAKEPOINTS(lParam);
            MainMouse.OnRightPressed(Point.x, Point.y);
        } break;

        case WM_LBUTTONUP:
        {
            const POINTS Point = MAKEPOINTS(lParam);
            MainMouse.OnLeftReleased(Point.x, Point.y);
        } break;

        case WM_RBUTTONUP:
        {
            const POINTS Point = MAKEPOINTS(lParam);
            MainMouse.OnRightReleased(Point.x, Point.y);
        } break;

        case WM_MOUSEWHEEL:
        {
            const POINTS Point = MAKEPOINTS(lParam);
            const int Delta = GET_WHEEL_DELTA_WPARAM(wParam);
            MainMouse.OnWheelDelta(Point.x, Point.y, Delta);
        } break;
    }

    return DefWindowProc(WindowHandle, Message, wParam, lParam);;
}

// Window Exceptions
Window::HrException::HrException(int Line, const char* File, HRESULT Result) noexcept : Exception(Line, File), Result(Result) {}

const char* Window::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << "(" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Description] " << GetErrorDescription() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
    return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT Result) noexcept
{
    char* MessageBuffer = nullptr;
    DWORD MessageLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                        FORMAT_MESSAGE_FROM_SYSTEM |
                                        FORMAT_MESSAGE_IGNORE_INSERTS,
                                        nullptr, Result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                        reinterpret_cast<LPSTR>(&MessageBuffer), 0, nullptr);
    if (MessageLength == 0)
    {
        return "Unindentified error code";
    }    

    std::string ErrorString = MessageBuffer;
    LocalFree(MessageBuffer);
    return ErrorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
    return Result;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
    return Exception::TranslateErrorCode(Result);
}

const char* Window::NoGFXException::GetType() const noexcept
{
	return "My Window Exception [No Graphics]";
}