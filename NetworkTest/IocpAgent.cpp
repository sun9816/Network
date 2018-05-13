#include "IocpAgent.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace Network;

IocpAgent::IocpAgent() :
	m_hIOCompletionPort(INVALID_HANDLE_VALUE) {
	_initSocket();
}

IocpAgent::~IocpAgent() {
	stop();
	::WSACleanup();
}

bool IocpAgent::start() {
	return _initIOCP() 
		&& _startThread();
}

bool IocpAgent::stop() {
	for (size_t i = 0; i < m_vtWorkerThread.size(); ++i) {
		PostQueuedCompletionStatus(m_hIOCompletionPort, 0, static_cast<ULONG_PTR>(EXIT_CODE), NULL);
	}

	for (auto & it : m_vtWorkerThread) {
		it->join();
	}

	m_vtWorkerThread.clear();
	m_vtSocketContent.clear();
	RELEASE_HANDLE(m_hIOCompletionPort);

	return true;
}

bool IocpAgent::connect(const String _Address) {
	SocketContent *pSocketContent = newSocketContent();
	do {
		pSocketContent->m_Socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		sockaddr_in clientAddr = { 0 };
		clientAddr.sin_family = AF_INET;
		if (::bind(pSocketContent->m_Socket, reinterpret_cast<sockaddr *>(&clientAddr), sizeof(clientAddr)) == SOCKET_ERROR) {
			break;
		}

		if (::CreateIoCompletionPort(reinterpret_cast<HANDLE>(pSocketContent->m_Socket),
			m_hIOCompletionPort, 
			reinterpret_cast<ULONG_PTR>(pSocketContent),
			0) == NULL) {
			break;
		}

		pSocketContent->m_Sockaddr.sin_family = AF_INET;
		inet_pton(AF_INET, _Address.split(":")[0].stdstr().c_str(), &pSocketContent->m_Sockaddr.sin_addr);
		pSocketContent->m_Sockaddr.sin_port = htons(_Address.split(":")[1].toInt());
		pSocketContent->m_Sockaddr.sin_family = AF_INET;

		if (!_postConnect(pSocketContent)) {
			break;
		}

		return true;
	} while (false);

	deleteSocketContent(pSocketContent);
	return false;
}

bool IocpAgent::send(SocketContent *_Socket, const String & _Buffer) {
	unique_lock<recursive_mutex> lock(m_mutexLock);

	if (_Socket->m_SendBuffer.empty()) {
		_Socket->m_SendBuffer.append(_Buffer);

		IoContent *pIo = newIoContent(_Socket, IOType::IT_SEND);
		pIo->rewriteBuffer(_Socket->m_SendBuffer.data(), _Socket->m_SendBuffer.size());
		return _postSend(_Socket, pIo);
	}
	else {
		_Socket->m_SendBuffer.append(_Buffer);
	}

	return true;
}

void IocpAgent::setRecvCallback(FunCallback _Callback) {
	m_funRecv = _Callback;
}

bool IocpAgent::_doConnect(SocketContent *_Socket, IoContent *_Io) {
	_Io->m_IoType = IOType::IT_RECV;
	return _postRecv(_Socket, _Io);
}

bool IocpAgent::_doRecv(SocketContent *_Socket, IoContent *_Io, DWORD _RecvBytes) {
	_Socket->m_RecvBuffer.append(_Io->m_szBuffer, _RecvBytes);
	
	if (m_funRecv && !m_funRecv(_Socket, _Io, _Socket->m_RecvBuffer)) {
		return false;
	}
	
	return _postRecv(_Socket, _Io);
}

bool IocpAgent::_doSend(SocketContent *_Socket, IoContent *_Io, DWORD _SendBytes) {
	unique_lock<recursive_mutex> lock(m_mutexLock);

	_Socket->m_SendBuffer.erase(_Socket->m_SendBuffer.begin(), _Socket->m_SendBuffer.begin() + _SendBytes);
	if (_Socket->m_SendBuffer.empty()) {
		deleteIoContent(_Socket, _Io);
		return true;
	}
	_Io->rewriteBuffer(_Socket->m_SendBuffer.data(), _Socket->m_SendBuffer.size());
	

	return _postSend(_Socket, _Io);;
}

bool IocpAgent::_doClose(SocketContent *_Socket, IoContent *_Io) {
	deleteSocketContent(_Socket);
	return true;
}

