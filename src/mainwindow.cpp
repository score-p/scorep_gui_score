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

#include <limits>

#include "mainwindow.hpp"

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , mp_layout( 0 )
    , mp_sizeTable( 0 )
    , mp_groupTable( 0 )
    , mp_functionTable( 0 )
    , mp_statusBar( 0 )
    , mp_menu( 0 )
    , mp_progressbar( 0 )
    , mp_connection( 0 )
    , mp_prototypeNumberItem( 0 )
    , mp_prototypeTextItem( 0 )
    , mp_signalMapper( 0 )
{
    m_windowTitle = "Score-P scoring GUI";

    /*signal mapper to determine the position of a clicked checkbox*/
    mp_signalMapper = new QSignalMapper( this );

    /*init layout*/
    QWidget* window = new QWidget( this );
    mp_layout = new QVBoxLayout( window );
    setCentralWidget( window );

    m_noFilter << "MPI" << "ALL" << "OMP" << "SHMEM";

    /*init*/
    initTables();
    initMenu();
    mp_statusBar   = new QStatusBar( this );
    mp_progressbar = new QProgressBar( this );

    /*init prototypes for tableItems*/
    mp_prototypeNumberItem = new QTableWidgetItem();
    mp_prototypeNumberItem->setTextAlignment( Qt::AlignRight | Qt::AlignCenter );
    mp_prototypeTextItem = new QTableWidgetItem();
    mp_prototypeTextItem->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    /*add widget to layout*/
    mp_layout->addWidget( mp_progressbar );
    mp_layout->addWidget( mp_sizeTable );
    mp_layout->addWidget( mp_groupTable );
    mp_layout->addWidget( mp_functionTable );

    setMenuBar( mp_menu );
    setStatusBar( mp_statusBar );


    /*do connections*/
    connect( mp_groupTable, SIGNAL( cellPressed( int, int ) ), this, SLOT( unselectFunctionTable() ) );
    connect( mp_functionTable, SIGNAL( cellPressed( int, int ) ), this, SLOT( unselectGroupTable() ) );
    connect( mp_groupTable->horizontalHeader(), SIGNAL( sectionResized( int, int, int ) ),
             this, SLOT( resizeFunctionTable( int, int, int ) ) );
    connect( mp_signalMapper, SIGNAL( mapped( int ) ), this, SLOT( changeStateSlot( int ) ) );

    mp_groupTable->installEventFilter( this );
    mp_functionTable->installEventFilter( this );
    mp_sizeTable->installEventFilter( this );

    reset();
}

void
MainWindow::initTables()
{
    mp_sizeTable     = new QTableWidget( 3, 3, this );
    mp_groupTable    = new QTableWidget( 1, 8, this );
    mp_functionTable = new QTableWidget( 1, 8, this );
    /*adjust tables*/
    /*minimum width*/
    mp_groupTable->setMinimumWidth( 500 );
    mp_functionTable->setMinimumWidth( 500 );
    mp_sizeTable->setMinimumWidth( 550 );
    /*no horizontal scrollbar*/
    mp_groupTable->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    mp_functionTable->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    /*width for checkbox column*/
    mp_groupTable->setColumnWidth( 0, 15 );
    mp_functionTable->setColumnWidth( 0, 15 );
    /*not editable*/
    mp_sizeTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
    mp_groupTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
    mp_functionTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
    /*last column expands on resize*/
    mp_functionTable->horizontalHeader()->setStretchLastSection( true );
    mp_groupTable->horizontalHeader()->setStretchLastSection( true );
    mp_sizeTable->horizontalHeader()->setStretchLastSection( true );
    /*edit selection*/
    mp_sizeTable->setSelectionMode( QAbstractItemView::NoSelection );
    mp_functionTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    mp_functionTable->setSelectionMode( QAbstractItemView::SingleSelection );
    mp_groupTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    mp_groupTable->setSelectionMode( QAbstractItemView::SingleSelection );
    /*dont highlight header if something is selected*/
    mp_groupTable->horizontalHeader()->setHighlightSections( false );

    /*fill headers*/
    QStringList horizontalHeaders;
    horizontalHeaders << "" << "type" << "max_buff[B]" << "visits" << "time[s]" << "time[%]" << "time/visit[us]" << "region";
    mp_groupTable->setHorizontalHeaderLabels( horizontalHeaders );
    horizontalHeaders.clear();
    horizontalHeaders << "" << "without filter" << "with filter";
    mp_sizeTable->setHorizontalHeaderLabels( horizontalHeaders );
    /*hide headers*/
    mp_groupTable->verticalHeader()->hide();
    mp_functionTable->horizontalHeader()->hide();
    mp_functionTable->verticalHeader()->hide();
    mp_sizeTable->verticalHeader()->hide();

    /*set table height*/
    mp_sizeTable->setFixedHeight( 92 + mp_sizeTable->horizontalHeader()->height() );
    fillSizeTable();
}

