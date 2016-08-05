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
#include <QTextStream>

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

    int ret = a.exec();

    if ( 0 == ret )
    {
        QTextStream cout(stdout);

        QString filtering_file = w.filterFile();
        if ( !filtering_file.isEmpty() )
        {
            cout << "Set\n\n"
                 << "        SCOREP_FILTERING_FILE=" << filtering_file << "\n\n"
                 << "to apply the filter at measurement time.\n";
        }

        QString total_memory = w.totalMemory();
        if ( total_memory.isEmpty() )
        {
            cout << "warning: The memory requirements cannot be satisfied by Score-P to avoid\n"
                 << "         intermediate flushes when tracing.\n"
                 << "Set\n\n"
                 << "        SCOREP_TOTAL_MEMORY=4G\n\n"
                 << "to get the maximum supported memory or reduce requirements using USR\n"
                 << "regions filters.\n";
        }
        else
        {
            cout << "When tracing set\n\n"
                 << "        SCOREP_TOTAL_MEMORY=" << total_memory << "\n\n"
                 << "to avoid intermediate flushes.\n";
        }
    }

    return ret;
}
