/**
 * @file include/channel.h
 * 
 * @brief A simple multi-producer, multi-consumer channel implementation.
 * 
 * @section Overview
 * 
 * This channel provides thread-safe communication between multiple producers and consumers.
 * It supports blocking push and pop operations, and can be closed to signal no more data will be sent.
 * The goal to develop my own variation based on Professors.
 */

 #pragma once

 #include <queue>
 #include <mutex>
 #include <condition_variable>

/**
 * @class Channel
 * 
 * @brief A thread-safe multi-producer, multi-consumer channel.
 * 
 * @tparam T The type of elements stored in the channel.
 * 
 * @return A Channel class that allows multiple threads to push and pop elements safely.
 * 
 * @details This class uses a std::queue to store elements, protected by a mutex and condition variable.
 * It supports multiple producers and consumers, and allows the channel to be closed to signal no more data.
 */
template <typename T>
class Channel {
    public: 
        Channel(): closed_(false) {}

        // Disable copying
        Channel(const Channel&) = delete;
        Channel& operator =(const Channel&) = delete;

        // Push a value into the channel.
        // Returns false if the channel is closed; true otherwise
        bool push(const T& value) {
            std::unique_lock<std::mutex> lock(mtx_);
            if (closed_) {
                return false;
            }
            queue_.push(value);
            cv_not_empty.notify_one();
            return true;
        }

        bool push(T&& value) {
            std::unique_lock<std::mutex> lock(mtx_);
            if(closed_) {
                return false;
            }
            queue_.push(std::move(value));
            cv_not_empty.notify_one();
            return true;
        }

        // Pop a value from the channel.
        // Returns false when teh channel is closed and empty
        bool pop(T& out) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_not_empty.wait(lock, [&] {
                return closed_ || !queue_.empty();
            });

            if (queue_.empty()) {
                // closed_ must be true here
                return false;
            }

            out = std::move(queue_.front());
            queue_.pop();
            return true;
        }

        // Close the channel. After this:
        // - all waiting pops wake up
        // - future pushes fail (return false)
        void close() {
            std::lock_guard<std::mutex> lock(mtx_);
            closed_ = true;
            cv_not_empty.notify_all();
        }
    
    private:
        std::queue<T> queue_;
        bool closed_;
        std::mutex mtx_;
        std::condition_variable cv_not_empty;
};