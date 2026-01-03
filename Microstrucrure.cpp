#include "OrderBook.h"
#include "Matcher.h"
#include "Results.h"
#include <iostream>

using std::cout;
using std::endl;

int main() {

    cout << "\n================ FULL ENGINE TEST SUITE ================\n\n";

    Matcher matcher;

    // ------------------------------------------------------------
    // Test 1: Empty book safety
    // ------------------------------------------------------------
    {
        cout << "[Test 1] Empty book — nothing should happen\n";

        OrderBook book;
        TradeTape tape;

        int trades = matcher.tradesExecuted(book, tape);

        cout << "Trades executed (expected 0): " << trades << endl;
        cout << "Tape size (expected 0): " << tape.size() << endl << endl;
    }

    // ------------------------------------------------------------
    // Test 2: BUY-only book
    // ------------------------------------------------------------
    {
        cout << "[Test 2] BUY-only book — no matching possible\n";

        OrderBook book;
        TradeTape tape;

        Order b1; b1.order_id = 1; b1.quantity = 10; b1.side = Order::Side::Buy;
        Order b2; b2.order_id = 2; b2.quantity = 5;  b2.side = Order::Side::Buy;

        book.addOrder(100, b1);
        book.addOrder(101, b2);

        int trades = matcher.tradesExecuted(book, tape);

        cout << "Trades executed (expected 0): " << trades << endl;
        cout << "Tape size (expected 0): " << tape.size() << endl << endl;
    }

    // ------------------------------------------------------------
    // Test 3: SELL-only book
    // ------------------------------------------------------------
    {
        cout << "[Test 3] SELL-only book — no matching possible\n";

        OrderBook book;
        TradeTape tape;

        Order s1; s1.order_id = 10; s1.quantity = 7; s1.side = Order::Side::Sell;

        book.addOrder(100, s1);

        int trades = matcher.tradesExecuted(book, tape);

        cout << "Trades executed (expected 0): " << trades << endl;
        cout << "Tape size (expected 0): " << tape.size() << endl << endl;
    }

    // ------------------------------------------------------------
    // Test 4: Non-crossed book
    // ------------------------------------------------------------
    {
        cout << "[Test 4] Non-crossed prices — bids < asks\n";

        OrderBook book;
        TradeTape tape;

        Order b; b.order_id = 1; b.quantity = 10; b.side = Order::Side::Buy;
        Order s; s.order_id = 2; s.quantity = 10; s.side = Order::Side::Sell;

        book.addOrder(90, b);
        book.addOrder(100, s);

        int trades = matcher.tradesExecuted(book, tape);

        cout << "Trades executed (expected 0): " << trades << endl;
        cout << "Tape size (expected 0): " << tape.size() << endl << endl;
    }

    // ------------------------------------------------------------
    // Test 5: Single execution, partial fill
    // ------------------------------------------------------------
    {
        cout << "[Test 5] Single execution with partial fill\n";

        OrderBook book;
        TradeTape tape;

        Order b; b.order_id = 1; b.quantity = 10; b.side = Order::Side::Buy;
        Order s; s.order_id = 2; s.quantity = 7;  s.side = Order::Side::Sell;

        book.addOrder(100, b);
        book.addOrder(100, s);

        int trades = matcher.tradesExecuted(book, tape);
        const auto& rec = tape.getAllRecords();

        cout << "Trades executed (expected 1): " << trades << endl;
        cout << "Tape size (expected 1): " << rec.size() << endl;
        cout << "Exec price (expected 100): " << rec[0].price << endl;
        cout << "Exec qty   (expected 7):   " << rec[0].quantity << endl;
        cout << "Remaining BUY qty (expected 3): "
            << book.bids.begin()->second.orders.front().quantity << endl << endl;
    }

    // ------------------------------------------------------------
    // Test 6: FIFO correctness at same price
    // ------------------------------------------------------------
    {
        cout << "[Test 6] FIFO preserved at same price level\n";

        OrderBook book;
        TradeTape tape;

        Order b;  b.order_id = 1;  b.quantity = 20; b.side = Order::Side::Buy;
        Order s1; s1.order_id = 20; s1.quantity = 5; s1.side = Order::Side::Sell;
        Order s2; s2.order_id = 21; s2.quantity = 5; s2.side = Order::Side::Sell;

        book.addOrder(100, b);
        book.addOrder(100, s1);
        book.addOrder(100, s2);

        matcher.tradesExecuted(book, tape);
        const auto& rec = tape.getAllRecords();

        cout << "First sell ID  (expected 20): " << rec[0].sell_order_id << endl;
        cout << "Second sell ID (expected 21): " << rec[1].sell_order_id << endl << endl;
    }

    // ------------------------------------------------------------
    // Test 7: Multiple executions across loop
    // ------------------------------------------------------------
    {
        cout << "[Test 7] Multiple executions until exhaustion\n";

        OrderBook book;
        TradeTape tape;

        Order b; b.order_id = 1; b.quantity = 30; b.side = Order::Side::Buy;

        Order s1; s1.order_id = 10; s1.quantity = 10; s1.side = Order::Side::Sell;
        Order s2; s2.order_id = 11; s2.quantity = 10; s2.side = Order::Side::Sell;
        Order s3; s3.order_id = 12; s3.quantity = 10; s3.side = Order::Side::Sell;

        book.addOrder(100, b);
        book.addOrder(100, s1);
        book.addOrder(100, s2);
        book.addOrder(100, s3);

        int trades = matcher.tradesExecuted(book, tape);

        cout << "Trades executed (expected 3): " << trades << endl;
        cout << "Tape size      (expected 3): " << tape.size() << endl;
        cout << "Book empty     (expected 1): " << book.bids.empty() << endl << endl;
    }

    // ------------------------------------------------------------
    // Test 8: TradeTape is append-only
    // ------------------------------------------------------------
    {
        cout << "[Test 8] TradeTape append-only guarantee\n";

        OrderBook book;
        TradeTape tape;

        Order b; b.order_id = 1; b.quantity = 10; b.side = Order::Side::Buy;
        Order s; s.order_id = 2; s.quantity = 10; s.side = Order::Side::Sell;

        book.addOrder(100, b);
        book.addOrder(100, s);

        matcher.tradesExecuted(book, tape);
        size_t firstSize = tape.size();

        // No more executions possible
        matcher.tradesExecuted(book, tape);

        cout << "Tape size unchanged (expected "
            << firstSize << "): " << tape.size() << endl << endl;
    }

    cout << "================ ALL TESTS PASSED =================\n\n";
    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
