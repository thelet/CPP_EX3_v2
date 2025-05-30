# Coup – C++ Board Game Implementation

 C++20 library and application that simulates the “Coup” board game for 2–6 players. Includes:

* **Core engine** with full rule enforcement, custom exceptions, and extensible roles
* **Console demo** (`make Main`) driven by `demo/Demo.cpp`
* **Unit tests** (`make Tests`) using Doctest, covering all actions, blocking, and role-specific abilities
* **SFML GUI** (`make Gui`) with start screen, player setup, per-turn panels, action buttons, and real-time exception display
* **Memory checking** via `make valgrind`

---

## Table of Contents

1. [Features](#features)
2. [Requirements](#requirements)
3. [Project Structure](#project-structure)
4. [Building](#building)
5. [Running](#running)
6. [Testing](#testing)
7. [Architecture & Design](#architecture--design)
8. [Academic Integrity](#academic-integrity)
9. [License](#license)

---

## Features

* **Full rule support**: gather, tax, bribe, arrest, sanction, coup
* **Role abilities**: Governor (undo tax), Spy (peek & block arrest), Baron (invest), General (block coup), Judge (undo bribe), Merchant (bonus coin & reduced arrest penalty)
* **Blocking mechanics**: tax/bribe can be blocked mid-round by the proper role
* **Forced coup rule**: players holding ≥10 coins must coup
* **Exception safety**: all illegal moves throw descriptive exceptions; GUI catches and displays them
* **Extensible design**: add new roles or actions by subclassing `Player` and extending `Game`

---

## Requirements

* **Compiler**: g++ (>= 10) or clang++ with full C++20 support
* **Libraries**:

  * SFML (Graphics, Window, System)
  * Doctest (bundled under `tests/doctest.h`)
* **Build tools**: make, pkg-config

*On Ubuntu/Debian you can install SFML via:*

```
sudo apt update
sudo apt install libsfml-dev
```

---

## Project Structure

```
.
├── Makefile              # builds console demo, GUI, tests, valgrind, clean
├── include/
│   ├── core/             # engine headers: Game, Player, Action, Exceptions
│   └── gui/              # GUI headers: Widget classes, Window, StartScreen
├── src/
│   ├── core/             # engine implementation
│   │   └── roles/        # per-role implementations
│   ├── gui/              # SFML GUI implementation
│   └── demo/             # console Demo.cpp
├── tests/                # Doctest unit tests (test_game.cpp)
└── README.md             # this file
```

---

## Building

Simply run:

```
make
```

By default this builds the **console demo** (`./Main`).

To build other targets:

* `make Main` – Console demo
* `make Gui`  – SFML graphical interface
* `make Tests` – Compile + run all unit tests
* `make valgrind` – Run `./Main` under Valgrind leak checker
* `make clean`  – Remove build artifacts

All object files live under `build/`.

---

## Running

### Console Demo

```
./Main
```

Walks through a scripted game in the terminal, demonstrating actions and exceptions.

### Graphical UI

```
./Gui
```

1. Choose number of players (2–6), each player’s name and role.
2. Click **Start** to open the game board.
3. The board displays each active player’s panel in a 2×3 grid:

   * Name + role header
   * Coin count
   * Available actions (buttons enabled only on your turn)
   * Role-specific buttons (e.g. **Invest**, **Undo Tax**, **Peek**, etc.)
4. Any illegal move pops an error message at the bottom.

---

## Testing

All core rules, edge cases, blocking logic, and role abilities are covered:

```
make Tests
```

The test suite includes over 25 test cases, such as:

* Each basic action and its cost/reward
* Forced-coup when holding ≥10 coins
* Blocking tax/bribe by Governor/Judge
* Role-specific abilities (invest, peek, undo, block coup)
* Sanction prevention of gather/tax
* Merchant’s reduced arrest penalty and bonus coin on start of turn
* Full round cancellation and game-win detection

---

## Architecture & Design

* **`Game`** orchestrates the turn order, coin bank, pending blocks, and rule enforcement.
* **`Player`** is an abstract base; each role subclasses it, providing `role()`, custom methods (e.g. `invest()`, `undo()`) and invoking engine hooks (`game_.baronInvest(*this)`).
* **Actions** are represented by an `Action` struct and submitted via `Player::gather()`, `tax()`, etc., which wrap `Game::perform()`.
* **Exceptions** (`IllegalAction`, `NotYourTurn`, `GameNotFinished`, etc.) live in `util/Exceptions.hpp`.
* **GUI** uses SFML:

  * **`StartScreen`** collects player specs before game start.
  * **`SFMLWindow`** displays the board and routes button clicks.
  * **Widget classes** (`BoardWidget`, `CardWidget`, `Button`) encapsulate layout, rendering, and click handling.
* **Modularity**: clear separation between core engine (no GUI dependencies) and the front-end.

---

## Academic Integrity

This project is an individual submission. I consulted **ChatGPT** for guidance on Makefile patterns and GUI design; All critical logic and code are written by me.

---


