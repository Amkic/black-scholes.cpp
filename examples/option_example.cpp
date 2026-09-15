#include "instruments/european_options.hpp"
#include "models/black_scholes.hpp"
#include <iostream>

using namespace quant;
using namespace std;

int main()
{
    double tolerance = 1e-6;
    double expected1 = 10.4506;
    EuropeanOption option1 (OptionType::Call, 100.0, 100.0, 1.0, 0.05, 0.2);
    EuropeanOption option2 (OptionType::Put, 100.0, 100.0, 1.0, 0.05, 0.2);


    double price_call = BlackScholes::calculate_option_price(option1);
    double price_put = BlackScholes::calculate_option_price(option2);
    bool parity_valid = BlackScholes::is_put_call_parity_valid(option1, option2, price_call, price_put);

    cout <<"Black-Scholes price call : " << price_call <<endl;
    cout <<"Black-Scholes price put : " << price_put <<endl;
    cout <<"parity valid : " << parity_valid << endl;

    return 0;
}