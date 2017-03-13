[![Build Status](https://travis-ci.org/bilgili/Tuyau.svg?branch=master)](https://travis-ci.org/bilgili/Tuyau)

# Introduction

Tuyau is a C++ library that helps creation and execution of filter graphs. It simplifies
the interfaces of filters through named and typed ports ( almost no templating is needed ). The
errors of graph generation can be identified on runtime. Given the thread pools, many
filters can be run concurrently and they can interact ( wait ) on a given data. One-to-one,
one-to-many, many-to-one kind of graph configurations are possible within the filter graphs.

## Known Bugs

Please file a [Bug Report](https://github.com/bilgili/Tuyau/issues) if you find any
other issue with this release.

* No known bugs

# About

Tuyau is a cross-platform toolkit, designed to run on any modern
operating system, including all Unix variants. Tuyau uses CMake to
create a platform-specific build environment. The following platforms
and build environments are tested:

* Linux: Ubuntu 12.10, 13.04, 13.10 RHEL 6 (Makefile, i386, x64)

