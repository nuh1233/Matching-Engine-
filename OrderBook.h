#pragma once
#include <iostream>
#include "PriceLevel.h"
#include <map>
#include "Order.h"
#include <stdexcept> 
class OrderBook {
public:

	// Ordered maps corresponding to buy sell order
	std::map<int, PriceLevel> bids;
	std::map<int, PriceLevel> asks;
	
	// Write path that checks if order exits, if it does it is added to price level or otherwise created
	bool addOrder(int price, const Order& orderData) {

		// Ternary if/else statement, True condition is if its a Buy, else if its sell. compact if else statement
		std::map<int, PriceLevel>& bookSide =
			(orderData.side == Order::Side::Buy) ? this->bids : this->asks;

		// if ask/bid was not found it is created in this
		if (bookSide.find(price) == bookSide.end()) {

			PriceLevel level;

			level.price = price;

			level.addOrder(orderData);

			bookSide[price] = level;

			return true;

		}
		// if it is found then this adds it to the Bookside Hashmap
		else {

			bookSide[price].addOrder(orderData);

			return false;
		}

	}
		
	// Read only and returns highest bid price
	 int bestBid() const{
		
		// checks if bids map is empty and if not returns best bid (const ensures read only)
		if(bids.empty()){
			
			throw std::runtime_error("there are no bid orders");
		}
		else {

			//rbegin is safer than end - 1 incase of dereferncing null
			return bids.rbegin()->first;
		}
	}
	
	 // Read only and returns highest ask price( same logic as best bid) const ensures read only
	 int bestAsk() const{

		// checks if bids map is empty and if not returns best bid
		if (asks.empty()) {
			
			throw std::runtime_error("there are no ask orders");
		}
		
		return asks.begin()->first;

	}

	 //Checks if matching is possible by seeing if the highest price willing to pay is same or greater than lowest price willing to sell
	 bool tryMatchOnce() const {

		 
		 // checks if ask and bid orders exist and if not return false
		 if (asks.empty() || bids.empty()) {
			 
			 return false;
		 
		 }
		 //returns true if matching is possible
		 else if (bestBid() >= bestAsk()) {
			 
			 return true;
		 }
		
		 return false;

	}


};