#pragma once

#include <ctgmath>

template<typename WeightType>
struct Activation {
	std::string type;
	std::function<WeightType(WeightType)> function;

	std::function<WeightType(WeightType value)> function_derivative;
};

template<typename WeightType>
struct LinearActivation : Activation<WeightType> {
    LinearActivation() {
        function = [] (WeightType x){ return x; };
        function_derivative = [] (WeightType x){ return 1.0; };
		type = "linear";
    }
};

template<typename WeightType>
struct TanhActivation : Activation<WeightType> {
    TanhActivation() {
        function = [] (WeightType x){ return tanh(x); };
        function_derivative = [] (WeightType x){ return 1.0 - pow(tanh(x), 2); };
		type = "tanh";
    }
};

template<typename WeightType>
struct LogisticActivation : Activation<WeightType> {
    LogisticActivation() {
        function = [] (WeightType x){ return 1.0 / (1.0 + exp(-x)); };
        function_derivative = [] (WeightType x){
            auto e_x = exp(-x);
            return e_x / pow(1 + e_x, 2); };
		type = "logistic";
    }
};

template<typename WeightType>
struct SoftplusActivation : Activation<WeightType> {
    SoftplusActivation() {
        function = [](WeightType x) { return log(1.0 + exp(-x)); };
        function_derivative = [](WeightType x) { return 1.0 / (1.0 + exp(-x)); };
		type = "softplus";
    }
};

template<typename WeightType>
struct RectifierActivation : Activation<WeightType> {
	RectifierActivation() {
        function = [this](WeightType x) { return max(leaky_parameter * x, x); };
        function_derivative = [this](WeightType x) { return leaky_parameter; };
		type = "rectifier";
    }
    WeightType leaky_parameter = 1;
};