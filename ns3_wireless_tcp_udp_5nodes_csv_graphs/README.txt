NS-3 wireless TCP/UDP examples (5 nodes)

Topology: five static wireless nodes in ad-hoc Wi-Fi mode, positioned 20 metres apart in a line.
Wi-Fi standard: 802.11g. Simulation duration: 10 seconds.
TCP: n0 -> n4, BulkSend, SendSize 1024 bytes.
UDP: n0 -> n4, OnOff, 1 Mbps, packet size 512 bytes.

Copy tcp-wireless-5nodes.cc and udp-wireless-5nodes.cc into your NS-3 scratch/ folder.
From NS-3 root, run:
  ./ns3 run scratch/tcp-wireless-5nodes
  ./ns3 run scratch/udp-wireless-5nodes

CSV output:
  tcp_wireless_results.csv
  udp_wireless_results.csv

Notebook files:
  tcp_wireless_graphs.ipynb
  udp_wireless_graphs.ipynb

Keep each CSV next to its matching notebook, then open the notebook in Jupyter and Run All.
PCAP files are also generated for Wireshark inspection.

This is an ad-hoc Wi-Fi network (no access point). Nodes are static; traffic is sent from n0 to n4.
FlowMonitor loss counts are estimates at the end of simulation. Throughput uses the configured 8-second sender interval.
NS-3 APIs can vary by version. If a build error occurs, share the exact error and NS-3 version.
