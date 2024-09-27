#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>

#define BLOCK_SIZE 512

// Define byte as an 8-bit unsigned integer.
using BYTE = uint8_t;

int main(int argc, char *argv[])
{
    // Ensure proper usage.
    if (argc != 2)
    {
        std::cerr << "Usage: ./recover file" << std::endl;
        return 1;
    }

    // Open memory card.
    std::ifstream file(argv[1], std::ios::binary);
    // Check if the file is valid.
    if (!file)
    {
        std::cerr << "Could not open " << argv[1] << "." << std::endl;
        return 2;
    }

    // Read 512 bytes into a buffer.
    BYTE buffer[BLOCK_SIZE];
    // Outfile stream.
    std::ofstream img;
    // JPEG's number.
    int n_JPEG = 0;

    // Repeat until end of card.
    while (file.read(reinterpret_cast<char *>(buffer), BLOCK_SIZE))
    {
        // If found the start of a new JPEG.
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // If it is not the first JPEG found.
            if (img.is_open())
            {
                // Close current JPEG.
                img.close();
            }

            // Create a new filename.
            std::ostringstream filename;
            filename << std::setw(3) << std::setfill('0') << n_JPEG++ << ".jpg";
            img.open(filename.str(), std::ios::binary);
        }

        // If already found JPEG.
        if (img.is_open())
        {
            img.write(reinterpret_cast<char *>(buffer), BLOCK_SIZE);
        }
    }

    // Close infile and outfile.
    file.close();
    if (img.is_open())
    {
        img.close();
    }

    return 0;
}
