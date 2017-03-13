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

#include "workers.h"
#include "executable.h"
#include "mtQueue.h"

#include <boost/thread/thread.hpp>

namespace tuyau
{

struct Workers::Impl
{
    Impl( Workers& workers,
          const size_t nThreads,
          const std::string& threadPoolName,
          const WorkerSetupFunc& setupFunc,
          const WorkerDestroyFunc& destroyFunc )
        : _workers( workers )
        , _name( threadPoolName )
        , _setupFunc( setupFunc )
        , _destroyFunc( destroyFunc )
    {
        for( size_t i = 0; i < nThreads; ++i )
            _threadGroup.create_thread( boost::bind( &Impl::execute, this ));
    }

    void execute()
    {
        if( _setupFunc )
            _setupFunc();

        while( true )
        {
            ExecutablePtr exec = _workQueue.pop();
            if( !exec )
                break;

            exec->execute();
        }

        if( _destroyFunc )
            _destroyFunc();
    }

    ~Impl()
    {
        for( size_t i = 0; i < getSize(); ++i )
            _workQueue.push( ExecutablePtr( ));
        _threadGroup.join_all();
    }


    void submitWork( ExecutablePtr executable )
    {
        _workQueue.pushFront( executable );
    }

    size_t getSize() const
    {
        return _threadGroup.size();
    }

    Workers& _workers;
    MTQueue< ExecutablePtr > _workQueue;
    boost::thread_group _threadGroup;
    const std::string _name;
    const WorkerSetupFunc _setupFunc;
    const WorkerDestroyFunc _destroyFunc;
};

Workers::Workers( const size_t nThreads,
                  const std::string& threadPoolName,
                  const WorkerSetupFunc& setupFunc,
                  const WorkerDestroyFunc& destroyFunc )
    : _impl( new Workers::Impl( *this,
                                nThreads,
                                threadPoolName,
                                setupFunc,
                                destroyFunc ))
{}

Workers::~Workers()
{}

void Workers::schedule( ExecutablePtr executable )
{
    _impl->submitWork( executable );
}

size_t Workers::getSize() const
{
    return _impl->getSize();
}

}
