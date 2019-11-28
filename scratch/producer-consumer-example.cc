#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int main(int argc, char* argv[]) {
	// Set default parameters for PointToPoint links and channels
	Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
	Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
	Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("20p"));

	CommandLine cmd;
	cmd.Parse(argc, argv);

	// Create nodes
	NodeContainer nodes;
	nodes.Create(3);

	// Connect nodes using two links
	PointToPointHelper p2p;
	p2p.Install(nodes.Get(0), nodes.Get(1));
	p2p.Install(nodes.Get(1), nodes.Get(2));

	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.InstallAll();

	// Choose forwarding strategy
	ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/multicast");

  	// Consumer
  	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  	consumerHelper.SetPrefix("/prefix");
  	consumerHelper.SetAttribute("Frequency", StringValue("10"));

	// Consumer application
  	auto apps = consumerHelper.Install(nodes.Get(0));
  	apps.Stop(Seconds(10.0));

  	// Producer
  	ndn::AppHelper producerHelper("ns3::ndn::Producer");
  	producerHelper.SetPrefix("/prefix");
  	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  	producerHelper.Install(nodes.Get(2));

  	Simulator::Stop(Seconds(20.0));

  	Simulator::Run();
  	Simulator::Destroy();

  	return 0;
}

}

int main(int argc, char* argv[]) {
  	return ns3::main(argc, argv);
}
