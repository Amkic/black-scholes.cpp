#include "instruments/european_options.hpp"
#include "models/black_scholes.hpp"
#include "models/monte_carlo.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace quant;
using namespace std;

int main(){
    const size_t simulations[] = {1000,10000,100000,1000000};

    EuropeanOption call(
        OptionType::Call,
        100.0,
        100.0,
        1.0,
        0.05,
        0.2
    );

    const double price_bs_call = BlackScholes::calculate_option_price(call);
    const MonteCarloResult price_mc_call = MonteCarlo::calculate_price(call, 100000,42);

    const bool error_result_call = abs(price_mc_call.price-price_bs_call) < 1.96 * price_mc_call.standard_error;

    cout <<"BlackSchole price : " << price_bs_call <<endl;
    cout <<"Monte Carlo Price : " << price_mc_call.price <<endl;
    cout <<"Error result : " << error_result_call<<endl;
    cout <<" standard error  : " << price_mc_call.standard_error<<endl;
    cout <<" absolute error : " << abs(price_mc_call.price-price_bs_call) << endl;

    EuropeanOption put(
        OptionType::Put,
        100.0,
        100.0,
        1.0,
        0.05,
        0.2
    );

    const double price_bs_put = BlackScholes::calculate_option_price(put);
    const MonteCarloResult price_mc_put = MonteCarlo::calculate_price(put, 100000,42);

    const bool error_result_put = abs(price_mc_put.price-price_bs_put) < 1.96 * price_mc_put.standard_error;

    cout <<"BlackSchole price : " << price_bs_put <<endl;
    cout <<"Monte Carlo Price : " << price_mc_put.price <<endl;
    cout <<"Error result : " << error_result_put<<endl;
    cout <<" standard error  : " << price_mc_put.standard_error<<endl;
    cout <<" absolute error : " << abs(price_mc_put.price-price_bs_put) << endl;

    for (size_t N : simulations) { 
        
        const MonteCarloResult result = MonteCarlo::calculate_price(call, N, 42);
        const double scaled_error = result.standard_error * sqrt(static_cast<double>(N));
        
        cout << N << "\t"<< result.price << "\t\t"<< result.standard_error << "\t"<< scaled_error << endl;
    }

    const MonteCarloResult standard = MonteCarlo::calculate_price(call, 100000, 42);

    const MonteCarloResult antithetic = MonteCarlo::calculate_price_antithetic(call, 100000, 42);

    const double variance_reduction = 1.0 - (antithetic.variance / standard.variance);
    const double standard_error_reduction = 1.0 - (antithetic.standard_error / standard.standard_error);


    std::cout << "\nAntithetic Variates\n";
    std::cout << "----------------------------\n";

    std::cout << "Standard MC\n";
    std::cout << "Price          : " << standard.price << '\n';
    std::cout << "Standard Error : " << standard.standard_error << '\n';

    std::cout << "\nAntithetic MC\n";
    std::cout << "Price          : " << antithetic.price << '\n';
    std::cout << "Standard Error : " << antithetic.standard_error << '\n';

    std::cout << "\nVariance reduction : " << 100.0 * variance_reduction << "%\n";

    std::cout << "SE reduction       : " << 100.0 * standard_error_reduction << "%\n";
}