void
MainWindow::initMenu()
{
    /*init MenuBar*/
    mp_menu = new QMenuBar( this );
    QAction* actionOpen   = new QAction( "Open", this );
    QAction* actionSave   = new QAction( "Save", this );
    QAction* actionExit   = new QAction( "Exit", this );
    QAction* actionSaveAs = new QAction( "Save As", this );
    /*set shortcuts*/
    actionOpen->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_O ) );
    actionSave->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ) );
    actionExit->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Q ) );
    actionSaveAs->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_S ) );
    actionOpen->setIcon( QIcon( ":icons/images/open.png" ) );
    actionSave->setIcon( QIcon( ":icons/images/save.png" ) );
    actionSaveAs->setIcon( QIcon( ":icons/images/save.png" ) );
    actionExit->setIcon( QIcon( ":icons/images/exit.png" ) );
    QMenu* fileMenu = new QMenu( "File" );
    fileMenu->addAction( actionOpen );
    fileMenu->addAction( actionSave );
    fileMenu->addAction( actionSaveAs );
    fileMenu->addAction( actionExit );
    QAction* actionShortcuts = new QAction( "Shortcuts", this );
    QMenu*   helpMenu        = new QMenu( "Help" );
    helpMenu->addAction( actionShortcuts );
    mp_menu->addMenu( fileMenu );
    mp_menu->addMenu( helpMenu );
    connect( actionOpen, SIGNAL( triggered( bool ) ), this, SLOT( openFile() ) );
    connect( actionSave, SIGNAL( triggered( bool ) ), this, SLOT( saveFile() ) );
    connect( actionSaveAs, SIGNAL( triggered( bool ) ), this, SLOT( saveFileAs() ) );
    connect( actionExit, SIGNAL( triggered( bool ) ), this, SLOT( close() ) );
    connect( actionShortcuts, SIGNAL( triggered( bool ) ), this, SLOT( showShortcuts() ) );
}

MainWindow::~MainWindow()
{
}


void
MainWindow::fillSizeTable()
{
    QString first  = "Estimated aggregate size of event trace:";
    QString second = "Estimated requirements for largest trace buffer (max_buf):";
    QString third  = "Estimated memory requirements (SCOREP_TOTAL_MEMORY):";

    QTableWidgetItem* item1 = new QTableWidgetItem( first, 0 );
    QTableWidgetItem* item2 = new QTableWidgetItem( second, 0 );
    QTableWidgetItem* item3 = new QTableWidgetItem( third, 0 );

    mp_sizeTable->setItem( 0, 0, item1 );
    mp_sizeTable->setItem( 1, 0, item2 );
    mp_sizeTable->setItem( 2, 0, item3 );

    mp_sizeTable->resizeColumnsToContents();
}

