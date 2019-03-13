sudo apt-get update
sudo apt-get install autoconf -y
sudo apt-get install gcc -y
sudo apt-get install g++ -y
sudo apt-get install cmake -y
sudo apt-get install libtool -y 
sudo apt-get install pkg-config -y
sudo apt-get install git -y
git clone https://github.com/zeromq/libzmq.git
cd libzmq
./autogen.sh
./configure
make
sudo make install
sudo ldconfig
cd ..
git clone git://github.com/zeromq/czmq.git
cd czmq
./autogen.sh
./configure
make
sudo make install
sudo ldconfig
cd ..
rm libzmq -r -f
rm czmq -r -f
