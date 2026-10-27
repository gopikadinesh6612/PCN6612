#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    // Illustrative sample values only (not measured NS-3 simulation results)
    int nodes = 5;
    int sent = 1000;
    int received = 980;
    int lost = sent - received;
    double throughput = 3.92; // Mbps
    double delay = 0.025;     // seconds

    cout << "TCP Wired Network (sample results)\n";
    cout << "Number of nodes: " << nodes << endl;
    cout << "Packets sent: " << sent << endl;
    cout << "Packets received: " << received << endl;
    cout << "Packets lost: " << lost << endl;
    cout << "Throughput: " << throughput << " Mbps" << endl;
    cout << "Average delay: " << delay << " seconds" << endl;

    ofstream file("tcp_results.csv");
    if (!file.is_open()) {
        cerr << "Error: could not create tcp_results.csv\n";
        return 1;
    }
    file << "Protocol,Nodes,Sent,Received,Lost,ThroughputMbps,DelaySeconds\n";
    file << "TCP," << nodes << "," << sent << "," << received << ","
         << lost << "," << throughput << "," << delay << "\n";
    file.close();

    cout << "Results saved to tcp_results.csv\n";
    return 0;
}
