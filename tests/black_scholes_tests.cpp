#include "instruments/european_options.hpp"
#include "models/black_scholes.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace quant;
using namespace std;


bool approximately_equal(double actual, double expected, double tolerance){
        return abs(actual -  expected) < tolerance;
    
}

bool put_call_parity(const EuropeanOption& call, double call_price, double put_price){
    double left_side = call_price - put_price;
    double right_side = call.spot() - call.strike() * exp(-call.risk_free_rate()*call.maturity());

    return approximately_equal(left_side, right_side, 1e-6);
}

void DisplayTest(bool test){
    if(test){
        cout<<"Test : PASSED" << endl;
    }
    else{
        cout<<"Test : FAILED" << endl;
    }
}

int main(){
    double tolerance = 1e-4;
    double expected_call = 10.4506;
    double expected_put = 5.573526;

    EuropeanOption call(
        OptionType::Call,
        100.0,
        100.0,
        1.0,
        0.05,
        0.2
    );

    EuropeanOption put(
        OptionType::Put,
        100.0,
        100.0,
        1.0,
        0.05,
        0.2
    );

    EuropeanOption call_10(
    OptionType::Call,
    100.0,
    100.0,
    1.0,
    0.05,
    0.10
    );

    EuropeanOption call_30(
        OptionType::Call,
        100.0,
        100.0,
        1.0,
        0.05,
        0.30
    );

    double price_10 = BlackScholes::calculate_option_price(call_10);

    double price_30 = BlackScholes::calculate_option_price(call_30);

    double implied_volatility_10 = BlackScholes::calculate_implied_volatility(call_10, price_10);

    double implied_volatility_30 = BlackScholes::calculate_implied_volatility(call_30, price_30);

    double call_price = BlackScholes::calculate_option_price(call);
    double put_price = BlackScholes::calculate_option_price(put);

    double market_price = call_price;


    double implied_volatility =  BlackScholes::calculate_implied_volatility(call, market_price);

    bool call_test = approximately_equal(call_price, expected_call, tolerance);
    bool put_test = approximately_equal(put_price, expected_put, tolerance);
    bool parity_test = put_call_parity(call, call_price, put_price);

    double call_delta = BlackScholes::calculate_delta(call);
    double put_delta = BlackScholes::calculate_delta(put);

    double call_gamma = BlackScholes::calculate_gamma(call);
    double put_gamma = BlackScholes::calculate_gamma(put);

    double call_vega = BlackScholes::calculate_vega(call);
    double put_vega = BlackScholes::calculate_vega(put);

    double call_theta = BlackScholes::calculate_theta(call);
    double put_theta = BlackScholes::calculate_theta(put);

    double call_rho = BlackScholes::calculate_rho(call);
    double put_rho = BlackScholes::calculate_rho(put);

    bool gamma_test = approximately_equal(call_gamma, put_gamma, tolerance);
    bool vega_test = approximately_equal(call_vega, put_vega, tolerance);
    bool delta_test = approximately_equal(call_delta - put_delta, 1.0, tolerance);
    bool theta_test = approximately_equal(call_theta - put_theta,
        -call.risk_free_rate() * call.strike() *exp(-call.risk_free_rate() * call.maturity()),
        tolerance
    );
    bool rho_test = approximately_equal(
        call_rho - put_rho,
        call.strike() * call.maturity() *
        exp(-call.risk_free_rate() * call.maturity()),
        tolerance
    );

    bool volatility_test = approximately_equal(implied_volatility, 0.2, 1e-6);
    bool volatility_10_test = approximately_equal(implied_volatility_10, 0.10, 1e-6);
    bool volatility_30_test = approximately_equal(implied_volatility_30, 0.30, 1e-6);


    cout <<"Call price : " << call_price<<endl;
    cout <<"Put price : " << put_price<<endl;

    cout <<"Call delta : " << call_delta<<endl;
    cout <<"Put delta  : " << put_delta<<endl;
    
    cout <<"Call gamma : " << call_gamma<<endl;
    cout <<"Put gamma  : " << put_gamma<<endl;

    cout <<"Call Vega : " << call_vega<<endl;
    cout <<"Put Vega  : " << put_vega<<endl;

    cout <<"Call theta : " << call_theta<<endl;
    cout <<"Put theta  : " << put_theta<<endl;

    cout <<"Call rho : " << call_rho<<endl;
    cout <<"Put rho  : " << put_rho<<endl;

    cout <<"Implied volatility: " << implied_volatility << endl;
    cout << "Implied volatility (10% case): "<< implied_volatility_10 << endl;
    cout << "Implied volatility (30% case): "<< implied_volatility_30 << endl;


    
    cout <<"=================Test==============="<<endl;
    cout <<"Call...:"<<endl;
    DisplayTest(call_test);
    cout <<"Put...:"<<endl;
    DisplayTest(put_test);
    cout <<"Put-call parity..."<<endl;
    DisplayTest(parity_test);
    cout <<"Put-call Gamma parity..."<<endl;
    DisplayTest(gamma_test);
    cout <<"Put-call vega parity..."<<endl;
    DisplayTest(vega_test);
    cout <<"Put-call delta parity..."<<endl;
    DisplayTest(delta_test);
    cout <<"Put-call theta parity..."<<endl;
    DisplayTest(theta_test);
    cout <<"Put-call rho parity..."<<endl;
    DisplayTest(rho_test);
    cout<<"implied volatility ..."<<endl;
    DisplayTest(volatility_test);
    cout << "implied volatility 10%..." << endl;
    DisplayTest(volatility_10_test);
    cout << "implied volatility 30%..." << endl;
    DisplayTest(volatility_30_test);
    cout <<"====================================="<<endl;


    return 0;

}