# Operating-Systems-Kent-2025
For my OS class.

# 🧵 Producer-Consumer with Shared Memory and Semaphores (POSIX)

This project implements a classic **Producer-Consumer** problem using **shared memory**, **POSIX semaphores**, and **threads** in C on a Linux system.

The producer generates items and places them in a shared buffer (a "table"), while the consumer retrieves and processes them. Synchronization is handled using semaphores to ensure correct access to the shared memory.

---

## 🧱 Requirements

- GCC compiler
- POSIX-compliant system (Linux)
- Link with `-pthread` and `-lrt` (real-time library for semaphores)

---

## 🛠️ Build Instructions

Compile both files using:

```bash
gcc producer.c -pthread -lrt -o producer
gcc consumer.c -pthread -lrt -o consumer
