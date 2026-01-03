#pragma once
#include "OrderBook.h"
#include <algorithm>
#include "Order.h"
#include "Results.h"
class Matcher {
public:
	//++++++++++++++++++++++++++++++This is the actual matching engine+++++++++++++++++++++++++++++++++++
	
	//=============================Single atomic execution begins=======================================
	
	//The gate for ordering, if orders cant be matched there is stoppage
	bool orderSelection(const OrderBook& book) const{
		
		//if a match isnt possible dont do anything
		if (!book.tryMatchOnce()) {

			return false;
		}
		else {
			
			//these are the actual values not references
			int bestBidPrice = book.bestBid();
			int bestAskPrice = book.bestAsk();
			
			//.at can be optimized in the future with [] since the bounds are checked already by tryMatchOnce()
			//find the pricelevel(it is a key(price) value(PriceLevel) pair
			const PriceLevel& buyLevel = book.bids.at(bestBidPrice);
			const PriceLevel& sellLevel = book.asks.at(bestAskPrice);

			//Get the order that is up next
			const Order& buyOrder = buyLevel.orders.front();
			const Order& sellOrder = sellLevel.orders.front();


			return true;
		}
	}
	//{Development code used bool, now transitioning to more verbose returns}
	//Executes only one trade 
	ExecutionResults executeOnce(OrderBook& book) const {

		//The gate, checks if a match is even possible and returns false if not
		if (!book.tryMatchOnce()) {

			return ExecutionResults{false,0,0,-1,-1};
		}

		int bestBidPrice = book.bestBid();
		int bestAskPrice = book.bestAsk();
		
		//.at can be optimized in the future with [] since the bounds are checked already by tryMatchOnce()
		PriceLevel& buyRef = book.bids.at(bestBidPrice);
		PriceLevel & sellRef = book.asks.at(bestAskPrice);

		//Get the order that is up next
		Order& buy = buyRef.orders.front();
		Order& sell = sellRef.orders.front();
		
		int buyId = buy.order_id;
		int sellId = sell.order_id;


		//Get Order Quantity
		int buyQuantity = buy.quantity;
		int sellQuantity = sell.quantity;

		//-------------------------Execution happens here-------------------------------
		//The execution quantity gets lowest values
		int executionQuantity = std::min(buyQuantity, sellQuantity);

		//temp value reduces order quantity by the execution quantity
		int buyEx = (buyQuantity - executionQuantity);
		int sellEx = (sellQuantity - executionQuantity);

		//After arithmetic, you update the actual values by reference
		buy.quantity = buyEx;
		sell.quantity = sellEx;
		//------------------------finalized here----------------------------------------

		//------------------------Order level clean up begins here----------------------------------
		// clean up step for removing orders that reach 0
		if (buy.quantity == 0) {
			
			//pops the order next in FIFO but doesnt remove pricelevel
			buyRef.orders.pop_front();
		}
		if (sell.quantity == 0) {

			//pops the order next in FIFO but doesnt remove pricelevel
			sellRef.orders.pop_front();

		}
		//-------------------------Order level Clean up ends here-------------------------------------


		//-------------------------Pricelevel clean up begins here------------------------------------
		//checks if pricelevel deque is empty and if so removes it
		if (buyRef.orders.empty()) {
			
			//removes the pricelevel explicitly
			book.bids.erase(bestBidPrice);
		}
		if (sellRef.orders.empty()) {

			//removes the pricelevel explicitly
			book.asks.erase(bestAskPrice);
		}
		//-------------------------PriceLevel clean up ends here-------------------------------------
		
		ExecutionResults result;
		result.executed = true;
		result.price = bestAskPrice;
		result.quantity = executionQuantity;
		result.buy_order_id = buyId;
		result.sell_order_id = sellId;

		return result;
	}
	//=============================Single atomic execution end=======================================

	//=============================Looping execution logic start=====================================

	int tradesExecuted(OrderBook& book,TradeTape& tape) {

		//Counts number of trades executed
		int numTrades = 0;
		
		//loop will run until there is a match possible calling executeOnce
		while (true) {
			
			
			//"book" ensures theres no hidden state and executeOnce and tradesExecuted operate on same book
			//loop will run until there is a match possible calling executeOnce
			ExecutionResults r = executeOnce(book);
			
			// stopping condition if executed once returns false
			if (!r.executed) 
				
				break;
			tape.record(r);
			//Increments per trade
			numTrades++;

		}

		return numTrades;
	}
	//=====================================Looping logic ends==================================================
};

