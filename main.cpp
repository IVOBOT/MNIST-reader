#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct MNISTHeader {
    uint32_t magic_number;
    uint32_t num_items;
};

void printImage(const std::vector<uint8_t>& image, uint32_t num_rows, uint32_t num_columns) {
    static const std::string grayscale_chars = "@%#*+=-:. ";
    static const std::string frame_chars [6] = {"╔","═","╗","║","╝","╚"};
    static const int horizontal_spacing = 2;
    static const double max_value = 255;
    const int spaced_columns = num_columns + (num_columns - 1) * horizontal_spacing;

    const uint32_t frame_width = 1;
    const uint32_t total_rows = num_rows + 2 * frame_width;
    const uint32_t total_columns = num_columns + 2 * frame_width;

    std::cout << frame_chars[0];
    for (uint32_t col = 0; col < spaced_columns ; ++col) {
        std::cout << frame_chars[1];
    }
    std::cout << frame_chars[2] << std::endl;

    for (uint32_t row = 0; row < num_rows; ++row) {
        std::cout << frame_chars[3];
        for (uint32_t col = 0; col < num_columns; ++col) {
            uint8_t pixel = image[row * num_columns + col];
            size_t index = pixel * (grayscale_chars.length() - 1) / max_value;

            char symbol = grayscale_chars[index];
            std::cout << symbol;
            if ( col != num_columns - 1 ) { for (int i = 0 ; i < horizontal_spacing; i++ ) std::cout << " "; }
        }
        std::cout << frame_chars[3];
        std::cout << std::endl;
    }

    std::cout << frame_chars[5];
    for (uint32_t col = 0; col < spaced_columns; ++col) {
        std::cout << frame_chars[1];
    }
    std::cout << frame_chars[4] << std::endl;
}

int main() {
    const uint32_t n = 5;  // Number of pictures
    const double max_value = 255;

    std::ifstream imagesFile("../train-images-idx3-ubyte", std::ios::binary);
    std::ifstream labelsFile("../train-labels-idx1-ubyte", std::ios::binary);

    if (!imagesFile || !labelsFile) {
        std::cout << "Can't open files." << std::endl;
        return 1;
    }

    MNISTHeader imagesHeader, labelsHeader;
    imagesFile.read(reinterpret_cast<char*>(&imagesHeader), sizeof(imagesHeader));
    labelsFile.read(reinterpret_cast<char*>(&labelsHeader), sizeof(labelsHeader));

    uint32_t num_images = __builtin_bswap32(imagesHeader.num_items);
    uint32_t num_rows, num_columns;
    imagesFile.read(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));
    imagesFile.read(reinterpret_cast<char*>(&num_columns), sizeof(num_columns));
    num_rows = __builtin_bswap32(num_rows);
    num_columns = __builtin_bswap32(num_columns);

    std::vector<uint8_t> images(num_images * num_rows * num_columns);
    std::vector<uint8_t> labels(num_images);

    imagesFile.read(reinterpret_cast<char*>(images.data()), num_images * num_rows * num_columns);
    labelsFile.read(reinterpret_cast<char*>(labels.data()), num_images);

    imagesFile.close();
    labelsFile.close();

    for (uint32_t i = 0; i < n; ++i) {
        uint8_t label = labels[i];
        std::cout << "Label of picture  " << i+1 << ": " << static_cast<int>(label) << std::endl;
        printImage(std::vector<uint8_t>(images.begin() + i * num_rows * num_columns, images.begin() + (i + 1) * num_rows * num_columns), num_rows, num_columns);
        std::cout << std::endl;
    }

    return 0;
}
