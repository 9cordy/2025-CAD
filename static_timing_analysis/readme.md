# Static Timing Analysis

## Introduction
This project implements a **Static Timing Analysis (STA)** tool designed to calculate the timing characteristics of digital circuits. It processes circuit netlists and library cell information to determine critical timing metrics, including Arrival Time (AT), Required Arrival Time (RAT), and Slack for every pin in the design.

## Features
* **Topological Propagation**: Implements a topological sorting algorithm to ensure timing data propagates correctly from primary inputs to primary outputs (Forward Pass) and vice versa (Backward Pass).
* **Delay Calculation**: Computes gate delays and transition times based on a Non-Linear Delay Model (NLDM) using 2D Look-Up Tables (LUT).
* **Bilinear Interpolation**: Supports precise delay and slew estimation through bilinear interpolation when input slew or output load falls between table indices.
* **Constraints Handling**: Accounts for setup time constraints and output required arrival times to identify potential timing violations (negative slack).

## Technical Implementation
### Timing Analysis Engine
* **Forward Pass**: Calculates the earliest and latest Arrival Times (AT) and Slew at each pin starting from Primary Inputs (PI).
* **Backward Pass**: Calculates the Required Arrival Times (RAT) starting from Primary Outputs (PO) and D-flip-flop (DFF) data pins.
* **Slack Calculation**: Determines the timing margin at each pin: $Slack = RAT - AT$.

### Data Structures
* **Timing Graph**: Represents the circuit as a directed acyclic graph (DAG) where nodes are pins and edges are timing arcs.
* **Parser**: A robust parser to read `.lib` (cell library) and `.v` (Verilog netlist) files.

## Compilation and Execution
The project provides a `Makefile` to compile the source code into an executable named `sta`.

```bash
# To compile the program
make

# To execute the STA solver
./sta <netlist_file> -l <lib_file> -i <input_patterns_file>