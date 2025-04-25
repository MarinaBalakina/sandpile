#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include<vector>


std::string input_path = "";
std::string output_path ="";
uint64_t max_iter = pow(2, 63);
uint64_t freq = 0;
bool use_output = false;
bool use_input = false;
bool use_freq = false;
bool use_max_iter = false;

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t bfType;      
    uint32_t bfSize;   
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BMPInfoHeader {
    uint32_t biSize;         
    int32_t biWidth; 
    int32_t biHeight;         
    uint16_t biPlanes;
    uint16_t biBitCount; 
    uint32_t biCompression; 
    uint32_t biSizeImage; 
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter; 
    uint32_t biClrUsed;  
    uint32_t biClrImportant; 
};

#pragma pack(pop)

void save_bmp(const std::string& name_of_pic, uint64_t** matrix, uint32_t height, uint32_t width);
bool is_num(std::string& num);
void expand_right(uint32_t**& matrix, uint32_t& rows, uint32_t& cols);
void expand_down(uint32_t**& matrix, uint32_t& rows, uint32_t& cols);
void expand_left(uint32_t**& matrix, uint32_t& rows, uint32_t& cols);
void expand_up(uint32_t**& matrix, uint32_t& rows, uint32_t& cols);
