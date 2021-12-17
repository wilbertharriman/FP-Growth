# FP-Growth

## What is FP-Growth?

FP-growth is an improved version of the Apriori Algorithm widely used for frequent pattern mining in data. It is used as an analytical process to find association in dataset. For example, grocery store transactions might tell us that people tend to buy chips and beer together.

## Usage

```bash
g++ -std=c++11 -o fpgrowth FPGrowth.cpp
./fpgrowth [min support] [input file] [output file]
```

## Example

```bash
g++ -std=c++11 -o fpgrowth FPGrowth.cpp
./fpgrowth 0.2 sample.txt out.txt
```

## References

- https://towardsdatascience.com/understand-and-build-fp-growth-algorithm-in-python-d8b989bab342
- https://towardsdatascience.com/fp-growth-frequent-pattern-generation-in-data-mining-with-python-implementation-244e561ab1c3
