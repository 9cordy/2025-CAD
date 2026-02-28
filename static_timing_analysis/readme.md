# Static Timing Analysis

## Introduction
This project implements a **Static Timing Analysis (STA)** engine designed to verify the timing integrity of digital circuits. It calculates critical timing metrics, including propagation delay and transition time for every pin by processing gate-level Verilog netlists and standard cell libraries (.lib).

## Algorithm Flow
* **Graph Construction**: Parses the gate-level netlist to build a timing graph where pins are nodes and timing arcs are edges.
* **Library Parsing**: Reads standard cell libraries (.lib) to extract input capacitance and 2D Look-Up Tables (LUT) for Non-Linear Delay Modeling.
* **Topological Sorting**: Performs a topological sort on the graph to establish a valid processing order for timing propagation.
* **Forward Pass (Arrival Time)**: 
    * Propagates the earliest and latest Arrival Times (AT) and Slews from Primary Inputs to Primary Outputs.
    * Computes gate delays using the **Non-Linear Delay Model (NLDM)** with 2D Look-Up Tables.
    * Applies **Bilinear Interpolation** to estimate values for input slew and output load points not explicitly in the tables.
* **Backward Pass (Required Time)**: 
    * Propagates Required Arrival Times (RAT) from Primary Outputs back to the inputs to identify critical paths and calculate timing slack.

## Compilation and Execution
```bash
# To compile
make

# To execute
./sta <netlist_file> -l <lib_file> -i <input_patterns_file>

# To remove object files
make clean