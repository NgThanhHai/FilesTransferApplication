#pragma once


// mainDlg dialog
#define WM_SOCKET WM_USER+2
class mainDlg : public CDialog
{
	DECLARE_DYNAMIC(mainDlg)

public:
	mainDlg(SOCKET s, CString name, CWnd* pParent = nullptr);   // standard constructor
	virtual ~mainDlg();
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
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString& s);
	void Split(CString src, CString des[2]);
	void mSend(CString Command);
	int mRecv(CString& Command);
	bool receiveFile(char* file_name, int port);
	int	buffLength;
	char* downloadFileName;
	CString Command;
	CString strResult[2];
	CListCtrl ListFile_Client;
	CButton BUTTON_UPLOAD;
	CButton BUTTON_DOWNLOAD;
	CButton BUTTON_LOGOUT;
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonLogout();
	void mainDlg::OnEnChangeEditUser();
	void Refresh();
	UINT sendFile(LPVOID pParam);
};
