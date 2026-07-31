# Sottoprogetto 03: eBPF Kernel Scheduler & Observability Tracer

## 👨‍💻 Agenti Assegnati
- **Agent 3A (Kernel Scheduler & Kprobe Specialist):** Sviluppo kprobe e tracciamento `sched_switch`.
- **Agent 3B (BPFtrace & FlameGraph Metrics Engineer):** Scripting di visualizzazione delle metriche di jitter.

---

## 🛠️ Esecuzione

```bash
sudo bpftrace scripts/sched_latency.bt
```
