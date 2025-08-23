#include <Event/Connection.h>
#include <print>

EventConnection::EventConnection(std::function<void()> disconnectFn)
	: m_disconnect(std::move(disconnectFn))
{}

bool EventConnection::connected() const {
	return static_cast<bool>(m_disconnect);
}

void EventConnection::disconnect() {
	if (m_disconnect) {
		try {
			m_disconnect();
		}
		catch (...) {};
		m_disconnect = nullptr;
	}
}
EventConnection::~EventConnection() {
	disconnect();
}

EventConnection& EventConnection::operator=(EventConnection&& other) noexcept
{
	if (this != &other) {
		disconnect();
		m_disconnect = std::move(other.m_disconnect);
		other.m_disconnect = nullptr;
	}
	return *this;
}

EventConnection::EventConnection(EventConnection&& other) noexcept
	: m_disconnect(std::move(other.m_disconnect)) {
	other.m_disconnect = nullptr;
}