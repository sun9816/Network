#pragma once
#include <set>
#include <mutex>
#include <vector>
#include <thread>
#include <memory>
#include <algorithm>

#include <WinSock2.h>
#include <mswsock.h>
#include <Windows.h>
#include <WS2tcpip.h>

#include "String.h"

#define MAX_BUFFER_LEN			(8*1024)
#define EXIT_CODE               NULL
#define RELEASE(x)              {if(x != nullptr){delete x;x=nullptr;}}
#define RELEASE_HANDLE(x)       {if(x != nullptr && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = nullptr;}}
#define RELEASE_SOCKET(x)       {if(x !=INVALID_SOCKET && x != SOCKET_ERROR) { closesocket(x);x=INVALID_SOCKET;}}

namespace Network {

	enum IOType {
		IT_CONNECT	,
		IT_SEND		,
		IT_RECV		,
		IT_NULL		,
	};

	struct IoContent {
		OVERLAPPED		m_Overlapped;
		WSABUF			m_wsaBuf;
		char			m_szBuffer[MAX_BUFFER_LEN];
		IOType			m_IoType;

		IoContent() {
			ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
			ZeroMemory(m_szBuffer, sizeof(m_szBuffer));

			m_wsaBuf.buf = m_szBuffer;
			m_wsaBuf.len = sizeof(m_szBuffer);
			m_IoType = IOType::IT_NULL;
		}

		~IoContent() {
		}

		void rewriteBuffer(const char * _Buffer, size_t _Size) {
			if (_Size > MAX_BUFFER_LEN) {
				_Size = MAX_BUFFER_LEN;
			}
			memcpy(m_szBuffer, _Buffer, _Size);

			m_wsaBuf.buf = m_szBuffer;
			m_wsaBuf.len = _Size;
		}
	};

	struct SocketContent {
		SOCKET										m_Socket;
		sockaddr_in									m_Sockaddr;
		String										m_RecvBuffer;
		String										m_SendBuffer;
		std::vector<std::shared_ptr<IoContent>>		m_vtIoContent;

		SocketContent() {
			ZeroMemory(&m_Socket, sizeof(m_Socket));
			m_Socket = INVALID_SOCKET;
		}

		~SocketContent() {
			m_vtIoContent.clear();
			RELEASE_SOCKET(m_Socket);
		}

		IoContent *newIoContent(IOType _Type) {
			std::shared_ptr<IoContent> pIo = std::make_shared<IoContent>();
			pIo->m_IoType = _Type;
			m_vtIoContent.push_back(pIo);
			return pIo.get();
		}

		bool deleteIoContent(IoContent *_Io) {
			auto it = find_if(m_vtIoContent.begin(), m_vtIoContent.end(), [=](const std::shared_ptr<IoContent> & it) {
				return _Io == it.get();
			});

			if (it == m_vtIoContent.end()) {
				return false;
			}
			m_vtIoContent.erase(it);
			return true;
		}
	};


	typedef std::function<bool(SocketContent *_Socket, IoContent *_Io, String & _Buffer)>	FunCallback;

	class IocpAgent {
	public:
		IocpAgent();
		virtual ~IocpAgent();

	public:
		bool start();
		bool stop();
		bool connect(const String _Address);

		bool send(SocketContent *_Socket, const String & _Buffer);

		void setRecvCallback(FunCallback _Callback);

	protected:
		virtual bool _doConnect(SocketContent *_Socket, IoContent *_Io);
		virtual bool _doRecv(SocketContent *_Socket, IoContent *_Io, DWORD _RecvBytes);
		virtual bool _doSend(SocketContent *_Socket, IoContent *_Io, DWORD _SendBytes);
		virtual bool _doClose(SocketContent *_Socket, IoContent *_Io);

	private:
		void _workerThread();
		bool _startThread();
		bool _initSocket();
		bool _initIOCP();
		bool _loadFunction(GUID & _GUID, void *& _Fun);
		bool _postConnect(SocketContent *_Socket);
		bool _postRecv(SocketContent *_Socket, IoContent *_Io = nullptr);
		bool _postSend(SocketContent *_Socket, IoContent *_Io = nullptr);

	private:
		int _cpuNumber();
		
		SocketContent *newSocketContent();
		IoContent *newIoContent(SocketContent *_Socket, IOType _Type);
		bool deleteSocketContent(SocketContent *_Socket);
		bool deleteIoContent(SocketContent *_Socket, IoContent *_Io);


	private:
		LPFN_CONNECTEX								m_lpfnConnectEx;
		HANDLE										m_hIOCompletionPort;
		std::vector<std::shared_ptr<SocketContent>>	m_vtSocketContent;
		std::vector<std::shared_ptr<std::thread>>	m_vtWorkerThread;
		std::recursive_mutex						m_mutexLock;
		FunCallback									m_funRecv;
	};
}

