/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef PGE_FF_UNITS_H
#define PGE_FF_UNITS_H

namespace PGE_FileLibrary
{

enum class TimeUnit
{
    FrameOneOf65sec = -1,
    Millisecond = 0,
    Decisecond,
    Second
};

template<typename NumT>
NumT TimeUnitsCVT(const NumT &in, TimeUnit from, TimeUnit into)
{
    if(from == into)
        return in;
    long double value = static_cast<long double>(in);
    switch(from)
    {
    case TimeUnit::FrameOneOf65sec:
        switch(into)
        {
        case TimeUnit::Millisecond:
            return static_cast<NumT>( (value * 1000.0l) / 65.0l );
        case TimeUnit::Decisecond:
            return static_cast<NumT>( (value * 10.0l) / 65.0l );
        case TimeUnit::Second:
            return static_cast<NumT>( value / 65.0l );
        default:;
        case TimeUnit::FrameOneOf65sec:
            return in;//Nothing to do
        };
    case TimeUnit::Millisecond:
        switch(into)
        {
        case TimeUnit::FrameOneOf65sec:
            return static_cast<NumT>( (value / 1000.0l) * 65.0l );
        case TimeUnit::Decisecond:
            return static_cast<NumT>( value / 1000.0l );
        case TimeUnit::Second:
            return static_cast<NumT>( value / 10.0l );
        default:;
        case TimeUnit::Millisecond:
            return in;//Nothing to do
        };
    case TimeUnit::Decisecond:
        switch(into)
        {
        case TimeUnit::FrameOneOf65sec:
            return static_cast<NumT>( (value / 10.0l) * 65.0l );
        case TimeUnit::Millisecond:
            return static_cast<NumT>( value * 100.0l );
        case TimeUnit::Second:
            return static_cast<NumT>( value / 10.0l );
        default:;
        case TimeUnit::Decisecond:
            return in;//Nothing to do
        };
    case TimeUnit::Second:
        switch(into)
        {
        case TimeUnit::FrameOneOf65sec:
            return static_cast<NumT>( value * 65.0l );
        case TimeUnit::Millisecond:
            return static_cast<NumT>( value * 1000.0l );
        case TimeUnit::Decisecond:
            return static_cast<NumT>( value * 10.0l );
        default:;
        case TimeUnit::Second:
            return in;//Nothing to do
        };
    };
    return in;
}

}//PGE_FileLibrary
#endif // PGE_FF_UNITS_H
