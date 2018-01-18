# Talk

Some Python notebooks from a talk I gave to the [CSAP](https://www.geog.leeds.ac.uk/research/centre-for-spatial-analysis-and-policy/) cluster meeting.

## Aims

- Quickly demonstrate how to use Python (`pandas`, `scipy`, `matplotlib`, `statsmodels`) to perform standard Regression techniques
- Argue for the benefit (or at least, alternative viewpoint) of using Bayesian techniques
- Show how to use `pymc3` to perform [Probabilistic programming](https://en.wikipedia.org/wiki/Probabilistic_programming_language)
- Quickly introduce `emcee`.

## Introduction

- [The notebook](1%20-%20Introduction.ipynb)

Introduce the data from

> "Expectations of brilliance underlie gender distributions across academic disciplines." Sarah-Jane Leslie, Andrei Cimpian, Meredith Meyer, and Edward Freeland. Science, Vol. 347, Issue 6219, pp. 262-265. http://science.sciencemag.org/content/347/6219/262

and perform some regression fitting and tests using `scipy` and `statsmodels`.

## Bayesian techniques

- [The notebook](2%20-%20Bayesian%20inference%20with%20pymc3.ipynb)
- To install `pmyc3`: `conda install pymc3`
- To install `corner`: `pip install corner`

We introduce `pymc3` and perform a little regression.

## Further bayesian techniques

- [The notebook](http://localhost:8888/notebooks/Talk/3%20-%20More%20regression%2C%20and%20emcee.ipynb)
- To install `emcee` `pip install emcee`

We look at improper priors with `pymc3`, and introduce `emcee`.


## Other resources

- [Bayesian Methods for hackers](http://camdavidsonpilon.github.io/Probabilistic-Programming-and-Bayesian-Methods-for-Hackers/) is an online book (also available in print!) aimed at Python programmers who have little background in statistics.
- [Statistical Rethinking ported to PyMC3](https://github.com/aloctavodia/Statistical-Rethinking-with-Python-and-PyMC3) is a Python implementation of the code from the (excellent) book [Statistical Rethinking](http://xcelab.net/rm/statistical-rethinking/)
- [BDA3](https://www.amazon.co.uk/Bayesian-Analysis-Chapman-Statistical-Science-ebook/dp/B00I60M6H6) is my goto book for all things Bayesian (even if it doesn't mention Python, and only scratches the surface of R and Stan).