# Sottoprogetto 05: Stress-Chaos-Test Suite (Red Team Chaos Injectors)

## 👨‍💻 Agenti Assegnati (Red Team)
- **Agent 09 (RDMA & Network Flood Generator):** Saturazione ad alta frequenza del link RoCE v2 (`src/rdma_saturator.c`).
- **Agent 10 (CPU Cache Thrash & Context-Switch Saturator):** Inquinamento L1/L2/L3 cache e tempestamento di context switch (`src/cache_thrash_saturator.c`).

---

## 🛠️ Compilazione ed Esecuzione

```bash
make

# Avvio saturatore di cache L1/L2/L3 e context switch
./cache_thrash_saturator

# Avvio saturazione RoCE RDMA
./rdma_saturator
```
