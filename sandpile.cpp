#include <iostream> 
#include "sandpile.h"


void save_bmp(const std::string& name_of_pic, uint32_t** matrix, uint32_t height, uint32_t width) {
    const uint16_t bits_per_pixel = 4; 
    const uint32_t color_table_size = 16; 

    int bytes_per_row = (width * bits_per_pixel + 7) / 8;
    int row_size = ((bytes_per_row + 3) / 4) * 4;
    int image_size = row_size * height;

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    fileHeader.bfType = 0x4D42;
    fileHeader.bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + (color_table_size * 4) + image_size;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + (color_table_size * 4);

    infoHeader.biSize = sizeof(BMPInfoHeader);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = bits_per_pixel;
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = image_size;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = color_table_size;
    infoHeader.biClrImportant = color_table_size;

    std::ofstream bmp_stream(name_of_pic, std::ios::binary);
    if (!bmp_stream) {
        std::cerr << "Error: The file could not be opened " << name_of_pic << "\n";
        return;
    }

    bmp_stream.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    bmp_stream.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    uint8_t colorTable[16][4] = {
        {255, 255, 255, 0}, // 0 - Белый (Blue=255, Green=255, Red=255)
        {0, 0, 0, 0},       // 1 - Черный
        {0, 0, 139, 0},     // 2 - Темно-красный (Blue=0, Green=0, Red=139)
        {0, 0, 255, 0},     // 3 - Красный (Blue=0, Green=0, Red=255)
        {34, 34, 178, 0},   // 4 - FireBrick (Blue=34, Green=34, Red=178)
    };

    for (int i = 0; i < color_table_size; ++i) {
        bmp_stream.write(reinterpret_cast<char*>(colorTable[i]), sizeof(colorTable[i]));
    }

    for (int y = height - 1; y >= 0; --y) {
        std::vector<uint8_t> row_data;
        for (uint32_t x = 0; x < width; x += 2) {
            uint8_t pixel_byte = 0;
            uint8_t pixel1 = static_cast<uint8_t>(matrix[y][x]);
            if (pixel1 > 15) pixel1 = 15; 
            pixel_byte |= (pixel1 << 4);

            if (x + 1 < width) {
                uint8_t pixel2 = static_cast<uint8_t>(matrix[y][x + 1]);
                if (pixel2 > 15) pixel2 = 15;
                pixel_byte |= (pixel2 & 0x0F);
            }

            row_data.push_back(pixel_byte);
        }

        while (row_data.size() < row_size) {
            row_data.push_back(0);
        }

        bmp_stream.write(reinterpret_cast<char*>(row_data.data()), row_size);
    }

    bmp_stream.close();
}


