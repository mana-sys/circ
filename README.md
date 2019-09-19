# circ
[![build status](https://travis-ci.com/mana-sys/circ.svg?token=q4tVM5BYGYfUJ3njM47q&branch=master)](https://travis-ci.com/mana-sys/circ) [![codecov](https://codecov.io/gh/mana-sys/circ/branch/master/graph/badge.svg?token=kjDMrcsIMx)](https://codecov.io/gh/mana-sys/circ)



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

