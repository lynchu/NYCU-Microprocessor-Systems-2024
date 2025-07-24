# NYCU Microprocessor Systems 2024

This repository contains all coursework and lab assignments for the Microprocessor Systems course at National Yang Ming Chiao Tung University (NYCU) for the 2024 semester. The focus of the course is on microprocessor architecture, hardware-software co-design, performance analysis, and custom accelerator development.

## Repository Structure

```
NYCU-Microprocessor-Systems-2024/
├── HW1_RealTime_Analysis/       # Real-time performance analysis of HW-SW platforms
├── HW2_BranchPredictor/          # Design and evaluation of branch predictors
├── HW3_CacheOptimization/        # Techniques for cache optimization
├── HW4_RTOS_Analysis/            # Analysis of real-time operating system behaviors
├── HW5_DomainAccelerator/        # Design of a domain-specific hardware accelerator
└── README.md                     # This file
```

Each homework directory includes:

* **Source Code**: Verilog, SystemVerilog, and C implementations
* **Reports**: Detailed write-ups in PDF or Markdown, describing design decisions, experiments, and results

## Assignment Overviews

### HW1: Real-time Analysis of a HW-SW Platform

* **Objective**: Measure and analyze the latency and throughput of a microprocessor system under real-time constraints.
* **Contents**:

  * `platform.v` – Top-level Verilog module
  * `software/rt_app.c` – Real-time application running on the soft-core CPU
  * `report/RT_Analysis_Report.pdf` – Experimental results and discussion

### HW2: Branch Predictor Design

* **Objective**: Implement and compare different branch prediction algorithms.
* **Contents**:

  * `bpu.v` – Branch prediction unit module
  * `patterns/` – Test vectors for static, bimodal, and gshare predictors
  * `report/BP_Design_Report.md` – Accuracy and performance comparison

### HW3: Cache Optimization

* **Objective**: Explore cache architectures and implement cache replacement policies.
* **Contents**:

  * `cache.sv` – SystemVerilog cache module
  * `policies/` – LRU, FIFO, and random policy implementations
  * `report/Cache_Opt_Report.pdf` – Cache performance analysis

### HW4: RTOS Analysis

* **Objective**: Study task scheduling and interrupt handling in a real-time operating system.
* **Contents**:

  * `os/rtos.v` – Simplified RTOS kernel in Verilog
  * `app/` – Sample tasks demonstrating periodic and aperiodic scheduling
  * `report/RTOS_Analysis.md` – Scheduling theory and timing analysis

### HW5: Domain-Specific Accelerator

* **Objective**: Design a custom hardware accelerator for a target application domain.
* **Contents**:

  * `accel.v` – Accelerator module interface
  * `driver/accel.c` – Host-side control software
  * `report/DSA_Design_Report.pdf` – Architecture, throughput, and area metrics

## Prerequisites & Tools

* **HDL Simulation**: ModelSim, Vivado Simulator, or similar tools supporting Verilog/SystemVerilog
* **Software Compilation**: GCC for cross-compiling C programs for soft-core CPU
* **Scripting**: Tcl for simulation automation
* **Environment**: Linux-based environment recommended

## How to Run

1. Clone the repository:

   ```bash
   git clone https://github.com/lynchu/NYCU-Microprocessor-Systems-2024.git
   cd NYCU-Microprocessor-Systems-2024
   ```
2. Navigate to an assignment folder, e.g., `HW1_RealTime_Analysis`.
3. Launch your simulator and source the test bench script, for example:

   ```tcl
   vsim -do testbench/rt_tb.tcl
   ```
4. Review waveforms and reports in the `report/` directory.

## Contributing

This repository is maintained by Chih-Ling (Lynn) Chu for coursework. Contributions are not expected.

## License

All course materials and code are provided for academic use within NYCU. Distribution is subject to instructor permission.
