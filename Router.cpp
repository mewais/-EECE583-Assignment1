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

    void addPin(uint32_t Net, uint32_t X, uint32_t Y)
    {
        // add the pin as pin to its net, and obstruction to others.
        for (uint32_t i = 0; i < NetGrids.size(); i++)
        {
            NetGrids[i][X][Y] = R_OBSTRUCTED;
        }
        NetGrids[Net][X][Y] = R_PIN;
    }

    std::tuple<uint32_t, uint32_t, uint32_t> getGridSize()
    {
        return std::make_tuple(NetGrids.size(), NetGrids[0].size(), NetGrids[0][0].size());
    }

    int32_t getGridElement(uint32_t Net, uint32_t X, uint32_t Y)
    {
        return NetGrids[Net][X][Y];
    }

    uint32_t LeeMoore(uint32_t threads, LAYOUT::LayoutWidget *MainWindow, bool BeVerbose, bool Blocking)
    {
        // Each net is handled by a thread (if possible)
        // Nets.size() is actually of "vector::size_type". throws a gcc error
        // without casting.
        if (BeVerbose)
            std::cout << "ROUTER: Router started.\n";
        omp_set_num_threads(std::min(threads, (uint32_t)Nets.size()));

        uint32_t Unconnected = 0;

        #pragma omp parallel for shared(Unconnected)
        for (uint32_t i = 0; i < Nets.size(); i++)
        {
            // Each thread will start by picking a start point. A start point is
            // the first pin that is not connected.
            bool LookFor = false;
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
                    // popping them from the queue in the process. continue
                    // until you reach a pin.
                    // NOTE: the first time we run this, we look for an "unvisited"
                    // target, after that we might only connect to visited ones.
                    // This is to prevent from connected each part of the wire on its own!
                    std::get<2>(Nets[i][j]) = true;
                    std::queue<std::pair<uint32_t, uint32_t>> Visited;
                    Visited.push(std::make_pair(std::get<0>(Nets[i][j]), std::get<1>(Nets[i][j])));
                    uint32_t TargetX = 0;           // Suppress annoying "uninitialized" warnings
                    uint32_t TargetY = 0;           // Suppress annoying "uninitialized" warnings
                    uint32_t TargetWeight = 0;      // Suppress annoying "uninitialized" warnings
                    uint32_t SourceX = std::get<0>(Visited.front());
                    uint32_t SourceY = std::get<1>(Visited.front());
                    bool Found = false;
                    while(Visited.size())
                    {
                        // Check for out of bounds error first.
                        // If the surrounding cells are clear. give weight and
                        // push to queue.
                        if (MainWindow)
                        {
                            MainWindow->update();
                            usleep(20000);
                            // std::cin.ignore();
                        }

                        uint32_t StartX = std::get<0>(Visited.front());
                        uint32_t StartY = std::get<1>(Visited.front());
                        uint32_t Weight = NetGrids[i][StartX][StartY];
                        if (BeVerbose)
                            std::cout << "LEEMOORE: Phase1: Net" << i << " Visited cell " << StartX << ", " << StartY << " with weight " << Weight << ".\n";
                        if (StartX < (std::get<1>(getGridSize())-1))
                        {
                            if (NetGrids[i][StartX+1][StartY] == R_BLANK)
                            {
                                if (BeVerbose)
                                    std::cout << "\tLEEMOORE: Phase1: Net" << i << " Visiting cell " << StartX+1 << ", " << StartY << ".\n";
                                NetGrids[i][StartX+1][StartY] = Weight+1;
                                Visited.push(std::make_pair(StartX+1, StartY));
                            }
                            else if (NetGrids[i][StartX+1][StartY] == R_PIN)
                            {
                                for(uint32_t k = j; k < Nets[i].size(); k++)
                                {
                                    if(std::get<0>(Nets[i][k]) == StartX+1 &&
                                        std::get<1>(Nets[i][k]) == StartY &&
                                        std::get<2>(Nets[i][k]) == LookFor &&
                                        ((StartX+1 != SourceX) || (StartY != SourceY)))
                                    {
                                        Found = true;
                                        LookFor = true;
                                        TargetX = StartX+1;
                                        TargetY = StartY;
                                        TargetWeight = Weight + 1;
                                        break;
                                    }
                                }
                                if (Found)
                                    break;
                            }
                        }
                        if (StartX > 0)
                        {
                            if (NetGrids[i][StartX-1][StartY] == R_BLANK)
                            {
                                if (BeVerbose)
                                    std::cout << "\tLEEMOORE: Phase1: Net" << i << " Visiting cell " << StartX-1 << ", " << StartY << ".\n";
                                NetGrids[i][StartX-1][StartY] = Weight+1;
                                Visited.push(std::make_pair(StartX-1, StartY));
                            }
                            else if (NetGrids[i][StartX-1][StartY] == R_PIN)
                            {
                                for(uint32_t k = j; k < Nets[i].size(); k++)
                                {
                                    if(std::get<0>(Nets[i][k]) == StartX-1 &&
                                        std::get<1>(Nets[i][k]) == StartY &&
                                        std::get<2>(Nets[i][k]) == LookFor &&
                                        ((StartX-1 != SourceX) || (StartY != SourceY)))
                                    {
                                        Found = true;
                                        LookFor = true;
                                        TargetX = StartX-1;
                                        TargetY = StartY;
                                        TargetWeight = Weight + 1;
                                        break;
                                    }
                                }
                                if (Found)
                                    break;
                            }
                        }
                        if (StartY < (std::get<2>(getGridSize())-1))
                        {
                            if (NetGrids[i][StartX][StartY+1] == R_BLANK)
                            {
                                if (BeVerbose)
                                    std::cout << "\tLEEMOORE: Phase1: Net" << i << " Visiting cell " << StartX << ", " << StartY+1 << ".\n";
                                NetGrids[i][StartX][StartY+1] = Weight+1;
                                Visited.push(std::make_pair(StartX, StartY+1));
                            }
                            else if (NetGrids[i][StartX][StartY+1] == R_PIN)
                            {
                                for(uint32_t k = j; k < Nets[i].size(); k++)
                                {
                                    if(std::get<0>(Nets[i][k]) == StartX &&
                                        std::get<1>(Nets[i][k]) == StartY+1 &&
                                        std::get<2>(Nets[i][k]) == LookFor &&
                                        ((StartX != SourceX) || (StartY+1 != SourceY)))
                                    {
                                        Found = true;
                                        LookFor = true;
                                        TargetX = StartX;
                                        TargetY = StartY+1;
                                        TargetWeight = Weight + 1;
                                        break;
                                    }
                                }
                                if (Found)
                                    break;
                            }
                        }
                        if (StartY > 0)
                        {
                            if (NetGrids[i][StartX][StartY-1] == R_BLANK)
                            {
                                if (BeVerbose)
                                    std::cout << "\tLEEMOORE: Phase1: Net" << i << " Visiting cell " << StartX << ", " << StartY-1 << ".\n";
                                NetGrids[i][StartX][StartY-1] = Weight+1;
                                Visited.push(std::make_pair(StartX, StartY-1));
                            }
                            else if (NetGrids[i][StartX][StartY-1] == R_PIN)
                            {
                                for(uint32_t k = j; k < Nets[i].size(); k++)
                                {
                                    if(std::get<0>(Nets[i][k]) == StartX &&
                                        std::get<1>(Nets[i][k]) == StartY-1 &&
                                        std::get<2>(Nets[i][k]) == LookFor &&
                                        ((StartX != SourceX) || (StartY-1 != SourceY)))
                                    {
                                        Found = true;
                                        LookFor = true;
                                        TargetX = StartX;
                                        TargetY = StartY-1;
                                        TargetWeight = Weight + 1;
                                        break;
                                    }
                                }
                                if (Found)
                                    break;
                            }
                        }
                        Visited.pop();
                        if (BeVerbose)
                            std::cout << "LEEMOORE: Phase1: Net" << i << " popping " << StartX << ", " << StartY << " with weight " << Weight << ".\n";
                    }
                    // Set the target as visited. we're going to connect it now.
                    for (uint32_t k = j; k < Nets[i].size(); k++)
                    {
                        if(std::get<0>(Nets[i][k]) == TargetX &&
                            std::get<1>(Nets[i][k]) == TargetY)
                        {
                            std::get<2>(Nets[i][k]) = true;
                        }
                    }
                    // Now that we could reach a target, we should now start phase
                    // 2 and connect it. every cell that is part of the wire is
                    // now added to the Net pins and marked visited.
                    if(!Found)
                    {
                        Unconnected++;
                    }
                    while(Found)
                    {
                        if (BeVerbose)
                            std::cout << "LEEMOORE: Phase2: Net" << i << " Connected cell " << TargetX << ", " << TargetY << " with weight " << TargetWeight << ".\n";

                        INFILE::setGridElement(TargetX, TargetY, i+IN_OBSTRUCTED+1);
                        NetGrids[i][TargetX][TargetY] = R_PIN;
                        if (Blocking)
                        {
                            for (uint32_t k = 0; k < NetGrids.size(); k++)
                            {
                                if(k != i)
                                {
                                    NetGrids[k][TargetX][TargetY] = R_NET_OBSTRUCTED;
                                }
                            }
                        }

                        if (MainWindow)
                        {
                            MainWindow->update();
                            usleep(20000);
                            // std::cin.ignore();
                        }

                        if (TargetX < (std::get<1>(getGridSize())-1))
                        {
                            if (NetGrids[i][TargetX+1][TargetY] == TargetWeight-1)
                            {
                                TargetWeight--;
                                TargetX++;
                                Nets[i].push_back(std::make_tuple(TargetX+1, TargetY, true));
                                continue;
                            }
                        }
                        if (TargetX > 0)
                        {
                            if (NetGrids[i][TargetX-1][TargetY] == TargetWeight-1)
                            {
                                TargetWeight--;
                                TargetX--;
                                Nets[i].push_back(std::make_tuple(TargetX-1, TargetY, true));
                                continue;
                            }
                        }
                        if (TargetY < (std::get<2>(getGridSize())-1))
                        {
                            if (NetGrids[i][TargetX][TargetY+1] == TargetWeight-1)
                            {
                                TargetWeight--;
                                TargetY++;
                                Nets[i].push_back(std::make_tuple(TargetX, TargetY+1, true));
                                continue;
                            }
                        }
                        if (TargetY > 0)
                        {
                            if (NetGrids[i][TargetX][TargetY-1] == TargetWeight-1)
                            {
                                TargetWeight--;
                                TargetY--;
                                Nets[i].push_back(std::make_tuple(TargetX, TargetY-1, true));
                                continue;
                            }
                        }
                        break;
                    }
                    // Now clear up the grid and start over if there's anything else.
                    for (uint32_t k = 0; k < NetGrids[i].size(); k++)
                    {
                        for (uint32_t l = 0; l < NetGrids[i][k].size(); l++)
                        {
                            if (NetGrids[i][k][l] != R_OBSTRUCTED && NetGrids[i][k][l] != R_PIN
                                && NetGrids[i][k][l] != R_BLANK && NetGrids[i][k][l] != R_NET_OBSTRUCTED)
                            {
                                NetGrids[i][k][l] = R_BLANK;
                            }
                        }
                    }
                }
            }
        }
        return Unconnected;
    }
}
