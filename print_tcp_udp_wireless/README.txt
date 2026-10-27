Wireless print-only C++ and Jupyter notebooks

Files:
- wireless-tcp.cpp: prints wireless TCP configuration statements only.
- wireless-udp.cpp: prints wireless UDP configuration statements only.
- wireless_tcp_graphs.ipynb: makes illustrative TCP throughput, packet-loss and delay graphs.
- wireless_udp_graphs.ipynb: makes illustrative UDP throughput, packet-loss and delay graphs.

Important: The C++ files do not run NS-3 and do not create real measurements. Notebook values are illustrative sample numbers only.

To run C++ in WSL from this folder:
  g++ wireless-tcp.cpp -o wireless-tcp
  ./wireless-tcp
  g++ wireless-udp.cpp -o wireless-udp
  ./wireless-udp

To run notebooks, install Python packages if needed:
  python3 -m pip install pandas matplotlib notebook
Then open Jupyter Notebook, open each .ipynb, and choose Run All.
