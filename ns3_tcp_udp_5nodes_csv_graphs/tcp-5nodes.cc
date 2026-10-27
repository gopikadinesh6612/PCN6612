#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include <fstream>
#include <sstream>
using namespace ns3;

int main()
{
    // 1. Create five nodes: n0 -- n1 -- n2 -- n3 -- n4
    NodeContainer nodes;
    nodes.Create(5);

    // 2. Create four wired links and assign a different subnet to each link
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // Install Internet stack before assigning IP addresses
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    Ipv4InterfaceContainer interfaces[4];

    for (uint32_t i = 0; i < 4; i++)
    {
        NodeContainer pair(nodes.Get(i), nodes.Get(i + 1));
        NetDeviceContainer devices = p2p.Install(pair);

        std::ostringstream subnet;
        subnet << "10.1." << (i + 1) << ".0";

        address.SetBase(subnet.str().c_str(), "255.255.255.0");
        interfaces[i] = address.Assign(devices);
    }

    // 3. Populate routes
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // 4. TCP receiver on n4
    uint16_t port = 5000;
    PacketSinkHelper sink(
        "ns3::TcpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), port));

    ApplicationContainer sinkApp = sink.Install(nodes.Get(4));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(10.0));

    // 5. TCP sender on n0; n4 address is the second address on link n3-n4
    BulkSendHelper source(
        "ns3::TcpSocketFactory",
        InetSocketAddress(interfaces[3].GetAddress(1), port));

    source.SetAttribute("MaxBytes", UintegerValue(0));
    source.SetAttribute("SendSize", UintegerValue(1024));

    ApplicationContainer sourceApp = source.Install(nodes.Get(0));
    sourceApp.Start(Seconds(1.0));
    sourceApp.Stop(Seconds(9.0));

    // 6. Enable PCAP packet tracing
    p2p.EnablePcapAll("tcp-5nodes");

    // 7. Install FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    // 8. Run simulation
    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    monitor->CheckForLostPackets();

    // 9. Save performance results to CSV
    std::ofstream csv("tcp_results.csv");
    csv << "protocol,flow_id,source_ip,destination_ip,tx_packets,rx_packets,"
           "lost_packets,rx_bytes,throughput_mbps,avg_delay_seconds\n";

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    auto stats = monitor->GetFlowStats();

    for (auto i = stats.begin(); i != stats.end(); ++i)
    {
        Ipv4FlowClassifier::FiveTuple tuple =
            classifier->FindFlow(i->first);

        // Keep only TCP flows
        if (tuple.protocol != 6)
            continue;

        double throughputMbps = i->second.rxBytes * 8.0 / (8.0 * 1000000.0);
        double avgDelaySeconds = 0.0;

        if (i->second.rxPackets > 0)
            avgDelaySeconds = i->second.delaySum.GetSeconds() /
                              i->second.rxPackets;

        csv << "TCP," << i->first << ","
            << tuple.sourceAddress << ","
            << tuple.destinationAddress << ","
            << i->second.txPackets << ","
            << i->second.rxPackets << ","
            << i->second.lostPackets << ","
            << i->second.rxBytes << ","
            << throughputMbps << ","
            << avgDelaySeconds << "\n";

        std::cout << "\nTCP Flow " << i->first
                  << "\nSource: " << tuple.sourceAddress
                  << "\nDestination: " << tuple.destinationAddress
                  << "\nTx packets: " << i->second.txPackets
                  << "\nRx packets: " << i->second.rxPackets
                  << "\nLost packets: " << i->second.lostPackets
                  << "\nThroughput: " << throughputMbps << " Mbps"
                  << "\nAverage delay: " << avgDelaySeconds << " seconds\n";
    }

    csv.close();

    Ptr<PacketSink> sinkPtr = DynamicCast<PacketSink>(sinkApp.Get(0));
    std::cout << "\nTCP application bytes received: "
              << sinkPtr->GetTotalRx() << "\n";
    std::cout << "Results saved to tcp_results.csv\n";

    Simulator::Destroy();
    return 0;
}
