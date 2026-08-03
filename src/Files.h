#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

class Files {
    public:
    using json = nlohmann::json;
    static const bool isValidJsonFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filePath << std::endl;
            return false;
        }
        return json::accept(file);
    }
};