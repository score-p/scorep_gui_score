/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include "mainwindow.hpp"
#include <QApplication>
#include <QFontDatabase>

int
main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    MainWindow   w;
    QFont        def( "DejaVu Sans", 10, QFont::Normal );
    /*QString fontPath = "./libs/Qt/fonts/courbi.pfa";
       int fontId = QFontDatabase::addApplicationFont(fontPath);
       if(fontId != -1) {
        QFont f("myFont");
        a.setFont(f);
       }*/
    a.setFont( def );
    /*first check if link is set*/
    w.show();
    if ( argc != 1 )
    {
        w.initOpen( argv[ 1 ] );
    }

    return a.exec();
}
