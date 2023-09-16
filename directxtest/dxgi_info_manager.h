#pragma once
#include "win_include.h"
#include <wrl.h>
#include <vector>
#include <string>
#include <dxgidebug.h>

class DXGIInfoManager
{
public:
    DXGIInfoManager();
    ~DXGIInfoManager() = default;
    DXGIInfoManager(const DXGIInfoManager&) = delete;
    DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
    void Set() noexcept;
    std::vector<std::string> GetMessages() const;
private:
    unsigned long long Next = 0u;
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> DXGIInfoQueue;
};