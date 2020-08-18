# Prometheus client
git clone https://github.com/jupp0r/prometheus-cpp.git
cd prometheus-cpp
git submodule init
git submodule update
mkdir _build
cd _build
cmake .. -DBUILD_SHARED_LIBS=ON
make -j 4
make install
