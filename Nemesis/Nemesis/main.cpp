#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <random>
#include <fstream>
#include "Agent/NeuralNetwork/MultiLayerPerceptron.hpp"
#include "Agent/TDLearner.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace std;
std::mt19937 randng(std::time(0));
std::uniform_real_distribution<float> dist(-M_PI, M_PI);

int main() {
	auto func = [](float x) { return sin(x); };
	TDLearner<MLP<float, float, float>> agent(MLP<float, float, float> (1, 1));
 //   agent.q_estimator->append_layer(Layer<float>(6, TanhActivation<float>()));
 //   agent.q_estimator->append_layer(Layer<float>(6, TanhActivation<float>()));
 //   agent.q_estimator->append_layer(Layer<float>(1, LinearActivation<float>()));
	//std::printf("uninitialized:\n");
	//agent.q_estimator->print_weights();
 //   agent.q_estimator->finalize();
	//std::printf("initialized:\n");
	////agent.q_estimator->print_weights();
	agent.q_estimator->load("test_net_1.json");
	std::cout << agent.q_estimator->as_json() << std::endl;
    agent.q_estimator->learning_rate = .4;
	agent.q_estimator->learning_decay = 1.0/10000;
	auto training_iter = 10000;
	auto training_div = training_iter / 100;

    for (int i = 0; i < training_iter; ++i) {
        float rand_value = dist(randng);
        float actual_ans = func(rand_value);
        auto a = std::vector<float>({rand_value});
        auto b = std::vector<float>({actual_ans});
        agent.update_values(a, b, {0});
		if (i % training_div == 0) {
			std::printf("training %.2f complete\n", i * 100 / ((float)training_iter));
		}
    }

    const int num_questions = 16;
    std::vector<float> test_answers(num_questions);
    std::vector<float> test_responses(num_questions);
    for (int i = 0; i < num_questions; ++i) {
        float rand_value = dist(randng);
        float actual_ans = func(rand_value);
        auto response = agent.get_value({rand_value})[0];
        std::printf("-----------------\nquestion: sin(%f) = ?,\n actual ans: %f response: %f\n",
                    rand_value, actual_ans, response);
    }

	agent.q_estimator->save("test_net_1.json");
	std::cin.get();
    return 0;
}