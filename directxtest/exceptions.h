#pragma once

#include <exception>
#include <string>

class MyException : public std::exception
{
public:
    MyException(int Line, const char* File) noexcept;
    const char* what() const noexcept override;
    virtual const char* GetType() const noexcept;
    int GetLine() const noexcept;
    const std::string& GetFile() const noexcept;
    std::string GetOriginString() const noexcept;
private:
    int Line;
    std::string File;
protected:
    mutable std::string whatBuffer;
};