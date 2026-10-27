WIRED TCP/UDP PRINT + CSV + NOTEBOOKS

Files:
- tcp-5nodes.cpp: prints illustrative TCP values and writes tcp_results.csv
- udp-5nodes.cpp: prints illustrative UDP values and writes udp_results.csv
- tcp_results_graphs.ipynb: reads tcp_results.csv and plots its metrics
- udp_results_graphs.ipynb: reads udp_results.csv and plots its metrics

IMPORTANT:
These are ordinary C++ programs with hardcoded illustrative sample values. They do not run an NS-3 simulation, so their CSV values are not real measured network results. For actual NS-3 results, use NS-3 simulation code that writes FlowMonitor metrics.

Compile and run from this folder:
g++ tcp-5nodes.cpp -o tcp-5nodes
./tcp-5nodes

g++ udp-5nodes.cpp -o udp-5nodes
./udp-5nodes

The CSV files are created in the current working directory where you run the programs.
Open each notebook in Jupyter and run all cells after generating the matching CSV file.
