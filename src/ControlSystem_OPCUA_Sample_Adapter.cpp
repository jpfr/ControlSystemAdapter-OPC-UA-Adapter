extern "C" {
	#include <unistd.h>
	#include <signal.h>
}

#include <iostream>

#include "open62541.h"
#include "ControlSystemAdapterOPCUA.h"

#include "ControlSystemSynchronizationUtility.h"
#include "ControlSystemPVManager.h"
#include "DevicePVManager.h"
#include "PVManager.h"
#include <ipc_manager.h>
//#include <DeviceSynchronizationUtility.h>


using std::cout;
using std::endl;
using namespace mtca4u;

/* GLOBAL VAR */
ControlSystemAdapterOPCUA *csaOPCUA;



/* FUNCTIONS */
static void SigHandler_Int(int sign) {
  cout << "Received SIGINT... terminating" << endl;
  csaOPCUA->getIPCManager()->stopAll();
  csaOPCUA->getIPCManager()->terminate();
  cout << "terminated threads" << endl;
}

/*
 * Example main without dynamic lib 
 */

int main() {
	
	signal(SIGINT,  SigHandler_Int); // Registriert CTRL-C/SIGINT
	signal(SIGTERM, SigHandler_Int); // Registriert SIGTERM
      
	// Create the managers
	std::pair<boost::shared_ptr<ControlSystemPVManager>, boost::shared_ptr<DevicePVManager> > pvManagers = createPVManager();
	
	boost::shared_ptr<DevicePVManager> devManager = pvManagers.second;
	boost::shared_ptr<ControlSystemPVManager> csManager = pvManagers.first;
	
	ControlSystemSynchronizationUtility syncUtil(csManager);
	
	/*
	* Generate dummy data
	*/
	ProcessScalar<int32_t>::SharedPtr intAdev = devManager->createProcessScalar<int32_t>(deviceToControlSystem, "intA");
	ProcessScalar<int32_t>::SharedPtr intBdev = devManager->createProcessScalar<int32_t>(deviceToControlSystem, "intB");
	ProcessArray<int32_t>::SharedPtr intCdev = devManager->createProcessArray<int32_t>(deviceToControlSystem, "intC", 10);
	ProcessScalar<int32_t>::SharedPtr intDdev = devManager->createProcessScalar<int32_t>(deviceToControlSystem, "intD");   
	
	std::cout << "Dummy Daten geschrieben..." << std::endl;
	
	// create a new ControlSystemAdapterOPCUA with OPCUA port and created controlsystem Manager
	csaOPCUA = new ControlSystemAdapterOPCUA(16664, pvManagers.first);

	/*
	 * Do whatever you want.
	 * For this, until the server is running -> the application is running
	 */
	while(csaOPCUA->getIPCManager()->isRunning()) {
		sleep(1);
	}
	
    return 0;
}