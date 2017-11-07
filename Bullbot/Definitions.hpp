#pragma once
#include <mutex>
#include <memory>
typedef std::unique_ptr<std::mutex> mutex_ptr;
typedef std::lock_guard<std::mutex> mutex_guard;

typedef std::array<std::string, 9> action_arr_t;
// float vector type
typedef std::vector<float> fvec_t;

// an action callback request raw state so that it can be preprocessed and fed to a learner, a response is expected in the form of a string
typedef std::function<int(fvec_t, std::array<std::string, 9>)> action_callback_t;

// an observation requuest raw state and raw results for a turn so that the observation may be documented
// this callback allows data to be trained on and offline
typedef std::function<void(std::vector<float>, std::vector<float>)> observation_callback_t;