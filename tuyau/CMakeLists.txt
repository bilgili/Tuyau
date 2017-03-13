# Copyright (c) 2011-2015, EPFL/Blue Brain Project
#                          Ahmet Bilgili <ahmet.bilgili@epfl.ch>
#                          Stefan.Eilemann@epfl.ch
#
# This file is part of Tuyau <https://github.com/bilgili/Tuyau>
#

set(TUYAU_PUBLIC_HEADERS
  types.h
  executable.h
  filter.h
  futureMap.h
  inputPort.h
  outputPort.h
  pipeFilter.h
  pipeline.h
  portData.h
  futurePromise.h
  promiseMap.h
  pushExecutor.h
  workers.h)

set(TUYAU_SOURCES
  executable.cpp
  futureMap.cpp
  inputPort.cpp
  outputPort.cpp
  pipeFilter.cpp
  pipeline.cpp
  futurePromise.cpp
  promiseMap.cpp
  pushExecutor.cpp
  workers.cpp)

set(TUYAU_LINK_LIBRARIES PUBLIC ${Boost_LIBRARIES})
common_library(Tuyau)
