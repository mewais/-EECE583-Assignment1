 #include "InFileReader.hpp"

namespace INFILE
{

    std::vector<std::vector<uint32_t>> Grid;

    int readInFile(std::string& FileName, bool BeVerbose)
    {
        // Open file
        std::ifstream InFile(FileName);

        if(!InFile)
        {
            std::cout << "ERouter: INFILE: Invalid File Name.\n";
            exit(0);
        }

        // Start by getting the Grid Size
        uint32_t GridXSize, GridYSize;
        InFile >> GridXSize >> GridYSize;
        Grid.resize(GridXSize);
        for (uint32_t i = 0; i < Grid.size(); i++)
        {
            Grid[i].resize(GridYSize);
        }
        if (BeVerbose)
            std::cout << "\tINFILE: Created grid of size (" << GridXSize << "," << GridYSize << ").\n";

        // Then Read Obstruction Locations
        // We read those directly into the Grid
        uint32_t NumObstructions;
        uint32_t TmpXLoc, TmpYLoc;
        InFile >> NumObstructions;
        if (BeVerbose)
            std::cout << "\tINFILE: Will read " << NumObstructions << " obstructions.\n";

        for (uint32_t i = 0; i < NumObstructions; i++)
        {
            InFile >> TmpXLoc >> TmpYLoc;
            Grid[TmpXLoc][TmpYLoc] = IN_OBSTRUCTED;
            if (BeVerbose)
                std::cout << "\tINFILE: OBSTRUCTION " << i+1 << ": X = " << TmpXLoc << ", Y = " << TmpYLoc << ".\n";
        }

        // Start reading pins, add those to the grid and create a list of nets.
        uint32_t NumWires;
        uint32_t NumPinsPerWire;
        InFile >> NumWires;
        // Now setup the copy Grids that the Router uses.
        ROUTER::sizeGrids(NumWires, GridXSize, GridYSize);
        for (uint32_t i = 0; i < Grid.size(); i++)
        {
            for (uint32_t j = 0; j < Grid[i].size(); j++)
            {
                if(Grid[i][j] == IN_OBSTRUCTED)
                {
                    ROUTER::addObstruction(i, j);
                }
            }
        }
        if (BeVerbose)
            std::cout << "\tINFILE: Will read " << NumWires << " wires.\n";
        for (uint32_t i = 0; i < NumWires; i++)
        {
            InFile >> NumPinsPerWire;
            if (BeVerbose)
                std::cout << "\tINFILE: WIRE " << i+1 << ": has " << NumPinsPerWire << " pins.\n";
            std::vector<std::tuple<uint32_t, uint32_t, bool>> Net;
            for (uint32_t j = 0; j < NumPinsPerWire; j++)
            {
                InFile >> TmpXLoc >> TmpYLoc;
                Grid[TmpXLoc][TmpYLoc] = i+IN_OBSTRUCTED+1;
                Net.push_back(std::make_tuple(TmpXLoc, TmpYLoc, false));
                ROUTER::addPin(i, TmpXLoc, TmpYLoc);
                if (BeVerbose)
                    std::cout << "\tINFILE: WIRE " << i+1 << ": PIN " << j+1 << ": X = " << TmpXLoc << ", Y = " << TmpYLoc << ".\n";
            }
            ROUTER::addNet(Net);
        }

        return 0;
    }

    std::pair<uint32_t, uint32_t> getGridSize()
    {
        return std::make_pair(Grid.size(), Grid[0].size());
    }

    uint32_t getGridElement(uint32_t X, uint32_t Y)
    {
        return Grid[X][Y];
    }

    void setGridElement(uint32_t X, uint32_t Y, uint32_t Value)
    {
        Grid[X][Y] = Value;
    }

    std::vector<std::vector<uint32_t>> getGrid()
    {
        return Grid;
    }
}
