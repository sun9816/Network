#include "IocpAgent.h"
#include <iostream>
using namespace std;
using namespace Network;


int main(void) {

	IocpAgent iocp;
	iocp.start();

	iocp.setRecvCallback([=](SocketContent *_Socket, IoContent *_Io, String & _Buffer) {
		cout << _Buffer.stdstr().c_str() << endl;
		_Buffer.clear();
		return true;
	});

	iocp.connect("127.0.0.1:8080");
	getchar();
	return 0;
}