void
MainWindow::changeStateSlot( int pos )
{
    mp_statusBar->clearMessage();
    /*what if partially checked?*/
    QList<dataCenter::groupData> tempGroups;
    tempGroups = mp_connection->getGroupData();
    bool       groupTable = false;
    QList<int> keys;

    if ( pos < tempGroups.size() )
    {
        /*groupTable*/
        groupTable = true;
        mp_groupTable->selectRow( pos );
        mp_functionTable->clearSelection();
    }
    else
    {
        pos = pos - tempGroups.size();
        mp_functionTable->selectRow( pos );
        mp_groupTable->clearSelection();
        /*functionTable*/
    }
    keys.append( pos );
    mp_connection->changeState( keys, groupTable );
    setWindowModified( true );
    updateTables();
}

void
MainWindow::changeState()
{
    mp_statusBar->clearMessage();
    /*get selected row
     * changeState*/
    /*function to detext selection an pass to connector*/
    QList<int>                   keys;
    QModelIndexList              selection;
    QList<dataCenter::groupData> tempGroups;
    QList<dataCenter::data>      tempData = mp_connection->getFunctionData();
    QString                      type;
    tempGroups = mp_connection->getGroupData();
    bool groupTable = false;

    if ( mp_functionTable->selectedItems().size() > 0 )
    {
        selection = mp_functionTable->selectionModel()->selectedRows();
    }
    if ( mp_groupTable->selectedItems().size() > 0 )
    {
        selection  = mp_groupTable->selectionModel()->selectedRows();
        groupTable = true;
    }

    for ( int i = 0; i < selection.count(); i++ )
    {
        QModelIndex index = selection.at( i );
        keys.append( index.row() );
    }
    if ( groupTable )
    {
        type = QString::fromStdString( tempGroups[ keys.at( 0 ) ].type );
    }
    else
    {
        type = QString::fromStdString( tempData[ keys.at( 0 ) ].type );
    }
    if ( groupTable == true )
    {
        /*check if positon == filter*/
        if ( keys.at( 0 ) == mp_groupTable->rowCount() - 1 )
        {
            keys.clear();
            for ( int i = 0; i < tempGroups.size(); i++ )
            {
                if ( !m_noFilter.contains( QString::fromStdString( tempGroups[ i ].type ) ) )
                {
                    keys.append( i );
                }
            }
        }
    }
    if ( !mp_connection->changeState( keys, groupTable ) )
    {
        mp_statusBar->showMessage( type + " cannot be excluded" );
    }
    else
    {
        setWindowModified( true );
    }
    updateTables();
}


void
MainWindow::openFile()
{
    mp_statusBar->clearMessage();
    /*----------------------------------------------------------------------------------------------------------------*/
    /* reset all*/
    /*----------------------------------------------------------------------------------------------------------------*/
    QString fileName;
    fileName = QFileDialog::getOpenFileName( this, tr( "Open profile" ), QDir::currentPath(), tr( "Profile files (*.cubex)" ) );
    /*get directory*/
    if ( fileName != 0 )
    {
        reset();
        m_fileName = fileName;
        setWindowTitle( m_fileName + "[*] - " + m_windowTitle );
        mp_connection->start( fileName );
        mp_groupTable->selectRow( 0 );
        update();
    }
}

void
MainWindow::saveFile()
{
    mp_statusBar->clearMessage();
    if ( mp_connection->hasFiltered() )
    {
        if ( m_filterFileName.isEmpty() )
        {
            QString saveFileName;
            saveFileName = QFileDialog::getSaveFileName( this, tr( "Save filter file" ), QDir::currentPath(), tr( "Filter files (*.filter)" ) );
            if ( saveFileName.isEmpty() )
            {
                return;
            }
            if ( !saveFileName.endsWith( ".filter" ) )
            {
                saveFileName += ".filter";
            }
            m_filterFileName = saveFileName;
            if ( mp_connection->createFilterFile( saveFileName ) )
            {
                mp_statusBar->showMessage( "Filter file saved at " + saveFileName );
                setWindowModified( false );
            }
            else
            {
                mp_statusBar->showMessage( "Saving filterfile failed. No Permissions" );
            }
        }
        else
        {
            if ( mp_connection->createFilterFile( m_filterFileName ) )
            {
                mp_statusBar->showMessage( "Filter file saved at " + m_filterFileName );
                setWindowModified( false );
            }
            else
            {
                mp_statusBar->showMessage( "Saving filter file failed. No Permissions" );
            }
        }
    }
    else
    {
        mp_statusBar->showMessage( "Error: No functions to filter" );
    }
    updateTables();
}

