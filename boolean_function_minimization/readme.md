# Boolean Function Minimization

## Introduction
This project implements a Boolean function minimizer that converts a functional specification (ON-set and Don't-care set) into an optimized **Sum-of-Products (SOP)** form. The goal is to minimize the total literal count to satisfy specific validation criteria, especially for large-scale test cases where global optimization is computationally expensive.

## Methodology
The solver implements a heuristic approach inspired by the **Espresso heuristic logic minimizer**, focusing on the **Expand** phase to achieve a minimal cover.

### Core Features:
* **Off-Set Mapping**: Efficiently represents the OFF-set using a bit-mapped structure (`Off_Set_Map`) to allow rapid intersection checks during expansion.
* **Heuristic Expansion**: For each minterm in the seeds (ON-set), the algorithm iteratively attempts to expand each bit into a "don't-care" (`-`).
* **Validity Checking**: An expansion is only accepted if the resulting implicant does not cover any minterms in the OFF-set.
* **Time-Limited Execution**: Includes a safety mechanism to stop processing and output the current best cover if the runtime approaches the 180-second (3-minute) limit.

## Technical Implementation
* **Implicant Representation**: Uses a custom `struct Implicant` storing `value` and `care` masks to handle `0`, `1`, and `-` states.
* **Bitwise Optimization**: Leverages bitwise operations for subset traversal and OFF-set validation to handle bit widths up to $n=24$.

## Compilation and Execution
[cite_start]The project includes a `Makefile` to build the executable named `sop`[cite: 422, 461].

```bash
# To compile the program
make

# To execute the minimizer
./sop <specification_file> <output_sop_file>