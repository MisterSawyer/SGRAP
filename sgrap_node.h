#pragma once

#include <mutex>
namespace sgrap {
	template <class T>
	class Node
	{
	public:
		Node() = default;
		Node(const Node& other);
		Node(Node&& other)noexcept;
		Node& operator=(const Node& other);
		Node& operator=(Node&& other)noexcept;

		explicit Node(const T& value);
		explicit Node(T&& value)noexcept;

		virtual ~Node() = default;

		bool operator==(const Node& other);

		const T& get() const;

		void set(const T& value);
		void set(T&& value);

	protected:

	private:
		mutable std::mutex node_mutex;
		T data;
	};

	struct NodeHashFuntion
	{
		template <class T>
		std::size_t operator() (Node<T>* const node) const
		{
			return std::hash<Node<T>*>{}(node);
		}
	};

	template<class T>
	inline Node<T>::Node(const Node& other)
	{
		data = other.data;
	}

	template<class T>
	inline Node<T>::Node(Node&& other) noexcept
	{
		data = std::move_if_noexcept(other.data);
	}

	template<class T>
	inline Node<T>& Node<T>::operator=(const Node& other)
	{
		std::lock_guard<std::mutex> lock(node_mutex);
		if (this == &other)return *this;

		data = other.data;

		return *this;
	}

	template<class T>
	inline Node<T>& Node<T>::operator=(Node&& other) noexcept
	{
		std::lock_guard<std::mutex> lock(node_mutex);
		if (this == &other)return *this;

		data = std::move_if_noexcept(other.data);

		return *this;
	}

	template<class T>
	inline Node<T>::Node(const T& value)
	{
		data = value;
	}

	template<class T>
	inline Node<T>::Node(T&& value) noexcept
	{
		data = std::move_if_noexcept(value);
	}

	template<class T>
	inline bool Node<T>::operator==(const Node& other)
	{
		return this == &other;
	}

	template<class T>
	inline const T& Node<T>::get() const
	{
		std::lock_guard<std::mutex> lock(node_mutex);
		return data;
	}

	template<class T>
	inline void Node<T>::set(const T& value)
	{
		std::lock_guard<std::mutex> lock(node_mutex);
		data = value;
	}

	template<class T>
	inline void Node<T>::set(T&& value)
	{
		std::lock_guard<std::mutex> lock(node_mutex);
		data = std::move(value);
	}
}