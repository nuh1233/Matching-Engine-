#pragma once
#include "Order.h"
#include <stdexcept>
#include <deque>
#include <iostream>



class PriceLevel {
 public:

	int price;

	// Stores the Order data in FIFO 
	std::deque<Order> orders;

	// Gets next order and if there is none throws an error
	Order getNextOrder() {
		if (orders.empty()) {
			throw std::runtime_error("there are no Orders");
		}
		return orders.front();
	};
	
	// Adds Order to deque in FIFO
	void addOrder(const Order& orderPlaced) {

		orders.push_back(orderPlaced);

	}

};