#pragma once
#include <string>
#include <vector>
#include <sstream>
namespace Utils {
    /// <summary>
    /// 文字列を指定した区切り文字で分割する関数
    /// </summary>
    /// <param name="input">分割対象の文字列</param>
    /// <param name="delimiter">区切り文字</param>
    /// <returns>分割された文字列の配列</returns>
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