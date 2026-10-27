NS-3.42 wired and wireless TCP/UDP lab examples
Based on the supplied PCN cheat sheet patterns.

FILES
- tcp-wired-5nodes.cc: 5-node wired line, TCP n0 -> n4
- udp-wired-5nodes.cc: 5-node wired line, UDP n0 -> n4
- tcp-wireless-5nodes.cc: 1 Wi-Fi AP + 4 stations, TCP STA0 -> STA3
- udp-wireless-5nodes.cc: 1 Wi-Fi AP + 4 stations, UDP STA0 -> STA3
- tcp_wired_wireless_graphs.ipynb: compares wired vs wireless TCP
- udp_wired_wireless_graphs.ipynb: compares wired vs wireless UDP

COMMON SETTINGS
- Simulation time: 20 seconds
- Wired: each point-to-point link is 10 Mbps, 2 ms, DropTail 50 packets
- Wireless: 802.11n infrastructure mode using one AP and four stations
- TCP: BulkSend, SendSize 1024 bytes, unlimited MaxBytes
- UDP: OnOff constant rate 1 Mbps, 512-byte packets
- FlowMonitor outputs throughput, loss percentage, average delay, PDR, and packet counts to CSV

RUN FROM NS-3 ROOT (NS-3.42)
Copy files into scratch:
  cp /path/to/tcp-wired-5nodes.cc scratch/
  cp /path/to/udp-wired-5nodes.cc scratch/
  cp /path/to/tcp-wireless-5nodes.cc scratch/
  cp /path/to/udp-wireless-5nodes.cc scratch/

Then run:
  ./ns3 run scratch/tcp-wired-5nodes
  ./ns3 run scratch/udp-wired-5nodes
  ./ns3 run scratch/tcp-wireless-5nodes
  ./ns3 run scratch/udp-wireless-5nodes

Each run creates a CSV file in the current NS-3 root folder and PCAP traces. Copy the four CSV files beside the notebooks, open the relevant notebook in Jupyter, and choose Run All.

The supplied cheat sheet lists the same core helpers: PointToPointHelper, WifiHelper, YansWifiPhyHelper, ApWifiMac/StaWifiMac, MobilityHelper, BulkSendHelper, OnOffHelper, PacketSinkHelper, Ipv4GlobalRoutingHelper, and FlowMonitor.
