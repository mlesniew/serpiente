# serpiente
Simple snake game


## Building

### Ubuntu

Install dependencies:
```
sudo apt install \
    g++ \
    liballegro4-dev \
    make \
    pkg-config \
```

Build using `make`:
```
make
```


### Docker

The repo contains a Dockerfile, which can be used to build the project.  The result binaries need to be extracted from
the image.  This can all be done automatically using the script:
```
./build.sh
```

The script also extracts the required allegro shared library, so it doesn't need to be installed using `apt`.
