#pragma once

#include <vector>
#include <array>
#include <ctime>
#include <random>
#include <stdexcept>
#include "NeuralNetwork.hpp"
#include "Activation.hpp"
#include "../../External/json/json.hpp"
#include <fstream>
#include <sstream>
using json = nlohmann::json;

std::uniform_real_distribution<> dis(-1, 1);

template<typename WeightType>
struct Layer : std::vector<Neuron<WeightType>> {
    Layer() {}

    Layer(size_t size, Activation<WeightType> activation) {
        add(size, activation);
    }
	Layer(std::vector<Neuron<WeightType>> layer_data) {
		assign(layer_data.begin(), layer_data.end());
	}
    Layer& add(size_t size, Activation<WeightType> activation) {
        for (int i = 0; i < size; ++i) {
            Neuron<WeightType> a;
            a.activation = activation;
            this->push_back(a);
        }
        return *this;
    }
};

template<typename InputType, typename OutputType, typename WeightType>
struct MLP : NeuralNetwork<InputType, OutputType> {
	typedef std::vector<InputType> InputVectorType;
	typedef std::vector<WeightType> WeightVectorType;
	typedef std::vector<OutputType> OutputVectorType;
    MLP(size_t n_inputs, size_t n_outputs) {
        this->num_inputs = n_inputs;
        this->num_outputs = n_outputs;
    }
    double learning_rate = .3;
	double learning_decay = 0.001;
	unsigned long int learning_epochs = 0;

    std::vector<Layer<WeightType>> layers;

    void append_layer(Layer<WeightType> a) {
        layers.push_back(a);
    }

    void pop_layer() {
        layers.pop_back();
    }

    void finalize(bool randomize_weights = true) {
        if (this->num_outputs != layers[layers.size() - 1].size()) {
            throw std::runtime_error("output size is incorrect for Multilayer Perceptron");
        }
        for (size_t i = 0; i < layers.size(); ++i) {
            for (size_t j = 0; j < layers[i].size(); ++j) {
                if (i == 0) {
                    layers[i][j].random_resize(this->num_inputs);
                } else {
					layers[i][j].random_resize(layers[i - 1].size());
                }
            }
        }
    }

    std::array<std::vector<WeightVectorType>, 2> propagate(InputVectorType input_data, float dropout) {
		if (layers.size() < 1) {
			throw std::exception("There must be at least 1 layer to propagate.");
		}
        //todo: explicitly show assumption that at least one layer exists
        static const int input_layer = 0;
        // create zero'd arrays of the same size
        std::vector<WeightVectorType> output(layers.size()), output_derivatives(layers.size());

        for (int i = 0; i < layers.size(); ++i) {
            output[i].resize(layers[i].size());
            output_derivatives[i].resize(layers[i].size());
        }

        // handle first layer special case where input is vector
        // convert input vector to correct type
		WeightVectorType input_data_weight(input_data.begin(), input_data.end());
        for (int j = 0; j < layers[0].size(); ++j) {
            auto output_output_d = layers[input_layer][j].activate(input_data_weight);
            output[input_layer][j] = output_output_d[0];
            output_derivatives[input_layer][j] = output_output_d[1];
        }
        // handle other cases with dynamic programming, input is the last layer
        for (int i = 1; i < layers.size(); ++i) {
            for (int j = 0; j < layers[i].size(); ++j) {
                auto output_output_d = layers[i][j].activate(output[i - 1]);
                output[i][j] = output_output_d[0];
                output_derivatives[i][j] = output_output_d[1];
            }
        }

        return {output, output_derivatives};
    };

    double backpropagate(InputVectorType sample, OutputVectorType target, float learning_rate) {
        // propagate
        auto out_outd = propagate(sample, 0);
        std::vector<WeightVectorType> outputs = out_outd.at(0);
        std::vector<WeightVectorType> output_derivatives = out_outd.at(1);

        // backpropagate
        const size_t output_layer = layers.size() - 1;
        // create zero array for error, handle first layer separately
        std::vector<WeightVectorType> sample_error(layers.size());

        for (int i = 0; i < output_layer; ++i) {
            sample_error[i].resize(layers[i].size());
        }

        // Do error step for last layer
        auto difference = subtract(target, outputs[outputs.size()-1]);
        sample_error[output_layer] = WeightVectorType (difference.begin(), difference.end());
        for (int i = output_layer - 1; i > -1; --i) {
            auto next_layer = i + 1;
            for (int j = 0; j < layers[i].size(); ++j) {
                std::vector<WeightType> next_layer_weights(layers[next_layer].size());
                for (int k = 0; k < layers[next_layer].size(); ++k) {
                    next_layer_weights[k] = layers[next_layer][k].weights[j];
                }
                sample_error[i][j] = dot(next_layer_weights, sample_error[next_layer]);
            }
        }
        // update
        for (int i = 0; i < layers[0].size(); ++i) {
            WeightType delta = sample_error[0][i] * output_derivatives[0][i];
            layers[0][i].update(learning_rate, delta, sample);
        }
        for (int i = 1; i < layers.size(); ++i) {
            for (int j = 0; j < layers[i].size(); ++j) {
                WeightType delta = sample_error[i][j] * output_derivatives[i][j];
				auto decayed_rate = learning_rate / (1.f + learning_decay * learning_epochs);
                layers[i][j].update(decayed_rate, delta, outputs[i - 1]);
				++learning_epochs;
            }
        }
        //printf("actual: %f , target: %f\n", outputs[outputs.size()-1][0],target[0]);
        return 0;
    }

