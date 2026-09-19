#include "models/monte_carlo.hpp"

#include <cmath>
#include <random>

namespace quant{

    MonteCarloResult MonteCarlo::calculate_price(const EuropeanOption& option, std::size_t simulations, unsigned int seed){
        std::mt19937 generator(seed);

        std::normal_distribution<double> normal_distribution(0.0, 1.0);

        const double S0 = option.spot();
        const double K = option.strike();
        const double T = option.maturity();
        const double r = option.risk_free_rate();
        const double sigma = option.volatility();

        double sum_payoff = 0.0;
        double sum_payoff_squared = 0.0;

        for(std::size_t i = 0; i < simulations; i++){
            const double Z = normal_distribution(generator);

            const double ST = S0 * exp((r-0.5*sigma*sigma)*T + sigma * sqrt(T) * Z);

            double payoff = 0.0;

            if(option.type() == OptionType::Call){
                payoff = std::max(ST - K, 0.0);
            }
            else{
                payoff = std::max(K - ST, 0.0);
            }

            sum_payoff += payoff;
            sum_payoff_squared += (payoff * payoff);
        }

        double mean_payoff = sum_payoff / simulations;
        double empirical_variance = (sum_payoff_squared - simulations *  mean_payoff *  mean_payoff)/(simulations-1);
        double price = exp(-r*T) * mean_payoff; 
        double standard_error = exp(-r*T) * sqrt(empirical_variance/simulations);

        return MonteCarloResult{price, empirical_variance, standard_error};
    }

    MonteCarloResult MonteCarlo::calculate_price_antithetic(const EuropeanOption& option, size_t simulations, unsigned int seed){
        std::mt19937 generator(seed);
        std::normal_distribution<double> normal_distribution(0.0,1.0);

        const double S0 = option.spot();
        const double K = option.strike();
        const double T = option.maturity();
        const double r = option.risk_free_rate();
        const double sigma = option.volatility();

        double sum_payoff = 0.0;
        double sum_payoff_squared = 0.0;

        const size_t pairs = simulations / 2;

        for (std::size_t i = 0; i < pairs; ++i) {
            const double Z = normal_distribution(generator);

            const double ST_positive = S0 * exp((r-0.5 * sigma * sigma) * T + sigma * sqrt(T) * Z);

            const double ST_negative =S0 * exp((r-0.5 * sigma * sigma) * T- sigma * sqrt(T) * Z);

            double payoff_positive = 0.0;
            double payoff_negative = 0.0;

            if (option.type() == OptionType::Call) {
                payoff_positive = std::max(ST_positive-K, 0.0);

                payoff_negative = std::max(ST_negative-K, 0.0);
            } else {
                payoff_positive = std::max(K-ST_positive, 0.0);

                payoff_negative = std::max(K - ST_negative, 0.0);
            }

            double mean_payoff = 0.5 * (payoff_positive + payoff_negative);

            sum_payoff += mean_payoff;
            sum_payoff_squared += mean_payoff * mean_payoff;
        }

        double mean_payoff = sum_payoff / pairs;

        double empirical_variance =(sum_payoff_squared- pairs * mean_payoff * mean_payoff)/(pairs - 1);

        const double price = exp(-r * T) * mean_payoff;

        const double standard_error =exp(-r * T) * sqrt(empirical_variance / pairs);

        return MonteCarloResult{
            price,
            empirical_variance,
            standard_error
        };
    }
}