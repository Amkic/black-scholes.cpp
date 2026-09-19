# Quant Pricing Engine

A C++ implementation of European option pricing using the Black-Scholes analytical model and Monte Carlo simulation.

The project was developed to explore how option pricing models can be translated into a structured C++ implementation, from the mathematical formulas to numerical simulation and variance reduction.

## Overview

The project currently supports:

* European Call and Put options
* Black-Scholes analytical pricing
* Greeks:

  * Delta
  * Gamma
  * Vega
  * Theta
  * Rho
* Implied volatility using the bisection method
* Monte Carlo pricing
* Monte Carlo convergence analysis
* Antithetic variates for variance reduction
* Basic validation between analytical and numerical prices
* CMake-based build system

The main idea is to have two independent pricing approaches for the same European option:

```text
                    European Option
                           │
              ┌────────────┴────────────┐
              │                         │
              ▼                         ▼
       Black-Scholes               Monte Carlo
        analytical                  simulation
              │                         │
              │                    Generate Z ~ N(0,1)
              │                         │
              │                    Simulate S(T)
              │                         │
              │                    Calculate payoff
              │                         │
              │                    Discount payoff
              │                         │
              └────────────┬────────────┘
                           │
                           ▼
                     Compare prices
```

The analytical Black-Scholes model provides a reference price, while Monte Carlo estimates the same price numerically.

---

## Project Structure

```text
quant-pricing-engine/
│
├── include/
│   ├── instruments/
│   │   └── european_options.hpp
│   │
│   └── models/
│       ├── black_scholes.hpp
│       └── monte_carlo.hpp
│
├── src/
│   ├── instruments/
│   │   └── european_option.cpp
│   │
│   └── models/
│       ├── BlackScholes.cpp
│       └── monte_carlo.cpp
│
├── examples/
│   └── option_example.cpp
│
├── tests/
│   ├── black_scholes_tests.cpp
│   └── BS_MC_test.cpp
│
└── CMakeLists.txt
```

The `EuropeanOption` class contains the characteristics of the financial instrument, while the pricing models operate on this object.

---

## 1. European Option

An option is represented by its main market parameters:

```text
Option type
Spot price
Strike price
Maturity
Risk-free interest rate
Volatility
```

For example:

```text
Call
S₀ = 100
K  = 100
T  = 1 year
r  = 5%
σ  = 20%
```

The `EuropeanOption` class is deliberately kept independent from the pricing models.

This allows the same option object to be passed to both Black-Scholes and Monte Carlo.

---

# 2. Black-Scholes

For a European Call, the Black-Scholes price is:

$$
C = S_0 N(d_1) - Ke^{-rT}N(d_2)
$$

with:

$$
d_1 =
\frac{
\ln(S_0/K) + (r+\frac{1}{2}\sigma^2)T
}{
\sigma\sqrt{T}
}
$$

and:

$$
d_2 = d_1-\sigma\sqrt{T}
$$

For a Put:

$$
P = Ke^{-rT}N(-d_2)-S_0N(-d_1)
$$

where \(N(x)\) is the cumulative distribution function of the standard normal distribution.

The normal CDF is implemented using the complementary error function:

```cpp
0.5 * erfc(-x / sqrt(2))
```

This avoids relying on an external numerical library.

---

## 3. Greeks

The Black-Scholes implementation also calculates the main Greeks:

```text
Delta
Gamma
Vega
Theta
Rho
```

These measure the sensitivity of the option price to changes in different parameters such as the underlying price, volatility, time and interest rate.

They are implemented directly from the Black-Scholes formulas rather than using numerical finite differences.

---

# 4. Implied Volatility

Implied volatility is the volatility that makes the theoretical Black-Scholes price equal to a given market price.

The problem can be written as:

$$
f(\sigma) = Price_{BS}(\sigma) - Price_{market} = 0
$$

The implementation solves this numerically using the **bisection method**.

The process is:

```text
Choose volatility interval
        │
        ▼
Calculate midpoint
        │
        ▼
Calculate Black-Scholes price
        │
        ▼
Compare with market price
        │
        ├── Price too high → reduce upper bound
        │
        └── Price too low  → increase lower bound
        │
        ▼
Repeat until convergence
```

The method is simple and robust, although it requires more iterations than some faster root-finding methods.

---

# 5. Monte Carlo Pricing

Monte Carlo provides a completely different way of obtaining the option price.

Under the risk-neutral measure, the terminal stock price follows:

