
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

#include "pushExecutor.h"

#include "filter.h"
#include "workers.h"
#include "executable.h"
#include "futureMap.h"
#include "futurePromise.h"
#include "pipeFilter.h"
#include "mtQueue.h"

#include <boost/thread.hpp>

#include <algorithm>
#include <iterator>

namespace tuyau
{

bool operator<( const Future& future1, const Future& future2 )
{
    return future1.getId() < future2.getId();
}

struct PushExecutor::Impl
{

    Impl( const size_t threadCount,
          const std::string& threadPoolName,
          const WorkerSetupFunc& setupFunc )
        : _workers( threadCount, threadPoolName, setupFunc )
        , _unlockPromise( DataInfo( "LoopUnlock", getType< bool >( )))
        , _workThread( boost::thread( boost::bind( &Impl::schedule, this )))
    {}

    ~Impl()
    {
        _mtWorkQueue.clear();
        _mtWorkQueue.push( 0 );
        {
            ScopedLock lock( _promiseReset );
             _unlockPromise.set( true );
        }
        _workThread.join();
    }

    void schedule()
    {
        std::set< Future > inputConditions;
        std::list< ExecutablePtr > executables;
        while( true )
        {
            Futures futures = { Future( _unlockPromise ) };
            futures.insert( futures.end(),
                            inputConditions.begin(), inputConditions.end( ));
            waitForAny( futures );
            {
                ScopedLock lock( _promiseReset );
                if( _unlockPromise.getFuture().isReady( ))
                {
                    while( !_mtWorkQueue.empty( ))
                    {
                        ExecutablePtr exec = _mtWorkQueue.pop();
                        // In destruction tine "0" is pushed to the queue
                        // and no further executable is scheduled
                        if( !exec )
                            return;

                        executables.push_back( exec );
                        const Futures& preConds = exec->getPreconditions();
                        inputConditions.insert( preConds.begin(), preConds.end( ));
                    }

                    _unlockPromise.reset();
                }
            }

            std::list< ExecutablePtr >::iterator it = executables.begin();
            while( it != executables.end( ))
            {
                ExecutablePtr executable = *it;
                const Futures& preConds = executable->getPreconditions();
                const FutureMap futureMap( preConds );
                if( futureMap.isReady( ))
                {
                    _workers.schedule( executable );
                    it = executables.erase( it );
                    for( const auto& future: futureMap.getFutures( ))
                        inputConditions.erase( future );
                }
                else
                    ++it;
            }
        }
    }

    void clear()
    {
        _mtWorkQueue.clear();
    }

    void schedule( ExecutablePtr exec )
    {
        ScopedLock lock( _promiseReset );
        const bool wasEmpty = _mtWorkQueue.empty();
        _mtWorkQueue.pushFront( exec );
        if( wasEmpty )
            _unlockPromise.set( true );
    }

    MTQueue< ExecutablePtr > _mtWorkQueue;
    Workers _workers;
    Promise _unlockPromise;
    boost::mutex _promiseReset;
    boost::thread _workThread;
};

PushExecutor::PushExecutor( const size_t threadCount,
                                const std::string& threadPoolName,
                                const WorkerSetupFunc& setupFunc )
    : _impl( new Impl( threadCount, threadPoolName, setupFunc ))
{
}

PushExecutor::~PushExecutor( )
{}

void PushExecutor::clear()
{
    _impl->clear();
}

void PushExecutor::schedule( ExecutablePtr executable )
{
    _impl->schedule( executable );
}

}
