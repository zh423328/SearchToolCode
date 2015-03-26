
// SearchToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <iostream>
#include <vector>
#include "afxwin.h"
using namespace std;

//��ʱ��
#define ONEVENTIME 0x01

// CSearchToolDlg �Ի���
class CSearchToolDlg : public CDialogEx
{
// ����
public:
	CSearchToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SEARCHTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_pInputPath;
	CString m_pOutPath;
	CProgressCtrl m_pProgressCtrl;
	CListCtrl m_pListCtrl;
	vector <CString> m_FileList;

	CString m_strExtName;

	void Recurse( CString path );
	void AnyalsizeExt();			//������׺��	

	BOOL IsFit(CString path);		//�Ƿ��������

	BOOL DoFile(CString filePath);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	vector<CString> m_pExtList;				//��׺���б�

	int	 m_nIndex;
	CButton m_pButton1;
	CButton m_pButton2;
	CButton m_pStart;
};
