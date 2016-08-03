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

#ifndef DATA_HPP
#define DATA_HPP

class dataCenter
{
public:
    enum states { INCLUDED, EXCLUDED, PARTIAL };
    struct data
    {
        bool        included;
        int         key;
        std::string type;
        int         maxBuf;
        int         visits;
        double      timeS;
        double      timeP;
        double      timePerVisit;
        std::string region;
        std::string mangledName;
    };
    struct groupData
    {
        states      state;
        int         key;
        std::string type;
        int         maxBuf;
        int         visits;
        double      timeS;
        double      timeP;
        double      timePerVisit;
        std::string region;
    };
    struct buffer
    {
        uint64_t numberOfVisits;
        uint64_t bytesPerVisit;
    };
    struct sizes
    {
        uint64_t traceSize;
        uint64_t maxBuf;
        uint64_t totalMemory;
    };
};




#endif // DATA_HPP
