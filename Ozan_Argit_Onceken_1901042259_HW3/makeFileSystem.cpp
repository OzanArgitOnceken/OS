#include <fstream>
#include <cstring>
#include <iostream>  	

struct DirectoryEntry {
    char filename[8];
    char extension[3];
    uint8_t attribute;
    uint16_t time;
    uint16_t date;
    uint16_t startingBlock;
    uint32_t size;
};

struct SuperBlock {
    uint16_t blockSize;
    uint32_t numBlocks;
    uint16_t rootDirPos;
    uint16_t fatPos;
    uint16_t fatLen;
};

void makeFileSystem(int blockSizeKb, const char* filename) {
    std::ofstream out(filename, std::ios::binary);

    // Create the super block
    SuperBlock sb;
    sb.blockSize = blockSizeKb * 1024;
    sb.numBlocks = 16 * 1024 * 1024 / sb.blockSize;
    sb.rootDirPos = 1;
    sb.fatPos = 2;
    sb.fatLen = (sb.numBlocks + 511) / 512;  // Each FAT entry is 12 bits (1.5 bytes)

    out.write(reinterpret_cast<char*>(&sb), sizeof(sb));

    // Create the root directory
    DirectoryEntry root;
    std::memset(&root, 0, sizeof(root));
    strcpy(root.filename, "");
    root.attribute = 0x10;
    out.seekp(sb.blockSize * sb.rootDirPos);
    out.write(reinterpret_cast<char*>(&root), sizeof(root));

    // Initialize the FAT
    uint16_t fat[sb.numBlocks];
    std::memset(fat, 0, sizeof(fat));
    out.seekp(sb.blockSize * sb.fatPos);
    out.write(reinterpret_cast<char*>(fat), sizeof(fat));

    // Fill the rest of the file with zeroes
    out.seekp(16 * 1024 * 1024 - 1);
    out.put('\0');
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: makeFileSystem <blockSizeKb> <filename>\n";
        return 1;
    }

    int blockSizeKb = std::stoi(argv[1]);
    const char* filename = argv[2];
    makeFileSystem(blockSizeKb, filename);

    return 0;
}

