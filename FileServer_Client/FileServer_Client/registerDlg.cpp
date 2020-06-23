// registerDlg.cpp : implementation file
//
#include "pch.h"
#include "stdafx.h"
#include "FileServer_Client.h"
#include "registerDlg.h"
#include "afxdialogex.h"
#include "FileServer_ClientDlg.h"


// registerDlg dialog

IMPLEMENT_DYNAMIC(registerDlg, CDialogEx)

registerDlg::registerDlg(CWnd* pParent)
	: CDialogEx(IDD_REGISTER, pParent)
{

}

registerDlg::~registerDlg()
{
}

void registerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_R_USERNAME, IDC_userName);
	DDX_Control(pDX, IDC_R_PASS, IDC_passWord);
	DDX_Control(pDX, IDC_R_REPASS, IDC_confirmPassWord);
}

BEGIN_MESSAGE_MAP(registerDlg, CDialogEx)
	ON_MESSAGE(WM_SOCKET, SockMsg)
	ON_BN_CLICKED(IDC_Register, &registerDlg::OnBnClickedRegister)
	ON_BN_CLICKED(IDC_Cancel, &registerDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void registerDlg::Split(CString src, CString des[2])
{
	int p1, p2;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

}

char* registerDlg::ConvertToChar(const CString& s)
{
	int nSize = s.GetLength();
	char* pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void registerDlg::mSend(CString Command)
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

int registerDlg::mRecv(CString& Command)
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

LRESULT registerDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
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
		case 2:
			if (strResult[1] == "1")
			{
				MessageBox(_T("Register Success!"));
				EndDialog(1);

			}
			else MessageBox(_T("Username has already taken!"), _T("Error"), MB_OK | MB_ICONERROR);
			break;
		}

	}
	case FD_CLOSE:

		UpdateData(FALSE);
		break;

	}
	return 0;
}


void registerDlg::OnBnClickedRegister()
{
	UpdateData(TRUE);

	CString in_userName;
	CString in_passWord;
	CString in_confirmPassWord;

	IDC_userName.GetWindowText(in_userName);
	IDC_passWord.GetWindowText(in_passWord);
	IDC_confirmPassWord.GetWindowText(in_confirmPassWord);

	CString IP = _T("127.0.0.1");

	if (in_userName == "" || in_passWord == "" || in_confirmPassWord == "")
	{
		MessageBox(_T("Username, password and confirm password are required"));
		return;
	}

	if (in_passWord != in_confirmPassWord)
	{
		MessageBox(_T("password is not the same"));
		return;
	}
	sClient = socket(AF_INET, SOCK_STREAM, 0);
	hostent* host = NULL;
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(_T("socket() failed"), _T("ERROR"), 0);
		return;
	}

	servAdd.sin_family = AF_INET;
	servAdd.sin_port = htons(PORT);

	char* cIP = ConvertToChar(IP);

	servAdd.sin_addr.s_addr = inet_addr(cIP);

	CStringA cpy_IP(IP);

	if (servAdd.sin_addr.s_addr == INADDR_NONE)
	{
		host = (gethostbyname(cpy_IP));
		if (host == NULL)
		{
			MessageBox(_T("Can not connect to server."), _T("ERROR"), 0);
		}
		CopyMemory(&servAdd.sin_addr, host->h_addr_list[0], host->h_length);
		return;
	}

	int err = connect(sClient, (struct sockaddr*)&servAdd, sizeof(servAdd));
	if (err == SOCKET_ERROR) {
		MessageBox(_T("Connect failed"), _T("ERROR"), 0);
		return;
	}
	 
	Command = _T("2\r\n");
	Command += in_userName + _T(" ") + in_passWord + _T("\r\n");

	mSend(Command);

	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);
}


void registerDlg::OnBnClickedCancel()
{
	EndDialog(1);
}
