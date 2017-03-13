#ifndef _tuyau_types_h_
#define _tuyau_types_h_

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <typeindex>
#include <vector>

namespace tuyau
{

class Executor;
class Executable;
class Filter;
class Future;
class FutureMap;
class FutureMap;
class InputPort;
class PortData;
class Promise;
class PromiseMap;
class OutputPort;
class UniqueFutureMap;
class Pipeline;
class PipeFilter;
class Workers;

typedef std::shared_ptr< PortData > PortDataPtr;
typedef std::shared_ptr< Executable > ExecutablePtr;
typedef std::unique_ptr< Filter > FilterPtr;

typedef std::list< Executable* > Executables;
typedef std::list< Future > Futures;
typedef std::list< Promise > Promises;

template < class T >
inline std::type_index getType()
{
    typedef typename std::remove_const<T>::type UnconstT;
    return std::type_index( typeid( UnconstT ));
}

typedef std::map< std::string, std::type_index > DataInfos;
typedef std::map< std::string, PipeFilter > PipeFilterMap;
typedef DataInfos::value_type DataInfo;

/** Locking object definitions */
typedef boost::shared_mutex ReadWriteMutex;
typedef boost::shared_lock< ReadWriteMutex > ReadLock;
typedef boost::unique_lock< ReadWriteMutex > WriteLock;
typedef boost::unique_lock< boost::mutex > ScopedLock;

typedef std::function<void()> WorkerSetupFunc;
typedef std::function<void()> WorkerDestroyFunc;

}
#endif // _tuyau_types_h_

