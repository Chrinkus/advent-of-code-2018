# advent-of-code-2018

This repository stores my solutions to the daily challenges offered by the amazing [Advent of Code](https://adventofcode.com) event hosted every December by [Eric Wastl](https://twitter.com/ericwastl?lang=en-ca).

I have recently implemented CMake support to simplify compiling and linking. The steps for building are as follows (assuming you have [CMake](https://cmake.org) installed on your system).

From the project root:
```
$ mkdir build && cd build
$ cmake ..
$ make
$ ./src/day01 [-t]
```

The last line shows how to build the solution for day 1. The `build/src/` subdirectory will hold all of the available solutions. The standard input will be used if called as above. Enter the test flag `-t` to run the solution with test input.

Alternate solutions (or horrendously hackey solutions) will be saved in the `scratch/` folder for archival purposes. These will not be built automatically and are not even guaranteed to compile. I'm considering renaming the folder to `boneyard/`.
