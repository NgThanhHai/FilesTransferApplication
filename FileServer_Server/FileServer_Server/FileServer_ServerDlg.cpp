
// FileServer_ServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileServer_Server.h"
#include "FileServer_ServerDlg.h"
#include "afxdialogex.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string file_name;
int iPort = 10000;
using namespace std;
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileServerServerDlg dialog



CFileServerServerDlg::CFileServerServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILESERVER_SERVER_DIALOG, pParent)
	, m_msgString(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileServerServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ONLINE_CLIENTS, listClients);
	DDX_Text(pDX, IDC_LOG, m_msgString);
	DDX_Control(pDX, IDC_FILE, listFile);
	

}

BEGIN_MESSAGE_MAP(CFileServerServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SOCKET, SockMsg)

	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CFileServerServerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CFileServerServerDlg::OnBnClickedButtonStop)
	
	ON_BN_CLICKED(IDC_UPLOAD, &CFileServerServerDlg::OnBnClickedUpload)
	ON_BN_CLICKED(IDC_REMOVE, &CFileServerServerDlg::OnBnClickedRemove)

END_MESSAGE_MAP()


// CFileServerServerDlg message handlers

BOOL CFileServerServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	ListView_SetExtendedListViewStyle
	(listFile.m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileServerServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileServerServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileServerServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CFileServerServerDlg::Split(CString src, CString des[2])
{
	int p1, p2;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

}
void Parse(CString source, CString& User, CString& Pass)
{
	CString demp = (CString)"";
	int pos = -1;

	for (int i = 0; i < source.GetLength(); i++) {
		if (source[i] == ' ') {
			User = demp;
			pos = i + 1;
			demp = (CString)"";
			break;
		}
		else {
			demp += source[i];
		}
	}
	for (int i = pos; i < source.GetLength(); i++) {
		demp += source[i];
	}
	Pass = demp;

}
void CFileServerServerDlg::mSend(SOCKET sk, CString Command)
{
	int Len = Command.GetLength();
	Len += Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff, 0, 1000);
	memcpy(sendBuff, (PBYTE)(LPCTSTR)Command, Len);
	send(sk, (char*)&Len, sizeof(Len), 0);
	send(sk, (char*)sendBuff, Len, 0);
	delete sendBuff;
}

