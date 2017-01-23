#ifndef INFILE_READER_HPP
#define INFILE_READER_HPP

#include <fstream>
#include <iostream>

#include <string>
#include <vector>
#include <utility>
#include <tuple>

#include "Layout.hpp"
#include "Router.hpp"

#define IN_BLANK 0
#define IN_OBSTRUCTED 1

namespace INFILE
{
    // The grid elements are filled by ints reprsenting what is in there.
    // Possible values are BLANK, OBSTRUCTED and numbers representing nets
    // passing through the block.
    // We assume pins to be routed are parts of the net, and so they're given
    // numbers as nets.
    // By default this vector is initalized to 0.
    extern std::vector<std::vector<uint32_t>> Grid;

    int readInFile(std::string& FileName, bool BeVerbose);
    std::pair<uint32_t, uint32_t> getGridSize();
    uint32_t getGridElement(uint32_t X, uint32_t Y);
    void setGridElement(uint32_t X, uint32_t Y, uint32_t Value);
    std::vector<std::vector<uint32_t>> getGrid();
}

#endif // INFILE_READER_HPP
