# Inkimera
### A non-photorealistic rendering focused Game Engine

Inkimera is an  cross-platform game engine written in C(99) focused on artistic rendering and simplicity.

The engine is built on the backs of giants:
- Graphics Library (https://github.com/raysan5/raylib)
- ECS (https://github.com/SanderMertens/flecs)
- NPR Research (https://github.com/semontesdeoca/MNPR)

**Inkimera is pre-alpha software and not ready for usage at this time.**

## Build from source

Required tools: `git`, `cmake`, `make`, and a C compiler such as `gcc` or `clang`.

```
git clone --recursive https://github.com/Inkimera/inkimera.git
cd inkimera
cmake .
make
```

## Editor

After you build the project run `./editor/inkimera_editor` to launch the editor.
