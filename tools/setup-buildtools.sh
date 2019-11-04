#!/bin/sh

if [ -f /bin/yum ]; then
# Prefer yum over apt-get
yum -y install automake
yum -y install autoconf
yum -y install libtool
yum -y install make gcc gcc-c++
yum -y install wget
yum -y install libcurl
yum -y install zlib-devel
yum -y install git
yum -y install gperftools-libs
yum -y install libcurl-devel
yum -y install rpm-build

# Install gcc-7
# FIXME: current tooling is CentOS-centric :-/
yum -y install centos-release-scl
yum -y install devtoolset-7
yum -y install devtoolset-7-valgrind

yum-config-manager --enable rhel-server-rhscl-7-rpms

if [ `gcc --version | grep 7` == "" ]; then
echo "*********************************************************"
echo "*** Please make sure you start the build with gcc-7   ***"
echo "*** > scl enable devtoolset-7 ./build.sh              ***"
echo "*********************************************************"
exit 3
fi

if [ `cmake --version | grep 3` == "" ]; then
yum -y remove cmake
wget https://cmake.org/files/v3.6/cmake-3.6.2.tar.gz
tar -zxvf cmake-3.6.2.tar.gz
cd cmake-3.6.2
./bootstrap --prefix=/usr/local
make
make install
cd ..
fi

else
# Use apt-get
export DEBIAN_FRONTEND=noninteractive
apt-get update -y
apt-get install -qq automake
apt-get install -qq libtool-bin
apt-get install -qq cmake
apt-get install -qq curl
apt-get install -qq libcurl4-openssl-dev
apt-get install -qq zlib1g-dev
apt-get install -qq git
apt-get install -qq build-essential
apt-get install -qq libssl-dev
apt-get install -qq libsqlite3-dev
# Stock sqlite may be too old
#apt install libsqlite3-dev
apt-get install -qq wget
apt-get install -qq clang-format
fi

## Change owner from root to current dir owner
chown -R `stat . -c %u:%g` *
