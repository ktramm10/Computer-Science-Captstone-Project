#include "StringHelper.h"
#include <sstream>
#include <iomanip>

std::string StringHelper::formatFloat(const float value, const int precision)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}