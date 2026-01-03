# Matching-Engine-
Overview

This project implements a deterministic, single-threaded order matching engine core in modern C++, built from first principles.

The focus is correctness, invariants, and execution semantics, not premature concurrency or framework abstractions.

The engine models the core mechanics of a real exchange:

Price-time priority

FIFO execution per price level

Partial and full fills

Explicit execution results

Append-only trade audit tape

Deterministic behavior under all tested edge cases

Concurrency and ingestion are intentionally deferred until replay and ownership invariants are proven.

---Design Goals

Determinism over throughput

Explicit state transitions

No hidden side effects

Auditable execution

Testable invariants

Separation of concerns

This is a systems project, not a UI or API project.

---Architecture
Core Components:
-Order

Represents a single buy or sell instruction.

Side: BUY / SELL

-Quantity

Unique order ID

-PriceLevel

Represents a single price point in the book.

Stores orders in FIFO order (std::deque)

Enforces time priority

No cross-price logic

-OrderBook

Maintains the full book state.

Separate bid and ask maps

Prices are keys, PriceLevel is the value

Uses ordered containers for deterministic best-price queries

Responsibilities:

Order insertion

Best bid / best ask queries

Match feasibility checks (tryMatchOnce)

-Matcher

Owns execution logic.

Responsibilities:

Single atomic execution (executeOnce)

Repeated execution loop (tradesExecuted)

Quantity calculation

Cleanup at order and price-level granularity

Execution is logically atomic in a single thread.

-ExecutionResults

Explicit execution contract.

Contains:

Whether a trade occurred

Execution price

Execution quantity

Buy order ID

Sell order ID

No implicit state is relied upon.

-TradeTape

Append-only audit log.

Stores ExecutionResults

Read-only access to execution history

Decoupled from matching logic

Owned by the caller

This enables replay, analysis, and external integration.

---Execution Model

-Gate
Check if a match is possible (tryMatchOnce)

-Price Discovery
Best bid ≥ best ask

-Order Selection
FIFO at the best price levels

-Execution
Quantity = min(buy.qty, sell.qty)

-Mutation
Quantities updated by reference

-Cleanup

Remove filled orders

Remove empty price levels

-Record

Append result to TradeTape

Each call to executeOnce is deterministic and self-contained.

---Testing & Validation

A comprehensive test harness validates:

Empty book behavior

BUY-only / SELL-only books

Non-crossed markets

Crossed markets

Partial fills

Full fills

FIFO correctness at same price

Repeated execution loops

TradeTape append-only guarantees

No phantom trades

No silent state corruption

Tests are written as behavioral verification, not unit mocks.

---Concurrency Model (Intentional Design)

This engine is single-threaded by design.

Concurrency is explicitly deferred until:

Deterministic replay is implemented

Ownership boundaries are formalized

Execution invariants are proven under replay

This mirrors how real matching engines are built:
single-writer core, multi-producer ingestion.

---What This Is Not (By Design)

❌ Not multi-threaded

❌ Not latency-optimized

❌ Not production-ready exchange software

❌ Not a framework or API server

Those concerns come after correctness.

---Future Extensions (Optional)

Deterministic replay from TradeTape

Snapshot / query layer

Single-writer event loop

Concurrent ingestion queues

Price representation upgrades

Symbol support

None of these require refactoring the core.

---Build & Run

Standard C++ toolchain (C++17+).

Compile and run the test harness in main.cpp to verify behavior.

---Why This Project Exists

This project exists to demonstrate:

Systems thinking

Execution correctness

Careful state management

Real-world trading mechanics

Engineering discipline over shortcuts

It is intentionally minimal, explicit, and rigorous.
