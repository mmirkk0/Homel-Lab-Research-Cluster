# Sottoprogetto 01: XDP Zero-Copy Engine

## 👨‍💻 Agenti Assegnati
- **Agent 1A (XDP Driver & eBPF Kernel Developer):** Sviluppo bytecode eBPF kernel (`src/xdp_prog.c`).
- **Agent 1B (AF_XDP Ring Buffer Engineer):** Sviluppo loader e gestione UMEM (`src/xdp_loader.c`).

---

## 🛠️ Compilazione ed Esecuzione

```bash
# Compilazione
make

# Esecuzione sul nodo 'linux' (interfaccia diretta enp12s0)
sudo ./xdp_loader enp12s0

# Esecuzione sul nodo 'lab' (interfaccia diretta enp2s0)
sudo ./xdp_loader enp2s0
```
