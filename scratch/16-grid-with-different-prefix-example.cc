#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

using namespace ns3;
using ns3::ndn::StackHelper;
using ns3::ndn::AppHelper;
using ns3::ndn::GlobalRoutingHelper;
using ns3::ndn::StrategyChoiceHelper;

int main(int argc, char* argv[]) {
  	// Set default parameters for PointToPoint links and channels
  	Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  	Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  	Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("10p"));

  	// Read optional command-line parameters
  	CommandLine cmd;
  	cmd.Parse(argc, argv);

  	// Creating 4x4 topology
  	PointToPointHelper p2p;
  	PointToPointGridHelper grid(4, 4, p2p);

  	// Install NDN stack on all nodes
  	StackHelper ndnHelper;
  	ndnHelper.InstallAll();

  	// Install global routing interface on all nodes
  	GlobalRoutingHelper ndnGlobalRoutingHelper;
  	ndnGlobalRoutingHelper.InstallAll();

  	// Get pointers to the producer and consumer nodes
  	Ptr<Node> producer1 = grid.GetNode(3, 3);
  	Ptr<Node> producer2 = grid.GetNode(0, 3);
  	Ptr<Node> consumer1 = grid.GetNode(0, 0);
  	Ptr<Node> consumer2 = grid.GetNode(3, 0);

  	// Define two name prefixes
  	std::string prefix1 = "/prefix1";
  	std::string prefix2 = "/prefix2";

  	// Install different forwarding strategies for prefix1, prefix2
  	StrategyChoiceHelper::InstallAll(prefix1, "/localhost/nfd/strategy/multicast");
  	StrategyChoiceHelper::InstallAll(prefix2, "/localhost/nfd/strategy/best-route");

  	// Install NDN applications
  	AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  	consumerHelper.SetPrefix(prefix1);
  	consumerHelper.SetAttribute("Frequency", StringValue("100"));
  	consumerHelper.Install(consumer1);

  	consumerHelper.SetPrefix(prefix2);
  	consumerHelper.Install(consumer2);

  	AppHelper producerHelper("ns3::ndn::Producer");
  	producerHelper.SetPrefix(prefix1);
  	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  	producerHelper.Install(producer1);

  	producerHelper.SetPrefix(prefix2);
  	producerHelper.Install(producer2);

  	// Add /prefix1 and /prefix2 origins to ndn::GlobalRouter
  	ndnGlobalRoutingHelper.AddOrigins(prefix1, producer1);
  	ndnGlobalRoutingHelper.AddOrigins(prefix2, producer2);

  	// Calculate and install FIBs
  	GlobalRoutingHelper::CalculateRoutes();

  	Simulator::Stop(Seconds(20.0));
  	Simulator::Run();
  	Simulator::Destroy();

  	return 0;
}
