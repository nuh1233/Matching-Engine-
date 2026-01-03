#pragma once
#include <iostream>

// Stores order details

class Order {
public:
	
	// seperate buy and sell in seperate containers 
	enum class Side { Buy, Sell };

	Side side;

	int order_id;

	int quantity;
};

