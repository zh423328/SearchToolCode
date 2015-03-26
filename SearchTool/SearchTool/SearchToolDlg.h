
// SearchToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include <iostream>
#include <vector>
#include "afxwin.h"
using namespace std;

//定时器
#define ONEVENTIME 0x01

// CSearchToolDlg 对话框
class CSearchToolDlg : public CDialogEx
{
// 构造
public:
	CSearchToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SEARCHTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	void AnyalsizeExt();			//分析后缀名	

	BOOL IsFit(CString path);		//是否符合条件

	BOOL DoFile(CString filePath);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	vector<CString> m_pExtList;				//后缀名列表

	int	 m_nIndex;
	CButton m_pButton1;
	CButton m_pButton2;
	CButton m_pStart;
};
