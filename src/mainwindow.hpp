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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QtGlobal>
#include <QPushButton>
#include <QTableWidget>
#include <QLayout>
#include <QDebug>
#include <QHeaderView>
#include <QPainter>
#include <QSizePolicy>
#include <QLabel>
#include <QFileDialog>
#include <QStatusBar>
#include <QMenuBar>
#include <QMainWindow>
#include <QCheckBox>
#include <QKeyEvent>
#include <QEvent>
#include <QMessageBox>
#include <QProgressBar>
#include <QSignalMapper>

#include "connector.hpp"


class Connector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* parent = 0 );
    ~MainWindow();
    void
    initOpen( QString fileName );
    int
    checkEstimator();

private:
    /*GUI elements*/
    QVBoxLayout*      mp_layout;
    QTableWidget*     mp_sizeTable;
    QTableWidget*     mp_groupTable;
    QTableWidget*     mp_functionTable;
    QStatusBar*       mp_statusBar;
    QMenuBar*         mp_menu;
    QList<QCheckBox*> m_groupBoxes;
    QList<QCheckBox*> m_functionBoxes;
    QProgressBar*     mp_progressbar;

    /*instance of Connector*/
    Connector* mp_connection;

    QStringList m_noFilter;

    QTableWidgetItem* mp_prototypeNumberItem;
    QTableWidgetItem* mp_prototypeTextItem;

    QSignalMapper* mp_signalMapper;

    QString m_fileName;
    QString m_filterFileName;
    QString m_windowTitle;

    /*functions*/
    void
    changeState();
    void
    fillSizeTable();
    void
    update();
    void
    reset();
    QString
    seperate( int number );
    void
    initTables();
    void
    initMenu();

    bool
    eventFilter( QObject* object,
                 QEvent*  event );

private slots:
    /*slots for the buttons*/
    void
    openFile();
    void
    saveFile();
    void
    saveFileAs();
    void
    changeStateSlot( int pos );
    void
    showShortcuts();

    /*slots for tables
     * guarantees that you cant select rows in both tables*/
    void
    unselectGroupTable();
    void
    unselectFunctionTable();
    void
    resizeFunctionTable( int index,
                         int oldSize,
                         int newSize );
};

#endif // MAINWINDOW_HPP
