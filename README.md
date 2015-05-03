# Basic-ray-tracing
Exercise project on ray tracing for computer graphics course at university

## Program interface
Program implemented as console application with parameters defined using command arguments. Scene description is defined in [yaml](http://yaml.org) files ([example file](https://github.com/hmpbmp/Basic-ray-tracing/blob/master/example.yaml))
```
raytracer.exe --scene=scene.yaml --resolution_x=1024 --resolution_y=768 --output=result.bmp --trace_depth=2
```

## Implemented functionality
* Basic scene creation
* Raytracing of basic objects
* CSG operations
* OBJ models raytracing


## Libraries used in this project
* [libyaml](https://github.com/yaml/libyaml) - A C library for parsing and emitting YAML 
* [tinyobjloader](https://github.com/syoyo/tinyobjloader) - Tiny but poweful single file wavefront obj loader written in C++
* [poly34](http://math.ivanovo.ac.ru/dalgebra/Khashin/poly/index_r.html) - Equation solver
* [glm](https://github.com/g-truc/glm) - OpenGL Mathematics (GLM)
