#pragma once

#include <functional>
#include "../Estimator.hpp"
#include "../Math.hpp"
#include "../Random.hpp"
#include "Activation.hpp"

template<typename WeightType>
struct Neuron {
    std::vector<WeightType> weights;
    WeightType bias;
    Activation<WeightType> activation;
	void random_resize(int num_inputs) {
		weights.resize(num_inputs);
		std::uniform_real_distribution<> dis(-1, 1);
		for (auto& each : weights) {
			each = dis(rng);
		}
		bias = dis(rng);
	}
    std::array<WeightType, 2> activate(std::vector<WeightType> neuron_input) {
        WeightType activation_value = bias + dot(weights, neuron_input);
        return {
                activation.function(activation_value),
                activation.function_derivative(activation_value)
        };
    }

    void update(WeightType learning_rate, WeightType delta, std::vector<WeightType> neuron_input) {
        WeightType change = learning_rate * delta;
        weights = add(weights, multiply(change, neuron_input));
        bias += change;
    }
};

template<typename InputType, typename OutputType>
struct NeuralNetwork : Estimator<InputType, OutputType> {
    virtual std::vector<OutputType> predict(std::vector<InputType> input) = 0;
    virtual double fit(std::vector<InputType> input, std::vector<OutputType> target) = 0;
};