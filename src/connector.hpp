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

#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <QtGlobal>
#include <QWidget>
#include <stdint.h>
#include <QDebug>
#include <QProcess>
#include <QFile>

#include "score/SCOREP_Score_Estimator.hpp"

class SCOREP_Score_Estimator;


class Connector
{
public:
    Connector();
    ~Connector();

    void
    start( QString fileName );
    dataCenter::sizes
    getSizes();

    QList<dataCenter::groupData>
    getGroupData();

    QList<dataCenter::data>
    getFunctionData();
    bool
    changeState( QList<int> keys,
                 bool       groupTable );
    bool
    hasFiltered();
    dataCenter::sizes
    getFilteredSizes();
    bool
    createFilterFile( QString fileName );
    QString
    getReadableByteNo( uint64_t bytes );

private:
    QHash<QString, QHash<int, dataCenter::buffer> > m_bufferData;//QHash<functionName, QHash<procNr, buffer> >
    QList<int>                                      m_excludedFunctions;
    QList<dataCenter::groupData>                    m_dataListGroup;
    QList<dataCenter::data>                         m_dataListFunction;

    /*instance of estimator*/
    SCOREP_Score_Estimator* mp_estimator;

    /*values in bytes*/
    uint64_t m_traceSize;
    uint64_t m_maxBuf;
    uint64_t m_totalMemory;
    uint64_t m_traceSizeFlt;
    uint64_t m_maxBufFlt;
    uint64_t m_totalMemoryFlt;

    QStringList m_noFilter;

    dataCenter::groupData
    switchState( dataCenter::groupData input );
    void
    calculateFilter();
    void
    calculateFilteredSizes();
    QString
    seperate( int number );
    uint64_t
    calculateTraceSize();
    uint64_t
    calculateMaxBuf();
};

#endif // CONNECTOR_HPP
