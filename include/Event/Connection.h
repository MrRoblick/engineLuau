#pragma once

#include <functional>

class EventConnection {
public:
	EventConnection() = default;
	explicit EventConnection(std::function<void()> disconnectFn);
	~EventConnection();

	EventConnection(const EventConnection&) = delete;
	EventConnection& operator=(const EventConnection&) = delete;

	EventConnection(EventConnection&& other) noexcept;
	EventConnection& operator=(EventConnection&& other) noexcept;

	void disconnect();
	bool connected() const;
private:
	std::function<void()> m_disconnect;
};