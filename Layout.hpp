#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include <QtCore/QSize>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <QtGui/QPainter>

#include <utility>
#include <algorithm>

#include <pthread.h>

#include "InFileReader.hpp"
#include "Router.hpp"

#define PixelsPerGridBlock 20

namespace LAYOUT
{
    void *InitRoute(void *PlaceHolder);

    // some of QT capabilities can only be inherited, I'm forced to use a class
    // here although nothing really requires an OOP structure.
    class LayoutWidget : public QWidget
    {
        protected:
            bool BeVerbose;

            void paintEvent(QPaintEvent *event);

        public:
            LayoutWidget(uint32_t ThreadCount, bool BeVerbose, bool Blocking);
    };
}

#endif // LAYOUT_HPP
