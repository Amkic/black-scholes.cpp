# Quant Pricing Engine

This project is a C++ implementation of pricing methods for European options.

The project started with the implementation of the Black-Scholes model and was progressively extended with the main Greeks, implied volatility and Monte Carlo pricing. The goal was to implement the different calculations directly in C++ and compare an analytical pricing method with a numerical one.

## European Options

The option is represented by its main parameters:

* Spot price \(S_0\)
* Strike price \(K\)
* Maturity \(T\)
* Risk-free interest rate \(r\)
* Volatility \(\sigma\)
* Option type: Call or Put

For example, the following parameters represent an at-the-money European Call:

```text
Spot        = 100
Strike      = 100
Maturity    = 1 year
Rate        = 5%
Volatility  = 20%
```

The option itself only contains these parameters. The pricing calculations are implemented separately in the pricing models.

## Black-Scholes

The first pricing method implemented is the Black-Scholes model.

For a European Call:

$$
C = S_0N(d_1) - Ke^{-rT}N(d_2)
$$

For a European Put:

$$
P = Ke^{-rT}N(-d_2) - S_0N(-d_1)
$$

where:

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

The cumulative distribution function of the standard normal distribution is required by the Black-Scholes formula. It is calculated using the standard C++ `erfc` function.

The implementation also checks put-call parity:

$$
C-P=S_0-Ke^{-rT}
$$

This provides a simple consistency check between the Call and Put prices.

## Greeks

The Black-Scholes implementation also calculates the main Greeks:

* Delta
* Gamma
* Vega
* Theta
* Rho

The Greeks are calculated directly from the analytical formulas.

For example, the Call Delta is:

$$
\Delta = N(d_1)
$$

and Gamma is:

$$
\Gamma =
\frac{N'(d_1)}
{S_0\sigma\sqrt{T}}
$$

where \(N'(d_1)\) is the standard normal probability density function.

These calculations use the same intermediate quantities as the pricing formula, such as \(d_1\) and \(d_2\).

## Implied Volatility

The project also includes an implied volatility calculation.

Given a market option price, the objective is to find the volatility \(\sigma\) for which the Black-Scholes price matches that market price:

$$
Price_{BS}(\sigma)=Price_{market}
$$

There is no direct closed-form solution for \(\sigma\), so the problem is solved numerically.

I used the bisection method. The algorithm starts with a volatility interval and repeatedly divides it in two. At each iteration, the Black-Scholes price is calculated using the midpoint of the interval and compared with the market price.

The interval is then reduced until the calculated price is sufficiently close to the target price.

This gives a simple and robust way of obtaining the implied volatility.

## Monte Carlo Pricing

After implementing the analytical model, I added a Monte Carlo pricer for comparison.

Under the risk-neutral measure, the terminal value of the underlying is simulated using:

$$
S_T =
S_0
\exp\left(
(r-\frac{1}{2}\sigma^2)T
+
\sigma\sqrt{T}Z
\right)
$$

with:

$$
Z\sim N(0,1)
$$

For each simulation, the program:

1. Generates a standard normal random variable \(Z\).
2. Calculates the terminal price \(S_T\).
3. Calculates the option payoff.
4. Repeats the simulation for a given number of paths.
5. Calculates the average payoff.
6. Discounts the result back to the present.

For a Call:

$$
Payoff = \max(S_T-K,0)
$$

For a Put:

$$
Payoff = \max(K-S_T,0)
$$

The Monte Carlo estimator is therefore:

$$
V \approx
e^{-rT}
\frac{1}{N}
\sum_{i=1}^{N} Payoff_i
$$

The implementation uses `std::mt19937` and `std::normal_distribution` from the C++ standard library.

A seed is provided to the Monte Carlo function so that the same simulation can be reproduced.

## Monte Carlo Error and Convergence

Unlike Black-Scholes, Monte Carlo does not give an analytical result. The price is estimated from a finite number of simulations.

The implementation calculates the empirical variance and standard error of the estimator.

The standard error is:

$$
SE =
e^{-rT}
\sqrt{\frac{s^2}{N}}
$$

where \(s^2\) is the sample variance of the simulated payoffs.

The standard error decreases approximately as:

$$
SE \propto \frac{1}{\sqrt{N}}
$$

I tested this by increasing the number of simulations:

```text
Simulations    Price       Standard Error
------------------------------------------
1,000          11.4193       0.5032
10,000         10.5951       0.1477
100,000        10.4741       0.0467
1,000,000      10.4682       0.0148
```

The results show the expected reduction in the standard error as the number of simulations increases.

Another way of checking the convergence is to look at:

$$
SE\sqrt{N}
$$

which remains approximately constant when the Monte Carlo estimator behaves according to the expected \(1/\sqrt{N}\) convergence rate.

## Antithetic Variates

To reduce the Monte Carlo variance, I also implemented antithetic variates.

Instead of generating only one value \(Z\), each simulation uses both:

$$
Z
$$

and:

$$
-Z
$$

This produces two terminal prices:

$$
S_T(Z)
$$

and:

$$
S_T(-Z)
$$

The two corresponding payoffs are averaged before being included in the estimator.

The idea is that the two simulations are negatively correlated, which can reduce the variance of the estimator without simply increasing the number of simulations.

For one test case, the results were:

```text
                         Standard MC    Antithetic MC
-----------------------------------------------------
Price                      10.4741         10.4428
Standard Error              0.0467          0.0328
```

The variance reduction measured for this run was approximately 75%.

The exact reduction depends on the option parameters and the generated sample.

## Black-Scholes vs Monte Carlo

One of the main points of the project is to compare the analytical Black-Scholes price with the Monte Carlo estimate for the same option.

For the example above:

```text
Black-Scholes price : 10.4506
Monte Carlo price   : 10.4741
Standard error      : 0.0467
```

The two values are not expected to be exactly identical because the Monte Carlo result is based on a finite random sample.

The difference between the two prices is:

$$
|10.4741-10.4506| \approx 0.0235
$$
