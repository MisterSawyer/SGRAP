#pragma once
#include <list>
#include <unordered_map>
#include <memory>
#include <type_traits>
#include <iostream>

#include "sgrap_node.h"

namespace sgrap {

	template<class T>
	class Graph
	{
	protected:
		using  NodeType = Node<T>;
	public:
		virtual ~Graph() = default;

		//tests whether there is an edge from the vertex x to the vertex y
		virtual bool adjacent(NodeType* const x, NodeType* const y)const = 0;
		//lists all vertices y such that there is an edge from the vertex x to the vertex y
		virtual std::list<NodeType*> neighbors(NodeType* const x)const = 0;
		//adds the vertex x
		virtual NodeType* addVertex(const NodeType& x);
		virtual NodeType* addVertex(NodeType&& x);

		// adds the vertex x, if it is not there
		virtual bool removeVertex(NodeType* const x);
		// adds the edge from the vertex x to the vertex y, if it is not there
		virtual bool addEdge(NodeType* const x, NodeType* const y) = 0;
		// removes the edge from the vertex x to the vertex y, if it is there
		virtual bool removeEdge(NodeType* const x, NodeType* const y) = 0;
	protected:
		std::pair<bool, typename std::list<std::unique_ptr<typename NodeType>>::const_iterator> exists(NodeType* const x) const;

		virtual bool removeAllconnectionsFrom(NodeType* const x) = 0;
		virtual void removeAllconnectionsTo(NodeType* const x) = 0;

	private:
		std::list<std::unique_ptr<NodeType>> nodes_list;
	};

	class GraphRepresentation
	{
		// potrzebne do checkowania static assertu
	};

	template <class T>
	class AdjacencyList : public GraphRepresentation, public virtual Graph<T>
	{
	public:
		virtual ~AdjacencyList() = default;

