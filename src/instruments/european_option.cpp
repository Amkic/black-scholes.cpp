#include "instruments/european_options.hpp"

namespace quant {

    EuropeanOption::EuropeanOption(
        OptionType eo_type,
        double eo_spot,
        double eo_strike,
        double eo_maturity,
        double eo_risk_free_rate,
        double eo_volatility
    )
        : type_(eo_type),
          spot_(eo_spot),
          strike_(eo_strike),
          maturity_(eo_maturity),
          risk_free_rate_(eo_risk_free_rate),
          volatility_(eo_volatility)
    {
    }

    OptionType EuropeanOption::type() const
    {
        return type_;
    }

    double EuropeanOption::spot() const
    {
        return spot_;
    }

    double EuropeanOption::strike() const
    {
        return strike_;
    }

    double EuropeanOption::maturity() const
    {
        return maturity_;
    }

    double EuropeanOption::risk_free_rate() const
    {
        return risk_free_rate_;
    }

    double EuropeanOption::volatility() const
    {
        return volatility_;
    }

}