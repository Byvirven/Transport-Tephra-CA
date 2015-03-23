# Transport-Tephra-CA
An OpenCL 1.2 cellular automata to modelize the volcanic tephra's transport

The aim of this work was to implement a cellular automata on GPUs, written in C++ and OpenCL, 
of the tephra fragment transportation model submitted by PhD. J.-L. Falcone of Geneva 
University - Earth Sciences Department

I implement this OpenCL CA for my bachelor thesis

Marco Dos Santos Oliveira : marco.dossantosoliveira 'at' sunrise.ch

# Compilation 

you can read the OpenCL official documentation for compilation or try to update this CLI based on your own compilation environment.

c++ -I /CUDAPATH/cuda/include/ -I /CUDAPATH/NVIDIA_GPU_Computing_SDK/OpenCL/common/inc/ -L /CUDAPATH/cuda/lib64/ -l OpenCL *.cpp -o output
