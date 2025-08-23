#pragma once
#include <Event/Connection.h>

#include <functional>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <atomic>
#include <vector>
#include <utility>

template<typename ...Args>
class Event {
public:
	Event() : m_impl(std::make_shared<Impl>()) {}
	using Handler = std::function<void(Args...)>;

	EventConnection connect(Handler h) {
		const uint64_t id = m_impl->nextId.fetch_add(1, std::memory_order_relaxed);
		{
			std::lock_guard<std::mutex> lk(m_impl->mutex);
			m_impl->handlers.emplace(id, std::move(h));
		}
		std::weak_ptr<Impl> implWeak = m_impl;
		auto disconnectFn = [implWeak, id]() {
			if (auto impl = implWeak.lock()) {
				std::lock_guard<std::mutex> lk(impl->mutex);
				impl->handlers.erase(id);
			}
			};

		return EventConnection(std::move(disconnectFn));
	}

	void fire(Args... args) {
		std::vector<Handler> toCall;
		{
			std::lock_guard<std::mutex> lk(m_impl->mutex);
			toCall.reserve(m_impl->handlers.size());
			for (auto& kv : m_impl->handlers) {
				toCall.push_back(kv.second);
			}
		}

		for (auto& fn : toCall) {
			try {
				fn(args...);
			}
			catch (...) {}
		}
	}

private:
	struct Impl {
		std::mutex mutex;
		std::unordered_map<uint64_t, Handler> handlers;
		std::atomic<uint64_t> nextId{ 1 };
	};

	std::shared_ptr<Impl> m_impl;
};