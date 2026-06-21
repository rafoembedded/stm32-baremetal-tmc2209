# STM32 Bare-Metal TMC2209 Controller

A low-level (bare-metal) C driver designed to control the Trinamic TMC2209 stepper motor driver via single-wire Half-Duplex UART using an STM32F103 microcontroller. The project is built entirely on the **CMSIS** library framework without HAL or LL abstraction layers, adhering strictly to the official **Trinamic TMC2209 Datasheet (v1.09)** specifications.

## Hardware & Peripherals Configuration

* **MCU:** STM32F103C8T6
* **Driver:** TMC2209 (configured in Single-Wire UART mode).
* **Communication Interface:** `USART3` (Pin `PB10`) running at 115200 Baud. Hardware Half-Duplex mode is enabled via the `USART_CR3_HDSEL` bit. Pin configuration: Alternate function open-drain.
* **Feedback Interface:** `TIM2` initialized in Quadrature Encoder Mode with robust hardware digital filtering (`fSAMPLING=fDTS/32, N=8`) and a software delay loop to stabilize the circuit against power-on voltage surges.
* **Debugging:** `USART1` (Pin `A9`, Alternate function push-pull) implemented for serial log output. The standard `__io_putchar` function is overridden to route data through the custom UART transmitter.

## Datasheet-Compliant Implementation Details

* **UART Frame Structure:** Data packets are constructed exactly as defined by Trinamic specs — strict 8-byte frames: `[Sync (0x05)] + [Slave Address (0x00)] + [Register Address | 0x80 (Write Bit)] + [4 Bytes Data] + [CRC8]`.
* **Echo Suppression Mechanism:** To prevent the single-wire half-duplex bus from echoing transmitted bytes back into the STM32 receiver, the `send_array` function explicitly clears the receiver enable bit (`CR1_RE`) prior to transmission, and reactivates it only after the Transmission Complete flag (`USART_SR_TC`) is raised.
* **Custom CRC8 Engine:** Implemented a software bit-shifting Cyclic Redundancy Check (CRC8) routine utilizing the exact polynomial specified in the official Trinamic driver documentation to validate data integrity.
* **Register Tuning:**
  * `GCONF (0x00)` — Bit configurations applied to enable stable UART communication.
  * `IHOLD_IRUN (0x10)` — Explicit parameter scaling: `IHOLD = 9`, `IRUN = 19`, and `IHOLDDELAY = 6` to optimize distinct running/standstill currents and manage thermal efficiency.
  * `CHOPCONF (0x6C)` — Initialized specific chopper parameters to ensure low-noise, smooth motor operation.
  * `VACTUAL (0x22)` — Software-driven continuous speed control via target velocity register writes.

## Project Architecture

This repository contains only source code files and compilation scripts:
* **`Src/main.c`** — Application entry point; initializes clock systems, core peripherals, and commands initial rotation via `vactual(5000)`.
* **`Src/tmc2209.c` / `Inc/tmc2209.h`** — Driver state initialization, specific register payloads, and the software CRC8 calculator.
* **`Src/uart.c` / `Inc/uart.h`** — Low-level, register-direct setup for the debug USART1 and the single-wire USART3 interface.
* **`Src/encoder.c` / `Inc/encoder.h`** — Configuration of the TIM2 hardware timer for quadrature processing and digital input filtering.
* **`STM32F103C8TX_FLASH.ld`** — Linker script detailing accurate bare-metal target memory layout mappings.
