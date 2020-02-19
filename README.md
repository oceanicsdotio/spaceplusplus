I am porting C++ space game to Rust and WebAssembly.

The point is to unify all of the graphics tooling I have in once place, and make them interoperable.

This will support doing fast iteration while creating computing meshes, and will enable interactive development of agent-based models similar to NetLogo. 

* bin contains model data and compiled C/C++
* include contains C/C++ headers
* public contains html, javascript, css, json, wasm_pack artifacts
  * glsl contains shader source code
* src is rust and C/C++ source code



Use `sh build-wasm.sh` to compile rust to webassembly and generate javascript bindings.

Use `sh build-cpp.sh` to compile C/C++ native code to the `bin` directory.

`Cargo.toml` describes the rust dependencies, and `CMakeLists.txt` may eventually have a valid C++ build.



## Rust crate

agent.rs - agent simulations

lib.rs - main routines

model.rs - model generation, triangulation

series.rs - datastreams

webgl.rs - webgl handlers and utilities





## Right-triangulated Irregular Networks (RTIN)



hierarchal data structure

height values

assume exact representation





alternate to sub-grid, or TIN

subset of points forming right isosceles triangle

multiresolution surface rendering



RTIN partitions in dataset



square into triangles



fisrt divide along NW-SE

new partitions are formed by spling non-terminal triangles

terminal value from underlying data

split T from right angle to midpoint of hypoenuse

if edge point causes neighbor (R) to become a quad, propagate 

if euqal size, process stops, else if R larger, continue to propagate



[4.8.8] laves net

tother types: square (4^4), 30-60-90 (4.6.12), equilateral (6^3)

square/eq, single refinement will split all cells

therefore cannot form a continuous non-uniform partition

for 4.8.8 split effects at most 2 triangles of each size

in 4.6.12, at most 12 of each equal and larger



binary tree

each triangle in partition is a leaf

root is square

left(0)/right(1) is determined by side of splitting ray

splits are from hypot to right vert



from a parent triangle ordered CCW with v3 as right angle

left -> (v3, v1, m), right -> (v2, v3, m)









