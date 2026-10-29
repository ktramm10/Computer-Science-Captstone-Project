#include "StringHelper.h"
#include <sstream>
#include <iomanip>

/**
 * @brief Formats a floating-point value with a fixed number of decimal places.
 * @param value The float value to format.
 * @param precision The number of decimal places to include.
 * @return A string containing the formatted floating-point value.
 */
std::string StringHelper::formatFloat(const float value, const int precision)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}