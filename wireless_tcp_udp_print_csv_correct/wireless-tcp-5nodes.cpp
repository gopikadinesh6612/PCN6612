#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    // Illustrative sample values only; this is NOT an actual NS-3 simulation.
    int nodes = 5;              // 1 access point + 4 wireless stations
    int sent = 1000;
    int received = 970;
    int lost = sent - received;
    double throughput = 3.88;   // Mbps (sample)
    double delay = 0.020;       // seconds (sample)

    cout << "TCP Wireless Network (sample results)\n";
    cout << "Number of nodes: " << nodes << endl;
    cout << "Topology: 1 access point + 4 stations" << endl;
    cout << "Packets sent: " << sent << endl;
    cout << "Packets received: " << received << endl;
    cout << "Packets lost: " << lost << endl;
    cout << "Throughput: " << throughput << " Mbps" << endl;
    cout << "Average delay: " << delay << " seconds" << endl;

    ofstream file("tcp_wireless_results.csv");
    if (!file.is_open()) {
        cerr << "Error: could not create tcp_wireless_results.csv\n";
        return 1;
    }
    file << "Protocol,Nodes,Topology,Sent,Received,Lost,ThroughputMbps,DelaySeconds\n";
    file << "TCP," << nodes << ",\"1 access point + 4 stations\","
         << sent << "," << received << "," << lost << ","
         << throughput << "," << delay << "\n";
    file.close();

    cout << "Results saved to tcp_wireless_results.csv\n";
    return 0;
}
