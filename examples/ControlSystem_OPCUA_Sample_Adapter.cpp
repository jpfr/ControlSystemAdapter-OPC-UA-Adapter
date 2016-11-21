/*
 * Copyright (c) 2016 Julian Rahm <Julian.Rahm@tu-dresden.de>, Chris Iatrou <Chris_Paul.Iatrou@tu-dresden.de>
 * Chair for Process Systems Engineering
 * Technical University of Dresden
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */


extern "C" {
    #include <unistd.h>
    #include <signal.h>
		#include "mtca_namespaceinit_generated.h" // Output des pyUANamespacecompilers
}


#include <iostream>
#include <math.h>

#include <chrono>
#include <time.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */
#include <sys/sysinfo.h>

#include "open62541.h"
#include "ControlSystemAdapterOPCUA.h"

#include "ChimeraTK/ControlSystemAdapter/ControlSystemSynchronizationUtility.h"
#include "ChimeraTK/ControlSystemAdapter/ControlSystemPVManager.h"
#include "ChimeraTK/ControlSystemAdapter/DevicePVManager.h"
#include "ChimeraTK/ControlSystemAdapter/PVManager.h"
#include "ipc_manager.h"

#include "runtimeValueGenerator.h"

using namespace std;
using namespace ChimeraTK;

/* GLOBAL VAR */
ControlSystemAdapterOPCUA *csaOPCUA;

