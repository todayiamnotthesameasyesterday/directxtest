#include "exceptions.h"
#include <sstream>

MyException::MyException(int Line, const char* File) noexcept : Line(Line), File(File) {}

const char* MyException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* MyException::GetType() const noexcept
{
    return "My Exception";
}

int MyException::GetLine() const noexcept
{
    return Line;
}

const std::string& MyException::GetFile() const noexcept
{
    return File;
}

std::string MyException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << File << std::endl
        << "[Line] " << Line;

    return oss.str();
}