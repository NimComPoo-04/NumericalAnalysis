# Internship Report

## Index

Introduction:
    Understanding the inner workings of software like mathematica

Phases:
    1. Learning
        1. Integral of Multivalued Functions 
        2. Numerical Integration methods
        3. Dealing with Improper Integrals

    2. Training
        1. Building a prototype in javascript utilizing simple features
            of the thing

    3. Project
        1. Implementing a general perpose integrator in C
        2. Dynamically loading custom functions
        3. Visualizing function by utilizing opengl

    4. Results
        1. Gamma function computation using general perpose use
        2. Slow efficiency utilizing different methods

Conclusion:
    We recreate a simple quadrature solving tool that can be of general perpose use

## Introduction

## Learning

Integrals of Multivalued Functions:
    * Generally solved using simpsons and trapezoidal rules
    * Put in a derivation of the simpsons rule from lagrange interpolation
    * Multiple Integrals calculated by integrating in order from inside out
        calculating the size of the rectangles
    * Improper integrals are dealt with by slowly increasing the size of the limits from
        0 to infinity, if there are multiple discontinuities in the integrals we split the
        integrals into multiple parts and compute them.

## Training
    * We build an simple prototype which can integrate and plot function
        by using graphs.

## Project
    * Create a simple commandline C program to compute integrals of any variable function.
    * Implement a macro system to assist writing functions in C
    * load functions by utilizing the 'tcc' compiler to compile C code dynamically into a shared
        object and execute it for speed
    * Visualizing the function and integrals by using opengl and extention library raylib to allow user interactivity

## Results
    * Calculation of gamma function and other related functions using this tool.
    * Specializing integral methods to compute integrals utilizing laconz aproximation

## Conclusionn
    We were able to build a simple integrator similar to the one used in mathematica. Also
understand the innerworking of such softwakjkj wkkjkjkjkj wh whkjkjkjkjkj wkkjkjre.
