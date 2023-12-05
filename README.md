# cpp-consistent-hashing-algorithms
This project collects C++ implementations and benchmarking tools of two of the most prominent consistent hashing algorithms for non-peer-to-peer contexts.

The implemented algorithms are:
* [2020] __anchor hash__ by [Gal Mendelson et al.](https://arxiv.org/pdf/1812.09674.pdf), using the implementation found on [Github](https://github.com/anchorhash/cpp-anchorhash)
* [2023] __memento hash__ by [M. Coluzzi et al.](https://arxiv.org/pdf/2306.09783.pdf)

## Benchmarks

The project includes a two benchmarking tools **speed_test** and **balance**, derived from the same tools provided by [Anchorhash](https://github.com/anchorhash/cpp-anchorhash)

**speed_test** also records **heap allocations** and the maximum allocated heap space.

## Java implementation
For a Java implementation of these and additional algorithms please refer to [this repository](https://github.com/SUPSI-DTI-ISIN/java-consistent-hashing-algorithms)
