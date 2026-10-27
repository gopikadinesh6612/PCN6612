NS-3 5-node wired TCP/UDP examples

Topology:
n0 -- n1 -- n2 -- n3 -- n4

Every link: 5 Mbps, 2 ms
Simulation: 10 seconds
TCP: n0 -> n4, SendSize 1024 bytes
UDP: n0 -> n4, rate 1 Mbps, packet size 512 bytes

1. Copy tcp-5nodes.cc and udp-5nodes.cc into your NS-3 scratch/ folder.
2. Run from the NS-3 root folder:
   ./ns3 run scratch/tcp-5nodes
   ./ns3 run scratch/udp-5nodes
3. The programs create tcp_results.csv / udp_results.csv and PCAP trace files.
4. Copy the matching CSV file into the folder where its notebook is located, or place the notebook and CSV together.
5. Open tcp_graph.ipynb or udp_graph.ipynb in Jupyter Notebook and Run All.

Notes:
- These examples use standard NS-3 helpers and modern C++ syntax.
- Throughput in CSV is calculated using the configured 8-second sender interval.
- FlowMonitor packet-loss figures are its end-of-simulation estimate, not a direct TCP retransmission count.
- If your installed NS-3 version reports an API incompatibility, share the error and version so the code can be adjusted.
