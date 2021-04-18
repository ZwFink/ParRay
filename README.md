# ParRay
Currently the ray tracer works serially. 
To compile and run, the following commands can be executed:
```bash
cd src
mkdir build
cd build
cmake ..
make
./tracer_marble_single_threaded > img.ppm
```
This will produce a ```ppm``` image that can be viewed using tools such as feh. The raytracer produces the image below.

![Image produced by the ray tracer](img.png)

# Environment Setup
The unit test requires CMake version at least 3.11 to run, due to the FetchContent() module.

To install CMake newest version on Ubuntu follow 
https://askubuntu.com/a/865294

The above process may need to install ```openssl``` and ```pkg-config```
Use the following commands to install them
```
sudo apt install pkg-config
sudo apt install libssl
``` 

