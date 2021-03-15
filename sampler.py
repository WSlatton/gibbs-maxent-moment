from BitVector import BitVector
import random
import numpy as np
import numpy.random as npr
from multiprocessing import Pool
import subprocess as sp
import os.path as path

path_to_executable = './gibbs_sampler_maxent'

class Sampler:
    def __init__(self, N, moments, burn_in=10, sample_interval=10, processes=6):
        """Create sampler with sample space {0,1}^N and distribution (proportional to)
        the maximum entropy distribution with basis functions given by moments and with
        the given coefficients on these basis functions. This is just a wrapper around
        a native C program.

        By a moment m (of order k), we mean the product h(x) = x_{m_1} ... x_{m_k}
        of a size k subset of the components of x. The parameter moments is a list of lists
        of indices. A list m of length k in moments corresponds to the product
        x[m[0]] * ... * x[m[k-1]], where x is an element of the sample space.

        :param N: the dimension of the sample space
        :param moments: a dict with keys tuples of the indices included in the moment
        and values the corresponding coefficient
        :param burn_in: the number of steps along the Markov Chain to perform before
        beginning to sample
        :param sample_interval: the number of steps along to Markov Chain to perform
        between each returned sample
        :param processes: the number of parallel workers to spawn and evenly divide
        the work between

        :return: a list of samples, where each sample is a tuple listing the indices for
        which the components of the sample are 1

        Note that one "step" along the Markov Chain means looping through each of the N indices
        and potentially updating that component of the state. That is, each of the N components
        of the state may potentially be changed in a single step.

        Example:
        Suppose we want to sample from a distribution on {0, 1}^3 proportional to
        p(x) = exp(0.2 * x_0 * x_1 - 0.7 * x_0 + 0.5 * x_0 * x_2 - 0.4 * x_2)
        We can then create the corresponding Sampler object with
        > sampler = Sampler(3, {
            (0, 1): 0.2,
            (0): -0.7,
            (0, 2): 0.5,
            (2): -0.4
        })
        and then draw 100 samples from it with
        > samples = sampler.sample_multiple(100)
        where samples will be an array of tuples such as
        > samples = [
            (0, 1),
            (),
            (0),
            (1, 2)
        ]
        where (1, 2) for example means that x = (0, 1, 1) as an element of {0, 1}^3
        """
        self.N = N

        # filter out empty moment
        moments = dict([(moment, coefficient) for moment, coefficient in moments.items() if len(moment) > 0])

        self.moments = list(moments.keys())
        self.coefficients = np.array(list(moments.values()))
        self.burn_in = burn_in
        self.sample_interval = sample_interval
        self.processes = processes

    def sample_one(self):
        """Return a single sample as a numpy array."""
        return self.sample_multiple(1)[0]

    def sample_multiple(self, number_of_samples):
        """Return specified number of samples as a list of numpy arrays."""
        parameters = "\n".join([
            " ".join(map(str, m)) for m in self.moments
        ]) + "\n\n" + "\n".join([
            str(c) for c in self.coefficients
        ])
        root = path.dirname(path.dirname(path.realpath(__file__)))
        number_of_samples_per_process = (self.processes - 1) * [number_of_samples // self.processes]
        number_of_samples_per_process.append(number_of_samples - sum(number_of_samples_per_process))

        open('in', 'w+').write(parameters)

        ps = [
            sp.Popen([
                path_to_executable,
                str(self.N),
                str(ns),
                str(self.sample_interval),
                str(self.burn_in)
            ], stdin=sp.PIPE, stdout=sp.PIPE)
            for ns in number_of_samples_per_process
        ]

        for p in ps:
            p.stdin.write(parameters.encode("utf8"))

        samples_strings = []

        for p in ps:
            output, _ = p.communicate()
            samples_strings += output.decode("utf8").split("\n")[:-1]

        samples = [
            tuple([int(term) for term in line.split(" ") if len(term) > 0])
            for line in samples_strings
        ]
        return samples


def _autocorrelation_binary(samples):
    """Compute the autocorrelation of a sequence of samples.

    :param samples: an (n, N) binary numpy array, where n is the number of samples
    and N is the number of neurons (the dimension of each sample)

    Autocorrelation is computed for each of the N channels in the usual way and
    then the L1 norm of these values is computed to return a single scalar for
    each time shift.
    """
    n, N = np.shape(samples)
    # convert {0, 1} variables to {-1, 1}
    samples = 2 * samples - 1
    channel_autocorrelations = np.zeros((n, N))
    max_autocorrelation = np.correlate(np.ones(n), np.ones(n), mode='same')

    for i in range(N):
        channel_autocorrelations[:, i] = np.correlate(samples[:, i], samples[:, i], mode='same') / max_autocorrelation

    normed_autocorrelation = np.sum(np.abs(channel_autocorrelations), axis=1) / N
    return normed_autocorrelation

def _sparse_to_binary(N, samples):
    binary = np.zeros((len(samples), N))

    for i in range(len(samples)):
        for j in samples[i]:
            binary[i, j] = 1

    return binary

def sample_autocorrelation(N, samples):
    """Compute the autocorrelation of a sequence of samples in the format
    produced by a Sampler object.

    :param N: dimension of state space {0, 1}^N
    :param samples: a list of samples, where each sample is representedl as
    a tuple of the indices with value 1

    Autocorrelation is computed for each of the N channels in the usual way and
    then the L1 norm of these values is computed to return a single scalar for
    each time shift.
    """
    return _autocorrelation_binary(_sparse_to_binary(N, samples))
