# PyMC3 and all that

I am again playing with Bayesian data analysis ideas, and using this as an excuse to look at `pymc3`.

Here are some notebooks:
- [Introduction](Introduction.ipynb)
- [Dirichlet](Dirichlet.ipynb) looks at using a Dirichlet prior for a catrgorical / multinomial distribution.
- [Regression](Regression.ipynb) simply linear regression.
- [Hawkes processes](Hawkes processes.ipynb) looks a fitting point process models using Bayesian ideas.
- [pymc3_example_1](pymc3_example_1.ipynb) recreation from the official docs

### Emcee

I've used emcee in the past, and I'm glad to see it's still under active development.
- [GitHub](https://github.com/dfm/emcee)
- [Docs](http://dfm.io/emcee/current/)
- It's pure Python (on top of `numpy`) so install with `pip install emcee`

The related project [corner](https://github.com/dfm/corner.py) is useful for visualisation.

### PyMC3

A more sophistaicated (both mathematically, and from a software point of view) MCMC sampler is pymc3.

- [GitHub](https://github.com/pymc-devs/pymc3)
- [Docs](http://docs.pymc.io/notebooks/getting_started.html)
- Using Anaconda on Windows, I found that `conda install pymc3` worked fine (if slowly).  I had no luck with the `conda-forge` version however.  YMMV.
