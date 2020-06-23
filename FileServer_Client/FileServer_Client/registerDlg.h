#pragma once
#include "pch.h"
#include "afxwin.h"

// registerDlg dialog

class registerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(registerDlg)

public:
	registerDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~registerDlg();
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRegister();
	
	CString Command;
	sockaddr_in servAdd;
	afx_msg void OnBnClickedCancel();
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString& s);
	void Split(CString src, CString des[2]);
	void mSend(CString Command);
	int mRecv(CString& Command);
	CString strResult[2];
	SOCKET sClient;
	int buffLength;
	CEdit IDC_userName;
	CEdit IDC_passWord;
	CEdit IDC_confirmPassWord;
};
