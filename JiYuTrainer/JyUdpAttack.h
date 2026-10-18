#pragma once
#include "stdafx.h"
#include <list>
#include <string>
#include <vector>
#include <WS2spi.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include "CAutoLock.h"
#include "JyPacket.h"

#define WM_MY_SCAN_IP_FINISH WM_USER+301
#define WM_MY_SCAN_IP_ADD WM_USER+302
#define WM_MY_SCAN_ADD_STR WM_USER+303
#define WM_MY_GET_SM_TICP_PORT_FINISH WM_USER+304
#define WM_MY_SEND_ADD_RESULT WM_USER+305

#define NOT_FOUND_JY_PORT 0xFFFFA
#define FAILED_FOUND_JY_PORT 0xFFFFB
#define SEND_BUFFER_SIZE JyPacket::kSendBufferSize
#define PACK_BUFFER_SIZE JyPacket::kBasePacketSize

struct JyNetworkIP {
	std::wstring hostName;
	std::wstring ipAddress;
};
class JyUdpAttack
{
private:

	bool isSaningIPs = false;
	std::wstring saningIPsAddrStart;
	std::wstring saningIPsAddrEnd;
	std::wstring saningIPsAddrPrev;
	std::wstring saningIPsAddrCurrent;
	CAutoLock saningIPsLock; 
	in_addr saningIPsAddrLocal;
	int saningIPsWorkingThread = 0;

public:

	HWND saningIPsReceivehWnd = nullptr;
	HWND checkStudentMainTCPPortReceivehhWnd = nullptr;
	HWND sendResultReceivehWnd = nullptr;

	JyUdpAttack();
	~JyUdpAttack();

	static JyUdpAttack* currentJyUdpAttack;

	//扫描局域网IP
	void ScanNetworkIP(HWND hWnd, std::wstring &start, std::wstring &end, std::wstring& host, std::wstring& current);
	//检测极域的TCP端口
	void CheckStudentMainTCPPort(HWND hWnd);
	//发送cmd
	void SendCommand(std::wstring& ip, DWORD port, std::wstring& cmd);
	//发送信息
	void SendText(std::wstring& ip, DWORD port, std::wstring& cmd);
	//发送关机指令
	void SendShutdown(std::wstring& ip, DWORD port);
	//发送重启指令
	void SendReboot(std::wstring& ip, DWORD port);

private:
	static void TaskFeedbackMessage(struct SendTask* task, std::wstring* msg);
	static DWORD WINAPI SendThread(void* data);
	static DWORD WINAPI CheckStudentMainTCPPortThread(void* data);
	static DWORD WINAPI ScanNetworkIPThread(JyUdpAttack*inst);
	static DWORD WINAPI ScanNetworkIPSubTaskThread(void* data);
};


