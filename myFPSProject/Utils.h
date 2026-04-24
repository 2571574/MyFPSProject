#pragma once
#include <string>
#include <vector>
#include <sstream>
namespace Utils {
    inline std::vector<std::string> Split(const std::string& input, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(input);
        while (std::getline(tokenStream, token, delimiter)) {
            if (!token.empty() && token.back() == '\r') {
                token.pop_back();
            }
            tokens.push_back(token);
        }
        return tokens;
    }
}