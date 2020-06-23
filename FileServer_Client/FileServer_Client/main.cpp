// main.cpp : implementation file
//

#include "pch.h"
#include "FileServer_Client.h"
#include "main.h"
#include "afxdialogex.h"
#include <string>	
#include <fstream>
#include "FileServer_ClientDlg.h"
using namespace std;
#define RECV_BUFFER_SIZE 10000
#define SEND_BUFFER_SIZE 10000
#define PORT 25000

int icPort = 30000;
// main dialog

IMPLEMENT_DYNAMIC(main, CDialogEx)

main::main(SOCKET &s, CString name , CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN, pParent)
	, m_log(_T(""))
{
	sClient = s;
	m_msgString = _T("You are logging in as username: ") + name;
	
	Command = _T("5\r\n");
	mSend(Command); //Gửi socketMsg yêu cầu Refresh 
	
	
}

main::~main()
{
}

void main::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_USER, m_msgString);
	DDX_Control(pDX, IDC_FIlE, ListFile_Client);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, BUTTON_UPLOAD);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, BUTTON_DOWNLOAD);
	DDX_Control(pDX, IDC_BUTTON_LOGOUT, BUTTON_LOGOUT);
	DDX_Text(pDX, IDC_LOG, m_log);

	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

}


BEGIN_MESSAGE_MAP(main, CDialogEx)
	ON_MESSAGE(WM_SOCKET,SockMsg)
	ON_EN_CHANGE(IDC_EDIT_USER, &main::OnEnChangeEditUser)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &main::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &main::OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &main::OnBnClickedButtonLogout)
END_MESSAGE_MAP()


// main message handlers

void main::OnEnChangeEditUser()
{

}

void main::Split(CString src, CString des[2])
{
	int p1, p2;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

}

char* main::ConvertToChar(const CString& s)
{
	int nSize = s.GetLength();
	char* pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void main::mSend(CString Command)
{
	int Len = Command.GetLength();
	Len += Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff, 0, 1000);
	memcpy(sendBuff, (PBYTE)(LPCTSTR)Command, Len);
	send(sClient, (char*)&Len, sizeof(Len), 0);
	send(sClient, (char*)sendBuff, Len, 0);
	delete[] sendBuff;
}

int main::mRecv(CString& Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer, 0, 1000);
	recv(sClient, (char*)&buffLength, sizeof(int), 0);
	recv(sClient, (char*)buffer, buffLength, 0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;
	delete[] buffer;
	if (Command.GetLength() == 0)
		return -1;
	return 0;
}

LRESULT main::SockMsg(WPARAM wParam, LPARAM lParam)
{

	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
	{
		CString temp;
		if (mRecv(temp) < 0)
			break;

		Split(temp, strResult);
		int flag1 = _ttoi(strResult[0]);

		switch (flag1)
		{
		case 3: //Refresh
		{

			ListFile_Client.InsertItem(0, strResult[1]);
			UpdateData(FALSE);
			break;
		}
		case 4: //receive file from server
		{	
			char* c_port = ConvertToChar(strResult[1]);
			int iPort = atoi(c_port);
			if (receiveFile(downloadFileName, iPort))
				MessageBox(_T("Download succeed!"));
			else MessageBox(_T("Download failed!"));

			break; 
		}		
		case 5: //Server remove file
		{
			CString fileN(strResult[1]);
			for (int i = 0; i < ListFile_Client.GetItemCount(); i++)
			{
				if (fileN == ListFile_Client.GetItemText(i, 0))
					ListFile_Client.DeleteItem(i);
			}
			UpdateData(FALSE);
			break;
		}
		case 6:
		{
			icPort++;
			string s_port = to_string(icPort);
			CString cs_port(s_port.c_str());

			UpdateData(FALSE);
			Command = _T("6\r\n") + cs_port + _T("\r\n");
			mSend(Command);
			AfxBeginThread(sendFile, 0);
			break;
		}
		case 9:case 10:
		{
			m_log += strResult[1] + _T("\r\n");
			
			UpdateData(FALSE);

			break;
		}
		}
	}
	case FD_CLOSE:
	{
		closesocket(sClient);
		MessageBox(_T("Server has been shutdown!"), _T("Error"), MB_OK | MB_ICONERROR);
		EndDialog(1);
		break;
	}
	}
	return 0;
}


