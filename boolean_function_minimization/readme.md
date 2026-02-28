# Boolean Function Minimization using Sum-of-Products

## Introduction
This project implements a Boolean function minimizer that optimizes functional specifications (ON-set and Don't-care set) into a minimal **Sum-of-Products (SOP)** form. It employs heuristic expansion strategies to significantly reduce the literal count and product terms, making it suitable for large-scale Boolean logic with up to 24 input variables.

## Algorithm Flow
* **OFF-Set Construction**: Calculates the OFF-set by identifying minterms not present in the ON-set or Don't-care set, represented using an efficient bit-mapped structure.
* **Seed Selection**: Iteratively selects minterms from the ON-set to act as "seeds" for the expansion process.
* **Heuristic Expansion (Expand)**: 
    * Attempts to expand each literal of a cube into a "don't-care" (`-`) state.
    * Systematically checks each bit position to maximize the size of the implicant.
* **Validity Verification**: 
    * Uses bitwise operations to verify if an expanded cube covers any minterms in the OFF-set.
    * Only expansions that remain "OFF-set free" are accepted into the final cover.
* **Redundancy Removal**: Removes duplicate or contained implicants to ensure the final SOP is as compact as possible.
* **Runtime Management**: Monitors execution time and terminates the expansion early (at 175 seconds) to guarantee a valid output within the competition time limit.

## Compilation and Execution
```bash
# To compile
make

# To execute
./sop <input_specification> <output_sop_file>

# To clean object files
make clean