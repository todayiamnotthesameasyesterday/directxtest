#pragma once
#include "win_include.h"
#include "exceptions.h"
#include <d3d11.h>
#include <wrl.h>
#include "dxgi_info_manager.h"

class Graphics
{
public:
    class Exception : public MyException
    {
        using MyException::MyException;
    };
    class HrException : public Exception
    {
    public:
        HrException(int Line, const char* File, HRESULT Result, std::vector<std::string> InfoMsgs = {}) noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
        std::string GetErrorDescription() const noexcept;
        std::string GetErrorInfo() const noexcept;
    private:
        HRESULT Result;
        std::string Info;
    };
    class InfoException : public Exception
    {
    public:
        InfoException(int Line, const char* File, std::vector<std::string> InfoMsgs) noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;
        std::string GetErrorInfo() const noexcept;
    private:
        std::string Info;
    };
    class DeviceRemovedException : public HrException
    {
        using HrException::HrException;
    public:
        const char* GetType() const noexcept override;
    private:
        std::string Reason;
    };
public:
    Graphics(HWND WindowHandle);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics() = default;
    void EndFrame();
    void ClearBuffer(float Red, float Green, float Blue) noexcept;
    void DrawTestTriangle();
private:
#ifndef NDEBUG
    DXGIInfoManager InfoManager;
#endif
    Microsoft::WRL::ComPtr<ID3D11Device> Device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> Target;
};