void IocpAgent::_workerThread() {
	SocketContent *pSocketContext = NULL;
	OVERLAPPED *pOverlapped = NULL;
	DWORD dwBytesTransfered = 0;

	while (true) {
		BOOL bRet = GetQueuedCompletionStatus(m_hIOCompletionPort, &dwBytesTransfered, reinterpret_cast<PULONG_PTR>(&pSocketContext), &pOverlapped, INFINITE);
		if (pSocketContext == EXIT_CODE) {
			break;
		}

		if (bRet) {
			IoContent* _Io = CONTAINING_RECORD(pOverlapped, IoContent, m_Overlapped);
			bool bResult = false;

			if (dwBytesTransfered > 0 || _Io->m_IoType == IOType::IT_CONNECT) {
				switch (_Io->m_IoType) {
				case IOType::IT_CONNECT:
					bResult = _doConnect(pSocketContext, _Io);
					break;
				case IOType::IT_RECV:
					bResult = _doRecv(pSocketContext, _Io, dwBytesTransfered);
					break;
				case IOType::IT_SEND:
					bResult = _doSend(pSocketContext, _Io, dwBytesTransfered);
					break;
				}
			}

			if (!bResult) {
				_doClose(pSocketContext, _Io);
			}
		}
		else {
			IoContent* _Io = CONTAINING_RECORD(pOverlapped, IoContent, m_Overlapped);
			OutputDebugString(TEXT("error"));
			switch (::WSAGetLastError()) {
			case ERROR_OPERATION_ABORTED:
				OutputDebugString(TEXT("³¬Ê±"));
				break;
			}
			_doClose(pSocketContext, _Io);
		}

	}
}

bool IocpAgent::_startThread() {
	int m_nProcessors = _cpuNumber() * 2 + 2;

	for (int i = 0; i < m_nProcessors; ++i) {
		m_vtWorkerThread.push_back(make_shared<thread>(&IocpAgent::_workerThread, this));
	}

	return true;
}

bool IocpAgent::_initSocket() {
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);

	GUID GuidConnectEx = WSAID_CONNECTEX;
	if (!_loadFunction(GuidConnectEx, reinterpret_cast<void *&>(m_lpfnConnectEx))) {
		return false;
	}

	return true;
}

bool IocpAgent::_initIOCP() {
	return (m_hIOCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) != NULL;
}

bool IocpAgent::_loadFunction(GUID & funGUID, void *& pFun) {
	DWORD dwBytes = 0;
	SOCKET s = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (s == INVALID_SOCKET) {
		return false;
	}

	pFun = nullptr;
	::WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &funGUID, sizeof(funGUID), &pFun, sizeof(pFun), &dwBytes, nullptr, nullptr);
	::closesocket(s);

	return pFun != nullptr;
}

bool IocpAgent::_postConnect(SocketContent *_Socket) {
	IoContent *_Io = newIoContent(_Socket, IOType::IT_CONNECT);

	BOOL bRet = m_lpfnConnectEx(_Socket->m_Socket,
		reinterpret_cast<sockaddr *>(&_Socket->m_Sockaddr),
		sizeof(_Socket->m_Sockaddr), nullptr, 0, nullptr,
		reinterpret_cast<OVERLAPPED *>(_Io));

	if (!bRet && (WSA_IO_PENDING != WSAGetLastError())) {
		deleteIoContent(_Socket, _Io);
		return false;
	}

	return true;
}

bool IocpAgent::_postRecv(SocketContent *_Socket, IoContent *_Io) {
	if (_Io == nullptr) {
		_Io = newIoContent(_Socket, IOType::IT_RECV);
	}

	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	int nBytesRecv = WSARecv(_Socket->m_Socket, &_Io->m_wsaBuf, 1, &dwBytes, &dwFlags, &_Io->m_Overlapped, nullptr);
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError())) {
		deleteIoContent(_Socket, _Io);
		return false;
	}

	return true;
}

bool IocpAgent::_postSend(SocketContent *_Socket, IoContent *_Io) {
	if (_Io == nullptr) {
		_Io = newIoContent(_Socket, IOType::IT_SEND);
	}

	DWORD dwFlags = 0;
	DWORD dwSendNumBytes = 0;
	int nRet = WSASend(_Socket->m_Socket, &_Io->m_wsaBuf, 1, &dwSendNumBytes, dwFlags, &_Io->m_Overlapped, nullptr);
	if ((SOCKET_ERROR == nRet) && (WSA_IO_PENDING != WSAGetLastError())) {
		deleteIoContent(_Socket, _Io);
		return false;
	}

	return true;
}

int IocpAgent::_cpuNumber() {
	SYSTEM_INFO si = { 0 };
	::GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

SocketContent *IocpAgent::newSocketContent() {
	unique_lock<recursive_mutex> lock(m_mutexLock);

	shared_ptr<SocketContent> socketPtr = make_shared<SocketContent>();
	m_vtSocketContent.push_back(socketPtr);
	return socketPtr.get();
}

IoContent *IocpAgent::newIoContent(SocketContent *_Socket, IOType _Type) {
	unique_lock<recursive_mutex> lock(m_mutexLock);

	return _Socket->newIoContent(_Type);
}

bool IocpAgent::deleteSocketContent(SocketContent *_Socket) {
	unique_lock<recursive_mutex> lock(m_mutexLock);

	auto it = find_if(m_vtSocketContent.begin(), m_vtSocketContent.end(), [=](const std::shared_ptr<SocketContent> & it) {
		return _Socket == it.get();
	});

	if (it == m_vtSocketContent.end()) {
		return false;
	}
	m_vtSocketContent.erase(it);
	return true;
}

bool IocpAgent::deleteIoContent(SocketContent *_Socket, IoContent *_Io) {
	unique_lock<recursive_mutex> lock(m_mutexLock);

	return _Socket->deleteIoContent(_Io);
}