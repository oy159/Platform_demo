#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <future>
#include <queue>
#include <functional>

class ThreadPool {
public:
	explicit ThreadPool(size_t threads);

	~ThreadPool();

	template<class F, class... Args>
	auto enqueue(F &&f, Args &&... args)
		-> std::future<std::invoke_result_t<F, Args...> > {
		using return_type = std::invoke_result_t<F, Args...>;

		auto task = std::make_shared<std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future(); {
			std::unique_lock lock(queue_mutex);

			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks.emplace([task]() { (*task)(); });
		}
		condition.notify_one();
		return res;
	}

	template<class F, class O, class... Args>
	auto enqueue(F &&f, O *o, Args &&... args)
		-> std::future<std::invoke_result_t<F, O, Args...> > {
		using return_type = std::invoke_result_t<F, O, Args...>;

		auto task = std::make_shared<std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), o, std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future(); {
			std::unique_lock lock(queue_mutex);

			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks.emplace([task]() { (*task)(); });
		}
		condition.notify_one();
		return res;
	}

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()> > tasks;

	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};

#endif //THREADPOOL_H
