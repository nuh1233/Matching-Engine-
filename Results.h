#pragma once
#include <vector>

//Seperate header allows for cleaner integration and later implementing more returns
//Legacy code used bools as a return type, more verbose returns are needed to secure invariants

	struct ExecutionResults {
		bool executed;
		int price;
		int quantity;
		int buy_order_id;
		int sell_order_id;
	};

	// This will be the audit tape stored in a vector and is read only
	class TradeTape {
	
	// vector data will be stored here
	private:
		
		std::vector<ExecutionResults> tape;
	
	public:

		//adds data to tape
		void record(const ExecutionResults& result) {
			
			tape.push_back(result);

		}
		//read only function externally to get all data
		const std::vector<ExecutionResults>& getAllRecords() const {

			return tape;

		}
		// get size of data
		std::size_t size() const {

			return tape.size();
		}

		//Clear data from tape
		void clear() {
			
			tape.clear();
		}
		
	};

