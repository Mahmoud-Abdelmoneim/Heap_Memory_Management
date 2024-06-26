# Heap Memory Manager 
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

* ___initailiztion___: Initializes the memory management system by setting up initial data structures.
* ___My_malloc___ : Allocates memory based on requested size, splitting blocks if necessary.
* ___my_free___: Frees the allocated memory block.
* ___SplitBlock___: Splits a memory block into two blocks.
* ___MerageTwoBlk___: Merges two adjacent free blocks into a single block.
* ___DisplayBlocks___: Utility function to display the current state of memory blocks.
# Compilation
To compile and run the code, follow these steps:

Include the malloc.h header file in your source code.
# makefile
use the makefile to build
use __make and make clean to clean__
