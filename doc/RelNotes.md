Tuyau API Documentation {#mainpage}
============

[TOC]

# Introduction {#Introduction}

Tuyau is a C++ library that helps creation and execution of filter graphs. It simplifies
the interfaces of filters through named and typed ports ( almost no templating is needed ). The
errors of graph generation can be identified on runtime. Given the thread pools, many
filters can be run concurrently and they can interact ( wait ) on a given data. One-to-one,
one-to-many, many-to-one kind of graph configurations are possible within the filter graphs.

Tuyau 1.0 is the first complete release. It can be retrieved by cloning
the [source code](https://github.com/bilgili/Tuyau).

## Features {#Features}

Tuyau provides the following major features:

* Tuyau interface communicating through ports ( name, data type )
* Execution pools that can schedule filter based on the completion of data
* Tools to create filter graphs

# New in this release {#New}

## New Features {#NewFeatures}

## Enhancements {#Enhancements}

## Optimizations {#Optimizations}

## Documentation {#Documentation}

## Bug Fixes {#Fixes}

## Known Bugs {#Bugs}

Please file a [Bug Report](https://github.com/bilgili/Tuyau/issues) if you find any
other issue with this release.

* No known bugs

# About {#About}

Tuyau is a cross-platform toolkit, designed to run on any modern
operating system, including all Unix variants. Hello uses CMake to
create a platform-specific build environment. The following platforms
and build environments are tested:

* Linux: Ubuntu 12.10, 13.04, 13.10 RHEL 6 (Makefile, i386, x64)

# Errata {#Errata}

