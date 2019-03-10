# DefectDeflect

## Zero Message Queue

### Installation of Zero Message Queue

Für Ubuntu 18.10

```
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
sudo apt-get install libczmq-dev -y
rm libzmq -r -f
```

### CZMQ API 

> http://czmq.zeromq.org/czmq4-0:czmq

- `zsock_t* zsock_new_req (const char *endpoint);`

- `zsock_t* zsock_new_rep (const char *endpoint);`

- `char* zstr_recv (void *source);`

- `int zstr_send (void *dest, const char *string);`

- `void zsock_destroy (zsock_t **self_p);`