void
MainWindow::saveFileAs()
{
    mp_statusBar->clearMessage();
    if ( mp_connection->hasFiltered() )
    {
        QString saveFileName;
        saveFileName = QFileDialog::getSaveFileName( this, tr( "Save filter file" ), QDir::currentPath(), tr( "Filter files (*.filter)" ) );
        if ( saveFileName == "" )
        {
            return;
        }
        if ( !saveFileName.endsWith( ".filter" ) )
        {
            saveFileName += ".filter";
        }
        m_filterFileName = saveFileName;
        if ( mp_connection->createFilterFile( saveFileName ) )
        {
            mp_statusBar->showMessage( "Filter file saved at " + saveFileName );
            setWindowModified( false );
        }
        else
        {
            mp_statusBar->showMessage( "Saving filter file failed. No Permissions" );
        }
    }
    else
    {
        mp_statusBar->showMessage( "Error: No functions to filter" );
    }
    updateTables();
}

void
MainWindow::initOpen( QString fileName )
{
    if ( fileName.endsWith( ".cubex" ) )
    {
        m_fileName = fileName;
        setWindowTitle( m_fileName + "[*] - " + m_windowTitle );
        mp_connection->start( fileName );
        mp_groupTable->selectRow( 0 );
        updateTables();
    }
    else
    {
        mp_statusBar->showMessage( "Error: Given file isn't a .cubex file" );
        updateTables();
    }
}

void
MainWindow::unselectFunctionTable()
{
    mp_functionTable->clearSelection();
}

void
MainWindow::unselectGroupTable()
{
    mp_groupTable->clearSelection();
}