    OutputVectorType predict(InputVectorType input) {
        return propagate(input, 0)[0][layers.size() - 1];
    };

    double fit(InputVectorType input, OutputVectorType target) {
        return backpropagate(input, target, learning_rate);
    };

	void print_weights() {
		for (auto layer : layers) {
			std::printf("[");
			for (auto neuron : layer) {
				std::printf("{");
				const int last = neuron.weights.size() - 1;
				for (int i = 0; i < neuron.weights.size(); ++i) {
					if (i < last) {
						std::printf("%f,", neuron.weights[i]);
					} else {
						std::printf("%f; ", neuron.weights[i]);
					}
				}
				std::printf("%f}", neuron.bias);
			}
			std::printf("]\n");
		}
	}

	void json_load(json j_file) {
		std::vector<Layer<WeightType>> tmp_layers;
		for (auto j_layer : j_file["layers"]) {
			std::vector<Neuron<WeightType>> layer;
			for (auto j_neuron : j_layer["neurons"]) {
				Neuron<WeightType> n;
				for (auto j_weight : j_neuron["weights"]) {
					n.weights.push_back(j_weight.get<WeightType>());
				}
				n.bias = j_neuron["bias"].get<WeightType>();

				auto j_activation = j_neuron["activation"];
				std::string activation_type = j_activation["type"].get<std::string>();
				if (activation_type == "linear") {
					n.activation = LinearActivation<WeightType>();
				} else if (activation_type == "tanh") {
					n.activation = TanhActivation<WeightType>();
				} else if (activation_type == "logistic") {
					n.activation = LogisticActivation<WeightType>();
				} else if (activation_type == "softplus") {
					n.activation = SoftplusActivation<WeightType>();
				} else if (activation_type == "rectifier") {
					RectifierActivation<WeightType> recti;
					recti.leaky_parameter = j_activation["leaky_parameter"].get<WeightType>();
					n.activation = recti;
				}
				layer.push_back(n);
			}
			tmp_layers.push_back(layer);
		}
	}

	json as_json() {
		json j_file;
		for (auto layer : layers) {
			json j_layer;
			for (auto neuron : layer) {
				json j_neuron;
				j_neuron["weights"] = neuron.weights;
				j_neuron["bias"] = neuron.bias;
				j_neuron["activation"]["type"] = neuron.activation.type;
				j_layer["neurons"].push_back(j_neuron);
			}
			j_file["layers"].push_back(j_layer);
		}
		return j_file;
	}

	void load(const char* filename) {
		std::ifstream ifile(filename);
		if (ifile.good()) {
			json j_file;
			ifile >> j_file;
			std::vector<Layer<WeightType>> tmp_layers;
			for (auto j_layer : j_file["layers"]) {
				std::vector<Neuron<WeightType>> layer;
				for (auto j_neuron : j_layer["neurons"]) {
					Neuron<WeightType> n;
					for (auto j_weight : j_neuron["weights"]) {
						n.weights.push_back(j_weight.get<WeightType>());
					}
					n.bias = j_neuron["bias"].get<WeightType>();

					auto j_activation = j_neuron["activation"];
					std::string activation_type = j_activation["type"].get<std::string>();
					if (activation_type == "linear") {
						n.activation = LinearActivation<WeightType>();
					} else if (activation_type == "tanh") {
						n.activation = TanhActivation<WeightType>();
					} else if (activation_type == "logistic") {
						n.activation = LogisticActivation<WeightType>();
					} else if (activation_type == "softplus") {
						n.activation = SoftplusActivation<WeightType>();
					} else if (activation_type == "rectifier") {
						RectifierActivation<WeightType> recti;
						recti.leaky_parameter = j_activation["leaky_parameter"].get<WeightType>();
						n.activation = recti;
					}
					layer.push_back(n);
				}
				tmp_layers.push_back(layer);
			}
			layers = tmp_layers;
		} else {
			//make better error catching
			std::printf("Error: File \"%s\" does not exist.\n", filename);
		}
	}

	void save(const char* filename) {
		json j_file;
		for (auto layer : layers) {
			json j_layer;
			for (auto neuron : layer) {
				json j_neuron;
				j_neuron["weights"] = neuron.weights;
				j_neuron["bias"] = neuron.bias;
				j_neuron["activation"]["type"] = neuron.activation.type;
				j_layer["neurons"].push_back(j_neuron);
			}
			j_file["layers"].push_back(j_layer);
		}
		std::ofstream ofile(filename);
		ofile << j_file;
		ofile.close();
	}

};