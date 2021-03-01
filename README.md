# gibbs-maxent-moment

An optimized Gibbs sampler for use with moment-based maximum entropy distributions. That is, with a binary sample space

![](https://raw.githubusercontent.com/WSlatton/gibbs-maxent-moment/main/readme_images/fig1.png)

and where each basis function is of the form

![](https://raw.githubusercontent.com/WSlatton/gibbs-maxent-moment/main/readme_images/fig2.png)

where *k* is the order of the moment and

![](https://raw.githubusercontent.com/WSlatton/gibbs-maxent-moment/main/readme_images/fig3.png)

are distinct indices. The distribution to be sampled from is then of the form

![](https://raw.githubusercontent.com/WSlatton/gibbs-maxent-moment/main/readme_images/fig4.png)

where *Z* is a normalization constant and

![](https://raw.githubusercontent.com/WSlatton/gibbs-maxent-moment/main/readme_images/fig5.png)

are the chosen coefficients.

## Usage
This sampler is in the form of a command-line program that reads parameters from its arguments and stdin and outputs the samples to stdout. Usage is
```
gibbs-sampler-maxent N number_of_samples sample_interval burn_in
```
where N is the dimension of the state space as above, number_of_samples is self-explanatory, sample_interval is the number of steps through the Markov chain to discard between each return sample, and burn_in is the number of steps to discard before returning the first sample. Note that by a single step we mean considering each of the N indices for an update once, so every index of the state may change in a single step.

The choice of moments to use and their coefficients are provided to stdin in a format like
```
3
0 1
2 4 5

0.2
-0.7
0.3
```
with one moment per line, a blank line, and then one coefficient per line in the same order. This example corresponds to using the moments

![](https://raw.githubusercontent.com/WSlatton/gibbs-maxent-moment/main/readme_images/fig6.png)

with corresponding coefficients

![](https://raw.githubusercontent.com/WSlatton/gibbs-maxent-moment/main/readme_images/fig7.png)

The samples are given one per line on stdout by listing the indices with value one. For example, the binary pattern (0, 1, 1, 0, 0, 0, 1) with N = 7 would be output as the line
```
1, 2, 6
```

## Building
The only dependency is the standard math library `libm` and the executable should be built using CMake in the usual way. Create a `build` directory wherever you like, enter it, and invoke
```
$ cmake -DCMAKE_BUILD_TYPE=Release path_to_repo_root
$ make
```
The executable `gibbs-maxent-moment` will then be built in the `build` directory and is ready to use.
