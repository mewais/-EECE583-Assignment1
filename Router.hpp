#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>

#include <unistd.h>

#include <omp.h>

#include "Layout.hpp"

#define R_BLANK 0
#define R_OBSTRUCTED -1
#define R_PIN 1

namespace LAYOUT
{
    class LayoutWidget;
}

namespace ROUTER
{
    // This structure saves nets, and their pins, and a boolean value that states
    // whether the pins is connected or not.
    extern std::vector<std::vector<std::tuple<uint32_t, uint32_t, bool>>> Nets;

    // Similar to the GUI grid, we store a grid for each net! Not just for the
    // duration of the LeeMoore algorithm, but throughout the whole routing.
    // The first use of this is obviously the LeeMoore. The second is instead
    // of "rip-up/re-route" we save all possible routes, and the problem of
    // solving overlapped nets becomes a problem of "choosing" non-conflicting
    // nets. The third use of this is that without it we cannot do multi-threading,
    // Think about it, LeeMoore works by assigning numburs to cells, what if two
    // nets try to assign numbers to the same cell?
    // TODO: This is not really memory efficient. However, Later it can be
    // converted to only save a "relevant" part of the grid.
    extern std::vector<std::vector<std::vector<int32_t>>> NetGrids;

    void addNet(std::vector<std::tuple<uint32_t, uint32_t, bool>> Net);
    void sizeGrids(uint32_t Nets, uint32_t X, uint32_t Y);
    void addObstruction(uint32_t X, uint32_t Y);
    void addPin(uint32_t Net, uint32_t X, uint32_t Y);

    std::tuple<uint32_t, uint32_t, uint32_t> getGridSize();
    int32_t getGridElement(uint32_t Net, uint32_t X, uint32_t Y);

    void LeeMoore(uint32_t threads, LAYOUT::LayoutWidget *MainWindow, bool BeVerbose);
}

#endif // ROUTER_HPP
