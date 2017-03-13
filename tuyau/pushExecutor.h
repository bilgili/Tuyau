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

#ifndef _SimpleExecutor_h_
#define _SimpleExecutor_h_

#include <tuyau/api.h>
#include "executor.h"
#include "types.h"

namespace tuyau
{

/**
 * Provides a very basic implementation for the Executor
 * class. It has a thread pool for executing multiple executables
 * asynchronously.
 *
 * The submitted executables are scheduled to the worker threads,
 * by looking at the preconditions if they are satisfied.
 */
class PushExecutor : public Executor
{
public:

    /**
     * @param threadCount is number of worker threads
     * @param threadPoolName the threads are renamed with the given name
     * @param setupFunc setups the thread before running (i.e. setCurrent() with OpenGL)
     * the worker threads will share the context with the given context
     */
    TUYAU_API PushExecutor( size_t threadCount,
                            const std::string& threadPoolName = "Simple Executor",
                            const WorkerSetupFunc& setupFunc = WorkerSetupFunc( ));

    TUYAU_API virtual ~PushExecutor();

    /** @copydoc Executor::schedule */
    TUYAU_API void schedule( ExecutablePtr executable ) final;

    /** @copydoc Executor::clear */
    TUYAU_API void clear() final;

private:

    struct Impl;
    std::unique_ptr<Impl> _impl;
};

}

#endif // _SimpleExecutor_h_
