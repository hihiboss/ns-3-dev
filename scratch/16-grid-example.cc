#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int main(int argc, char* argv[]) {
  	// Set default parameters for PointToPoint links and channels
  	Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  	Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  	Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("10p"));

  	CommandLine cmd;
  	cmd.Parse(argc, argv);

  	// Create 4x4 topology
  	PointToPointHelper p2p;
  	PointToPointGridHelper grid(4, 4, p2p);

  	// Install NDN stack on all nodes
  	ndn::StackHelper ndnHelper;
  	ndnHelper.InstallAll();

  	// Set BestRoute strategy
  	ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

  	// Install global routing interface on all nodes
  	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  	ndnGlobalRoutingHelper.InstallAll();

  	// Get containers for the consumer or producer
  	Ptr<Node> producer = grid.GetNode(3, 3);
  	NodeContainer consumerNodes;
  	consumerNodes.Add(grid.GetNode(0, 0));

  	// Install NDN applications
  	std::string prefix = "/prefix";

  	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  	consumerHelper.SetPrefix(prefix);
  	consumerHelper.SetAttribute("Frequency", StringValue("100"));
  	consumerHelper.Install(consumerNodes);

  	ndn::AppHelper producerHelper("ns3::ndn::Producer");
  	producerHelper.SetPrefix(prefix);
  	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  	producerHelper.Install(producer);

  	// Add /prefix origins to ndn::GlobalRouter
  	ndnGlobalRoutingHelper.AddOrigins(prefix, producer);

  	// Calculate and install FIBs
  	ndn::GlobalRoutingHelper::CalculateRoutes();

  	Simulator::Stop(Seconds(20.0));

  	Simulator::Run();
  	Simulator::Destroy();

  	return 0;
}

}

int main(int argc, char* argv[]) {
  	return ns3::main(argc, argv);
}
