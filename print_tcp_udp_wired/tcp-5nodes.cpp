
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    // Hardcoded sample TCP results
    int nodes = 5;
    int sent = 1000;
    int received = 980;
    int lost = sent - received;
    double throughput = 3.92;
    double delay = 0.025;

    cout << "TCP Wired Network Simulation\n";
    cout << "Number of nodes: " << nodes << endl;
    cout << "Packets sent: " << sent << endl;
    cout << "Packets received: " << received << endl;
    cout << "Packets lost: " << lost << endl;
    cout << "Throughput: " << throughput << " Mbps" << endl;
    cout << "Average delay: " << delay << " seconds" << endl;

    // Save sample results to CSV
    ofstream file("tcp_results.csv");

    file << "Protocol,Nodes,Sent,Received,Lost,Throughput,Delay\n";
    file << "TCP," << nodes << "," << sent << ","
         << received << "," << lost << ","
         << throughput << "," << delay << "\n";

    file.close();

    cout << "Results saved to tcp_results.csv\n";

    return 0;
}
