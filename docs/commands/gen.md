# craft gen

Generates a file with boilerplate code.

## Usage

```bash
craft gen <file>
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `<file>` | yes | File to generate with extension |

## Description

Creates a new file with starter boilerplate. Supported file types are `.c`, `.cpp`, `.h`, and `.hpp`.

### File Placement

When run inside a Craft project:
- Header files (`.h`, `.hpp`) are placed in the first `include_dirs` from `craft.toml`
- Source files (`.c`, `.cpp`) are placed in the first `source_dirs` from `craft.toml`

When run outside a Craft project:
- Files are placed in the current directory

Craft will not overwrite an existing file.

### Generated Content

**Header file (`.h`):**
```c
#ifndef UTILS_H
#define UTILS_H

// Code here

#endif // UTILS_H
```

**Header file (`.hpp`):**
```cpp
#pragma once

// Code here
```

**Source file (`.c`):**
```c
#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello, World!\n");
    return 0;
}
```
or
```c
#include "header.h"

// Add your implementation here
```

**Source file (`.cpp`):**
```cpp
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;
}
```
or
```cpp
#include "header.hpp"

// Add your implementation here
```

The generated source file automatically includes the corresponding header if one exists.

## Examples

```bash
# Generate a C++ header and source pair
craft gen utils.hpp
craft gen utils.cpp

# Generate a C header and source pair
craft gen math.h
craft gen math.c
```

## See Also

- [craft project](project.md) — create a new project
- [craft.toml reference](../craft-toml.md)