#include "models/black_scholes.hpp"
#include "instruments/european_options.hpp"
#include <cmath>
#include <iostream>

namespace quant {

    double calculate_d1(const EuropeanOption& option){
        double numerateur = log(option.spot() / option.strike()) + (option.risk_free_rate() + option.volatility()*option.volatility()/2)*option.maturity();
        double denominateur = option.volatility() * sqrt(option.maturity());

        return numerateur/denominateur;
    }

    double calculate_d2(const EuropeanOption& option, double d1){
        double d2 = d1 - option.volatility() * sqrt(option.maturity());
        return d2;
    }

    double calculate_N(double x){
        return 0.5 * erfc(- (x/sqrt(2)));
    }

    double calculate_normal_distribution(double x){
        const double PI = 3.14159265358979323846;
        return 1.0/sqrt(2*PI) * exp(-(x*x)/2);
    }

    double BlackScholes::calculate_option_price(const EuropeanOption& option){
        double S = option.spot();
        double K = option.strike();
        double r = option.risk_free_rate();
        double T = option.maturity();
        double Sigma = option.volatility();

        double d1 = calculate_d1(option);
        double d2 = calculate_d2(option, d1);
        double N_d1 = calculate_N(d1);
        double N_d2 = calculate_N(d2);

        double price;

        if(option.type() == OptionType::Call){
            price = S * N_d1 - K * exp(-(r*T)) * N_d2;
        }
        else{
            double N_minus_d1 = calculate_N(-d1);
            double N_minus_d2 = calculate_N(-d2);
            price = K * exp(-r*T) * N_minus_d2 - S * N_minus_d1;
        }

        return price;
    }

    bool BlackScholes::is_put_call_parity_valid(
        const EuropeanOption& call_option,
        const EuropeanOption& put_option,
        double call_price,
        double put_price
    )
    {
        double left_side = call_price - put_price;
        double right_side = call_option.spot() - call_option.strike() * exp(-call_option.risk_free_rate() * call_option.maturity());

        return abs(left_side-right_side) < 1e-6; 
    }

    double BlackScholes::calculate_delta(const EuropeanOption& option){
        double d1 = calculate_d1(option);

        if (option.type() == OptionType::Call) {
            return calculate_N(d1);
        }
        else{
            return calculate_N(d1) -1;
        }
    }

    double BlackScholes::calculate_gamma(const EuropeanOption& option){
        double d1 = calculate_d1(option);
        double N_prime_d1 = calculate_normal_distribution(d1);

        return N_prime_d1 / (option.spot() * option.volatility() * sqrt(option.maturity()));
    }

    double BlackScholes::calculate_vega(const EuropeanOption& option){
        double d1 = calculate_d1(option);
        double N_prime_d1 = calculate_normal_distribution(d1);

        return option.spot() * N_prime_d1 * sqrt(option.maturity());
    }

    double BlackScholes::calculate_theta(const EuropeanOption& option){
        double S = option.spot();
        double K = option.strike();
        double r = option.risk_free_rate();
        double T = option.maturity();
        double Sigma = option.volatility();
        double d1 = calculate_d1(option);
        double d2 = calculate_d2(option,d1);
        double N_prime_d1 = calculate_normal_distribution(d1);
        double N_d2 = calculate_N(d2);
        double N_minus_d2 = calculate_N(-d2);

        if(option.type() == OptionType::Call){
            return -(S * N_prime_d1 * Sigma)/(2*sqrt(T)) - r * K * exp(-r * T) * N_d2;
        }
        else{
            return -(S * N_prime_d1 * Sigma)/(2*sqrt(T)) + r * K * exp(-r * T) * N_minus_d2;
        }
        
    }

    double BlackScholes::calculate_rho(const EuropeanOption& option){
        double K = option.strike();
        double r = option.risk_free_rate();
        double T = option.maturity();
        double d1 = calculate_d1(option);
        double d2 = calculate_d2(option, d1);
        double N_d2 = calculate_N(d2);
        double N_minus_d2 = calculate_N(-d2);

        if(option.type() == OptionType::Call){
            return K * T * exp(-r*T) * N_d2;
        }
        else{
            return -K * T * exp(-r*T) * N_minus_d2;
        }
    }

    double BlackScholes::calculate_implied_volatility(const EuropeanOption& option, double market_price){
        
        if(market_price <= 0.0){
            return -1.0;
        }

        double sigma_low = 0.0001;
        double sigma_high = 5.0;

        int max_iterations = 100;
        double tolerance = 1e-8;

        if(market_price <= 0.0){
            return -1.0;
        }

        EuropeanOption low_option(
            option.type(),
            option.spot(),
            option.strike(),
            option.maturity(),
            option.risk_free_rate(),
            sigma_low
        );

        EuropeanOption high_option(
            option.type(),
            option.spot(),
            option.strike(),
            option.maturity(),
            option.risk_free_rate(),
            sigma_high
        );

        double f_low = BlackScholes::calculate_option_price(low_option) - market_price;

        double f_high = BlackScholes::calculate_option_price(high_option) - market_price;

        if(f_low * f_high > 0.0){
            return -1.0;
        }

        for (int i = 0; i < max_iterations; i++)
        {
            double sigma_mid = 0.5 * (sigma_low + sigma_high);

            EuropeanOption candidate(
                option.type(),
                option.spot(),
                option.strike(),
                option.maturity(),
                option.risk_free_rate(),
                sigma_mid
            );

            double model_price = BlackScholes::calculate_option_price(candidate);

            double f_mid = model_price - market_price;

            double error = model_price - market_price;

            if (fabs(error) < tolerance)
            {
                return sigma_mid;
            }

            if (f_low * f_mid < 0)
            {
                sigma_high = sigma_mid;
                f_high = f_mid;
            }
            else
            {
                sigma_low = sigma_mid;
                f_low = f_mid;
            }
        }

        return 0.5 * (sigma_low + sigma_high);
    }
}