# cpp-consistent-hashing-algorithms
This project collects C++ implementations and benchmarking tools of two of the most prominent consistent hashing algorithms for non-peer-to-peer contexts.

The implemented algorithms are:
* [2020] __anchor hash__ by [Gal Mendelson et al.](https://arxiv.org/pdf/1812.09674.pdf), using the implementation found on [Github](https://github.com/anchorhash/cpp-anchorhash)
* [2023] __memento hash__ by [M. Coluzzi et al.](https://arxiv.org/pdf/2306.09783.pdf)

## Benchmarks

The project includes a two benchmarking tools **speed_test** and **balance**, derived from the same tools provided by [Anchorhash](https://github.com/anchorhash/cpp-anchorhash)

**speed_test** also records **heap allocations** and the maximum allocated heap space.

## Building

Clone the repository:
```bash
git clone https://github.com/slashdotted/cpp-consistent-hashing-algorithms.git
```
Move into the project's directory and update the [vcpkg](https://vcpkg.io/en/) submodule:
```bash
cd cpp-consistent-hashing-algorithms/
git submodule update --init
cd vcpkg
./bootstrap-vcpkg.sh -useSystemBinaries
```
Generate the build file (for [Ninja](https://ninja-build.org/)):
```bash
cd ..
cmake -B build/ -S . -GNinja -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake ninja
```
Move into the **build** directory and start building:
```bash
cd build
ninja
```
## Running the benchmarks
The **speed_test** benchmark performs several random key lookups, the syntax is:
```bash
./speed_test Algorithm AnchorSet WorkingSet NumRemovals Numkeys ResFilename
```
where
 * **Algorithm** can be *memento* (for MementoHash using *boost::unordered_flat_map* for the removal set), *mementoboost* (for MementoHash using *boost::unordered_map* for the removal set), *mementostd* (for MementoHash using *std::unordered_map* for the removal set), *mementomash* (for MementoHash using a hash table similar to Java's HashMap), or *anchor* (for AnchorHash)
 * **AnchorSet** is the size of the Anchor set (**a**): this parameter is used only by *anchor* but must be set to a value *at least equal to WorkingSet* even with *MementoHash*;
 * **WorkingSet** is the size of the initial Working set (**w**);
 * **NumRemovals** is the number of nodes that should be removed (randomly) before starting the benchmark;
 * **Numkeys** is the number of keys that will be queried during the benchmark;
 * **ResFilename** is the filename containing the results of the benchmark;
By default, details about the allocate memory will also be produced in the output. For example:
```bash
./speed_test memento 1000000 1000000 20000 1000000 memento.txt
Algorithm: memento, AnchorSet: 1000000, WorkingSet: 1000000, NumRemovals: 20000, NumKeys: 1000000, ResFileName: memento.txt, Random: rand()
   @StartBenchmark: Allocations: 0, Allocated: 0, Deallocations: 0, Deallocated: 0, Maximum: 0
   @AfterAlgorithmInit: Allocations: 0, Allocated: 0, Deallocations: 0, Deallocated: 0, Maximum: 0
   @AfterRemovals: Allocations: 11, Allocated: 802488, Deallocations: 10, Deallocated: 401076, Maximum: 802488
   @EndBenchmark: Allocations: 11, Allocated: 802488, Deallocations: 10, Deallocated: 401076, Maximum: 802488
Memento<boost::unordered_flat_map> Elapsed time is 0.333966 seconds, maximum heap allocated memory is 802488 bytes, sizeof(Memento<boost::unordered_flat_map>) is 56
```

The **balance** benchmark performs a balance test and accepts the same parameters as **speed_test**. Example:

```bash
./balance memento 1000000 1000000 20000 1000000 memento.txt
Algorithm: memento, AnchorSet: 1000000, WorkingSet: 1000000, NumRemovals: 20000, NumKeys: 1000000, ResFileName: memento.txt
Memento<boost::unordered_flat_map>: LB is 8.82
```

## Java implementation
For a Java implementation of these and additional algorithms please refer to [this repository](https://github.com/SUPSI-DTI-ISIN/java-consistent-hashing-algorithms)

## Credits
The AnchorHash implementation is based on code Copyright (c) 2020 anchorhash released under the MIT License
MementoHash is based on the Java implementation found on [this repository](https://github.com/SUPSI-DTI-ISIN/java-consistent-hashing-algorithms), released under the GNU GPL-3.0 license 
