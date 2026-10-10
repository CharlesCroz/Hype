# Hype
Hypervisor for Zynq-7000 based device

## Context
This is a self-training project to understand better the lowest layers of complex embedded software, notably how to develop a layer that will handle hardware capabilities (interrupts, memory, connected devices).

## Specifications (high level)
The hypervisor Hype shall provide TSP for several Partitions to run concurrently on a Zynq-7000 based device. 

**REQ-HL-000** : The physical memory shall be statically allocated to the Partitions with sections having different visibility and permission set per-Partition (e.g. section A of memory is read-only for Partition X and read-write for Partition Y). 

**REQ-HL-001** : CPUs run time shall be statically allocated to the Partitions (e.g. over period [T0, Tmax], Partition X shall run on CPU A during [T1,T2], [T3,T4], ..., [Tn-1, Tn] with T0 <= T1 < T2 < ... < Tn <= Tmax).

**REQ-HL-002** : Partitions shall be able to interact with each other using statically determined IPCs (inter-Partitions communication) (e.g. semaphores, mutex, message queues).

**REQ-HL-003** : Partitions are either Standard Partition or Admin Partitions. Admin Partition only shall be able to freeze, resume, or reboot Standard or Admin Partition.

**REQ-HL-004** : Partitions shall have access to Hardware Drivers according to statically defined permissions (e.g Partition A can access Hardware Driver X function F0, F1.... Fj).

**REQ-HL-005** : The project shall come with comprehensive set of tools to configure and compile Hype and Partitions.

**REQ-HL-006** : The development shall be done according to modern standards of quality (testing, documentation).