void
MainWindow::updateTables()
{
    if ( m_fileName.isEmpty() )
    {
        return;
    }
    QStringList horizontalHeaders;
    horizontalHeaders << "" << "type" << "max_buff[B]" << "visits" << "time[s]" << "time[%]" << "time/visit[us]" << "region";
    dataCenter::sizes            tempSizes;
    dataCenter::sizes            tempFilteredSizes;
    QList<dataCenter::data>      tempFunctions;
    QList<dataCenter::groupData> tempGroups;
    QVector<int>                 maxWidth;

    /*get data from Connector*/
    tempSizes     = mp_connection->getSizes();
    tempFunctions = mp_connection->getFunctionData();
    tempGroups    = mp_connection->getGroupData();

    /*set progressbar values*/
    mp_progressbar->setMaximum( tempSizes.traceSize );
    if ( mp_connection->hasFiltered() )
    {
        tempFilteredSizes = mp_connection->getFilteredSizes();
        mp_progressbar->setValue( tempFilteredSizes.traceSize );
    }
    else
    {
        mp_progressbar->setValue( tempSizes.traceSize );
    }

    mp_functionTable->setRowCount( tempFunctions.size() );
    mp_groupTable->setRowCount( tempGroups.size() );

    /*fill tables*/
    /*sizeTable*/
    mp_sizeTable->setItem( 0, 1, mp_prototypeNumberItem->clone() );
    mp_sizeTable->setItem( 1, 1, mp_prototypeNumberItem->clone() );
    mp_sizeTable->setItem( 2, 1, mp_prototypeNumberItem->clone() );
    mp_sizeTable->item( 0, 1 )->setText( mp_connection->getReadableByteNo( tempSizes.traceSize ) );
    mp_sizeTable->item( 1, 1 )->setText( mp_connection->getReadableByteNo( tempSizes.maxBuf ) );
    mp_sizeTable->item( 2, 1 )->setText( mp_connection->getReadableByteNo( tempSizes.totalMemory ) );
    /*groupTable*/


    QFont        def( "DejaVu Sans", 10, QFont::Normal );
    QFontMetrics fm( def );
    maxWidth.reserve( 8 );
    for ( int i = 0; i < mp_functionTable->columnCount(); i++ )
    {
        maxWidth.push_back( fm.width( horizontalHeaders.at( i ) ) );
    }

    for ( int i = 0; i < tempGroups.size(); i++ )
    {
        int checkboxWidth = 0;
        if ( !m_noFilter.contains( QString::fromStdString( tempGroups[ i ].type ) ) )
        {
            QCheckBox* tempBox = new QCheckBox( this );
            tempBox->installEventFilter( this );
            mp_groupTable->setCellWidget( i, 0, tempBox );
            checkboxWidth = tempBox->width();
            mp_signalMapper->setMapping( tempBox, i );
            connect( tempBox, SIGNAL( clicked( bool ) ), mp_signalMapper, SLOT( map() ) );
            if ( tempGroups[ i ].state == dataCenter::PARTIAL )
            {
                tempBox->setCheckState( Qt::PartiallyChecked );
            }
            else if ( tempGroups[ i ].state == dataCenter::INCLUDED )
            {
                tempBox->setCheckState( Qt::Checked );
                tempBox->setChecked( true );
            }
            else
            {
                tempBox->setCheckState( Qt::Unchecked );
                tempBox->setChecked( false );
            }
        }
        mp_groupTable->setItem( i, 1, mp_prototypeTextItem->clone() );
        mp_groupTable->setItem( i, 2, mp_prototypeNumberItem->clone() );
        mp_groupTable->setItem( i, 3, mp_prototypeNumberItem->clone() );
        mp_groupTable->setItem( i, 4, mp_prototypeNumberItem->clone() );
        mp_groupTable->setItem( i, 5, mp_prototypeNumberItem->clone() );
        mp_groupTable->setItem( i, 6, mp_prototypeNumberItem->clone() );
        mp_groupTable->setItem( i, 7, mp_prototypeTextItem->clone() );
        mp_groupTable->item( i, 1 )->setText( ( QString::fromStdString( tempGroups[ i ].type ) ) );
        mp_groupTable->item( i, 2 )->setText( seperate( tempGroups[ i ].maxBuf ) );
        mp_groupTable->item( i, 3 )->setText( seperate( tempGroups[ i ].visits ) );
        mp_groupTable->item( i, 4 )->setText( QString::number( tempGroups[ i ].timeS, 'f', 2 ) );
        mp_groupTable->item( i, 5 )->setText( QString::number( tempGroups[ i ].timeP, 'f', 2 ) );
        mp_groupTable->item( i, 6 )->setText( QString::number( tempGroups[ i ].timePerVisit, 'f', 2 ) );
        mp_groupTable->item( i, 7 )->setText( QString::fromStdString( tempGroups[ i ].region ) );
        maxWidth[ 0 ] = qMax( maxWidth[ 0 ], checkboxWidth + 4 );
        maxWidth[ 1 ] = qMax( maxWidth[ 1 ], fm.width( QString::fromStdString( tempGroups[ i ].type ) ) );
        maxWidth[ 2 ] = qMax( maxWidth[ 2 ], fm.width( seperate( tempGroups[ i ].maxBuf ) ) );
        maxWidth[ 3 ] = qMax( maxWidth[ 3 ], fm.width( seperate( tempGroups[ i ].visits ) ) );
        maxWidth[ 4 ] = qMax( maxWidth[ 4 ], fm.width( QString::number( tempGroups[ i ].timeS, 'f', 2 ) ) );
        maxWidth[ 5 ] = qMax( maxWidth[ 5 ], fm.width( QString::number( tempGroups[ i ].timeP, 'f', 2 ) ) );
        maxWidth[ 6 ] = qMax( maxWidth[ 6 ], fm.width( QString::number( tempGroups[ i ].timePerVisit, 'f', 2 ) ) );
        maxWidth[ 7 ] = qMax( maxWidth[ 7 ], fm.width( QString::fromStdString( tempGroups[ i ].region ) ) );
    }
    int height = 2 + mp_groupTable->rowCount() * mp_groupTable->rowHeight( 0 ) + mp_groupTable->horizontalHeader()->height();

    mp_groupTable->setMaximumHeight( height );
    mp_groupTable->setMinimumHeight( height );

    /*functionTable*/
    for ( int i = 0; i < tempFunctions.size(); i++ )
    {
        if ( !m_noFilter.contains( QString::fromStdString( tempFunctions[ i ].type ) ) )
        {
            QCheckBox* tempBox = new QCheckBox( this );
            tempBox->installEventFilter( this );
            mp_functionTable->setCellWidget( i, 0, tempBox );
            mp_signalMapper->setMapping( tempBox, i + tempGroups.size() );
            connect( tempBox, SIGNAL( clicked( bool ) ), mp_signalMapper, SLOT( map() ) );
            if ( tempFunctions[ i ].included )
            {
                tempBox->setChecked( true );
            }
            else
            {
                tempBox->setChecked( false );
            }
        }
        mp_functionTable->setItem( i, 1, mp_prototypeTextItem->clone() );
        mp_functionTable->setItem( i, 2, mp_prototypeNumberItem->clone() );
        mp_functionTable->setItem( i, 3, mp_prototypeNumberItem->clone() );
        mp_functionTable->setItem( i, 4, mp_prototypeNumberItem->clone() );
        mp_functionTable->setItem( i, 5, mp_prototypeNumberItem->clone() );
        mp_functionTable->setItem( i, 6, mp_prototypeNumberItem->clone() );
        mp_functionTable->setItem( i, 7, mp_prototypeTextItem->clone() );
        mp_functionTable->item( i, 1 )->setText( QString::fromStdString( tempFunctions[ i ].type ) );
        mp_functionTable->item( i, 2 )->setText( seperate( tempFunctions[ i ].maxBuf ) );
        mp_functionTable->item( i, 3 )->setText( seperate( tempFunctions[ i ].visits ) );
        mp_functionTable->item( i, 4 )->setText( QString::number( tempFunctions[ i ].timeS, 'f', 2 ) );
        mp_functionTable->item( i, 5 )->setText( QString::number( tempFunctions[ i ].timeP, 'f', 2 ) );
        mp_functionTable->item( i, 6 )->setText( QString::number( tempFunctions[ i ].timePerVisit, 'f', 2 ) );
        mp_functionTable->item( i, 7 )->setText( QString::fromStdString( tempFunctions[ i ].region ) );
        maxWidth[ 1 ] = qMax( maxWidth[ 1 ], fm.width( QString::fromStdString( tempFunctions[ i ].type ) ) );
        maxWidth[ 2 ] = qMax( maxWidth[ 2 ], fm.width( seperate( tempFunctions[ i ].maxBuf ) ) );
        maxWidth[ 3 ] = qMax( maxWidth[ 3 ], fm.width( seperate( tempFunctions[ i ].visits ) ) );
        maxWidth[ 4 ] = qMax( maxWidth[ 4 ], fm.width( QString::number( tempFunctions[ i ].timeS, 'f', 2 ) ) );
        maxWidth[ 5 ] = qMax( maxWidth[ 5 ], fm.width( QString::number( tempFunctions[ i ].timeP, 'f', 2 ) ) );
        maxWidth[ 6 ] = qMax( maxWidth[ 6 ], fm.width( QString::number( tempFunctions[ i ].timePerVisit, 'f', 2 ) ) );
        maxWidth[ 7 ] = qMax( maxWidth[ 7 ], fm.width( QString::fromStdString( tempFunctions[ i ].region ) ) );
    }
    /*set column widths*/
    int minWidth = 0;
    for ( int i = 1; i < mp_groupTable->columnCount(); i++ )
    {
        maxWidth[ i ] += 8;
        minWidth      += maxWidth[ i ];
        mp_groupTable->setColumnWidth( i, maxWidth[ i ] );
        mp_functionTable->setColumnWidth( i, maxWidth[ i ] );
    }
    mp_groupTable->setMinimumWidth( minWidth );
    mp_functionTable->setMinimumWidth( minWidth );
    mp_sizeTable->setMinimumWidth( minWidth );
    if ( mp_connection->hasFiltered() )
    {
        /*sizeTable*/
        /*add new size*/
        QStringList sizeLabel;
        sizeLabel << "" << "without Filter" << "with Filter";
        mp_sizeTable->setHorizontalHeaderLabels( sizeLabel );
        QTableWidgetItem* one = new QTableWidgetItem();
        one->setTextAlignment( Qt::AlignRight | Qt::AlignCenter );
        one->setText( mp_connection->getReadableByteNo( tempFilteredSizes.traceSize ) );
        QTableWidgetItem* two = one->clone();
        two->setText( mp_connection->getReadableByteNo( tempFilteredSizes.maxBuf ) );
        QTableWidgetItem* tri = one->clone();
        tri->setText( mp_connection->getReadableByteNo( tempFilteredSizes.totalMemory ) );
        mp_sizeTable->setItem( 0, 2, one );
        mp_sizeTable->setItem( 1, 2, two );
        mp_sizeTable->setItem( 2, 2, tri );
        mp_sizeTable->setFixedHeight( mp_sizeTable->horizontalHeader()->height() + 92 );
        mp_sizeTable->setColumnWidth( 1, 90 );
    }
    else
    {
        /*reset new size column*/
        mp_sizeTable->setItem( 0, 2, new QTableWidgetItem( "" ) );
        mp_sizeTable->setItem( 1, 2, new QTableWidgetItem( "" ) );
        mp_sizeTable->setItem( 2, 2, new QTableWidgetItem( "" ) );
    }
    //mp_groupTable->setSortingEnabled(true);
    //mp_functionTable->setSortingEnabled(true);
}

