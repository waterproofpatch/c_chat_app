# C Chat Application

Application for a C based centralized chat server.

## Prerequisites

* ceedling (for CMock)
* gcc (for compilation)

## Build

```bash
make
```

## Run
Start the server, then connect with one or more clients:

```bash
./bin/server/server.bin
```

```bash
./bin/client/client.bin someusername
```

## Test

```bash
make test
```

View the coverage results as HTML in build/artifacts/gcov/GcovCoverageResults.html