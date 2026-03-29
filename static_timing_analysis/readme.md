# Static Timing Analysis

## Introduction
This project implements a simplified **Static Timing Analysis (STA)** engine for **combinational circuits**. It analyzes gate-level Verilog netlists together with a standard cell library (.lib) and input pattern files to calculate key timing information, including output loading, propagation delay, output transition time, and the longest/shortest timing paths.

## Algorithm Flow
* **Graph Construction**: Parses the gate-level netlist and builds the circuit connectivity using nets and cells.
* **Library Parsing**: Reads the simplified Liberty file and extracts input capacitance, timing table indices, and 2D Look-Up Tables for delay and transition modeling.
* **Topological Sorting**: Performs a topological sort on the combinational circuit to obtain a valid forward-processing order.
* **Step 1 - Output Loading Calculation**:
    * Computes the output loading of each cell by summing the input capacitances of all fanout cells.
    * Assigns a fixed load of **0.03 pF** to primary outputs.
* **Step 2 - Worst-Case Delay Propagation**:
    * Propagates timing in topological order.
    * Determines each gate's input transition time from its latest-arriving predecessor.
    * Looks up **cell rise / cell fall** and **rise transition / fall transition** values from the Liberty timing tables.
    * Applies **interpolation / extrapolation** when the queried load or transition point is not exactly on a LUT index.
    * Selects the worse one between rise delay and fall delay as the gate's worst-case propagation delay.
* **Step 3 - Path Extraction**:
    * Compares all primary outputs to find the **longest** and **shortest** delays.
    * Recovers the corresponding paths by tracing predecessor cells.
* **Step 4 - Pattern-Based Gate Timing Simulation**:
    * Simulates logic values according to each input pattern.
    * Recomputes gate delay and transition time based on the actual output logic value.

## Compilation and Execution
```bash
# To compile
make

# To execute
./sta <netlist_file> -l <lib_file> -i <input_patterns_file>

# To remove object files
make clean