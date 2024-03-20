# cpp-consistent-hashing-algorithms

## Overview

This project collects C++ implementations of the most prominent consistent hashing algorithms for non-peer-to-peer contexts.

The implemented algorithms are:

- [1997] **Ring** by [D. Karger et al.](https://www.cs.princeton.edu/courses/archive/fall09/cos518/papers/chash.pdf)
- [1998] **Rendezvous** by [Thaler and Ravishankar](https://ieeexplore.ieee.org/abstract/document/663936)
- [2014] **Jump** by [Lamping and Veach](https://arxiv.org/pdf/1406.2294.pdf)
- [2015] **Multi-probe** by [Appleton and O’Reilly](https://arxiv.org/pdf/1505.00062.pdf)
- [2016] **Maglev** by [D. E. Eisenbud](https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/44824.pdf)
- [2020] **Anchor** by [Gal Mendelson et al.](https://arxiv.org/pdf/1812.09674.pdf)
- [2021] **Dx** by [Chaos Dong and Fang Wang](https://arxiv.org/pdf/2107.07930.pdf)
- [2023] **Power** by [Eric Leu](https://arxiv.org/pdf/2307.12448.pdf)
- [2023] **Memento** by [M. Coluzzi et al.](https://arxiv.org/pdf/2306.09783.pdf)

### Benchmarks

The project includes a benchmarking tool designed explicitly for consistent hashing algorithms.
The tool allows benchmarking the following metrics in a fair and agnostic way:

- **Balance**: The ability of the algorithm to spread the keys evenly across the cluster nodes.
- **Init time**: The time the algorithm requires to initialize its internal structure.
- **Lookup time**: The time the algorithm needs to find the node a given key belongs to.
- **Memory usage**: The amount of memory the algorithm uses to store its internal structure.
- **Monotonicity**: The ability of the algorithm to move the minimum amount of resources when the cluster scales.
- **Resize balance**: The ability of the algorithm to keep its balance after adding or removing nodes.
- **Resize time**: The time the algorithm requires to reorganize its internal structure after adding or removing nodes.

The format of the configuration file is described in detail in the [`configs/template.yaml`](configs/template.yaml) file.
The tool will use the [`configs/default.yaml`](configs/default.yaml) file that represents the default configuration if no configuration file is provided.

## Instructions

1. Clone the repository:

```shell
git clone https://github.com/robertovicario/cpp-consistent-hashing-algorithms.git
```

2. Navigate to the cloned repository:

```shell
cd cpp-consistent-hashing-algorithms
```

3. Run repository setup:

```shell
# Ensure script has executable permissions:
# chmod +x repo.sh
./repo.sh
```

4. Setup project dependencies with CMake:

```shell
./cmake.sh
```

5. Build the project with Ninja:

```shell
cd build
ninja
```

6. Start the framework:

```shell
./main <configuration>.yaml
```

## Licence

This project is distributed under [GNU General Public License version 3](https://opensource.org/license/gpl-3-0). You can find the complete text of the license in the project repository.

### Credits

- The foundation of this framework draws inspiration from [java-consistent-hashing-algorithms](https://github.com/SUPSI-DTI-ISIN/java-consistent-hashing-algorithms.git), initially developed by Massimo Coluzzi from the Institute of Information Systems and Networking (ISIN) at SUPSI.

## Contacts

- Amos Brocco: amos.brocco@supsi.ch
- Roberto Vicario: roberto.vicario@student.supsi.ch

