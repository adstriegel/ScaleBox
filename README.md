# ScaleBox

ScaleBox ("Scalability in a Box") is a C++ packet-processing framework developed at the
[NetScale Laboratory](http://netscale.cse.nd.edu) at the University of Notre Dame (PI: Dr.
Aaron Striegel). It provides a common substrate — packet capture/injection, a chained
input/output module pipeline, pooled packet objects, statistics, and timers — on top of
which a series of network measurement and traffic-shaping research systems have been built
over the years by members of the lab.

Most subsystems can be understood as one of two things: **core framework** code that every
build shares, or a **research system** built on top of that framework to study a specific
problem (last-mile wireless performance, stealth multicast, mobile traffic mixing, etc.).
A few directories are early scaffolding or retired code kept around for reference — those
are called out below.

## Architecture

The core pipeline lives in `src/pkt` and `src/core`:

- An **Adapter** (`pkt/Adapter.h`) owns a physical or virtual interface (live capture via
  `AdapterPCap`, or file-based replay via `AdapterFile`) and reads/writes `Packet` objects.
- Each **Packet** (`pkt/Packet.h`) is a pooled object (`mem/MemoryPool.h`,
  `mem/MemPoolObject.h`) to avoid per-packet heap churn on the hot path.
- Packets flow through a chain of **IOModules** (`pkt/IOModule.h`) — e.g. `InputLog`,
  `InputPurge`, `InputPassthrough`, `OutputWrite`, `Filter` — that inspect, transform, log,
  or drop them.
- `core/Main.cc` wires up the adapters and module chains for a given build; `mon/Monitor.h`
  and `mon/Thread_Timer.h` drive periodic/background work; `stat/Stats.h` collects runtime
  counters; `xml/FileXML.h` handles simple XML I/O for config and result dumps.

Everything else in `src/` either supports that core (`mem`, `util`, `viz`, `sandbox`,
`test`) or is a research system that plugs into it as a set of adapters/modules.

## Key modules

### Core framework

| Directory | Purpose |
|---|---|
| `core` | Program entry point (`Main.cc`), console/logging, timestamps |
| `pkt` | The packet pipeline itself: `Packet`, `Adapter`s, `IOModule` chain, filters, packet queue/scheduler |
| `mem` | Custom pooled-object allocator (`MemoryPool`, `MemPoolObject`) used by `Packet` and others to avoid per-packet `new`/`delete` |
| `xml` | Minimal XML DOM (`FileXML`, `NodeDOM`, `NodeElem`) used for config and result output |
| `mon` | Background monitoring: periodic timers (`Thread_Timer`) and archival (`Thread_Archive`) |
| `stat` | Runtime statistics collection and reporting (`Stats`, `StatManager`, per-thread counters) |
| `util` | Shared utilities: IP helpers, a generic `ParamDictionary`, `PracticalSocket`, SpookyHash, 802.11 helpers, tokenizing |
| `vnet` | Virtual networking building blocks: a virtual adapter, rate limiter, and link-delay emulation, used to emulate WAN conditions in-lab |
| `viz` | Lightweight JSON/D3.js output helpers used to render measurement results as web graphs (used by FMNC's web demo) |
| `sandbox` | Scratch/experimental I/O module harness for trying out new modules in isolation |
| `test` | Small standalone test-support/timer harnesses |

### Research systems

| Directory | Purpose |
|---|---|
| `fmnc` | **Fast Mobile Network Characterization** — probes last-mile wireless and next-to-last-mile broadband links using in-band TCP packet-slicing (in the spirit of TCP Sting) to force rapid-fire ACKs and infer bottleneck behavior without client-side software. Includes a web-facing demo mode (`createTest_WebDemo` and friends) that serves live test results as D3.js graphs. |
| `RIPPS` | An earlier last-mile monitoring tool using the same packet-tap/slicing approach that FMNC builds on; ScaleBox's `RIPPS_Support`/`RIPPS_Monitor` wire it into the core framework. See `src/RIPPS/README` for lab notes. |
| `TWiCE` | **Transparent Wireless Capacity Enhancement** — a gateway/cache system (`TWiCE_Gateway`, `TWiCE_CacheTable`, `TWiCE_PacketPool`) for improving effective wireless capacity for downstream clients. |
| `whirlwind` | A gateway architecture sitting between the WAN and downstream clients, modeling per-client "user elements" (`whirlwind_model_ue`) at the gateway and a rendezvous point. Present in source but not currently wired into the top-level `Makefile` build. |
| `psm` | **Proximity Secure Mixing** — a man-in-the-middle/proxy gateway (`psm_gateway`) that mixes mobile client traffic for privacy purposes. |
| `sneayknat` | A NAT traversal/manipulation module (`SneakyNAT`), developed with NSF/Sun/Intel/Notre Dame grant support noted in its headers. |
| `stmc` | **Stealth Multicast** — ingress/egress `IOModule`s (`InputIngressVGDM`/`InputEgressVGDM`) and transport logic (`TransportStealth`) that convert unicast gateway traffic to/from multicast for downstream stealth clients. |

### Caching subsystems

| Directory | Purpose |
|---|---|
| `pktcache` | A packet-cache `IOModule` set (`ChildCacheModule`, `PktCacheTable`) for caching/replaying packet content, currently disabled by a compile-time flag (`PKTCACHE_MODULE_ENABLED`). |
| `packetcache` | A parallel, independently-evolved packet-caching implementation (`PacketCacheModule`, `PacketCacheTable`, `PacketCacheEntry`) with its own `Makefile` target (`PktCache`). |

### Early-stage / retired

| Directory | Purpose |
|---|---|
| `vhost` | A scaffolded user-space virtual host network stack (`VirtualHost`, `VNetStack`, per-layer modules for 802.3/ARP/IP/TCP/UDP, virtual sockets). Most files are empty stubs — treat as in-progress, not functional. |
| `boneyard` | Retired code kept for reference (older RIPPS I/O modules superseded by the current `RIPPS` implementation). |

## Building

The build is driven by `src/Makefile` (`g++ -std=c++0x`, linking `-lpcap -lpthread`) and
targets a Linux host. From `src/`:

```bash
make ScaleBox   # the main framework binary
make pcache     # pktcache-based variant
make PktCache   # packetcache-based variant
make twicegw    # TWiCE gateway
make viztest    # viz/JSON helper test binary
```

Build output (`*.o` files and the resulting binaries) is git-ignored — see `.gitignore`.

## Status

This is an active academic research codebase spanning multiple projects and several
graduate students' work over a number of years, so subsystems vary widely in maturity —
from production-grade (the core `pkt`/`mem` framework, `fmnc`) to early scaffolding
(`vhost`) to intentionally retired (`boneyard`). Individual subdirectories may have their
own `README` or task-list files with project-specific notes (see `src/fmnc/README` and
`src/RIPPS/README`).
