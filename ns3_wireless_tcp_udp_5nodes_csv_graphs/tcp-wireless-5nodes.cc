#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/aodv-module.h"

#include <fstream>
#include <sstream>

using namespace ns3;

int main()
{
    // 1. Create five wireless nodes
    NodeContainer nodes;
    nodes.Create(5);

    // 2. Configure Wi-Fi PHY and channel
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    // 3. Configure Wi-Fi MAC in ad-hoc mode
    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    // 4. Give nodes positions in a grid
MobilityHelper mobility;

mobility.SetPositionAllocator("ns3::GridPositionAllocator",
    "MinX", DoubleValue(0.0),
    "MinY", DoubleValue(0.0),
    "DeltaX", DoubleValue(20.0),
    "DeltaY", DoubleValue(20.0),
    "GridWidth", UintegerValue(3),
    "LayoutType", StringValue("RowFirst"));

mobility.SetMobilityModel(
    "ns3::ConstantPositionMobilityModel");

mobility.Install(nodes);

    // 5. Install Internet stack and assign IP addresses
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // 6. TCP receiver on node 4
    uint16_t port = 5000;
    PacketSinkHelper sink(
        "ns3::TcpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), port));

    ApplicationContainer sinkApp = sink.Install(nodes.Get(4));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(10.0));

    // 7. TCP sender on node 0
    BulkSendHelper source(
        "ns3::TcpSocketFactory",
        InetSocketAddress(interfaces.GetAddress(4), port));

    source.SetAttribute("MaxBytes", UintegerValue(0));
    source.SetAttribute("SendSize", UintegerValue(1024));

    ApplicationContainer sourceApp = source.Install(nodes.Get(0));
    sourceApp.Start(Seconds(1.0));
    sourceApp.Stop(Seconds(9.0));

    // 8. Enable PCAP tracing
    phy.EnablePcap("tcp-wireless", devices, true);

    // 9. Install FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    // 10. Run simulation
    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    monitor->CheckForLostPackets();

    // 11. Save results to CSV
    std::ofstream csv("tcp_wireless_results.csv");
    csv << "Protocol,FlowID,SourceIP,DestinationIP,TxPackets,RxPackets,"
           "LostPackets,RxBytes,ThroughputMbps,AverageDelaySeconds\n";

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    auto stats = monitor->GetFlowStats();

    for (auto i = stats.begin(); i != stats.end(); ++i)
    {
        Ipv4FlowClassifier::FiveTuple tuple =
            classifier->FindFlow(i->first);

        if (tuple.protocol != 6)
            continue;

        double duration = 8.0; // sender runs from 1s to 9s
        double throughputMbps =
            i->second.rxBytes * 8.0 / (duration * 1000000.0);
        double avgDelaySeconds = 0.0;

        if (i->second.rxPackets > 0)
            avgDelaySeconds =
                i->second.delaySum.GetSeconds() / i->second.rxPackets;

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
                  << "\nSource IP: " << tuple.sourceAddress
                  << "\nDestination IP: " << tuple.destinationAddress
                  << "\nPackets sent: " << i->second.txPackets
                  << "\nPackets received: " << i->second.rxPackets
                  << "\nPackets lost: " << i->second.lostPackets
                  << "\nThroughput: " << throughputMbps << " Mbps"
                  << "\nAverage delay: " << avgDelaySeconds << " seconds\n";
    }

    csv.close();

    Ptr<PacketSink> sinkPtr = DynamicCast<PacketSink>(sinkApp.Get(0));
    std::cout << "\nTCP application bytes received: "
              << sinkPtr->GetTotalRx() << "\n";
    std::cout << "Saved results to tcp_wireless_results.csv\n";

    Simulator::Destroy();
    return 0;
}
