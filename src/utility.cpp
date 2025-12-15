#include "utility.h"
#include <cstdio>

namespace utility
{
    std::vector<char> loadFile(std::filesystem::path path)
    {
        FILE *file = fopen(path.c_str(), "rb");

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        rewind(file);

        std::vector<char> buf(size + 1);
        fread(buf.data(), size, 1, file);
        fclose(file);

        buf[size] = '\0';

        return buf;
    }
}