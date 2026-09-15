#pragma once

namespace quant {

    enum class OptionType {
        Call, //droit d acheter
        Put //droit de vendre
    };

    class EuropeanOption {
        public:
        EuropeanOption(
            OptionType type, 
            double spot, 
            double strike, 
            double maturity, 
            double risk_free_rate,
            double volatility 
        );

        OptionType type() const;
        double spot() const;
        double strike() const;
        double maturity() const;
        double risk_free_rate() const;
        double volatility() const;

        private:
            OptionType type_;
            double spot_;
            double strike_;
            double maturity_;
            double risk_free_rate_;
            double volatility_;
    };

}