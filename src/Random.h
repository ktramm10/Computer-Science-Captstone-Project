#pragma once
#include <random>

/**
 * @class Random
 * @brief Utility class for generating random numbers.
 * @details This class provides static methods to generate random float and integer values within a specified range.
 */
class Random {
    public:
    /**
     * @brief Generates a random float or integer within the specified range.
     * @param min The minimum value of the range (inclusive).
     * @param max The maximum value of the range (inclusive).
     * @return A random float or integer within the specified range.
     */
    static float range(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator());
    }
    /**
     * @brief Generates a random integer within the specified range.
     * @param min The minimum value of the range (inclusive).
     * @param max The maximum value of the range (inclusive).
     * @return A random integer within the specified range.
     */
    static int range(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator());
    }
    
    private:
    static std::mt19937& generator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
};