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

#ifndef _Pipeline_h_
#define _Pipeline_h_

#include <tuyau/api.h>
#include "types.h"
#include "pipeFilter.h"
#include "executable.h"

namespace tuyau
{

/**
 * Implements the executable graph. Accesing the copies of the object from
 * other threads for non-const functions is not thread safe.
 */
class Pipeline : public Executable
{
public:

    typedef std::unique_ptr< Executable > UniqueExecutablePtr;

    TUYAU_API Pipeline();
    TUYAU_API ~Pipeline();

    /**
     * Adds a pipeline to be executed.
     * @param pipeline is added to list of executables ( shallow copied )
     * @param wait If true, on scheduled execution, pipeline
     * can wait on given pipeline post conditions.
     */
    void add( const std::string& name,
              Pipeline& pipeline,
              bool wait = true )
    {
        _add( name,
              UniqueExecutablePtr( new Pipeline( pipeline )),
              wait );
    }

    /**
     * Creates and adds a pipefilter ( given the FilterT type of filter )
     * @param FilterT is the type of filter to be added to list of executables.
     * @name name the name of the filter instance.
     * @param args for the FilterT construction
     * @param wait If true, on scheduled execution, pipeline
     * can wait on the post conditions of the pipefilter.
     * @return returns the generated pipe filter.
     * @throw std::runtime_error if an executable with same name is present
     */

    template< class FilterT, bool wait = true, class... Args >
    PipeFilter add( const std::string& name,
                    Args&&... args )
    {
        PipeFilterT< FilterT > pipeFilter( name, args... );
        _add( name,
              UniqueExecutablePtr( new PipeFilter( pipeFilter )),
              wait );
        return pipeFilter;
    }

    /**
     * @param name of the executable
     * @return the executable
     * @throw std::runtime_error if a pipe filter or pipeline does not exist
     */
    TUYAU_API Executable& getExecutable( const std::string& name );

    /**
     * @copydoc Executable::execute
     */
    TUYAU_API void execute() final;

    /**
     * @copydoc Executable::getPostconditions
     */
    TUYAU_API Futures getPostconditions() const final;

    /**
     * @copydoc Executable::getPreconditions
     */
    TUYAU_API Futures getPreconditions() const final;

    /**
     * @copydoc Executable::reset
     */
    TUYAU_API void reset() final;

private:

    void _add( const std::string& name,
               UniqueExecutablePtr exec,
               bool wait );
private:

    void _schedule( Executor& executor ) final;

    ExecutablePtr clone() const final;

    struct Impl;
    std::shared_ptr< Impl > _impl;

};

}

#endif // _Pipeline_h_