int CFileServerServerDlg::mRecv(SOCKET sk, CString& Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer, 0, 1000);
	recv(sk, (char*)&buffLength, sizeof(int), 0);
	recv(sk, (char*)buffer, buffLength, 0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;

	if (Command.GetLength() == 0)
		return -1;
	return 0;

}
bool CFileServerServerDlg::CheckUsernameExist(CString User)
{
	std::ifstream fileIn("account.txt", std::ios::in);
	while (!fileIn.eof())
	{
		std::string line;
		std::getline(fileIn, line);
		size_t found = line.find(" ");
		if (found != std::string::npos)
		{
			std::string user = line.substr(0, found);
			if (ConvertToChar(User) == user)
			{
				fileIn.close();
				return true;
			}
		}
		
	}
	fileIn.close();
	return false;
}
void CFileServerServerDlg::UpdateAccount(CString User, CString Pass)
{
	std::ofstream fileIn("account.txt", std::ios::app);
	fileIn << ConvertToChar(User) << " " << ConvertToChar(Pass)<<std::endl;
	fileIn.close();
}
bool CFileServerServerDlg::CheckAccountExist(CString User, CString Pass)
{
	std::ifstream fileIn("account.txt", std::ios::in);

	while (!fileIn.eof())
	{
		std::string line;
		std::getline(fileIn, line);
		size_t found = line.find(" ");
		if (found != std::string::npos)
		{
			std::string user = line.substr(0, found);
			std::string pass = line.substr(found + 1);
			if (ConvertToChar(User) == user && ConvertToChar(Pass) == pass)
			{
				fileIn.close();
				return true;
			}
		}

	}
	fileIn.close();
	return false;
}
LRESULT CFileServerServerDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{
	CString User, Pass;
	if (WSAGETSELECTERROR(lParam))
	{
		closesocket(wParam);
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
	{
		pSock[numberSocket].sockClient = accept(wParam, NULL, NULL);
		break;
	}
	case FD_READ:
	{
		
		CString temp;
		if (mRecv(wParam, temp) < 0)
			break;
		Split(temp, strResult);
		int flag = _ttoi(strResult[0]);
		char* tem = ConvertToChar(strResult[1]);
		switch (flag)
		{
		case 1://Login
		{
	
			CString User, Pass;
			Parse(strResult[1], User, Pass);
			int flag1 = 0;

			if (numberSocket > 0)
			{
				for (int i = 0; i < numberSocket; i++)
				{
					if (strcmp(ConvertToChar(User), pSock[i].Name) == 0)//user da login
					{
						flag1 = 1;
						break;
					}
				}
			}
			if (CheckAccountExist(User, Pass) == true && flag1 == 0)
			{
				strcpy(pSock[numberSocket].Name, ConvertToChar(User));
				Command = _T("1\r\n1\r\n");
				m_msgString += User + _T(" login\r\n");
				listClients.InsertItem(0, User);
				UpdateData(FALSE);
				numberSocket++;
			}
			else
			{
				Command = _T("1\r\n0\r\n");
			}

			mSend(wParam, Command);
			UpdateData(FALSE);

			if (CheckAccountExist(User, Pass) == true && flag1 == 0)
			{
				CString log = _T("9\r\n") + User + _T(" login\r\n");

				for (int i = 0; i < numberSocket; i++)
				{
					if (pSock[i].sockClient != wParam)
						mSend(pSock[i].sockClient, log);

				}
			}

			break;
		}

		case 2: //register
		{
			CString User, Pass;
			Parse(strResult[1], User, Pass);
			if (CheckUsernameExist(User))
			{
				Command = _T("2\r\n0\r\n");
			}
			else
			{
				Command = _T("2\r\n1\r\n");
				m_msgString += User + _T(" register succesfully\r\n");
				UpdateAccount(User, Pass);
			}
			
			mSend(wParam, Command);
			UpdateData(FALSE);
			break;
		}
		case 4: //Client download file
		{
			int post = -1;
			for (int i = 0; i < numberSocket; i++)
			{
				if (pSock[i].sockClient == wParam)
				{
					if (i < numberSocket)
						post = i;
				}
			}
			
			iPort++;
			string s_port = to_string(iPort);
			CString cs_port(s_port.c_str());

			m_msgString += ((CString)pSock[post].Name + _T(" download file: ") + strResult[1]);
			m_msgString += "\r\n";

			UpdateData(FALSE);
			Command = _T("4\r\n") + cs_port + _T("\r\n");
			mSend(wParam, Command);
			char* fileName = ConvertToChar(strResult[1]);
			string sFileName(fileName);
			file_name = getFilePath(sFileName);
			AfxBeginThread(sendFile, 0);
			break;
		}
		case 5: //refresh file
		{
			CString demp = listFile.GetItemText(0, 0);
			Command = _T("3\r\n") + demp + _T("\r\n");
			mSend(wParam, Command);
			for (int i = 0; i < listFile.GetItemCount(); i++) {
				CString demp = listFile.GetItemText(i, 0);
				Command = _T("3\r\n") + demp + _T("\r\n");
				mSend(wParam, Command);
			}
			UpdateData(FALSE);
			break;
		}
		case 6: //clients upload file
		{
			char* c_port = ConvertToChar(strResult[1]);
			int iPort = atoi(c_port);
			CString cs_port(c_port);
			if (receiveFile(uploadFileName, iPort))
			{
				MessageBox(_T("Failed!"));
				break;
			}
			string path = getExePath() + uploadFileName;
			ofstream fi("filePath.txt", ios::app);
			fi << path << endl;
			
			fi.close();
			CString fn(uploadFileName);
			listFile.InsertItem(0, fn);
			int pos = -1;
			for (int i = 0; i < numberSocket; i++)
			{
				if (pSock[i].sockClient == wParam)
				{
					pos = i;
				}
				else
				{
					CString log = _T("3\r\n") + fn + _T("\r\n");
					mSend(pSock[i].sockClient, log);
				}
			}
			m_msgString += (CString)pSock[pos].Name + _T(" upload file: ") + fn + _T("\r\n");
			UpdateData(FALSE);
			break;
		}
		case 7:
		{
			uploadFileName = ConvertToChar(strResult[1]);
			Command = _T("6\r\n");
			mSend(wParam, Command);
			break;
		}

		}
		break;
	}
	case FD_CLOSE:
	{
		UpdateData();
		int post = -1;
		for (int i = 0; i < numberSocket; i++)
		{
			if (pSock[i].sockClient == wParam)
			{
				post = i;
				break;
			}
		}

		if (post != -1)
		{
			m_msgString += pSock[post].Name;
			m_msgString += _T(" logout\r\n");

			CString User;
			User += pSock[post].Name;
			CString log = _T("10\r\n") + User + _T(" logout\r\n"); //Gui cho cac clients


			CString str;

			for (int i = 0; i < listClients.GetItemCount(); i++)
			{

				CString cs_name(pSock[post].Name);
				if (cs_name == listClients.GetItemText(i, 0))
				{
					listClients.DeleteItem(i);
					break;
				}
			}
			for (int j = post; j < numberSocket - 1; j++)
			{
				pSock[j].sockClient = pSock[j + 1].sockClient;
				strcpy(pSock[j].Name, pSock[j + 1].Name);
			}
			numberSocket--;

			for (int i = 0; i < numberSocket; i++)
			{
				mSend(pSock[i].sockClient, log);
			}
			closesocket(wParam);
		}
		UpdateData(FALSE);
		break;
	}
	}
	return 0;
}

