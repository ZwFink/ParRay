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

# Running the program
The binaries are output to build/bin folder.
## Generating a random scene
```bash
cd build/bin
./gen_random_scene
```
The above command will output a json ```.data``` file, which can be fed into the algorithm.
## Running single threaded BVH on the generated data file
```bash
cd build/bin
./sphere_bvh_single_threaded /path/to/scene_file
```
## Running multi-threaded BVH on the generated data file
To run on 6 processes with 4 threads per process.
```bash 
mpiexec -np 6 --bind-to none ./bin/bvh_mpi random_spheres_scene.data 4 > img.ppm
```
## Running bench mark
```bash
./build/bin/bm_ray_tracing
```
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

### Google benchmark environment setup
To install google benchmark in ubuntu run the following command
```
sudo apt-get update
sudo apt-get install libbenchmark-dev
```

