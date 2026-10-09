# IZG — Software GPU (C++17)

A software rasterizer: I hand-implemented the whole GL-style rendering
pipeline in C++ — the parts you would normally get from OpenGL.

## My code (`student/`)

- `gpu.cpp` — buffer clearing, vertex/index fetch, vertex shader calls,
  triangle rasterization, fragment shader calls
- `prepareModel.cpp` — model/vertex assembly used by the tests

## Around it

SDL2 window, Catch2 conformance + performance tests, Doxygen docs and large
test models in `resources/` — provided by the course, not my work (hence the
repository size).

## Build

```bash
mkdir build && cd build && cmake .. && make && ./izgProject_tests
```

Semestral project for *Základy počítačové grafiky (IZG)* at FIT VUT Brno.
