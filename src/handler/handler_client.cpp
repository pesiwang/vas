#include "handler/handler_server.h"
#include "handler/handler_client.h"
#include "event_base.h"

using namespace std;

CHandler_Client::CHandler_Client(int fd, int timeout) : CHandler(fd, timeout)
{
}

CHandler_Client::~CHandler_Client()
{
}

bool CHandler_Client::onRead(map<int, CBuffer*>& results)
{
	if(!CHandler::onRead(results))
		return false;

	CBuffer* buffer = new CBuffer();
	buffer->append(this->_input);
	this->_input->shrink(this->_input->size());

	results[this->_fd] = buffer;
	return true;
}
