#pragma once


// main dialog
#define WM_SOCKET WM_USER+2

class main : public CDialogEx
{
	DECLARE_DYNAMIC(main)

public:
	main(SOCKET &s, CString , CWnd* pParent = nullptr);   // standard constructor

	virtual ~main();
	CString m_msgString;
	SOCKET sClient;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditUser();

	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString& s);
	void Split(CString src, CString des[2]);
	void mSend(CString Command);
	int mRecv(CString& Command);
	bool receiveFile(char* file_name, int port);
	int	buffLength;
	char* downloadFileName;
	CString Command;
	CString upLoadFileName;
	CString strResult[2];
	CListCtrl ListFile_Client;
	CButton BUTTON_UPLOAD;
	CButton BUTTON_DOWNLOAD;
	CButton BUTTON_LOGOUT;
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonLogout();

	void Refresh();
	static UINT sendFile(LPVOID pParam);
	CString m_log;
};
