#include "graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),InfoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) InfoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),InfoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) InfoManager.Set(); (call); {auto v = InfoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

Graphics::Graphics(HWND WindowHandle)
{
    DXGI_SWAP_CHAIN_DESC SwapDesc = {};
    SwapDesc.BufferDesc.Width = 0;
    SwapDesc.BufferDesc.Height = 0;
    SwapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SwapDesc.BufferDesc.RefreshRate.Numerator = 0;
    SwapDesc.BufferDesc.RefreshRate.Denominator = 0;
    SwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    SwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SwapDesc.SampleDesc.Count = 1;
    SwapDesc.SampleDesc.Quality = 0;
    SwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapDesc.BufferCount = 1;
    SwapDesc.OutputWindow = WindowHandle;
    SwapDesc.Windowed = TRUE;
    SwapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapDesc.Flags = 0;

    UINT SwapCreateFlags = 0u;

#ifndef NDEBUG
    SwapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    
    HRESULT hr;
    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                                  nullptr, SwapCreateFlags, nullptr, 0,
                                  D3D11_SDK_VERSION, 
                                  &SwapDesc, &SwapChain, 
                                  &Device, nullptr, &Context));
    
    wrl::ComPtr<ID3D11Resource> BackBuffer;
    GFX_THROW_INFO(SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &BackBuffer));
    GFX_THROW_INFO(Device->CreateRenderTargetView(BackBuffer.Get(),nullptr, &Target));
}

void Graphics::EndFrame()
{
    HRESULT hr;
#ifndef NDEBUG
    InfoManager.Set();
#endif

    if (FAILED(hr = SwapChain->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVED_EXCEPT(Device->GetDeviceRemovedReason());
        }
        else
        {
            throw GFX_EXCEPT(hr);
        }
    }
}

void Graphics::ClearBuffer(float Red, float Green, float Blue) noexcept
{
    const float Colors[] = {Red, Green, Blue, 1.0f};
    Context->ClearRenderTargetView(Target.Get(), Colors);
}

// Graphics exceptions
Graphics::HrException::HrException(int Line, const char* File, HRESULT hr, std::vector<std::string> InfoMsgs) noexcept :
Exception(Line, File), Result(hr) 
{
    for (const auto& m : InfoMsgs)
    {
        Info += m;
        Info.push_back('\n');
    }

    // Remove final newline
    if(!Info.empty())
    {
        Info.pop_back();
    }
}

const char* Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << "(" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;
    
    if (!Info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
    return "My Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return Result;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    return DXGetErrorStringA(Result);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
    char Buffer[512];
    DXGetErrorDescriptionA(Result, Buffer, sizeof(Buffer));
    return Buffer;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return Info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "My Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int Line, const char* File, std::vector<std::string> InfoMsgs) noexcept
    : Exception(Line, File)
{
    for (const auto& m : InfoMsgs)
    {
        Info += m;
        Info.push_back('\n');
    }

    if (!Info.empty())
    {
        Info.pop_back();
    }
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "My Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return Info;
}

// DRAWING!!

void Graphics::DrawTestTriangle()
{
        namespace wrl = Microsoft::WRL;
        HRESULT hr;

        struct Vertex
        {
            float X;
            float Y;
        };

        const Vertex Vertices[] = 
        {
            {0.0f, 0.5f},
            {0.5f, -0.5f},
            {-0.5f, -0.5f}
        };

        wrl::ComPtr<ID3D11Buffer> VertexBuffer;

        D3D11_BUFFER_DESC VertextBufferDesc = {};
        VertextBufferDesc.ByteWidth = sizeof(Vertices);
        VertextBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertextBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertextBufferDesc.CPUAccessFlags = 0u;
        VertextBufferDesc.MiscFlags = 0u;
        VertextBufferDesc.StructureByteStride = sizeof(Vertex);
        

        D3D11_SUBRESOURCE_DATA SubResDesc = {};
        SubResDesc.pSysMem = Vertices;

        Device->CreateBuffer(&VertextBufferDesc, &SubResDesc, &VertexBuffer);

        // Bind Shaders to the pipeline
        const UINT Stride = sizeof(Vertex);
        const UINT Offset = 0u;
        Context->IASetVertexBuffers(0u, 1u, VertexBuffer.GetAddressOf(), &Stride, &Offset);

        // Create Pixel Shader
        wrl::ComPtr<ID3D11PixelShader> PixelShader;
        wrl::ComPtr<ID3DBlob> Blob;
        GFX_THROW_INFO(D3DReadFileToBlob(L"pixel_shader.cso", &Blob));
        GFX_THROW_INFO(Device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, &PixelShader));

        // Bind Pixel Shader
        Context->PSSetShader(PixelShader.Get(), nullptr, 0u);

        // Create Vertex Shader
        wrl::ComPtr<ID3D11VertexShader> VertexShader;
        GFX_THROW_INFO(D3DReadFileToBlob(L"vertex_shader.cso", &Blob));
        GFX_THROW_INFO(Device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, &VertexShader));

        // Bind Vertex Shader
        Context->VSSetShader(VertexShader.Get(), nullptr, 0u);

        // Input (vertex) layout (2D position only)
        wrl::ComPtr<ID3D11InputLayout> InputLayout;
        const D3D11_INPUT_ELEMENT_DESC InputElementDesc[] = 
        {
            {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };
        GFX_THROW_INFO(Device->CreateInputLayout(InputElementDesc, (UINT)std::size(InputElementDesc),
                                                 Blob->GetBufferPointer(), Blob->GetBufferSize(),
                                                 &InputLayout));

        // Bind vertex layout
        Context->IASetInputLayout(InputLayout.Get());

        // Bind Render target
        Context->OMSetRenderTargets(1u, Target.GetAddressOf(), nullptr);

        // Set Primitive Topology to Triangle list (groups of 3 vertices)
        Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Configure Viewport
        D3D11_VIEWPORT Viewport;
        Viewport.Width = 800;
        Viewport.Height = 600;
        Viewport.MinDepth = 0;
        Viewport.MaxDepth = 1;
        Viewport.TopLeftX = 0;
        Viewport.TopLeftY = 0;
        Context->RSSetViewports(1u, &Viewport);

        GFX_THROW_INFO_ONLY(Context->Draw((UINT)std::size(Vertices), 0u));
}