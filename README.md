# Custom Memory Allocation Implementation
This project provides a custom implementation of memory allocation functions, namely My_malloc and my_free. It aims to demonstrate the basic functionality of memory allocation and deallocation through a simplified memory management system.

# Table of Contents
- Introduction
- Usage
- Features
- Code Overview
- Compilation
- Contributing
- License
# Introduction
In this project, we have implemented custom memory allocation functions My_malloc and my_free, which mimic the behavior of standard malloc and free functions found in the C standard library. The implementation includes functionalities like memory allocation, deallocation, splitting blocks, merging adjacent free blocks, and basic memory management.

# Usage
To use the custom memory allocation functions in your project, include the malloc.h header file and call My_malloc to allocate memory and my_free to release allocated memory. Ensure that you include the necessary headers and link the implementation file appropriately.

# Features
Custom memory allocation and deallocation functions.
Support for splitting and merging memory blocks for efficient memory management.
Basic memory allocation strategy to fulfill memory allocation requests.
# Code Overview
The main components of the code include:

* --initailiztion--: Initializes the memory management system by setting up initial data structures.
* --My_malloc-- : Allocates memory based on requested size, splitting blocks if necessary.
* #### my_free: Frees the allocated memory block.
* SplitBlock: Splits a memory block into two blocks.
* MerageTwoBlk: Merges two adjacent free blocks into a single block.
* DisplayBlocks: Utility function to display the current state of memory blocks.
# Compilation
To compile and run the code, follow these steps:

Include the malloc.h header file in your source code.
Compile the source code along with the implementation file.
Execute the compiled executable.
Example:
