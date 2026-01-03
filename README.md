# Matching-Engine

A deterministic, single-threaded order matching engine core in modern C++, built from first principles. The focus is correctness, explicit invariants, and execution semantics—deliberately deferring concurrency and framework abstractions.

## Highlights
- Price–time priority with FIFO per price level
- Partial and full fills with explicit `ExecutionResults`
- Append-only `TradeTape` for auditability and replay
- Deterministic behavior across tested edge cases
- Separation of concerns between book state, matching, and tape

---

## Design Goals
- Determinism over throughput
- Explicit state transitions; no hidden side effects
- Auditable execution with an append-only tape
- Testable invariants and repeatable results
- Clear separation of responsibilities (book vs. matcher vs. tape)

This is a systems project, not a UI or API server.

---

## Architecture

### Core Types
- `Order`
  - Represents a single buy or sell instruction.
  - Fields: `id`, `side` (`BUY`/`SELL`), `price`, `quantity`.
- `PriceLevel`
  - Represents all orders at a single price.
  - Data structure: FIFO queue (e.g., `std::deque`) to enforce time priority.
  - No cross-price logic.
- `OrderBook`
  - Maintains the full state (separate bid and ask maps).
  - Keys: price; values: `PriceLevel`.
  - Ordered containers enable deterministic best-price queries.
  - Responsibilities:
    - Order insertion
    - Best bid / best ask queries
    - Feasibility checks (e.g., `tryMatchOnce`)
- `Matcher`
  - Owns execution logic.
  - Responsibilities:
    - Single atomic execution step (`executeOnce`)
    - Repeated execution loop (`tradesExecuted`)
    - Quantity calculation and cleanup at order/price-level granularity
  - Logically atomic in a single thread.
- `ExecutionResults`
  - Explicit execution contract containing:
    - `did_trade` (bool)
    - `price`
    - `quantity`
    - `buy_order_id`
    - `sell_order_id`
  - No implicit state is relied upon.
- `TradeTape`
  - Append-only audit log of `ExecutionResults`.
  - Read-only access to execution history.
  - Decoupled from matching logic; owned by the caller.
  - Enables replay, analysis, and external integration.

---

## Execution Model

1. Gate: check if a match is possible (`tryMatchOnce`).
2. Price discovery: `best_bid >= best_ask`.
3. Order selection: FIFO at the best price levels.
4. Execution: `executed_qty = min(buy.qty, sell.qty)`.
5. Mutation: update quantities by reference.
6. Cleanup: remove filled orders and empty price levels.
7. Record: append `ExecutionResults` to `TradeTape`.

Each call to `executeOnce` is deterministic and self-contained.

---

## Testing & Validation

The test harness validates:
- Empty book behavior
- BUY-only / SELL-only books
- Non-crossed markets
- Crossed markets
- Partial and full fills
- FIFO correctness at the same price
- Repeated execution loops
- TradeTape append-only guarantees
- No phantom trades or silent state corruption

Tests emphasize behavioral verification over mocks.

---

## Concurrency Model (Intentional)

The engine is single-threaded by design. Concurrency is deferred until:
- Deterministic replay is implemented
- Ownership boundaries are formalized
- Execution invariants are proven under replay

This mirrors real matching engines: single-writer core with multi-producer ingestion.

---

## What This Is Not
- ❌ Multi-threaded
- ❌ Latency-optimized
- ❌ Production-ready exchange software
- ❌ A framework or API server

Those concerns come after correctness.

---

## Future Extensions
- Deterministic replay from `TradeTape`
- Snapshot/query layer
- Single-writer event loop
- Concurrent ingestion queues
- Price representation upgrades
- Symbol support

None of these require refactoring the core.

---

## Getting Started

### Prerequisites
- C++17 or newer
- A standard toolchain (e.g., GCC, Clang, or MSVC)

### Build & Run
Minimal example (adjust files as needed):
```bash
# From repository root
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o engine main.cpp
./engine
```

If you use CMake:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/engine
```

---

## Quick Example (Pseudocode)

```cpp
OrderBook book;
TradeTape tape;
Matcher matcher{book, tape};

// Insert orders
book.add(Order{/*id=*/1, /*side=*/BUY,  /*price=*/100, /*qty=*/10});
book.add(Order{/*id=*/2, /*side=*/SELL, /*price=*/ 99, /*qty=*/ 7});

// Execute until no more matches
while (matcher.executeOnce().did_trade) {
    // results are appended to tape automatically
}

// Inspect audit trail
for (const auto& r : tape.results()) {
    std::cout << "Trade: qty=" << r.quantity
              << " price=" << r.price
              << " buy_id=" << r.buy_order_id
              << " sell_id=" << r.sell_order_id << "\n";
}
```

Note: Adjust API names to match your actual implementation (`add`, `executeOnce`, `results`, etc.).

---

## Project Status
Educational and correctness-focused core. Not production-grade; performance and concurrency are intentionally out of scope for now.

---

## License
Add a license (e.g., MIT or Apache-2.0) to clarify usage and contributions.

---

## Why This Project Exists
To demonstrate:
- Systems thinking and careful state management
- Execution correctness and explicit invariants
- Real-world trading mechanics
- Engineering discipline over shortcuts

Intentionally minimal, explicit, and rigorous.