QString
MainWindow::seperate( int number )
{
    QString temp = QString::number( number );
    if ( temp.size() > 3 )
    {
        temp.insert( temp.size() - 3, "," );
    }
    /*7 because of the added ,*/
    if ( temp.size() > 7 )
    {
        temp.insert( temp.size() - 7, "," );
    }
    return temp;
}


bool
MainWindow::eventFilter( QObject* object, QEvent* event )
{
    Q_UNUSED( object );
    bool ret = false;
    if ( event->type() == QEvent::KeyPress )
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
        if ( keyEvent->key() == Qt::Key_Space )
        {
            ret = true;
            changeState();
        }
        if ( keyEvent->key() == Qt::Key_Tab )
        {
            ret = true;
            if ( mp_functionTable->selectedItems().size() > 0 )
            {
                /*switch to groupTable*/
                mp_functionTable->clearSelection();
                mp_groupTable->selectRow( 0 );
            }
            else
            {
                mp_groupTable->clearSelection();
                mp_functionTable->selectRow( 0 );
            }
        }
        if ( keyEvent->key() == Qt::Key_Up )
        {
            ret = true;
            /* check which table*/
            if ( mp_functionTable->selectedItems().size() > 0 )
            {
                /*functionTable*/
                /*check if row unselectable*/
                if ( mp_functionTable->currentRow() != 0 )
                {
                    int nextPossible = 1;
                    while ( mp_functionTable->item( mp_functionTable->currentRow() - nextPossible, 1 )->flags() == Qt::NoItemFlags )
                    {
                        nextPossible++;
                        if ( mp_functionTable->currentRow() - nextPossible == 0 )
                        {
                            break;
                        }
                    }
                    mp_functionTable->selectRow( mp_functionTable->currentRow() - nextPossible );
                }
            }
            else
            {
                if ( mp_groupTable->currentRow() != 0 )
                {
                    int nextPossible = 1;
                    while ( mp_groupTable->item( mp_groupTable->currentRow() - nextPossible, 1 )->flags() == Qt::NoItemFlags )
                    {
                        nextPossible++;
                        if ( mp_groupTable->currentRow() - nextPossible == 0 )
                        {
                            break;
                        }
                    }
                    mp_groupTable->selectRow( mp_groupTable->currentRow() - nextPossible );
                }
            }
        }
        if ( keyEvent->key() == Qt::Key_Down )
        {
            ret = true;
            /* check which table*/
            if ( mp_functionTable->selectedItems().size() > 0 )
            {
                /*functionTable*/
                /*check if row unselectable*/
                if ( mp_functionTable->currentRow() != mp_functionTable->rowCount() - 1 )
                {
                    int nextPossible = 1;
                    while ( mp_functionTable->item( mp_functionTable->currentRow() + nextPossible, 1 )->flags() == Qt::NoItemFlags )
                    {
                        nextPossible++;
                        if ( mp_functionTable->currentRow() + nextPossible == mp_functionTable->rowCount() )
                        {
                            break;
                        }
                    }
                    mp_functionTable->selectRow( mp_functionTable->currentRow() + nextPossible );
                }
            }
            else
            {
                if ( mp_groupTable->currentRow() != mp_groupTable->rowCount() - 1 )
                {
                    int nextPossible = 1;
                    while ( mp_groupTable->item( mp_groupTable->currentRow() + nextPossible, 1 )->flags() == Qt::NoItemFlags )
                    {
                        nextPossible++;
                        if ( mp_groupTable->currentRow() + nextPossible == mp_groupTable->rowCount() )
                        {
                            break;
                        }
                    }
                    mp_groupTable->selectRow( mp_groupTable->currentRow() + nextPossible );
                }
            }
        }
    }
    return ret;
}

