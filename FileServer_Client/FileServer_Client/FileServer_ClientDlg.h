
// FileServer_ClientDlg.h : header file
//

#pragma once


#include "registerDlg.h"
#define PORT 25000
#define WM_SOCKET WM_USER+2

// CFileServerClientDlg dialog
class CFileServerClientDlg : public CDialogEx
{
// Construction
public:
	CFileServerClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILESERVER_CLIENT_DIALOG };
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
public:
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonRegister();
	CString _UserName;
	CString _PassWord;
	SOCKET sClient;
	CString IP;
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString& s);
	void Split(CString src, CString des[2]);
	void mSend(CString Command);
	int mRecv(CString& Command);
	int buffLength;
	sockaddr_in servAdd;
	CString Command;
	CString strResult[2];

	CString m_msgString;

	CEdit l_username;
	CEdit l_pass;
	CEdit IPA;
	CIPAddressCtrl IPADDRESS_IN;
	afx_msg void OnIpnFieldchangedIpaddress(NMHDR* pNMHDR, LRESULT* pResult);
};
