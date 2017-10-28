#pragma once
#include <mutex>
#include <memory>
typedef std::unique_ptr<std::mutex> mutex_ptr;
typedef std::lock_guard<std::mutex> mutex_guard;

// an action callback request raw state so that it can be preprocessed and fed to a learner, a response is expected in the form of a string
typedef std::function<std::string(std::string)> action_callback_t;

// an observation requuest raw state and raw results for a turn so that the observation may be documented
typedef std::function<void(std::string, std::string)> observation_callback_t;