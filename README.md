# SpacePP

Just a humble asteroid mining simulator. This is used to develop and test control
and group behavior models for multi-agent automation.

It also contains methods for doing volume rendering of 3D textures (voxels). 

* `/bin` contains model data and compiled C/C++
* `/include` contains C/C++ headers
* `/src` contains C/C++ source code

Use `sh compile.sh` to compile to the `bin` directory.

Running `texture` from inside bin will result in a seg fault because it won't be able to find the density data file. 