bool main::receiveFile(char* file_name, int port)
{
		if (AfxSocketInit() == FALSE)
		{
			return FALSE;
		}
		CSocket ClientSocket;
		ClientSocket.Create();


		// Ket noi den Server
		if (ClientSocket.Connect(_T("127.0.0.1"), port) != 0)
		{

			int file_size = 0, bytes_recevived, bytes_to_receive;
			byte* buffer = NULL;

			// Mo file
			FILE* fo = fopen(file_name, "wb");

			// Nhan kich thuoc file
			int size = 0;
			bytes_to_receive = sizeof(file_size);
			do
			{
				size = (file_size)+sizeof(file_size) - bytes_to_receive;
				bytes_recevived = recv(ClientSocket, (char*)&size, bytes_to_receive, 0);

				if (bytes_recevived == SOCKET_ERROR || bytes_recevived == 0)
					return 0;

				bytes_to_receive -= bytes_recevived;
			} while (bytes_to_receive > 0);

			// Nhan file's data
			buffer = new byte[RECV_BUFFER_SIZE];
			bytes_to_receive = size;

			do {
				int buffer_size;
				buffer_size = (RECV_BUFFER_SIZE > bytes_to_receive) ? bytes_to_receive : RECV_BUFFER_SIZE;

				do {
					bytes_recevived = recv(ClientSocket, (char*)buffer, buffer_size, 0);
				} while (bytes_recevived == -1);

				fwrite((char*)buffer, bytes_recevived, 1, fo);

				memset(buffer, 0, RECV_BUFFER_SIZE);
				bytes_to_receive -= bytes_recevived;
			} while (bytes_to_receive > 0);
			if (buffer) delete[] buffer;
			fclose(fo);
		}
	
		ClientSocket.Close();
	
	return 1;
}

void main::OnBnClickedButtonUpload()
{
	// TODO: Add your control notification handler code here
	CFileDialog t(true);
	if (t.DoModal() == IDOK)
	{
		upLoadFileName = t.GetFileName();
		ListFile_Client.InsertItem(0, t.GetFileName());
		Command = _T("7\r\n") + upLoadFileName + _T("\r\n");
		mSend(Command);

	}
}

void main::OnBnClickedButtonDownload()
{
	// TODO: Add your control notification handler code here
	for (int nItem = 0; nItem < ListFile_Client.GetItemCount(); nItem++)
	{
		if (ListFile_Client.GetItemState(nItem, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CString fileName = ListFile_Client.GetItemText(nItem, 0);
			CString cs = _T("You have downloaded file: ");
			downloadFileName = ConvertToChar(fileName);
			cs += fileName;
			Command = _T("4\r\n") + fileName + _T("\r\n");
			mSend(Command);
			WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
			MessageBox(cs);
		}
	}
}

void main::OnBnClickedButtonLogout()
{
	INT_PTR i = MessageBox(_T("Ban muon logout?"), _T("Confirm"), MB_OKCANCEL);
	if (i == IDCANCEL) return;
	closesocket(sClient);
	EndDialog(1);
	CFileServerClientDlg t;
	t.DoModal();
	
}

void main::Refresh()
{

	ListFile_Client.DeleteAllItems();
	Command = _T("5\r\n");
	mSend(Command); //Gui thong tin username + password ve cho server
	MessageBox(Command);
	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);
}

UINT main::sendFile(LPVOID pParam)
{
	
	{
		// TODO: code your application's behavior here.

		// Khoi tao thu vien Socket
		if (AfxSocketInit() == FALSE)
		{
			return FALSE;
		}

		CSocket ServerSocket; //cha

		if (ServerSocket.Create(icPort, SOCK_STREAM, NULL) == 0)
		{

			return FALSE;
		}
		else
		{
			if (ServerSocket.Listen(32) == FALSE)
			{
				ServerSocket.Close();
				return FALSE;
			}

		}


		CSocket Connector;
		if (ServerSocket.Accept(Connector))
		{
			int file_size, bytes_to_send, bytes_sent;
			byte* buffer = NULL;

			// Mo file
			string file_name;
			FILE* fi = fopen(file_name.c_str(), "rb");
			if (!fi)
				return 0;

			// Lay kich thuoc file
			fseek(fi, 0, SEEK_END);
			file_size = ftell(fi);
			fseek(fi, 0, SEEK_SET);

			// Gui kich thuoc file

			bytes_to_send = sizeof(file_size);
			do
			{
				int size = file_size + sizeof(file_size) - bytes_to_send;
				bytes_sent = send(Connector, (char*)&size, bytes_to_send, 0);

				if (bytes_sent == SOCKET_ERROR)
					return 0;

				bytes_to_send -= bytes_sent;
			} while (bytes_to_send > 0);


			// Gui file's data 
			buffer = new byte[SEND_BUFFER_SIZE];
			bytes_to_send = file_size;// So bytes data can gui

			do {
				int buffer_size;
				buffer_size = (SEND_BUFFER_SIZE > bytes_to_send) ? bytes_to_send : SEND_BUFFER_SIZE;
				fread(buffer, buffer_size, 1, fi);

				// Neu gui bi loi thi gui lai goi tin do
				do {
					bytes_sent = send(Connector, (char*)buffer, buffer_size, 0);
				} while (bytes_sent == -1);

				// Cap nhat lai so bytes can gui
				bytes_to_send -= bytes_sent;
			} while (bytes_to_send > 0);

			if (buffer) delete[] buffer;
			fclose(fi);
		}
		Connector.Close();
		ServerSocket.Close();
	}
	return 1;
	return 0;
}

