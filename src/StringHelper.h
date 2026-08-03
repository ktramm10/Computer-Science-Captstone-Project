#pragma once

#include <string>

class StringHelper {
public:
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string formatFloat(const float value, const int precision);
};