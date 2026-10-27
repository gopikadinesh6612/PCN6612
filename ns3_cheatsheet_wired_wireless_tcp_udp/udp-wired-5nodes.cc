#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include "ns3/point-to-point-module.h"
#include <fstream>
#include <sstream>
using namespace ns3;

int main()
{
    // Five wired nodes in a line: n0 -- n1 -- n2 -- n3 -- n4
    NodeContainer nodes;
    nodes.Create(5);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    p2p.SetQueue("ns3::DropTailQueue<Packet>",
                 "MaxSize", StringValue("50p"));

    Ipv4AddressHelper ipv4;
    Ipv4InterfaceContainer linkInterfaces[4];

    for (uint32_t i = 0; i < 4; ++i)
    {
        NetDeviceContainer linkDevices =
            p2p.Install(nodes.Get(i), nodes.Get(i + 1));

        std::ostringstream subnet;
        subnet << "10.1." << (i + 1) << ".0";
        ipv4.SetBase(subnet.str().c_str(), "255.255.255.0");
        linkInterfaces[i] = ipv4.Assign(linkDevices);
    }

    InternetStackHelper internet;
    internet.Install(nodes);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    const uint16_t port = 5000;
    PacketSinkHelper sink(
        "ns3::UdpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(nodes.Get(4));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(20.0));

    OnOffHelper udpSource(
        "ns3::UdpSocketFactory",
        InetSocketAddress(linkInterfaces[3].GetAddress(1), port));
    udpSource.SetConstantRate(DataRate("1Mbps"), 512);
    ApplicationContainer sourceApp = udpSource.Install(nodes.Get(0));
    sourceApp.Start(Seconds(1.0));
    sourceApp.Stop(Seconds(20.0));

    p2p.EnablePcapAll("wired-udp");
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();

    // FlowMonitor metrics and CSV output
    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    std::ofstream csv("udp_wired_results.csv");
    csv << "Protocol,FlowID,SourceIP,DestinationIP,TxPackets,RxPackets,"
           "LostPackets,LossPercent,RxBytes,ThroughputMbps,AverageDelayMs,PDRPercent\n";

    auto stats = monitor->GetFlowStats();

    for (const auto &kv : stats)
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(kv.first);
        const auto &s = kv.second;

        if (tuple.protocol != 17 || tuple.destinationPort != 5000)
            continue;

        double duration = (s.timeLastRxPacket - s.timeFirstTxPacket).GetSeconds();
        double throughputMbps =
            (s.rxPackets > 0 && duration > 0.0)
                ? (s.rxBytes * 8.0 / duration / 1000000.0) : 0.0;
        double delayMs =
            (s.rxPackets > 0) ? (s.delaySum.GetSeconds() / s.rxPackets * 1000.0) : 0.0;
        double lossPercent =
            (s.txPackets > 0)
                ? (100.0 * (s.txPackets - s.rxPackets) / s.txPackets) : 0.0;
        double pdrPercent =
            (s.txPackets > 0) ? (100.0 * s.rxPackets / s.txPackets) : 0.0;

        csv << "UDP," << kv.first << ","
            << tuple.sourceAddress << ","
            << tuple.destinationAddress << ","
            << s.txPackets << ","
            << s.rxPackets << ","
            << s.lostPackets << ","
            << lossPercent << ","
            << s.rxBytes << ","
            << throughputMbps << ","
            << delayMs << ","
            << pdrPercent << "\n";

        std::cout << "\nUDP Flow " << kv.first
                  << " (" << tuple.sourceAddress << " -> "
                  << tuple.destinationAddress << "):\n"
                  << "Packets sent: " << s.txPackets << "\n"
                  << "Packets received: " << s.rxPackets << "\n"
                  << "Packet loss: " << lossPercent << " %\n"
                  << "Throughput: " << throughputMbps << " Mbps\n"
                  << "Average delay: " << delayMs << " ms\n"
                  << "PDR: " << pdrPercent << " %\n";
    }

    csv.close();
    std::cout << "\nSaved results to udp_wired_results.csv\n";

    Ptr<PacketSink> sinkPtr = DynamicCast<PacketSink>(sinkApp.Get(0));
    std::cout << "Application bytes received: " << sinkPtr->GetTotalRx() << "\n";
    Simulator::Destroy();
    return 0;
}
