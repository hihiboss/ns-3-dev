#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int main(int argc, char* argv[]) {
	CommandLine cmd;
  	cmd.Parse(argc, argv);

  	AnnotatedTopologyReader topologyReader("", 1);
  	topologyReader.SetFileName("scratch/11-nodes-with-2-bottlenecks-topology.txt");
  	topologyReader.Read();

  	// Install NDN stack on all nodes
  	ndn::StackHelper ndnHelper;
  	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1"); // ! Attention ! If set to 0, then MaxSize is infinite
  	ndnHelper.InstallAll();

  	// Set BestRoute strategy
  	ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

  	// Get containers for the consumer and producer
  	Ptr<Node> consumers[4] = {
		Names::Find<Node>("c1"), 
		Names::Find<Node>("c2"),
        Names::Find<Node>("c3"), 
		Names::Find<Node>("c4")
	};
  	Ptr<Node> producers[4] = {
		Names::Find<Node>("p1"), 
		Names::Find<Node>("p2"),  
		Names::Find<Node>("p3"), 
		Names::Find<Node>("p4")
	};

  	if (consumers[0] == 0 || consumers[1] == 0 || consumers[2] == 0 || consumers[3] == 0
      || producers[0] == 0 || producers[1] == 0 || producers[2] == 0 || producers[3] == 0) {
    	NS_FATAL_ERROR("Error in topology: one nodes c1, c2, c3, c4, p1, p2, p3, or p4 is missing");
  	}

  	for (int i = 0; i < 4; i++) {
    	std::string prefix = "/data/" + Names::FindName(producers[i]);

    	/////////////////////////////////////////////////////////////////////////////////
    	// install consumer app on consumer node c_i to request data from producer p_i //
    	/////////////////////////////////////////////////////////////////////////////////

    	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    	consumerHelper.SetAttribute("Frequency", StringValue("10"));

    	consumerHelper.SetPrefix(prefix);
    	ApplicationContainer consumer = consumerHelper.Install(consumers[i]);
    	consumer.Start(Seconds(i));
    	consumer.Stop(Seconds(19 - i));

    	///////////////////////////////////////////////
    	// install producer app on producer node p_i //
    	///////////////////////////////////////////////

    	ndn::AppHelper producerHelper("ns3::ndn::Producer");
    	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

    	// install producer that will satisfy Interests in /dst1 namespace
    	producerHelper.SetPrefix(prefix);
    	ApplicationContainer producer = producerHelper.Install(producers[i]);
  	}

  	// Manually configure FIB routes
  	ndn::FibHelper::AddRoute("c1", "/data", "n1", 1); // link to n1
  	ndn::FibHelper::AddRoute("c2", "/data", "n1", 1); // link to n1
  	ndn::FibHelper::AddRoute("c3", "/data", "n1", 1); // link to n1
  	ndn::FibHelper::AddRoute("c4", "/data", "n1", 1); // link to n1

  	ndn::FibHelper::AddRoute("n1", "/data", "n2", 1);  // link to n2
  	ndn::FibHelper::AddRoute("n1", "/data", "n12", 1); // link to n12

  	ndn::FibHelper::AddRoute("n12", "/data", "n2", 1); // link to n2

  	ndn::FibHelper::AddRoute("n2", "/data/p1", "p1", 1); // link to p1
  	ndn::FibHelper::AddRoute("n2", "/data/p2", "p2", 1); // link to p2
  	ndn::FibHelper::AddRoute("n2", "/data/p3", "p3", 1); // link to p3
  	ndn::FibHelper::AddRoute("n2", "/data/p4", "p4", 1); // link to p4

  	// Schedule simulation time and run the simulation
  	Simulator::Stop(Seconds(20.0));
  	Simulator::Run();
  	Simulator::Destroy();

  	return 0;
}

} // namespace ns3

int	main(int argc, char* argv[]) {
  	return ns3::main(argc, argv);
}
