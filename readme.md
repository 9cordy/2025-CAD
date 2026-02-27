# 2025 Special Topics in Computer Aided Design

This repository contains key projects from the 2025 CAD, focusing on logic optimization and timing verification.

---

## 1. Boolean Function Minimizer (SOP Optimization)

### Introduction
This project implements an efficient Boolean function minimizer that converts functional specifications into an optimized **Sum-of-Products (SOP)** form. [cite_start]It is designed to handle complex Boolean functions with bit widths up to $n=24$[cite: 436].

### Key Features
* **Espresso-based Heuristics**: Implements the **Expand** phase to iteratively maximize implicants while ensuring no OFF-set minterms are covered.
* **Bit-mapped OFF-set Logic**: Utilizes a 64-bit word mapping technique (`Off_Set_Map`) to achieve high-speed validity checking during expansion.
* **Adaptive Runtime Management**: Features a self-monitoring timer to ensure the solver outputs the best available solution within the 3-minute competition limit.



---

## 2. Static Timing Analysis (STA) Solver

### Introduction
A comprehensive **Static Timing Analysis (STA)** engine that evaluates the timing integrity of digital circuits. It processes gate-level Verilog netlists and standard cell libraries to identify critical paths and timing violations.

### Key Features
* **Timing Propagation Engine**: Performs a dual-pass (Forward/Backward) topological traversal to compute Arrival Time (AT), Required Arrival Time (RAT), and Slack for all circuit pins.
* **Non-Linear Delay Modeling (NLDM)**: Implements precise delay estimation using 2D Look-Up Tables (LUT) and **Bilinear Interpolation** for slew and load sensitivities.
* **Setup Constraint Analysis**: Automatically identifies timing-critical pins with negative slack to assist in design closure.



---

## Development Environment
* [cite_start]**Language**: C++ [cite: 421]
* [cite_start]**Compiler**: `g++` on Linux environment [cite: 464]
* **Build System**: Makefile-based compilation for all modules

## How to Build
To build each project, navigate to its respective directory and run:
```bash
make