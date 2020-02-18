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