void
MainWindow::showShortcuts()
{
    QMessageBox msgBox;
    msgBox.setText( "Tabulator\t switch between function- and grouptable\n"
                    "Space\t change state\n"
                    "Key Up\n"
                    "Key Down\t navigate in table\n"
                    "Ctrl+o\t open file\n"
                    "Ctrl+s\t create filter file\n" );
    msgBox.exec();
}

void
MainWindow::resizeFunctionTable( int index, int oldSize, int newSize )
{
    Q_UNUSED( oldSize );
    mp_functionTable->setColumnWidth( index, newSize );
}

void
MainWindow::reset()
{
    setWindowModified( false );
    setWindowTitle( m_windowTitle );
    if ( mp_connection )
    {
        delete mp_connection;
    }
    mp_connection = new Connector();
    mp_groupTable->clearContents();
    mp_sizeTable->clearContents();
    fillSizeTable();
    mp_functionTable->clearContents();
    mp_progressbar->reset();
}

QString
MainWindow::filterFile()
{
    return m_filterFileName;
}

QString
MainWindow::totalMemory()
{
    if ( mp_connection )
    {
        dataCenter::sizes tempSizes = mp_connection->getSizes();
        if ( tempSizes.totalMemory > std::numeric_limits<uint32_t>::max() )
        {
            return QString();
        }
        return mp_connection->getReadableByteNo( tempSizes.totalMemory );
    }

    return QString();
}