char* CFileServerServerDlg::ConvertToChar(const CString& s)
{
	int nSize = s.GetLength();
	char* pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void CFileServerServerDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(PORT);
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockServer, (SOCKADDR*)&serverAdd, sizeof(serverAdd));
	listen(sockServer, 32);
	int err = WSAAsyncSelect(sockServer, m_hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
	if (err)
		MessageBox((LPCTSTR)"Cant call WSAAsyncSelect");
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	numberSocket = 0;
	pSock = new SockName[200];
	m_msgString += (_T("Sever was started\r\n"));
	UpdateData(0);
	fstream f("account.txt", ios::app);
	f.close();

	fstream fp("filePath.txt", ios::in|ios::app);
	while (!fp.eof())
	{
		string line;
		getline(fp, line);
		if (line != "")
		{
			string fileName = getFileName(line);
			CString temp;
			temp += fileName.c_str();
			listFile.InsertItem(0, temp);
		}
	}
	UpdateData(FALSE);
	fp.close();
}

void CFileServerServerDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	
	for (int i = 0; i < numberSocket; i++)
	{
		closesocket(pSock[i].sockClient);
	}
	delete[] pSock;
	closesocket(sockServer);
	OnCancel();
}

void CFileServerServerDlg::OnBnClickedUpload()
{
	// TODO: Add your control notification handler code here
	CFileDialog t(true);
	if (t.DoModal() == IDOK)
	{
		std::fstream f;
		f.open("filePath.txt", std::fstream::app);
		listFile.InsertItem(0, t.GetFileName());
		std::string line = CStringA(t.GetPathName());
		f << line << std::endl;
		f.close();

		for (int i = 0; i < numberSocket; i++)
		{
			
			CString log = _T("3\r\n") + t.GetFileName() + _T("\r\n");
			mSend(pSock[i].sockClient, log);
		}
	}
}

void CFileServerServerDlg::OnBnClickedRemove()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nItem = 0; //Represents the row number inside CListCtrl
	for (nItem = 0; nItem < listFile.GetItemCount();)
	{
		BOOL bChecked = listFile.GetCheck(nItem);
		if (bChecked == 1)
		{

			//Xoa 1 dong trong file luu duong dan
			{
				//Xoa tren giao dien
				std::string line;

				std::string delLine = CStringA(listFile.GetItemText(nItem, 0));

				for (int i = 0; i < numberSocket; i++)
				{
					CString log = _T("5\r\n") + listFile.GetItemText(nItem, 0) + _T("\r\n");
					mSend(pSock[i].sockClient, log);
				}

				listFile.DeleteItem(nItem);
				std::ifstream fi;

				fi.open("filePath.txt");
				if (fi)
				{
					std::ofstream fo;
					fo.open("temp.txt");
					while (!fi.eof())
					{
						getline(fi, line);
						size_t found = line.find(delLine);
						if (found != std::string::npos || line == "")
							continue;
						fo << line << std::endl;
					}

					fo.close();
					fi.close();
					remove("filePath.txt");
					rename("temp.txt", "filePath.txt");
				}

			}
		}
		else nItem++;
	}

	UpdateData(FALSE);
}

UINT CFileServerServerDlg::sendFile(LPVOID pParam)
{
	{

		// Khoi tao thu vien Socket
		if (AfxSocketInit() == FALSE)
		{
			//cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}

		CSocket ServerSocket; //cha
	
		if (ServerSocket.Create(iPort, SOCK_STREAM, NULL) == 0) //SOCK_STREAM or SOCK_DGRAM.
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
			int file_size, bytes_to_send, bytes_sent;
			byte* buffer = NULL;

			// Mo file

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
}

string CFileServerServerDlg::getExePath()
{
	
	LPWSTR buff11 = new WCHAR[MAX_PATH];
	GetModuleFileNameW(NULL, buff11, MAX_PATH);
	string path = CW2A(buff11);
	delete[] buff11;
	string::size_type pos = path.find_last_of("\\/");
	return path.substr(0,pos+1);
}

string getFilePath(string s)
{
	ifstream fi;
	fi.open("filePath.txt");
	if (fi)
	{
		while (!fi.eof())
		{
			string line = "";
			getline(fi, line);
			if (line.find(s) != string::npos)
				return line;

		}
		fi.close();
	}
	return "";
}

bool CFileServerServerDlg::receiveFile(char* file_name, int port)
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
	else MessageBox(_T("Không thể kết nối đến server"));
	ClientSocket.Close();

	return 1;
}

string getFileName(string s)
{
	string res = "";
	for (int i = s.size() - 1; i >= 0; i--)
	{
		if (s[i] == '\\') break;
		res = s[i] + res;
	}
	return res;
}