void expand_right(uint32_t**& matrix, uint32_t& rows, uint32_t& cols) {
    uint32_t** new_matrix = new uint32_t*[rows];
    for (uint32_t i = 0; i < rows; i++) {
        new_matrix[i] = new uint32_t[cols + 1];
    }

    for (uint32_t i = 0; i < rows; i++) {
        for (uint32_t j = 0; j <= cols; j++) {
            if (j < cols) {
                new_matrix[i][j] = matrix[i][j];
            } else {
                new_matrix[i][j] = 0;
            }
        }
    }

    for (uint32_t i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    matrix = new_matrix;
    cols++;
}

void expand_down(uint32_t**& matrix, uint32_t& rows, uint32_t& cols) { 
    uint32_t** new_matrix = new uint32_t*[rows + 1];
    for (uint32_t i = 0; i <= rows; i++) {
        new_matrix[i] = new uint32_t[cols];
    }

    for (uint32_t i = 0; i <= rows; i++) {
        for (uint32_t j = 0; j < cols; j++) {
            if (i < rows && j < cols) {
                new_matrix[i][j] = matrix[i][j];
            } else {
                new_matrix[i][j] = 0;
            }
        }
    }

    for (uint32_t i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    matrix = new_matrix;
    rows++;
}


void expand_left(uint32_t**& matrix, uint32_t& rows, uint32_t& cols) { 
    uint32_t** new_matrix = new uint32_t*[rows];
    for (uint32_t i = 0; i < rows; i++) { 
        new_matrix[i] = new uint32_t[cols + 1];
    }

    for (uint32_t i = 0; i < rows; i++) {
        new_matrix[i][0] = 0; 
        for (uint32_t j = 0; j < cols; j++) { 
            new_matrix[i][j + 1] = matrix[i][j];
        }
    }

    for (uint32_t i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    matrix = new_matrix;
    cols++;
}

void expand_up(uint32_t**& matrix, uint32_t& rows, uint32_t& cols) {
    uint32_t** new_matrix = new uint32_t*[rows + 1];
    for (uint32_t i = 0; i <= rows; i++) {
        new_matrix[i] = new uint32_t[cols];
    }

    for (uint32_t j = 0; j < cols; j++) {
        new_matrix[0][j] = 0;
    }

    for (uint32_t i = 0; i < rows; i++) {
        for (uint32_t j = 0; j < cols; j++) {
            new_matrix[i + 1][j] = matrix[i][j];
        }
    }

    for (uint32_t i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    matrix = new_matrix;
    rows++;
}

bool is_num(std::string& num) {
    for (size_t i = 0; i < num.length(); ++i) {
        if (!isdigit(num[i])) {
            return false;
        }
    }
    return true;
}
std::string short_input = "-i";
std::string long_input = "--input";
std::string short_output = "-o";
std::string long_output = "--output";
std::string short_iter = "-m";
std::string long_iter = "--max-iter";
std::string short_freq = "-f";
std::string long_freq = "--freq";

int main(int argc, char **argv) {
    for(int l = 1; l < argc; l++){
        std::string cur_arg = argv[l];
        if( cur_arg == short_input || cur_arg == long_input){
            if (++l >= argc ) {
                std::cout<<"tgctvyhb   ";
                return EXIT_FAILURE;
            }
            input_path = argv[l];
            use_output = true;
        }
        else if(cur_arg == short_output || cur_arg == long_output){
            if (++l >= argc) {
                return EXIT_FAILURE;
            }
            output_path = argv[l];
            use_output = true;
        }
        else if(cur_arg == short_iter|| cur_arg == long_iter){
            if (++l >= argc) {
                return EXIT_FAILURE;
            }
            std::string el = argv[l];
            if (!is_num(el)){
                std::cout<<"Error: isn't a number"<<"\n";
                continue;
            }
            max_iter = std::stoll(argv[l]);
        }
        else if(cur_arg == short_freq || cur_arg == long_freq){
            if (++l >= argc) {
                return EXIT_FAILURE;
            }
            std::string el = argv[l];
            if (!is_num(el)){
                std::cout<<"Error: isn't a number"<<"\n";
                continue;
            }
            freq = std::stoll(argv[l]);
            use_freq = true;
        }
       
    }

    // Получение количества ячеек
    std::ifstream File(input_path); 
    std::string Line;
    if (!File.is_open()) {              // Проверка, открыт ли файл
        std::cout << input_path << '\n';
        std::cout << "Error: File isn't open.";
        return EXIT_FAILURE;
    }
    uint32_t num_of_cells = 0;
    while(getline(File, Line)){
        num_of_cells++;
    }
    File.close();

    int16_t** x_coordinats = new int16_t*[num_of_cells]; 
    int16_t** y_coordinats = new int16_t*[num_of_cells]; 
    int16_t** sand_in_cells = new int16_t*[num_of_cells];


    std::ifstream file(input_path); 
    std::string line;
    if (!file.is_open()) {              
        std::cout << input_path << '\n';
        std::cout << "Error: File isn't open.";
        return EXIT_FAILURE;
    }
    uint32_t index = 0, max_x = 1, max_y = 1;
    while(getline(file, line)){
        size_t first_space = line.find(" ");
        size_t last_space = line.find_last_of(" ");
        if(first_space == std::string::npos || last_space == std::string::npos || first_space == last_space){
            std::cout << "Error: Invalid line format.\n";
            continue;
        }
        int16_t x = static_cast<int16_t>(std::stoi(line.substr(0, first_space)));
        int16_t y = static_cast<int16_t>(std::stoi(line.substr(first_space + 1, last_space - first_space - 1)));
        int16_t sand = static_cast<int16_t> (std::stoi(line.substr(last_space + 1)));
        if(std::abs(x) > max_x){
            max_x = std::abs(x);
        }
        if(std::abs(y) > max_y){
            max_y = std::abs(y);
        }

        x_coordinats[index] = new int16_t(x);
        y_coordinats[index] = new int16_t(y);
        sand_in_cells[index] = new int16_t(sand);
        index++;
    }
    file.close();


    uint32_t** matrix = new uint32_t*[2 * max_y + 1]; 
    for (uint32_t i = 0; i < (2 * max_y + 1); i++){
        matrix[i] = new uint32_t [2 * max_x + 1]; 
    }
    for(int32_t row = 0; row < 2 * max_y + 1; row++){
        for(int32_t cols = 0 ; cols < 2 * max_x + 1; cols++){
            matrix[row][cols] = 0;
        }
    }
    for (uint32_t i = 0; i < num_of_cells; i++) {
        int32_t row = max_y - *y_coordinats[i];
        int32_t col = max_x + *x_coordinats[i];
        matrix[row][col] = *sand_in_cells[i];
    }
    
    uint32_t len_x = 2 * max_x +1, len_y = 2 * max_y +1;
    uint64_t count_freq = freq;
    int num_of_pic = 1;
    for(max_iter; max_iter != 0; max_iter--){
        bool do_expand_up = false;
        bool do_expand_left = false;
        bool do_expand_right = false;
        bool do_expand_down = false;
        bool use_max_iter = false;
        bool last_iter = true;
        for(int32_t y = 0; y < static_cast<int32_t>(len_y); y++){
            for(int32_t x = 0; x < static_cast<int32_t>(len_x); x++){
                if(matrix[y][x] >= 4){
                    if(y == 0){
                        do_expand_up = true;
                    }
                    if(x == 0){
                        do_expand_left = true;
                    }
                    if(x == static_cast<int32_t>(len_x) -1){
                        do_expand_right = true;
                    }
                    if(y == static_cast<int32_t>(len_y) -1){
                        do_expand_down = true;
                    }

                    use_max_iter = true;
                    last_iter = false;
                }
            }
        }

        if(do_expand_up){
            expand_up(matrix, len_y, len_x);
        }
        if(do_expand_left){
            expand_left(matrix, len_y, len_x);
        }
        if(do_expand_right){
            expand_right(matrix, len_y, len_x);
        }
        if(do_expand_down){
            expand_down(matrix, len_y, len_x);
        }

        uint32_t** scattering_matrix = new uint32_t*[len_y];
        for (uint32_t y = 0; y < len_y; y++) {
            scattering_matrix[y] = new uint32_t[len_x];
            for(uint32_t x = 0; x < len_x; x++){
                scattering_matrix[y][x] = 0; 
            }
        }


        for(uint32_t y = 0 ; y < len_y; y++){
            for(uint32_t x = 0; x < len_x; x++){
                if(matrix[y][x] >= 4){
                    scattering_matrix[y][x] += matrix[y][x] - 4;
                    if(x +1 < len_x){
                        scattering_matrix[y][x +1] +=1;
                    }
                    if(x >=1){
                        scattering_matrix[y][x -1] +=1;
                    }
                    if(y +1 < len_y){
                        scattering_matrix[y +1][x] +=1;
                    }
                    if(y >=1){
                        scattering_matrix[y -1][x] +=1;
                    }
                    use_max_iter = true;
                }
                else{
                    scattering_matrix[y][x] += matrix[y][x];
                }
            }
        }

        for (uint32_t y = 0; y < len_y; ++y){
            delete[] matrix[y];
        }
        delete[] matrix;
        matrix = scattering_matrix;
        count_freq--;
        if(count_freq == 0 && use_freq){
            std::string num_to_str = std::to_string(num_of_pic);
            std::string bmp_file = output_path + num_to_str + ".bmp";
            save_bmp(bmp_file, matrix, len_y, len_x);

            count_freq = freq;

        }
        else if(!use_freq && last_iter || max_iter == 1){
            std::string num_to_str = std::to_string(num_of_pic);
            std::string bmp_file = output_path +num_to_str + ".bmp";
            save_bmp(bmp_file, matrix, len_y, len_x);
        }

        num_of_pic++;
        if(last_iter){
            break;
        }
    }

    for(uint32_t i = 0; i < num_of_cells; i++) {
        delete x_coordinats[i];
        delete y_coordinats[i];
        delete sand_in_cells[i];
    }
    delete[] x_coordinats;
    delete[] y_coordinats;
    delete[] sand_in_cells;

    for(uint32_t y = 0; y < len_y; y++) {
        delete[] matrix[y];
    }
    delete[] matrix;
    return EXIT_SUCCESS;
}
