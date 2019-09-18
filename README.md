# circ
![build status](https://travis-ci.com/benlalanes/circ.svg?token=TKyn2Ryua6spkE1fPLDz&branch=master) [![codecov](https://codecov.io/gl/mana-/circ/branch/master/graph/badge.svg?token=Sm7VYUwH18)](https://codecov.io/gl/mana-/circ)


_circ_ is a simple implementation of a subset of the IRC protocol using C, written as an execise in
network programming/Linux systems programming. 

## Overview

The _circ_ project supports a subset of the commands specified in the IRC protocol. The list of currently
supported commands can be found below.

Currently supported commands:
- `NICK`
- `USER`
- `PING`
- `PONG`
- `PRIVMSG`
- `JOIN`
- `PART`
- `LIST`
- `TOPIC`

Additionally, _circ_ runs only as a single server.

## Usage

The cIRC server can be started using the following command:

```bash
circ
```

## Development

_circ_ depends on the following libraries:
- _glib2.0_ - for data structures

To set up the project build, navigate to the root of the project source, create a build
directory, and run the `cmake` executable. For example:

```bash
mkdir build
cd build
cmake ..
```
        
To run the tests, simply run:
```bash
make test
```

To run the tests with extra output, run:
```bash
ctest -VV
```

