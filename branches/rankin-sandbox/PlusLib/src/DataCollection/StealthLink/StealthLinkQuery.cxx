#include <StealthLink/StealthLink.h>
#include <iostream>

using namespace std;
using namespace MNavStealthLink;

const char* const HOST = "130.15.7.88";



/*******************************************************************************
 * The main program. Here it's just written for Windows. Take a
 * command-line argument: the host on which the StealthLink 2
 * application is running.
 */
int main(int argc, char* argv[])
{
    using namespace std;
    using namespace MNavStealthLink;

   // if (argc != 2 || argv[1] == NULL) {
     //   cerr << "Usage: " << *argv << " <Stealth hostname>" << endl;
       // return -1;
    //}

    // Create a StealthServer object.
    //
   // StealthServer server(argv[1]);
	 StealthServer server(HOST);
    Error err;
	
    // Connect to the server
    if (!server.connect(err)) {
        cerr << "Failed to connect to Stealth server application on host "
             << server.getHost() << ", port " << server.getPort() << ": "
             << err.reason() << endl;
        return -1;
    }
    cout << "Connected to server on host " << server.getHost()
         << " on port " << server.getPort() << endl;

    // This is a very simple program. We just print out all the
    // interesting facts from the StealthLink 2 server that we    // can--sometimes in a couple different ways
    //
    Version             version;
    DateTime            time;
	
	// Try different classes 
	Frame			frame; 
	Instrument		inst;
	LocalizerInfo	localizerInfo;
	Exam			exam;	
	NavData			navData;
	Registration	registration;

	// require the necessary information from the server
	server.get(inst,time);
	server.get(frame,time);
	server.get(localizerInfo,time);
	server.get(exam,time);
	server.get(navData,time);
	server.get(registration,time);

	// use the information
	LocalizerInfo::LocalizerType typeLoclizer = localizerInfo.type;
	std::cout<< "Inst Transform [0][0] = "  << inst.localizer_T_instrument[0][1] << std::endl;
    std::cout<< "navData Transform [0][0] = "  << navData.localizer_T_instrument[0][1] << std::endl;
	std::cout << " R info " << " F Name " << registration.name << " Num of Points " << registration.numPoints << " R type " << registration.type << std::endl;
	try {
        server.get(version, time);
    }
    catch (exception e) {
        cerr << "Couldn't even get the server's StealthLink version: "
             << e.what() << endl;
        return -1;
    }

    cout << "On the server, StealthLink version " << version
         << " at server time " << time << endl;

    cout << "The footswitch is "
         << (server.getFootswitch().state == Footswitch::DOWN ? "down" : "up")
         << endl;

    // The following will not throw on errors in the StealthLink
    // system. Rather, 'err' gets set, and it returns false.
    //
    InstrumentNameList  instrumentList;
	InstProjection		insProjection;
    if (!server.get(instrumentList, err)) {
       cerr << "Trouble in paradise! Error: " << err.reason() << endl;
       return 1;
    }

    // Must have succeeded in getting the list of instruments.
    //

    cout << "Instrument List: " << endl;
    int index = 1;
    for (InstrumentNameList::iterator i = instrumentList.begin();
         i != instrumentList.end(); i++) {
        cout << "   Instrument " << index++ << ": " << *i << endl;
    }
	cin >> index;
    server.disconnect();    // not really necessary as the destructor will do this.
    return 0;
}