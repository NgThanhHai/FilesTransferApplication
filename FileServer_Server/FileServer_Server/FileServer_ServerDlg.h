
// FileServer_ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "stdlib.h"
#include "time.h"
#include <vector>
#include "afxcmn.h"
#include <string>
#include <fstream>
#include "stdafx.h"

#define PORT 25000
#define WM_SOCKET WM_USER+1
#define SEND_BUFFER_SIZE 10000
#define RECV_BUFFER_SIZE 10000
using namespace std;
// CFileServerServerDlg dialog
class CFileServerServerDlg : public CDialogEx
{
// Construction
public:
	CFileServerServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILESERVER_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	bool CheckUsernameExist(CString User);
	void UpdateAccount(CString User, CString Pass);
	bool CheckAccountExist(CString User, CString Pass);
public:
	
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	struct sockaddr_in serverAdd;

	SOCKET sockServer;
	int numberSocket;
	CString strResult[2];
	int buffLength;
	CString Command;

	struct SockName 
	{
		SOCKET sockClient;
		char Name[200];
	};

	SockName* pSock;

	char* ConvertToChar(const CString& s);
	void Split(CString src, CString des[2]);

	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();

	void mSend(SOCKET sk, CString Command);
	int mRecv(SOCKET sk, CString& Command);
	
	
	CListCtrl listClients;
	CString m_msgString;
	afx_msg void OnBnClickedUpload();
	
	afx_msg void OnBnClickedRemove();

	static UINT sendFile(LPVOID pParam);

	
	string getExePath();
	
	CListCtrl listFile;

	char* downloadFileName;
	char* uploadFileName;
	bool receiveFile(char*, int);
};
std::string getFilePath(std::string);
std::string getFileName(std::string);
extern std::string file_name;