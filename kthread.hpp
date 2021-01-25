// KT header-only library
// Requirements: C++17

#pragma once
#include <thread>
#include <type_traits>

namespace kt {
///
/// \brief std::thread wrapper that joins on destruction / move
///
class kthread {
	template <typename T>
	static constexpr bool guard = !std::is_convertible_v<std::decay_t<T>, kthread> && !std::is_same_v<std::decay_t<T>, std::thread>;

  public:
	///
	/// \brief Yield execution of the calling thread
	///
	static void yield();
	///
	/// \brief Sleep calling thread for a specific duration (approximate)
	///
	template <typename Dur>
	static void sleep_for(Dur&& duration);

	kthread() = default;
	template <typename F, typename... Args, typename = std::enable_if_t<guard<F>>>
	explicit kthread(F&& func, Args&&... args);
	kthread(kthread&&) = default;
	kthread& operator=(kthread&&) noexcept;
	virtual ~kthread();

	///
	/// \brief Join the thread wrapped in this instance, blocking the calling thread
	///
	bool join();
	///
	/// \brief Swap this instance with rhs
	///
	void swap(kthread& rhs) noexcept;

  private:
	std::thread m_thread;
};

// impl

inline void kthread::yield() {
	std::this_thread::yield();
}
template <typename Dur>
void kthread::sleep_for(Dur&& duration) {
	std::this_thread::sleep_for(duration);
}
template <typename F, typename... Args, typename>
inline kthread::kthread(F&& func, Args&&... args) : m_thread(std::forward<F>(func), std::forward<Args>(args)...) {
}
inline kthread& kthread::operator=(kthread&& rhs) noexcept {
	if (&rhs != this) {
		join();
		m_thread = std::move(rhs.m_thread);
	}
	return *this;
}
inline kthread::~kthread() {
	join();
}
inline void kthread::swap(kthread& rhs) noexcept {
	if (&rhs != this) {
		std::swap(m_thread, rhs.m_thread);
	}
}
inline bool kthread::join() {
	if (m_thread.joinable()) {
		m_thread.join();
		m_thread = {};
		return true;
	}
	return false;
}
} // namespace kt
