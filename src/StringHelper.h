#pragma once

#include <string>

/**
 * @class StringHelper
 * @brief Utility class for common string formatting and case conversion helpers.
 */
class StringHelper {
public:
    /**
     * @brief Converts a string to uppercase.
     * @param str The string to convert.
     * @return The uppercase version of the string.
     */
    static std::string toUpper(const std::string& str);

    /**
     * @brief Converts a string to lowercase.
     * @param str The string to convert.
     * @return The lowercase version of the string.
     */
    static std::string toLower(const std::string& str);

    /**
     * @brief Formats a floating-point value with a fixed number of decimal places.
     * @param value The float value to format.
     * @param precision The number of decimal places to include.
     * @return A string containing the formatted floating-point value.
     */
    static std::string formatFloat(const float value, const int precision);
};