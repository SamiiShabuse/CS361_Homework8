/**
 * TO give a generic channel with push pop and close
 * using a mutex and cv 
 * 
 * will cover thread safe queue, producer/consuemr,
 * and CSP style message passing
 */

 #pragma once

 #include <queue>
 #include <mutex>
 #include <condition_variable>

 /**
  * A Simple multi-producer, multi-consumer channel.
  * A push(): blocks only to add capacity later
  * pop(): blocks until there is a data or the chanell is closed
  * close(): wakes all waiting ops; further pusheses are rejected
  */
template <typename T>
class Channel {
    public: 
        Channel(): closed_(false) {}

        // Disable copying
        Channel(const Channel&) = delete;
        Channel& operator =(const Channel&) = delete;

        // Push a value into the channel.
        // Returns false if the channel is closd; true otherwise.
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
        // Returns false when teh channel is closed and empty.
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