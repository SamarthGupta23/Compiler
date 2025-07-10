# 🛠️ Custom Bytecode Compiler & Virtual Machine

A simple yet powerful **compiler + virtual machine** written from scratch in C++.

This project compiles a **custom language** into bytecode, which is then executed by a **stack-based virtual machine**. The entire toolchain — from **scanner** to **parser** to **VM** — is built from scratch with no external dependencies.

---

## 📦 Features

- ✅ **Custom Scanner (`scanner.cpp`)** – converts source code into tokens
- ✅ **Recursive Descent Parser** – turns tokens into opcodes
- ✅ **Bytecode Generator** – compiles parsed code to bytecode
- ✅ **Stack-Based Virtual Machine** – executes the bytecode
- ✅ **Minimal Language Support**
  - Only global variables (no scoping yet)
  - Supported types:
    - `nil`
    - `boolean`
    - `number` (double)

---

## 📝 Language Overview

This language is minimal, but provides a foundation for building out a full interpreted language. You can find:

- 📄 Example source code: [`code.lol`](code.lol)
- 📄 Compiled bytecode output: [`insides.lol`](insides.lol)

---

## 🚀 Getting Started

### 🧰 Prerequisites

- C++17 or later
- **CLion** (recommended) or any C++ IDE

### 🛠️ Setup Instructions

1. Create a new project in **CLion**
2. Paste all provided `.cpp` and `.h` files into the project
3. Build and run — there are **no external dependencies**

---

## 📂 Project Structure

| File            | Description                                 |
|-----------------|---------------------------------------------|
| `scanner.cpp`   | Tokenizes raw source code into tokens       |
| `parser.cpp`    | Converts tokens into VM instructions        |
| `vm.cpp`        | Executes the bytecode using a stack machine |
| `code.lol`      | Sample source code                          |
| `insides.lol`   | Bytecode output for the sample              |

---

## 🔮 Future Roadmap

- [ ] Local variables and block scoping
- [ ] String and array data types
- [ ] Function declarations and calls
- [ ] Basic standard library (I/O, math)
- [ ] Control flow constructs (if, while, for)

---

## 📜 License

This project is open-source under the MIT License.
