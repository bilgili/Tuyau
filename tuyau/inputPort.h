/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *
 * This file is part of Tuyau <https://github.com/bilgili/Tuyau>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _InputPort_h_
#define _InputPort_h_

#include <tuyau/api.h>
#include "types.h"

namespace tuyau
{

/**
 * Input connection for the PipeFilter.
 */
class InputPort
{
public:

    /**
     * @param dataInfo is the name and type information for the data.
     */
    TUYAU_API explicit InputPort( const DataInfo& dataInfo );
    TUYAU_API ~InputPort();

    /**
     * @return name of the port
     */
    TUYAU_API std::string getName() const;

    /**
     * @return data type of the port
     */
    TUYAU_API std::type_index getDataType() const;

    /**
     * @return the number of the connected outputs to the port
     */
    TUYAU_API size_t getSize() const;

    /**
     * @return Return all the futures this port has.
     */
    TUYAU_API const Futures& getFutures() const;

    /**
     * Connects an output port to input port
     * @param port is the connected output port.
     */
    TUYAU_API void connect( const OutputPort& port );

    /**
     * Disconnects an output port from input port
     * @param port is the disconnected output port.
     * @return true if output port is found as a connection and removed
     * for the connections
     */
    TUYAU_API bool disconnect( const OutputPort& port );

private:

    struct Impl;
    std::unique_ptr<Impl> _impl;
};

}

#endif // _InputPort_h_

