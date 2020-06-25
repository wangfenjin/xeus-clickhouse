FROM jupyter/base-notebook:latest

USER root

# Install all OS dependencies for notebook server that starts but lacks all
# features (e.g., download as all possible file formats)
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update \
 && apt-get install -yq --no-install-recommends \
    git \
    build-essential

USER $NB_UID
WORKDIR $HOME

RUN conda config --set always_yes yes --set changeps1 no
RUN conda install cmake xeus=0.24.0 xtl nlohmann_json cppzmq cpp-tabulate==1.2 -c conda-forge

RUN git clone --recursive git://github.com/wangfenjin/xeus-clickhouse.git

WORKDIR $HOME/xeus-clickhouse

RUN mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$CONDA_DIR -DCMAKE_PREFIX_PATH=$CONDA_DIR .. && make -j2 install