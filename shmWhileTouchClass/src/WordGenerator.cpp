#include "../include/WordGenerator.hpp"
#include <iostream>
#include <string>
#include <random>

std::string generateFixedLengthMessage(size_t targetSize) {
    if (targetSize == 0) return "";
    
    const std::string charset = 
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .,!?";
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, charset.size() - 1);
    
    std::string message;
    message.reserve(targetSize);
    
    for (size_t i = 0; i < targetSize; ++i) {
        message.push_back(charset[distribution(generator)]);
    }
    
    return message;
}
