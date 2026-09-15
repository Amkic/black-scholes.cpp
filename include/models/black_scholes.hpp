#pragma once

#include "instruments/european_options.hpp"

namespace quant {

    class BlackScholes {
        public:
            static double calculate_option_price(const EuropeanOption& option);
            static bool is_put_call_parity_valid(
                const EuropeanOption& call_option,
                const EuropeanOption& put_option,
                double call_price,
                double put_price
            );
            static bool approximately_equal(double actual, double expected, double tolerance);

            static double calculate_delta(const EuropeanOption& option);

            static double calculate_gamma(const EuropeanOption& option);

            static double calculate_vega(const EuropeanOption& option);

            static double calculate_theta(const EuropeanOption& option);

            static double calculate_rho(const EuropeanOption& option);

            static double calculate_implied_volatility(const EuropeanOption& option, double market_price);

    };
}

