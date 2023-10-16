# BWT-ICFL

An implementation of a different approach to the traditional BWT, using ICFL.

## Abstract

This was code written for my Bachelors' Thesis back in 2021.

It contains source codes for simplified factorizations, sample implementation for traditional BWT and BWT with CFL using naive methods and the source code for the new approach.
Also contains an example main file that can read a file, passed by argument, or from stdin. The output is then either stdout or a redirected file.
It is used in the context of a broader compression pipeline that uses various transformation and a form of arithmetic encoding to compress an input file.

This repository is no longer maintained, and the code probably contains a few things which could (and will) be improved.
Please refer to my profile for updated versions.

## How to use

No dependencies required, you can simply:

```bash
make all
make clean
./bwt.out <input_file >output_file
```
