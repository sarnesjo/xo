# xo

xo is a superoptimizer, a tool for generating optimal assembly programs.

## Installation

xo requires that [BuDDy][1], a package for manipulating binary decision
diagrams, is installed on your system.

[1]: http://sourceforge.net/projects/buddy/

To install xo, do the following:

    ./bootstrap

This only applies if you've fetched the code from the development repository,
not if you've obtained it by downloading a release version.

    ./configure CPPFLAGS='-I/path/to/bdd/include -L/path/to/bdd/lib' LDFLAGS='-L/path/to/bdd/lib'

Passing CPPFLAGS and LDFLAGS is only required if you've installed BuDDy in a
non-standard location.

    make

Build the package.

    make check

Run the test suite. This is optional, but recommended.

    make install

Install the package on your system.

## Usage

xo has four forms:

    xo [-m M] [-qv] PROGRAM

In its standard form, xo generates programs and tests them for equivalence with
the given program.

By default, xo will generate programs up to 5 instructions in length, test them
by running them on test vectors (which may yield false positives), and by
modeling them as binary decision diagrams, (which should yield correct output,
barring any bugs in the implementation), and print those which pass both tests
on stdout.

The -m option sets the maximum length of the generated programs.

The -v option increases verbosity. Passing it once causes xo to also print
programs which pass the probabilistic test, as well as some diagnostic
information, on stderr. Passing it twice causes xo to also print every generated
program on stderr, which impacts performance considerably.

The -q option decreases verbosity, and is currently useless.

The syntax used for the assembly programs is detailed below.

    xo -L

In this form, xo prints a list of the supported instructions, along with their
arity, on stdout, and exits.

    xo -H

In this form, xo prints brief usage instructions on stdout and exits.

    xo -V

In this form, xo prints its version number on stdout and exits.

## Syntax

xo supports a small subset of the x86 instruction set. Intel syntax is used,
with some modifications:

* Registers are named r0--r7.
* Statements are terminated by semicolons.
* Immediate values are not supported.
* Effective addresses are not supported.

Example:

    add r0,r0; sbb r1,r1; sub r1,r0; adc r0,r1;

## Caveats

xo is a prototype, and has numerous shortcomings:

* Support for many important instructions, such as neg, mul and lea, as well as
  shift and rotate instructions, is missing.
* Immediate values are not supported.
* Multiple output registers are not supported. The output register is assumed to
  be the last register written to.
* Only 32-bit arithmetic is simulated.
* The equivalence test is unnecessarily slow, as values for the input program
  are not cached.
* The code for the generator is a mess.

## License

xo is free and unencumbered software released into the public domain. See
UNLICENSE for details.

## Author

xo was created by Jesper Särnesjö, as part of a degree project at KTH.
