## How to write documentation
- The `doxygen_internal` directory contains HTML and formatting related doxygen files. These .in
files are processed by cmake, and all `@VAR@` expressions are replaced by the value of `VAR` (as defined in cmake)

- Documentation should all be written in `.md.in` files in this directory. Cmake will process them before running doxygen.
