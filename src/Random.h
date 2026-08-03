#pragma once
#include <random>

class Random {
    public:
    static float range(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator());
    }
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