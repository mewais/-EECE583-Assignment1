#include "Router.hpp"


namespace ROUTER
{
    std::vector<std::vector<std::tuple<uint32_t, uint32_t, bool>>> Nets;
    std::vector<std::vector<std::vector<int32_t>>> NetGrids;

    void addNet(std::vector<std::tuple<uint32_t, uint32_t, bool>> Net)
    {
        Nets.push_back(Net);
    }

    void sizeGrids(uint32_t Nets, uint32_t X, uint32_t Y)
    {
        NetGrids.resize(Nets);
        for (uint32_t i = 0; i < Nets; i++)
        {
            NetGrids[i].resize(X);
            for (uint32_t j = 0; j < NetGrids[i].size(); j++)
            {
                NetGrids[i][j].resize(Y);
            }
        }
    }

    void addObstruction(uint32_t X, uint32_t Y)
    {
        for (uint32_t i = 0; i < NetGrids.size(); i++)
        {
            NetGrids[i][X][Y] = R_OBSTRUCTED;
        }
    }

    std::tuple<uint32_t, uint32_t, uint32_t> getGridSize()
    {
        return std::make_tuple(NetGrids.size(), NetGrids[0].size(), NetGrids[0][0].size());
    }

    int32_t getGridElement(uint32_t Net, uint32_t X, uint32_t Y)
    {
        return NetGrids[Net][X][Y];
    }

    void LeeMoore(uint32_t threads, LAYOUT::LayoutWidget *MainWindow, bool BeVerbose)
    {
        // Each net is handled by a thread (if possible)
        // Nets.size() is actually of "vector::size_type". throws a gcc error
        // without casting.
        if (BeVerbose)
            std::cout << "ROUTER: Router started.\n";
        omp_set_num_threads(std::min(threads, (uint32_t)Nets.size()));

        #pragma omp parallel for
        for (uint32_t i = 0; i < Nets.size(); i++)
        {
            // Each thread will start by picking a start point. A start point is
            // the first pin that is not connected.
            for (uint32_t j = 0; j < Nets[i].size(); j++)
            {
                if (std::get<2>(Nets[i][j]) == false)
                {
                    // This is the start point. Without knowing a "specific"
                    // target, we will just start doing the LeeMoore until
                    // we reach a target!
                    // The algorithm works as follows. from the starting point
                    // you visit the cells around you, if they're blank you assign
                    // a weight to them, and push them in a queue. next time you
                    // visit the queue and do the same for the cells in there,
                    // popping them from the queue in the process.
                    LeeMooreForward(1, i, std::get<0>(Nets[i][j]), std::get<1>(Nets[i][j]), MainWindow, BeVerbose);
                }
            }
        }
    }

    std::tuple<uint32_t, uint32_t, uint32_t> LeeMooreForward(uint32_t Weight, uint32_t Net, uint32_t StartX, uint32_t StartY, LAYOUT::LayoutWidget *MainWindow, bool BeVerbose)
    {
        // If there are any unobstructed cells around, increase the number in them
        // This function returns the "Nearest" Coordinate it could find and its weight.
        if (BeVerbose)
            std::cout << "ROUTER: Net " << Net << ": LeeMoore visiting cell " << StartX << ", " << StartY << " with weight " << Weight << ".\n";
        std::tuple<uint32_t, uint32_t, uint32_t> First, Second, Third, Fourth, Min;
        bool FoundSomething = false;

        // Update the block we just visited with a weight.
        NetGrids[Net][StartX][StartY] = Weight;

        // If the GUI is allowed, we will have to "update" the GUI so it reflects
        // changes. we should also call a delay method to allow seeing
        if (MainWindow)
        {
            MainWindow->update();
            usleep(250000);
        }

        // Check for out of bounds error first.
        if (StartX > 0 && StartX < (std::get<1>(getGridSize())-1))
        {
            if (NetGrids[Net][StartX+1][StartY] == R_BLANK)
            {
                FoundSomething = true;
                First = LeeMooreForward(Weight+1, Net, StartX+1, StartY, MainWindow, BeVerbose);
                Min = First;
            }
            if (NetGrids[Net][StartX-1][StartY] == R_BLANK)
            {
                FoundSomething = true;
                Second = LeeMooreForward(Weight+1, Net, StartX-1, StartY, MainWindow, BeVerbose);
                if (std::get<2>(Second) < std::get<2>(Min))
                    Min = Second;
            }
        }
        if (StartX > 0 && StartX < (std::get<2>(getGridSize())-1))
        {
            if (NetGrids[Net][StartX][StartY+1] == R_BLANK)
            {
                FoundSomething = true;
                Third = LeeMooreForward(Weight+1, Net, StartX, StartY+1, MainWindow, BeVerbose);
                if (std::get<2>(Third) < std::get<2>(Min))
                    Min = Third;
            }
            if (NetGrids[Net][StartX][StartY-1] == R_BLANK)
            {
                FoundSomething = true;
                Fourth = LeeMooreForward(Weight+1, Net, StartX, StartY-1, MainWindow, BeVerbose);
                if (std::get<2>(Fourth) < std::get<2>(Min))
                    Min = Fourth;
            }
        }
        if (!FoundSomething)
        {
            // We couldn't advance anymore. This is the end of it. return self.
            return std::make_tuple(StartX, StartY, Weight);
        }

        return Min;
    }
}
