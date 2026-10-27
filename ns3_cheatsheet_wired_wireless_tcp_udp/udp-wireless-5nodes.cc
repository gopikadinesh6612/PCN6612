#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include <fstream>
using namespace ns3;

int main()
{
    // Five wireless devices total: one AP and four stations
    NodeContainer apNode;
    apNode.Create(1);
    NodeContainer staNodes;
    staNodes.Create(4);

    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.Set("TxPowerStart", DoubleValue(16.0));
    wifiPhy.Set("TxPowerEnd", DoubleValue(16.0));

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
        "DataMode", StringValue("HtMcs7"),
        "ControlMode", StringValue("HtMcs0"));

    Ssid ssid = Ssid("campus-wifi");
    WifiMacHelper wifiMac;

    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevice = wifi.Install(wifiPhy, wifiMac, apNode);

    wifiMac.SetType("ns3::StaWifiMac",
        "Ssid", SsidValue(ssid),
        "ActiveProbing", BooleanValue(false));
    NetDeviceContainer staDevices = wifi.Install(wifiPhy, wifiMac, staNodes);

    // Place AP in the center and stations around it; all nodes stay still.
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positions =
        CreateObject<ListPositionAllocator>();
    positions->Add(Vector(20.0, 20.0, 0.0)); // AP
    positions->Add(Vector(0.0, 0.0, 0.0));   // STA 0
    positions->Add(Vector(40.0, 0.0, 0.0));  // STA 1
    positions->Add(Vector(0.0, 40.0, 0.0));  // STA 2
    positions->Add(Vector(40.0, 40.0, 0.0)); // STA 3
    mobility.SetPositionAllocator(positions);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNode);
    mobility.Install(staNodes);

    NodeContainer allNodes;
    allNodes.Add(apNode);
    allNodes.Add(staNodes);

    InternetStackHelper internet;
    internet.Install(allNodes);

    NetDeviceContainer allWifiDevices;
    allWifiDevices.Add(apDevice);
    allWifiDevices.Add(staDevices);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.2.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(allWifiDevices);

    const uint16_t port = 5000;
    PacketSinkHelper sink(
        "ns3::UdpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(staNodes.Get(3));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(20.0));

    // interfaces: index 0 is AP, indexes 1-4 are STA 0-3.
    OnOffHelper source(
        "ns3::UdpSocketFactory",
        InetSocketAddress(interfaces.GetAddress(4), port));
    source.SetConstantRate(DataRate("1Mbps"), 512);
    ApplicationContainer sourceApp = source.Install(staNodes.Get(0));
    sourceApp.Start(Seconds(1.0));
    sourceApp.Stop(Seconds(20.0));

    wifiPhy.EnablePcap("wireless-udp", apDevice.Get(0));
    wifiPhy.EnablePcap("wireless-udp", staDevices, true);

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();

    // FlowMonitor metrics and CSV output
    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    std::ofstream csv("udp_wireless_results.csv");
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
    std::cout << "\nSaved results to udp_wireless_results.csv\n";

    Ptr<PacketSink> sinkPtr = DynamicCast<PacketSink>(sinkApp.Get(0));
    std::cout << "Application bytes received: " << sinkPtr->GetTotalRx() << "\n";
    Simulator::Destroy();
    return 0;
}
