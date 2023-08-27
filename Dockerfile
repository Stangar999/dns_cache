FROM gcc:12.2 as build

RUN apt-get update && \
    apt-get install -y \
      libboost-dev \
      libgtest-dev \
      cmake
	  