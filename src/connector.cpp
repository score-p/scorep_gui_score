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

#include "connector.hpp"

Connector::Connector() :
    m_traceSize( 0 ),
    m_maxBuf( 0 ),
    m_totalMemory( 0 )

{
    m_noFilter << "MPI" << "ALL" << "OMP" << "SHMEM";
}

Connector::~Connector()
{
}

void
Connector::start( QString fileName )
{
    m_dataListFunction.clear();
    m_dataListGroup.clear();

    mp_estimator = new SCOREP_Score_Estimator( fileName.toStdString(), 0 );
    mp_estimator->calculate( true, true, &m_bufferData );

    mp_estimator->getSizes( &m_traceSize, &m_maxBuf, &m_totalMemory );

    /*get function group data*/
    for ( uint64_t i = 0; i < mp_estimator->getTypeNum(); i++ )
    {
        if ( mp_estimator->getGroupInformation( i ).maxBuf != -1 )
        {
            m_dataListGroup.append( mp_estimator->getGroupInformation( i ) );
        }
    }
    /*append  FLT group*/
    dataCenter::groupData temp;
    temp.type         = "FLT";
    temp.state        = dataCenter::EXCLUDED;
    temp.maxBuf       = 0;
    temp.timeP        = 0;
    temp.visits       = 0;
    temp.timePerVisit = 0;
    temp.timeS        = 0;
    m_dataListGroup.append( temp );

    /*get function data*/
    for ( uint64_t i = 0; i < mp_estimator->getRegionNum(); i++ )
    {
        dataCenter::data temp = mp_estimator->getRegionInformation( i );
        if ( temp.maxBuf != -1 )
        {
            m_dataListFunction.append( temp );
        }
    }
}

dataCenter::sizes
Connector::getSizes()
{
    dataCenter::sizes tempSizes;
    tempSizes.traceSize   = m_traceSize;
    tempSizes.maxBuf      = m_maxBuf;
    tempSizes.totalMemory = m_totalMemory;
    return tempSizes;
}

dataCenter::sizes
Connector::getFilteredSizes()
{
    dataCenter::sizes tempSizes;
    tempSizes.traceSize   = m_traceSizeFlt;
    tempSizes.maxBuf      = m_maxBufFlt;
    tempSizes.totalMemory = m_totalMemoryFlt;
    return tempSizes;
}

QList<dataCenter::groupData> Connector::getGroupData()
{
    return m_dataListGroup;
}

QList<dataCenter::data> Connector::getFunctionData()
{
    return m_dataListFunction;
}

