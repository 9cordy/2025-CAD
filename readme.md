# 2025 Special Topics on Computer-Aided Design (CAD)

This repository contains two algorithmic projects developed for the "Special Topics on Computer-Aided Design" course in 2025. The implementations focus on front-end logic synthesis and timing verification methodologies.

---

## Tech Stack
* **Language**: C / C++
* **Algorithms**: Topological Sort, Heuristic Logic Expansion, Graph Traversal
* **Concepts**: Boolean Algebra (SOP Minimization), Static Timing Analysis (STA), Non-Linear Delay Model (NLDM), Interpolation

---

## Projects Overview

### 1. [Boolean Function Minimization using Sum-of-Products](./boolean_function_minimization)
* **Description**: A Boolean function minimizer that converts functional specifications (ON-set and Don't-care set) into a minimized Sum-of-Products (SOP) form, supporting up to 24 input variables.
* **Implementation Details**:
  * Constructs the OFF-set using a bit-mapped structure to represent minterms.
  * Executes a heuristic "Expand" phase to iteratively expand literals into "don't-care" (`-`) states while verifying against the OFF-set via bitwise operations.
  * Performs redundancy removal to eliminate duplicate or contained implicants.
  * Incorporates a runtime management mechanism to terminate the expansion process at 175 seconds and return the current valid cover.

### 2. [Static Timing Analysis](./static_timing_analysis)
* **Description**: A static timing analyzer that processes gate-level Verilog netlists and standard cell libraries (`.lib`) to compute propagation delay and transition time.
* **Implementation Details**:
  * Parses standard cell libraries to extract input capacitance and Non-Linear Delay Model (NLDM) 2D Look-Up Tables (LUT).
  * Constructs a timing graph and applies Topological Sorting to establish the correct sequence for timing propagation.
  * Computes gate delays and output transitions using Bilinear Interpolation for input slew and output load points not explicitly defined in the LUTs.
  * Executes Forward and Backward passes to evaluate pin metrics and trace the longest critical paths.

---

## Compilation and Execution
Each project is independently maintained with its own `Makefile`. 

To run a specific project:
1. Navigate to the corresponding sub-directory.
2. Run `make` to compile the executable.
3. Follow the specific execution commands outlined in the local `readme.md` of each project.