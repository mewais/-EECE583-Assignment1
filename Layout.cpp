#include "Layout.hpp"

#include <iostream>

namespace LAYOUT
{
    void *InitRoute(void *PlaceHolder)
    {
        // This is a stupid intermediate function needed for pthreads.
        std::tuple<uint32_t, LAYOUT::LayoutWidget *, bool, bool> *Params;
        Params = (std::tuple<uint32_t, LAYOUT::LayoutWidget *, bool, bool>*) PlaceHolder;
        int Unconnected = ROUTER::LeeMoore(std::get<0>(*Params), std::get<1>(*Params), std::get<2>(*Params), std::get<3>(*Params));
        std::cout << "Failed to connect " << Unconnected << " pins.\n";
        return NULL;                // disable stupid gcc warning
    }

    LayoutWidget::LayoutWidget(uint32_t ThreadCount, bool BeVerbose, bool Blocking) : BeVerbose(BeVerbose)
    {
        // As far as I know, QT doesn't have a ready for use Grid Coordinate
        // System, It does however use a "pixel" coordinate system for drawing.
        // I will have to create (paint) a layout grid. This has to be done in
        // paintEvent.

        // Show the window and adjust the size to the size of layout
        if (BeVerbose)
            std::cout << "LAYOUT: Starting GUI.\n";
        this->resize(std::get<0>(INFILE::getGridSize())*PixelsPerGridBlock,
                    std::get<1>(INFILE::getGridSize())*PixelsPerGridBlock);
        this->show();
        // Now we have initialized the GUI, we can now start the router
        pthread_t RouteThread;
        std::tuple<uint32_t, LAYOUT::LayoutWidget *, bool, bool>* Params;
        Params = new std::tuple<uint32_t, LAYOUT::LayoutWidget *, bool, bool>;
        *Params = std::make_tuple(ThreadCount, this, BeVerbose, Blocking);
        pthread_create(&RouteThread, NULL, InitRoute, (void *)(Params));
    }

    void LayoutWidget::paintEvent(QPaintEvent *event)
    {
        // Create a Paint Area and Painter
        QPainter GridPainter(this);
        GridPainter.setPen(Qt::black);
        // draw vertical and horizontal lines that resemble the grid itself.
        for (uint32_t i = 0; i <= std::get<0>(INFILE::getGridSize())*PixelsPerGridBlock; i+=PixelsPerGridBlock)
        {
            GridPainter.drawLine(i, 0, i, std::get<1>(INFILE::getGridSize())*PixelsPerGridBlock);
        }
        for (uint32_t i = 0; i <= std::get<1>(INFILE::getGridSize())*PixelsPerGridBlock; i+=PixelsPerGridBlock)
        {
            GridPainter.drawLine(0, i, std::get<0>(INFILE::getGridSize())*PixelsPerGridBlock, i);
        }

        // Now we can draw the layout itself. We need to convert blocks to pixels.
        for (uint32_t i = 0; i < std::get<0>(INFILE::getGridSize()); i++)
        {
            for (uint32_t j = 0; j < std::get<1>(INFILE::getGridSize()); j++)
            {
                uint32_t Tmp;
                Tmp = INFILE::getGridElement(i, j);
                if(Tmp)
                {
                    // Could've used a random color generator here or whatever.
                    // but it's not really worth the effort. Sorry.
                    // The values for text locations were selected by trial and
                    // error. Do not change!
                    if (Tmp == 1)
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::blue);
                    }
                    else if (Tmp == 2)
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::red);
                        // GridPainter.drawText((i+0.3)*PixelsPerGridBlock, (j+0.75)*PixelsPerGridBlock, "1");
                    }
                    else if (Tmp == 3)
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::green);
                        // GridPainter.drawText((i+0.3)*PixelsPerGridBlock, (j+0.75)*PixelsPerGridBlock, "2");
                    }
                    else if (Tmp == 4)
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::yellow);
                        // GridPainter.drawText((i+0.3)*PixelsPerGridBlock, (j+0.75)*PixelsPerGridBlock, "3");
                    }
                    else if (Tmp == 5)
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::gray);
                        // GridPainter.drawText((i+0.3)*PixelsPerGridBlock, (j+0.75)*PixelsPerGridBlock, "4");
                    }
                    else if (Tmp == 6)
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::magenta);
                        // GridPainter.drawText((i+0.3)*PixelsPerGridBlock, (j+0.75)*PixelsPerGridBlock, "5");
                    }
                    else if (Tmp == 7)
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::cyan);
                        // GridPainter.drawText((i+0.3)*PixelsPerGridBlock, (j+0.75)*PixelsPerGridBlock, "6");
                    }
                    else
                    {
                        GridPainter.fillRect((i*PixelsPerGridBlock)+1, (j*PixelsPerGridBlock)+1,
                            PixelsPerGridBlock-1, PixelsPerGridBlock-1, Qt::black);
                        // GridPainter.drawText((i+0.3)*PixelsPerGridBlock, (j+0.75)*PixelsPerGridBlock, "7");
                    }
                }
            }
        }

        // Now we do the same for LeeMoore Algorithm, need to read values
        // from each net's grid to display weights
        // NOTE: I choose to this in a different loop and not in the one before
        // because this is better for cache access. More locality and stuff.
        for (uint32_t i = 0; i < std::get<0>(ROUTER::getGridSize()); i++)
        {
            for (uint32_t j = 0; j < std::get<1>(ROUTER::getGridSize()); j++)
            {
                for (uint32_t k = 0; k < std::get<2>(ROUTER::getGridSize()); k++)
                {
                    if (ROUTER::getGridElement(i, j, k) > 0)
                    {
                        GridPainter.drawText((j+0.3)*PixelsPerGridBlock, (k+0.75)*PixelsPerGridBlock, std::to_string(ROUTER::getGridElement(i, j, k)).c_str());
                    }
                }
            }
        }
    }
}
