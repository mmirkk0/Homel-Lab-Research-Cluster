# ==============================================================================
# Subproject 09: FD.io VPP + VXLAN Overlay Setup Script
# Vector Packet Processing (VPP) Startup & VXLAN Tunnel Command Configuration
# ==============================================================================

# 1. Create Loopback & Assign Underlay IP
create loopback interface
set int state loop0 up
set int ip address loop0 10.0.0.1/32

# 2. Bind physical network interface to DPDK / AF_XDP driver
set int state GigabitEthernet0/8/0 up
set int ip address GigabitEthernet0/8/0 192.168.178.44/24

# 3. Create VXLAN Overlay Tunnel (UDP 4789 Encap)
create vxlan tunnel src 10.0.0.1 dst 10.0.0.2 vni 100 encap-vrf-id 0

# 4. Create L2 Bridge Domain and bind Tenant interface + VXLAN tunnel
create bridge-domain 100 learn 1 forward 1 uu-flood 1 flood 1 arp 1
set int l2 bridge vxlan_tunnel0 100
set int l2 bridge memif1/0 100

# 5. Enable SIMD AVX2 Vector Processing Node Profiling
set node dispatch-rate threshold 1000
