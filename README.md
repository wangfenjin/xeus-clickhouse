# xeus-clickhouse

[![Travis](https://travis-ci.com/wangfenjin/xeus-clickhouse.svg?branch=master)](https://travis-ci.com/github/wangfenjin/xeus-clickhouse)
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/wangfenjin/xeus-clickhouse/master?filepath=example%2Fclickhouse.ipynb) 

**xeus-clickhouse is and early developer preview and is not suitable for general usage yet. Features and implementation are subject to change.**

`xeus-clickhouse` is a Jupyter kernel for ClickHouse. It's SQL based on the native implementation of the Jupyter protocol [xeus](https://github.com/jupyter-xeus/xeus).

## Give it a Try!

Try it using binder or launch Jupyter using `docker` and launch a new SQL notebook by selecting the **xclickhouse** kernel:

```bash
# start jupyter with clickhouse kernal
docker run -d -p 8888:8888 wangfenjin/xeus-clickhouse

# start a local clickhouse for testing
docker run -d --name jupyter-clickhouse-server -p 8123:8123 --ulimit nofile=262144:262144 yandex/clickhouse-server

# open the example/clickhouse.ipynb and connect to local server by 
#      %CONNECT --host host.docker.internal --port 8123
```

## Installation

To ensure that the installation works, it is preferable to install xeus-clickhouse in a fresh conda environment. It is also needed to use a miniconda installation because with the full anaconda you may have a conflict.

The safest usage is to create an environment named xeus-clickhouse with your miniconda installation

```bash
conda create -n xeus-clickhouse
conda activate xeus-clickhouse
```

### Installing from conda

WIP

### Installing from source

To install the xeus-clickhouse dependencies

```bash
conda install cmake nlohmann_json xtl cppzmq xeus cpp-tabulate=1.2 -c conda-forge
```

Then you can compile the sources

```bash
mkdir build
cd build
cmake -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX ..
make
make install
```

## Documentation

Refer to examples/

## Dependencies

``xeus-clickhouse`` depends on

- [xeus](https://github.com/wangfenjin/xeus)
- [clickhouse-cpp](https://github.com/ClickHouse/clickhouse-cpp)
- [Tabulate](https://github.com/p-ranav/tabulate)

## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) to know how to contribute and set up a development environment.

## Credits

This project is inspired by [xeus-clickhouse](https://github.com/jupyter-xeus/xeus-sqlite), and reuse many code/doc from the project! Thanks!

## License

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
