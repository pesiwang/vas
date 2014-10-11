#include <iostream>
#include "vas/event_base.h"
#include "vas/helper.h"
#include "echo_observer.h"
#include "echo_listener.h"

using namespace vas;
using namespace std;

int main(int argc, char* argv[]){
	if(argc != 2){
		cerr<<"usage: "<<argv[0]<<" <path_to_config>"<<endl;
		return -1;
	}
	Helper::Config::load(argv[1]);

	EchoListener* listener = new EchoListener(Helper::Socket::listen(Helper::Config::valueAsString("network.host").c_str(), Helper::Config::valueAsInteger("network.port")));
	EchoObserver* observer = EchoObserver::instance();

	EventBase::instance()->addObserver(EventBase::EVENT_STARTED, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_STOPPED, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_TIMER, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_LISTENER_REGISTERED, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_LISTENER_UNREGISTERED, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_SERVICE_REGISTERED, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_SERVICE_UNREGISTERED, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_SERVICE_DATA_ARRIVED, observer);
	EventBase::instance()->addObserver(EventBase::EVENT_SERVICE_DATA_SENT, observer);
	EventBase::instance()->registerListener(listener);
	EventBase::instance()->dispatch();
	return 0;
}
