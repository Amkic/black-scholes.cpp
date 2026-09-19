#pragma once

#include "instruments/european_options.hpp"
#include <cstddef>

namespace quant{

    struct MonteCarloResult{
        double price;
        double variance;
        double standard_error;
    };

    class MonteCarlo{
        public:
        static MonteCarloResult calculate_price(const EuropeanOption& option, std::size_t simulations, unsigned int seed);

        static MonteCarloResult calculate_price_antithetic(const EuropeanOption& option, std::size_t simulations, unsigned int seed);
    };
}