/* FUNCTIONS */
static void SigHandler_Int(int sign) {
    cout << "Received SIGINT... terminating" << endl;
    csaOPCUA->stop();
    csaOPCUA->terminate();
    cout << "terminated threads" << endl;
}

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
// 	ProcessScalar<int8_t>::SharedPtr intA8dev = devManager->createProcessScalar<int8_t>(controlSystemToDevice, "int8Scalar");
// 	ProcessScalar<uint8_t>::SharedPtr intAu8dev = devManager->createProcessScalar<uint8_t>(deviceToControlSystem, "uint8Scalar");
// 	ProcessScalar<int16_t>::SharedPtr intA16dev = devManager->createProcessScalar<int16_t>(deviceToControlSystem, "int16Scalar");
// 	ProcessScalar<uint16_t>::SharedPtr intAu16dev = devManager->createProcessScalar<uint16_t>(controlSystemToDevice, "uint16Scalar");
// 	ProcessScalar<int32_t>::SharedPtr intA32dev = devManager->createProcessScalar<int32_t>(controlSystemToDevice, "int32Scalar");
// 	ProcessScalar<uint32_t>::SharedPtr intAu32dev = devManager->createProcessScalar<uint32_t>(controlSystemToDevice, "uint32Scalar");
// 	ProcessScalar<float>::SharedPtr intAfdev = devManager->createProcessScalar<float>(controlSystemToDevice, "floatScalar");
// 	ProcessScalar<double>::SharedPtr intAddev = devManager->createProcessScalar<double>(controlSystemToDevice, "doubleScalar");
	
	ProcessArray<int8_t>::SharedPtr intB15A8dev = devManager->createProcessArray<int8_t>(controlSystemToDevice, "int8Array_s15", 15);
	ProcessArray<uint8_t>::SharedPtr intB10Au8dev = devManager->createProcessArray<uint8_t>(controlSystemToDevice, "uint8Array_s10", 10);
	ProcessArray<int16_t>::SharedPtr intB15A16dev = devManager->createProcessArray<int16_t>(controlSystemToDevice, "int16Array_s15", 15);
	ProcessArray<uint16_t>::SharedPtr intB10Au16dev = devManager->createProcessArray<uint16_t>(controlSystemToDevice, "uint16Array_s10", 10);
	ProcessArray<int32_t>::SharedPtr intB15A32dev = devManager->createProcessArray<int32_t>(controlSystemToDevice, "int32Array_s15", 15);
	ProcessArray<uint32_t>::SharedPtr intB10Au32dev = devManager->createProcessArray<uint32_t>(controlSystemToDevice, "uint32Array_s10", 10);
	ProcessArray<double>::SharedPtr intB15Afdev = devManager->createProcessArray<double>(controlSystemToDevice, "doubleArray_s15", 15);
	ProcessArray<float>::SharedPtr intB10Addev = devManager->createProcessArray<float>(controlSystemToDevice, "floatArray_s10", 10);
	
	csManager->getProcessArray<int32_t>("int32Array_s15")->get().at(0) = 12;
	csManager->getProcessArray<int32_t>("int32Array_s15")->get().at(1) = 13;
	csManager->getProcessArray<int32_t>("int32Array_s15")->get().at(2) = 14;
	csManager->getProcessArray<int32_t>("int32Array_s15")->get().at(3) = 15;
	csManager->getProcessArray<int32_t>("int32Array_s15")->get().at(4) = 16;
	
	// data generation cycle time in ms
	ProcessArray<int32_t>::SharedPtr dtDev = devManager->createProcessArray<int32_t>(controlSystemToDevice, "dt", 1);
	// time since server start in ms
	ProcessArray<int32_t>::SharedPtr tDev = devManager->createProcessArray<int32_t>(deviceToControlSystem, "t", 1);
	ProcessArray<int32_t>::SharedPtr periodDev = devManager->createProcessArray<int32_t>(controlSystemToDevice, "period", 1);
	ProcessArray<double>::SharedPtr amplitudeDev = devManager->createProcessArray<double>(controlSystemToDevice, "amplitude", 1);
	ProcessArray<double>::SharedPtr double_sineDev = devManager->createProcessArray<double>(deviceToControlSystem, "double_sine", 1);
	ProcessArray<int32_t>::SharedPtr int_sineDev = devManager->createProcessArray<int32_t>(deviceToControlSystem, "int_sine", 1);
	
	// example mapping variable
	ProcessArray<int8_t>::SharedPtr intB15A8devMap = devManager->createProcessArray<int8_t>(controlSystemToDevice, "Mein/Name_ist#int8Array_s15", 15);
	ProcessArray<uint8_t>::SharedPtr intB10Au8devMap1 = devManager->createProcessArray<uint8_t>(controlSystemToDevice, "Dein/Name/ist/uint8Array_s10", 10);
	ProcessArray<uint8_t>::SharedPtr intB10Au8devMap2 = devManager->createProcessArray<uint8_t>(controlSystemToDevice, "Unser/Name/ist_uint8Array_s10", 10);
	ProcessArray<uint32_t>::SharedPtr intAu32devMap = devManager->createProcessArray<uint32_t>(controlSystemToDevice, "Ist/Name/dieser/uint32Scalar", 1);
	ProcessArray<int32_t>::SharedPtr intA32devMap = devManager->createProcessArray<int32_t>(controlSystemToDevice, "Ist/Name/dieser/int32Scalar", 1);
	ProcessArray<double>::SharedPtr doubledevMap = devManager->createProcessArray<double>(controlSystemToDevice, "Ist/Name/dieser/doubleScalar", 1);
	
	// start values
	int32_t microseconds = 1000000;
	int32_t period = 3.141;
	double amplitude = 10;
		
	// Set values
	csManager->getProcessArray<int32_t>("dt")->set(vector<int32_t> {microseconds});
	csManager->getProcessArray<int32_t>("period")->set(vector<int32_t> {period});
	csManager->getProcessArray<double>("amplitude")->set(vector<double> {amplitude});
	cout << "Dummy Daten geschrieben..." << std::endl;	
	
	// Only for ValueGenerator
	ipc_manager *mgr;
	csaOPCUA = new ControlSystemAdapterOPCUA(csManager, "../uamapping.xml");
	mgr = csaOPCUA->getIPCManager();
	runtimeValueGenerator *valGen = new runtimeValueGenerator(csManager);
	mgr->addObject(valGen);
	mgr->doStart();	
	
	// Server is running
	std::cout << "Server läuft..." << std::endl;	
	while(csaOPCUA->isRunning()) {
		// do something crazy shit
	}
	
	return 0;
}