		//tests whether there is an edge from the vertex x to the vertex y
		virtual bool adjacent(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y)const override final;
		//lists all vertices y such that there is an edge from the vertex x to the vertex y
		virtual std::list<typename Graph<T>::NodeType*> neighbors(typename Graph<T>::NodeType* const x)const override final;
		//adds the vertex x
		virtual typename Graph<T>::NodeType* addVertex(const typename Graph<T>::NodeType& x) override final;
		virtual typename Graph<T>::NodeType* addVertex(typename Graph<T>::NodeType&& x) override final;
		// adds the vertex x, if it is not there
		virtual bool removeVertex(typename Graph<T>::NodeType* const x) override final;
		// adds the edge from the vertex x to the vertex y, if it is not there
		virtual bool addEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y) override;
		// removes the edge from the vertex x to the vertex y, if it is there
		virtual bool removeEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y) override;

		virtual bool removeAllconnectionsFrom(typename Graph<T>::NodeType* const x) override final;
		virtual void removeAllconnectionsTo(typename Graph<T>::NodeType* const x) override final;
	private:
		std::unordered_map<typename Graph<T>::NodeType*, std::list<typename Graph<T>::NodeType*>, NodeHashFuntion> adjacency_list;
	};

	template <class T, template<typename> typename Representation>
	class DirectedGraph : public Representation<T>
	{
	public:
		DirectedGraph()
		{
			static_assert(std::is_base_of<GraphRepresentation, Representation<T>>::value, "Representation does not inherit from GraphRepresentation");
		}
		virtual ~DirectedGraph() = default;

		virtual bool addEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y) override final;
		virtual bool removeEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y) override final;
	private:
	};

	template <class T, template<typename> typename Representation>
	class UndirectedGraph : public Representation<T>
	{
	public:
		UndirectedGraph()
		{
			static_assert(std::is_base_of<GraphRepresentation, Representation<T>>::value, "Representation does not inherit from GraphRepresentation");
		}
		virtual ~UndirectedGraph() = default;

		virtual bool addEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y) override final;
		virtual bool removeEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y) override final;
	private:
	};

	/* ======================================================
	* ================== GRAPH FUNCTIONS ==================
	  ======================================================*/

	template<class T>
	inline typename Graph<T>::NodeType* Graph<T>::addVertex(const NodeType& x)
	{
		auto new_node = std::make_unique<NodeType>(x);
		NodeType* ptr = new_node.get();
		nodes_list.push_back(std::move(new_node));

		return ptr;
	}

	template<class T>
	inline typename Graph<T>::NodeType* Graph<T>::addVertex(NodeType&& x)
	{
		auto new_node = std::make_unique<NodeType>(std::move(x));
		NodeType* ptr = new_node.get();
		nodes_list.push_back(std::move(new_node));

		return ptr;
	}

	template<class T>
	inline std::pair<bool, typename std::list<std::unique_ptr<typename Graph<T>::NodeType>>::const_iterator> Graph<T>::exists(NodeType* const x) const
	{

		auto it = std::find_if(std::begin(nodes_list), std::end(nodes_list),
			[x](const std::unique_ptr<NodeType>& member_node) {
				return x == member_node.get();
			});

		if (it == nodes_list.end())return std::make_pair(false, it);

		return std::make_pair(true, it);
	}

	template<class T>
	inline bool Graph<T>::removeVertex(NodeType* const x)
	{
		auto existance = exists(x);

		if (!existance.first)return false;

		nodes_list.erase(existance.second);

		return true;
	}

	/* ======================================================
	* =============== ADJECENCYLIST FUNCTIONS ===============
	  ======================================================*/

	template<class T>
	inline bool AdjacencyList<T>::adjacent(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y) const
	{
		auto list_x_it = adjacency_list.find(x);
		if (list_x_it == adjacency_list.end())return false;

		auto list_y_it = adjacency_list.find(y);
		if (list_y_it == adjacency_list.end())return false;

		return 	(std::find(list_x_it->second.begin(), list_x_it->second.end(), y) != list_x_it->second.end())
			|| (std::find(list_y_it->second.begin(), list_y_it->second.end(), x) != list_y_it->second.end());
	}

	template<class T>
	inline std::list<typename Graph<T>::NodeType*> AdjacencyList<T>::neighbors(typename Graph<T>::NodeType* const x) const
	{
		auto list_x_it = adjacency_list.find(x);
		if (list_x_it == adjacency_list.end())return std::list<typename Graph<T>::NodeType*>();

		return list_x_it->second;
	}

	template<class T>
	inline typename Graph<T>::NodeType* AdjacencyList<T>::addVertex(const typename Graph<T>::NodeType& x)
	{
		// when element already exists insert won't modify its mapped value
		auto new_node = Graph<T>::addVertex(x);
		typename Graph<T>::NodeType* ptr = new_node;
		adjacency_list.insert(std::make_pair(std::move(new_node), std::list<typename Graph<T>::NodeType*>()));
		return ptr;
	}

	template<class T>
	inline typename Graph<T>::NodeType* AdjacencyList<T>::addVertex(typename Graph<T>::NodeType&& x)
	{
		//adjacency_list.insert(std::make_pair(std::move(x), std::list<typename Graph<T>::NodeType*>()));
		auto new_node = Graph<T>::addVertex(std::move(x));
		typename Graph<T>::NodeType* ptr = new_node;
		adjacency_list.insert(std::make_pair(std::move(new_node), std::list<typename Graph<T>::NodeType*>()));
		return ptr;
	}

	template<class T>
	inline bool AdjacencyList<T>::removeVertex(typename Graph<T>::NodeType* const x)
	{
		//Removing vertex from memory
		Graph<T>::removeVertex(x);

		//Removing vertex from graph structure
		auto list_x_it = adjacency_list.find(x);
		if (list_x_it == adjacency_list.end())return false;

		adjacency_list.erase(list_x_it);
		return true;
	}

	template<class T>
	inline bool AdjacencyList<T>::addEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y)
	{
		auto list_x_it = adjacency_list.find(x);
		if (list_x_it == adjacency_list.end())return false;

		auto y_it = std::find(list_x_it->second.begin(), list_x_it->second.end(), y);
		// if connection already exists -> return false
		if (y_it != list_x_it->second.end())return false;

		//add new connection
		list_x_it->second.push_back(y);

		return true;
	}

	template<class T>
	inline bool AdjacencyList<T>::removeEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y)
	{
		auto list_x_it = adjacency_list.find(x);
		if (list_x_it == adjacency_list.end())return false;

		auto y_it = std::find(list_x_it->second.begin(), list_x_it->second.end(), y);
		// if there is no connection -> return false
		if (y_it == list_x_it->second.end())return false;

		list_x_it->second.erase(y_it);

		return true;
	}

	template<class T>
	inline bool AdjacencyList<T>::removeAllconnectionsFrom(typename Graph<T>::NodeType* const x)
	{
		auto list_x_it = adjacency_list.find(x);
		if (list_x_it == adjacency_list.end())return false;

		adjacency_list.erase(list_x_it);
		return true;
	}

	template<class T>
	inline void AdjacencyList<T>::removeAllconnectionsTo(typename Graph<T>::NodeType* const x)
	{
		for (auto& v : adjacency_list)
		{
			auto it = std::remove_if(v.second.begin(),
				v.second.end(),
				[x](typename Graph<T>::NodeType* connection) {return connection == x; });
		}
	}

	/* ======================================================
	* =============== DIRECTED GRAPH FUNCTIONS ===============
	  ======================================================*/
	template <class T, template<typename> typename Representation>
	inline bool DirectedGraph<T, Representation>::addEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y)
	{
		return Representation<T>::addEdge(x, y);
	}

	template <class T, template<typename> typename Representation>
	inline bool DirectedGraph<T, Representation>::removeEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y)
	{
		return Representation<T>::removeEdge(x, y);
	}

	/* ======================================================
	* =============== UNDIRECTED GRAPH FUNCTIONS ===============
	  ======================================================*/
	template <class T, template<typename> typename Representation>
	inline bool UndirectedGraph<T, Representation>::addEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y)
	{
		return 	Representation<T>::addEdge(x, y) && Representation<T>::addEdge(y, x);
	}

	template <class T, template<typename> typename Representation>
	inline bool UndirectedGraph<T, Representation>::removeEdge(typename Graph<T>::NodeType* const x, typename Graph<T>::NodeType* const y)
	{
		return Representation<T>::removeEdge(x, y) && Representation<T>::removeEdge(y, x);
	}
}