bool
Connector::hasFiltered()
{
    if ( m_excludedFunctions.size() == 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

QString
Connector::getReadableByteNo( uint64_t bytes )
{
    const uint32_t base = 1024;
    uint32_t       exp;
    for ( exp = 0; bytes > 4 * base; exp++ )
    {
        bytes /= base;
    }
    // round up;
    bytes++;
    int     intBytes = bytes;
    QString result   = QString::number( intBytes );
    switch ( exp )
    {
        case 0:
            result.append( "bytes" );
            break;
        case 1:
            result.append( "kB" );
            break;
        case 2:
            result.append( "MB" );
            break;
        case 3:
            result.append( "GB" );
            break;
        case 4:
            result.append( "TB" );
            break;
        case 5:
            result.append( "PB" );
            break;
        case 6:
            result.append( "EB" );
            break;
        case 7:
            result.append( "ZB" );
            break;
        case 8:
            result.append( "YB" );
            break;
    }
    return result;
}

bool
Connector::changeState( QList<int> keys, bool groupTable )
{
    /*return type QString for error messages*/
    bool               ret = true;
    QListIterator<int> it( keys );
    int                key;
    while ( it.hasNext() )
    {
        key = it.next();
        if ( groupTable == true )
        {
            /*group*/
            if ( m_noFilter.contains( QString::fromStdString( m_dataListGroup.at( key ).type ) ) )
            {
                ret = false;
            }
            else
            {
                /*switch state*/
                dataCenter::groupData tempData = switchState( m_dataListGroup.at( key ) );
                m_dataListGroup.replace( key, tempData );

                if ( tempData.state == dataCenter::EXCLUDED )
                {
                    if ( tempData.type == "FLT" )
                    {
                        /*include all*/
                        for ( int i = 0; i < m_dataListFunction.size(); i++ )
                        {
                            if ( !m_noFilter.contains( QString::fromStdString( m_dataListFunction[ i ].type ) ) )
                            {
                                if ( m_dataListFunction[ i ].included == false )
                                {
                                    int index = m_excludedFunctions.indexOf( i );
                                    m_dataListFunction[ i ].included = true;
                                    m_excludedFunctions.removeAt( index );
                                }
                            }
                        }
                        for ( int i = 0; i < m_dataListGroup.size(); i++ )
                        {
                            if ( !m_noFilter.contains( QString::fromStdString( m_dataListGroup[ i ].type ) ) )
                            {
                                if ( m_dataListGroup[ i ].state == dataCenter::EXCLUDED || m_dataListGroup[ i ].state == dataCenter::PARTIAL )
                                {
                                    m_dataListGroup[ i ].state = dataCenter::INCLUDED;
                                }
                            }
                        }
                    }
                    else
                    {
                        /*exclude all functions of this region*/
                        for ( int i = 0; i < m_dataListFunction.size(); i++ )
                        {
                            if ( m_dataListFunction[ i ].type == tempData.type )
                            {
                                if ( m_dataListFunction[ i ].included == true )
                                {
                                    dataCenter::data temp = m_dataListFunction[ i ];
                                    temp.included = false;
                                    m_dataListFunction.replace( i, temp );
                                    m_excludedFunctions.append( i );
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ( tempData.type == "FLT" )
                    {
                        /*exclude all*/
                        for ( int i = 0; i < m_dataListFunction.size(); i++ )
                        {
                            if ( !m_noFilter.contains( QString::fromStdString( m_dataListFunction[ i ].type ) ) )
                            {
                                if ( m_dataListFunction[ i ].included == true )
                                {
                                    m_dataListFunction[ i ].included = false;
                                    m_excludedFunctions.append( i );
                                }
                            }
                        }
                        for ( int i = 0; i < m_dataListGroup.size(); i++ )
                        {
                            if ( !m_noFilter.contains( QString::fromStdString( m_dataListGroup[ i ].type ) ) )
                            {
                                if ( m_dataListGroup[ i ].state == dataCenter::INCLUDED || m_dataListGroup[ i ].state == dataCenter::PARTIAL )
                                {
                                    m_dataListGroup[ i ].state = dataCenter::EXCLUDED;
                                }
                            }
                        }
                    }
                    else
                    {
                        /*include all functions of this region*/
                        for ( int i = 0; i < m_dataListFunction.size(); i++ )
                        {
                            if ( m_dataListFunction[ i ].type == tempData.type )
                            {
                                if ( m_dataListFunction[ i ].included == false )
                                {
                                    dataCenter::data temp  = m_dataListFunction[ i ];
                                    int              index = m_excludedFunctions.indexOf( i );
                                    temp.included = true;
                                    m_dataListFunction.replace( i, temp );
                                    m_excludedFunctions.removeAt( index );
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            /*function*/
            if ( m_noFilter.contains( QString::fromStdString( m_dataListFunction.at( key ).type ) ) )
            {
                ret = false;
            }
            else
            {
                bool same = true;
                /*switch state*/
                dataCenter::data tempData = m_dataListFunction.at( key );
                tempData.included = !tempData.included;
                m_dataListFunction.replace( key, tempData );

                if ( tempData.included == false )
                {
                    /*excluded*/
                    m_excludedFunctions.append( key );
                }
                else
                {
                    /*included*/
                    int index = m_excludedFunctions.indexOf( key );
                    m_excludedFunctions.removeAt( index );
                }
                /*check if all functions of this type has the same state*/
                for ( int i = 0; i < m_dataListFunction.size(); i++ )
                {
                    if ( m_dataListFunction[ i ].type == tempData.type )
                    {
                        if ( m_dataListFunction[ i ].included != tempData.included )
                        {
                            same = false;
                        }
                    }
                }
                if ( same == true )
                {
                    /*all functions of this type has the same state->change group state*/
                    for ( int i = 0; i < m_dataListGroup.size(); i++ )
                    {
                        if ( m_dataListGroup[ i ].type == tempData.type )
                        {
                            dataCenter::groupData temp = m_dataListGroup[ i ];
                            if ( tempData.included )
                            {
                                temp.state = dataCenter::INCLUDED;
                            }
                            else
                            {
                                temp.state = dataCenter::EXCLUDED;
                            }

                            m_dataListGroup.replace( i, temp );
                        }
                    }
                }
                else
                {
                    for ( int i = 0; i < m_dataListGroup.size(); i++ )
                    {
                        if ( m_dataListGroup[ i ].type == tempData.type )
                        {
                            dataCenter::groupData temp = m_dataListGroup[ i ];
                            temp.state = dataCenter::PARTIAL;
                            m_dataListGroup.replace( i, temp );
                        }
                    }
                }
            }
        }
    }
    calculateFilter();
    calculateFilteredSizes();
    return ret;
}

void
Connector::calculateFilter()
{
    /*calculates group FLT*/
    dataCenter::groupData groupFlt;
    groupFlt.state        = m_dataListGroup.at( m_dataListGroup.size() - 1 ).state;
    groupFlt.type         = "FLT";
    groupFlt.maxBuf       = 0;
    groupFlt.visits       = 0;
    groupFlt.timeP        = 0;
    groupFlt.timeS        = 0;
    groupFlt.timePerVisit = 0;
    /*delete row filter*/
    m_dataListGroup.pop_back();
    if ( m_excludedFunctions.size() == 0 )
    {
        groupFlt.state = dataCenter::EXCLUDED;
    }
    else
    {
        bool allExcluded = true;
        /*check if everything possible is excluded or not*/
        for ( int i = 0; i < m_dataListGroup.size(); i++ )
        {
            if ( m_noFilter.contains( QString::fromStdString( m_dataListGroup[ i ].type ) ) )
            {
                /*ignore*/
            }
            else
            {
                if ( m_dataListGroup.at( i ).state == dataCenter::INCLUDED ||
                     m_dataListGroup.at( i ).state == dataCenter::PARTIAL )
                {
                    allExcluded = false;
                }
            }
        }
        if ( allExcluded )
        {
            groupFlt.state = dataCenter::INCLUDED;
        }
        else
        {
            groupFlt.state = dataCenter::PARTIAL;
        }
    }
    for ( int i = 0; i < m_excludedFunctions.size(); i++ )
    {
        dataCenter::data temp = m_dataListFunction[ m_excludedFunctions[ i ] ];
        groupFlt.maxBuf      += temp.maxBuf;
        groupFlt.visits      += temp.visits;
        groupFlt.timeP       += temp.timeP;
        groupFlt.timeS       += temp.timeS;
        groupFlt.timePerVisit = groupFlt.timeS / groupFlt.visits * 1000000;
    }
    /*add row filter*/
    m_dataListGroup.append( groupFlt );
}

bool
Connector::createFilterFile( QString fileName )
{
    bool ret = false;
    if ( m_excludedFunctions.size() != 0 )
    {
        QFile file( fileName );
        ret = file.open( QIODevice::WriteOnly );
        QTextStream stream( &file );
        stream << "#this file is generated bei scoreQt" << endl;
        stream << "SCOREP_REGION_NAMES_BEGIN" << endl;
        stream << "EXCLUDE MANGLED" << endl;
        for ( int i = 0; i < m_excludedFunctions.size(); i++ )
        {
            stream << "\t" << QString::fromStdString( m_dataListFunction[ m_excludedFunctions[ i ] ].mangledName ) << endl;
        }
        stream << "SCOREP_REGION_NAMES_END" << endl;
        file.close();
    }
    return ret;
}

void
Connector::calculateFilteredSizes()
{
    uint64_t                                                traceSize = 0;
    QHashIterator<QString, QHash<int, dataCenter::buffer> > it( m_bufferData );
    QList <int>                                             procList;
    bool                                                    included;
    while ( it.hasNext() )
    {
        it.next();
        included = true;
        for ( int i = 0; i < m_excludedFunctions.size(); i++ )
        {
            if ( it.key() == QString::fromStdString( m_dataListFunction[ m_excludedFunctions[ i ] ].region ) )
            {
                included = false;
            }
        }
        if ( included )
        {
            QHashIterator<int, dataCenter::buffer> innerIt( it.value() );
            while ( innerIt.hasNext() )
            {
                innerIt.next();
                traceSize += innerIt.value().bytesPerVisit * innerIt.value().numberOfVisits;
                while ( innerIt.key() > procList.size() - 1 )
                {
                    procList.append( 0 );
                }
                procList[ innerIt.key() ] += innerIt.value().bytesPerVisit * innerIt.value().numberOfVisits;
            }
        }
    }
    int tempMaxBuf = 0;
    for ( int i = 0; i < procList.size(); i++ )
    {
        tempMaxBuf = qMax( tempMaxBuf, procList[ i ] );
    }
    m_maxBufFlt      = tempMaxBuf;
    m_traceSizeFlt   = traceSize;
    m_totalMemoryFlt = mp_estimator->updateMemory( m_maxBufFlt );
}


QString
Connector::seperate( int number )
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

dataCenter::groupData
Connector::switchState( dataCenter::groupData input )
{
    dataCenter::groupData temp = input;
    if ( temp.state == dataCenter::INCLUDED )
    {
        temp.state = dataCenter::EXCLUDED;
    }
    else if ( temp.state == dataCenter::EXCLUDED || temp.state == dataCenter::PARTIAL )
    {
        temp.state = dataCenter::INCLUDED;
    }
    return temp;
}