$$
S_T =
S_0
\exp\left[
\left(r-\frac{1}{2}\sigma^2\right)T
+
\sigma\sqrt{T}Z
\right]
$$

where:

$$
Z \sim N(0,1)
$$

For each simulation, the program therefore:

```text
Generate Z ~ N(0,1)
        │
        ▼
Calculate S(T)
        │
        ▼
Calculate option payoff
        │
        ▼
Repeat N times
        │
        ▼
Calculate average payoff
        │
        ▼
Discount at exp(-rT)
        │
        ▼
Monte Carlo price
```

For a Call:

$$
Payoff = \max(S_T-K,0)
$$

For a Put:

$$
Payoff = \max(K-S_T,0)
$$

The final estimator is:

$$
V \approx e^{-rT}\frac{1}{N}
\sum_{i=1}^{N} Payoff_i
$$

The simulations use `std::mt19937` together with a normal distribution from the C++ standard library.

A fixed seed can be supplied to make the simulation reproducible.

---

# 6. Monte Carlo Error

Monte Carlo does not give exactly the same value as Black-Scholes because the result is estimated from a finite number of simulations.

The implementation calculates the empirical variance and standard error:

$$
SE =
e^{-rT}
\sqrt{\frac{s^2}{N}}
$$

The theoretical convergence rate is:

$$
SE = O\left(\frac{1}{\sqrt{N}}\right)
$$

This means that increasing the number of simulations improves the precision, but relatively slowly.

For example, increasing the number of simulations by a factor of 100 only reduces the standard error by approximately a factor of 10.

Example observed during testing:

```text
Simulations    Price       Standard Error
------------------------------------------
1,000          11.4193       0.5032
10,000         10.5951       0.1477
100,000        10.4741       0.0467
1,000,000      10.4682       0.0148
```

The quantity:

```text
Standard Error × sqrt(N)
```

remains approximately constant, which is consistent with the expected \(1/\sqrt{N}\) convergence.

---

# 7. Antithetic Variates

The project also implements a simple variance reduction technique: **antithetic variates**.

Instead of generating only:

$$
Z
$$

the simulation also uses:

$$
-Z
$$

For each generated random variable, two terminal prices are therefore calculated:

$$
S_T(Z)
$$

and

$$
S_T(-Z)
$$

Their payoffs are averaged before being added to the Monte Carlo estimator.

The idea is to introduce a negative correlation between the two simulated payoffs and reduce the variance of the estimator.

Example result:

```text
                         Standard MC    Antithetic MC
-----------------------------------------------------
Price                      10.4741         10.4428
Standard Error              0.0467          0.0328
```

For this simulation, the estimated variance reduction was approximately:

```text
75.4%
```

The exact reduction depends on the option and the random sample.

---

# 8. Analytical vs Monte Carlo

One of the main purposes of the project is to compare the two approaches.

For example:

```text
Black-Scholes price : 10.4506
Monte Carlo price   : 10.4741
Standard error      : 0.0467
```

The difference between the two prices is expected because Monte Carlo is an estimator.

A useful way to interpret the result is to compare the difference with the Monte Carlo standard error rather than expecting the two prices to be identical.

As the number of simulations increases, the Monte Carlo estimate converges toward the analytical Black-Scholes value.

---

# 9. Build

The project uses CMake.

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

The compiled programs are then available in the `build` directory.

For example:

```bash
./build/option_example
```

and:

```bash
./build/BS_MC_test
```

---

# 10. Purpose of the Project

This project started as a way to work through quantitative finance concepts from the implementation side rather than treating the formulas as black boxes.

The progression was:

```text
European option representation
            ↓
Black-Scholes pricing
            ↓
Greeks
            ↓
Implied volatility
            ↓
Monte Carlo pricing
            ↓
Monte Carlo convergence
            ↓
Variance reduction
            ↓
Comparison between analytical
and numerical pricing
```

The project also provided an opportunity to work with C++ classes, separate interfaces from implementations, numerical algorithms, random number generation and CMake.

The implementation is intentionally kept relatively small. The objective is to have a clear pricing engine that can be understood from the underlying mathematics to the C++ implementation, rather than building a large framework around a few pricing models.

---

## Possible Extensions

Some natural extensions would be:

* Monte Carlo pricing for additional derivatives
* Confidence intervals
* Additional variance reduction methods
* Faster random number generation
* American option pricing
* Binomial tree pricing
* Performance benchmarking
* Parallel Monte Carlo simulations

These are not currently part of the project.
