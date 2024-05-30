# MPI-TravellingSalesman
Parallel implementation of the Travelling Salesman Problem using MPI, demonstrating efficient parallel algorithms and inter-node communication strategies.

This repository is dedicated to the Travelling Salesman Problem (TSP), solved using parallel computing methods via MPI. The TSP is a classic optimization problem aiming to find the shortest possible route that visits each city once and returns to the origin city.

## Project Overview

- **Serial Version**: Implements the TSP using a serial approach where cities are processed one at a time.
- **Parallel Version**: Utilizes MPI to divide the problem among multiple processors, significantly reducing computation time by exploring multiple routes concurrently.

## Features
- **Dynamic Graph Generation**: Creates a graph dynamically with specified number of cities and random distances.
- **Parallel Algorithm**: Divides cities among MPI processes to solve the TSP in segments.
- **Inter-node Communication**: Uses MPI functions to manage data exchange between nodes during the computation.
- **Performance Evaluation**: Compares the serial and parallel implementations in terms of runtime and scalability.

## Setup and Compilation

Ensure MPI is installed on your system. Clone the repository and compile the program using:

```bash
git clone https://github.com/yourusername/MPI-TravellingSalesman.git
cd MPI-TravellingSalesman
mpicc -o tsp TSP.c
Run the program using MPI:
mpiexec -n <number_of_processes> ./tsp
