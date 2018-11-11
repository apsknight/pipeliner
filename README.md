# Pipeliner

>*[COA (CS3L002) Assignment](problem_statement.pdf) : A Program to detect and resolve data-dependency in an assembly program by explicitly inserting `NOP` instructions.* 

[![Build Status](https://travis-ci.org/apsknight/pipeliner.svg?branch=master)](https://travis-ci.org/apsknight/pipeliner)

[![asciicast](https://asciinema.org/a/xg6MERA5Kkn2VNHUbq23UTI07.png)](https://asciinema.org/a/xg6MERA5Kkn2VNHUbq23UTI07)

## Getting Started

Following instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

The Program requires `g++` compiler with support for `C++11` for compilation.

### Installing

Clone Repository on your machine and `cd` into it

```
git clone https://github.com/apsknight/pipeliner.git && cd pipeliner
```

Build executable from [Makefile](./Makefile).

```
make
```

The `make` instruction with create an executable file with name `pipeliner` in the current repository.

##  Usage
The executable program takes an input file as an command line arument. This input file contains the Assembly Program which need to be checked. An example Assembly Program: [program.asm](program.asm)

### Detect dependency and memory delays

```
./pipeliner program.asm
```

### Detect dependency and memory delays
Running above command with `-r` or `--resolve` flag will resolve depedency from program `program.asm` and create a file `output.txt` with new program.
```
./pipeliner -r program.asm
```

## Author
[Aman Pratap Singh](https://github.com/apsknight)

## License
[GNU GPV v3.0](LICENSE)