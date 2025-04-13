#!/bin/bash
set -x

curdir=$(dirname $0)
mkdir _tmp/
cd _tmp/

sudo apt update
sudo apt install -y gcc-multilib build-essential libelf-dev libdw-dev pkg-config python3-pyelftools

# install llvm/clang
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 15
sudo bash "$curdir"/update-alternatives-clang.sh 15 100

# install libbpf
cd "$curdir/_tmp/"
mkdir libbpf
cd ./libbpf
_VERSION="1.4.7"
VERSION="v$_VERSION"
wget https://github.com/libbpf/libbpf/archive/refs/tags/$VERSION.tar.gz
tar -xf $VERSION.tar.gz
cd libbpf-$_VERSION/src/
make
sudo make install
echo "/usr/lib64/" | sudo tee /etc/ld.so.conf.d/libbpf.conf
echo 'export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/lib64/pkgconfig/"' | tee -a $HOME/.bashrc
sudo ldconfig

# install bpftool
sudo apt install -y linux-tools-`uname -r`
