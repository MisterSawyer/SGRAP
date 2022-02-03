#include "sgrap.h"
#include <iostream>

int main()
{
	auto A = sgrap::DirectedGraph<int, sgrap::AdjacencyList>();
	auto B = sgrap::UndirectedGraph<char, sgrap::AdjacencyList>();
	
	A.addVertex(sgrap::Node<int>(12));

	auto v0 = B.addVertex(sgrap::Node<char>('c'));
	auto v1 = B.addVertex(sgrap::Node<char>('d'));
	std::cout << v0->get() <<" "<<v1->get();

	return 0;
}