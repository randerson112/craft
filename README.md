# Craft

**Craft** is a lightweight CLI tool built with C designed to simplify the process of creating, building, and running C++ projects.  
It provides a minimal developer workflow similar to Cargo (Rust) but tailored for C++.  

With Craft, you can quickly spin up new projects, generate boilerplate files, build executables, and run them — all from the command line.  

---

## 📦 Requirements

To build and use Craft, you’ll need:

- **CMake** (>= 3.15 recommended)  
- **C++17 compatible compiler** (e.g., `g++`, `clang++`, or MSVC)  
- **Make** (on Linux/macOS) or **Ninja/MSBuild** (on Windows)

---

## ✨ Features

- **`craft project <path>`** – Create a new project at the given path.  
- **`craft init [path]`** – Initialize a Craft project in the current or specified directory.  
- **`craft build`** – Build all source files in the current project.  
- **`craft compile <src> [out]`** – Compile a single source file into an executable.  
- **`craft run <exe>`** – Run a compiled executable.  
- **`craft gen <file>`** – Generate boilerplate files (`.hpp`, `.cpp`, `CMakeLists.txt`).  
- **`craft help [command]`** – Show usage information for all commands, or a detailed guide for a specific one.  

---

## 🛠 Build from Source

Clone the repository and build with CMake:

```bash
git clone https://github.com/yourusername/craft.git
cd craft
mkdir build && cd build
cmake ..
make
```

This will produce the craft executable inside the build directory.

---

## 🌍 Add Craft to path

Make sure to add the craft executable to your system path:

### Linux/MacOS
```bash
sudo cp build/craft /usr/local/bin/
```

Or add it to PATH in ~.bashrc or ~.zshrc
```bash
export PATH="$PATH:/path/to/craft/build"
```

### Windows
```powershell
setx PATH "$($env:PATH);C:\path\to\craft\build"
```

Now craft can be used from anywhere on your system!
