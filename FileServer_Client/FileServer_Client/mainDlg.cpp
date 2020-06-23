// mainDlg.cpp : implementation file
//

#include "pch.h"
#include "FileServer_Client.h"
#include "mainDlg.h"
#include "afxdialogex.h"
#include <string>	
#include <fstream>

#define RECV_BUFFER_SIZE 10000
#define SEND_BUFFER_SIZE 10000
#define PORT 25000
using namespace std;
// mainDlg dialog

IMPLEMENT_DYNAMIC(mainDlg, CDialog)

mainDlg::mainDlg(SOCKET s, CString name, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MAIN, pParent)
{
	sClient = s;
}

mainDlg::~mainDlg()
{
}

void mainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(mainDlg, CDialog)
END_MESSAGE_MAP()


// mainDlg message handlers
void mainDlg::OnEnChangeEditUser()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void mainDlg::Split(CString src, CString des[2])
{
	int p1, p2;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

}

char* mainDlg::ConvertToChar(const CString& s)
{
	int nSize = s.GetLength();
	char* pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void mainDlg::mSend(CString Command)
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

int mainDlg::mRecv(CString& Command)
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

LRESULT mainDlg::SockMsg(WPARAM wParam, LPARAM lParam)
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
			break;
		}
		case 4: //receive file from server
		{	//char* fileName = ConvertToChar(strResult[1]);
			char* c_port = ConvertToChar(strResult[1]);
			int iPort = atoi(c_port);
			CString cs_port(c_port);
			if (receiveFile(downloadFileName, iPort))
				MessageBox(_T("Download succeed!"));
			else MessageBox(_T("Download failed!"));

			break;
		}
		}
	}
	}
	return 0;
}


bool mainDlg::receiveFile(char* file_name, int port)
{
	/*if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));

	}
	else*/
	{
		// TODO: code your application's behavior here.

		// Khoi tao Thu vien
		if (AfxSocketInit() == FALSE)
		{
			//cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}

		// Tao socket dau tien
		CSocket ClientSocket;
		ClientSocket.Create();

		// Ket noi den Server
		if (ClientSocket.Connect(_T("127.0.0.1"), port) != 0)
		{
			//cout << "Ket noi toi Server thanh cong !!!" << endl << endl;
			///-------------------------------------------------------------------------------
			/// Tham so truyen vao: char* file_name, CSocket ClientSocket
			///-----------------------------------------------------------------------------------
			// Khai bao
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
			///-------------------------------------------------------------------------------
		}
		else
		{
			//cout << "Khong the ket noi den Server !!!" << endl;
		}

		// Dong ket noi
		ClientSocket.Close();
	}
	return 1;
}


void mainDlg::OnBnClickedButtonUpload()
{
	// TODO: Add your control notification handler code here
	CFileDialog t(true);
	if (t.DoModal() == IDOK)
	{
		//gửi thông điệp + IP client
		//tạo socket
		//gửi file 
	}
}

void mainDlg::OnBnClickedButtonDownload()
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
			//MessageBox(cs);
		}
	}
}

void mainDlg::OnBnClickedButtonLogout()
{
	INT_PTR i = MessageBox(_T("Ban muon logout?"), _T("Confirm"), MB_OKCANCEL);
	if (i == IDCANCEL) return;
	closesocket(sClient);
	EndDialog(1);
}

void mainDlg::Refresh()
{

	/*ListFile_Client.DeleteAllItems();*/
	Command = _T("5\r\n");
	mSend(Command); //Gui thong tin username + password ve cho server
	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	/*UpdateData(FALSE);*/
}

UINT mainDlg::sendFile(LPVOID pParam)
{
	/*if (!AfxWiznInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
	}
	else*/
	{
		// TODO: code your application's behavior here.

		// Khoi tao thu vien Socket
		if (AfxSocketInit() == FALSE)
		{
			//cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}

		CSocket ServerSocket; //cha

		if (ServerSocket.Create(PORT, SOCK_STREAM, NULL) == 0) //SOCK_STREAM or SOCK_DGRAM.
		{

			return FALSE;
		}
		else
		{
			//cout << "Server khoi tao thanh cong !!!" << endl;

			if (ServerSocket.Listen(32) == FALSE)
			{
				//cout << "Khong the lang nghe tren port nay !!!" << endl;
				ServerSocket.Close();
				return FALSE;
			}

		}


		CSocket Connector;
		// Khoi tao mot socket de duy tri viec ket noi va trao doi du lieu
		if (ServerSocket.Accept(Connector))
		{
			//cout << "Da co Client ket Noi !!!" << endl << endl;
			///-----------------------------------------------------------------------------------
			/// Tham so truyen vao: char* file_name, CSocket Connector
			///-----------------------------------------------------------------------------------
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
			///------------------------------------------------------------------------------------
		}
		Connector.Close();
		ServerSocket.Close();
	}
	return 1;
	return 0;
}
