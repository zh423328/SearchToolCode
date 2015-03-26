
// SearchToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchTool.h"
#include "SearchToolDlg.h"
#include "afxdialogex.h"

#include <io.h>
#include <direct.h>
#include "LuaManager.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSearchToolDlg 对话框




CSearchToolDlg::CSearchToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchToolDlg::IDD, pParent)
	, m_pInputPath(_T(""))
	, m_pOutPath(_T(""))
	, m_strExtName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nIndex = 0;
}

void CSearchToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_pInputPath);
	DDX_Text(pDX, IDC_EDIT2, m_pOutPath);
	DDX_Control(pDX, IDC_PROGRESS1, m_pProgressCtrl);
	DDX_Control(pDX, IDC_LIST1, m_pListCtrl);
	DDX_Text(pDX, IDC_EDIT3, m_strExtName);
	DDX_Control(pDX, IDC_BUTTON1, m_pButton1);
	DDX_Control(pDX, IDC_BUTTON2, m_pButton2);
	DDX_Control(pDX, IDC_BUTTON3, m_pStart);
}

BEGIN_MESSAGE_MAP(CSearchToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CSearchToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSearchToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSearchToolDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CSearchToolDlg 消息处理程序

BOOL CSearchToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSearchToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSearchToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSearchToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//选择输入文件夹
void CSearchToolDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	//打开文件夹
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH*2];
	szFolder[0] = _T('\0');

	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
			bRet = TRUE;
		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}

		m_pInputPath = szFolder;

		UpdateData(FALSE);		//拷贝到控件上
	}

}


void CSearchToolDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	//打开文件夹
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH*2];
	szFolder[0] = _T('\0');

	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
			bRet = TRUE;
		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}

		m_pOutPath = szFolder;

		UpdateData(FALSE);		//拷贝到控件上
	}
}


//处理文件
DWORD WINAPI DoFileThread(LPVOID lpThreadParameter)
{
	CSearchToolDlg *pDlg = (CSearchToolDlg*)lpThreadParameter;
	if (pDlg)
	{
		for (int i = 0; i < pDlg->m_FileList.size();++i)
		{
			CString filePath = pDlg->m_FileList[i];
			pDlg->DoFile(filePath);
		}
	}

	return 0;
}

void CSearchToolDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	AnyalsizeExt();

	//获取
	if(m_pInputPath.IsEmpty()||!(m_pInputPath[0] >= 'A' && m_pInputPath[0] <='Z' && m_pInputPath.ReverseFind('\\')))
	{
		AfxMessageBox(_T("无效输入路径"),MB_OK);
		return; 
	}

	//无效输出路径
	if(m_pOutPath.IsEmpty()||!(m_pOutPath[0] >= 'A' && m_pOutPath[0] <='Z' && m_pOutPath.ReverseFind('\\')))
	{
		AfxMessageBox(_T("无效输出路径"),MB_OK);
		return; 
	}

	Recurse(m_pInputPath);

	m_pListCtrl.InsertColumn(0,_T("索引"),LVCFMT_LEFT,40);
	m_pListCtrl.InsertColumn(1,_T("文件名"),LVCFMT_LEFT,500);

	m_pListCtrl.DeleteAllItems();

	m_pProgressCtrl.SetRange(0,m_FileList.size());

	//设置按钮锁定
	m_pButton1.EnableWindow(FALSE);
	m_pButton2.EnableWindow(FALSE);
	m_pStart.EnableWindow(FALSE);

	LuaManager::shareLuaManager()->LoadLuaFile("tool.lua");
	DWORD dwThread = 0;
	CreateThread(NULL,0,DoFileThread,this,0,&dwThread);

	//for (int i = 0; i < m_FileList.size();++i)
	//{
	//	CString filePath = m_FileList[i];
	//	DoFile(filePath);
	//}
}

void CSearchToolDlg::Recurse( CString path )
{
	//搜索文件
	CFileFind finder;

	CString inputfilename = path + "/*.*";

	BOOL bWorking = finder.FindFile(inputfilename);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		//文件是否是隐藏
		if (finder.IsHidden())
			continue;

		//.文件和..文件夹
		if (finder.IsDots())
			continue;

		//判断是否是文件夹
		if (finder.IsDirectory())
		{
			CString foldrname = finder.GetFileName();
			CString txt = finder.GetFilePath();

			
			txt.Replace(m_pInputPath,m_pOutPath);

			//判断文件或者目录是否存在
			if (_access(CT2A(txt.GetString()),0)== -1)
			{
				mkdir(CT2A(txt.GetString()));
			}

			//读取文件
			Recurse(finder.GetFilePath());
			continue;
		}

		if (finder.IsDirectory() == FALSE)
		{
			if(IsFit(finder.GetFileName()))
				m_FileList.push_back(finder.GetFilePath());
		}
	}
}

void CSearchToolDlg::AnyalsizeExt()
{
	if (m_strExtName.IsEmpty())
		return;

	m_pExtList.clear();	

	m_strExtName.MakeLower();

	//保存名字
	string strName(CT2A(m_strExtName.GetString()));

	//提取后缀名
	char *p = strtok((char*)strName.c_str(),";");

	while(p)
	{
		if (p[0] !='.')
		{
			CString strExt;
			strExt.Format(_T(".%s"),CA2T(p));

			m_pExtList.push_back(strExt);
		}
		else
		{
			CString strExt(p);
			strExt.Trim();
			m_pExtList.push_back(strExt);

		}
		p = strtok(NULL,";");
	}
}

BOOL CSearchToolDlg::IsFit( CString fileName )
{
	if (m_pExtList.empty())
		return TRUE;

	//解析文件名

	string str(CT2A(fileName.GetString()));

	
	char *p = strchr((char*)str.c_str(),'.');

	if (p == NULL)
		return FALSE;

	CString pathExt(p);

	//去空格
	pathExt.Trim();

	for (int i = 0; i < m_pExtList.size();++i)
	{
		CString strNowExt = m_pExtList[i];
		if (strNowExt.CompareNoCase(pathExt) == 0)	//相同
			return TRUE;
	}

	return FALSE;
}

//读取文件操作lua
BOOL CSearchToolDlg::DoFile(CString filePath)
{
	CString strLv1;
	strLv1.Format(_T("%d"),m_nIndex+1);

	m_pListCtrl.InsertItem(m_nIndex,strLv1);

	CString strTip = _T("正在提取");

	strTip += filePath;

	m_pListCtrl.SetItemText(m_nIndex,1,strTip);

	m_pListCtrl.SendMessage(WM_VSCROLL,MAKEWPARAM(SB_BOTTOM, 0), 0); 

	//m_pListCtrl.InsertItem(m_pListCtrl.GetItemCount(),strTip);

	CString filepathpathtxt = m_pOutPath + "\\chinese.txt";
	
	CString filePathTo = filePath;

	filePathTo.Replace(m_pInputPath,m_pOutPath);		//替换

	string strFilePathtxt(CT2A(filepathpathtxt.GetString()));
	string strFilePath(CT2A(filePath.GetString()));
	string strFilePathTo(CT2A(filePathTo.GetString()));


	CParamGroup ParamIn;
	_ParamData *data1 = new _ParamData();
	char szText1[128] = {0};
	sprintf(szText1,"%s",strFilePath.c_str());
	data1->SetParam(szText1,"string",strlen(szText1));
	ParamIn.Push(data1);

	_ParamData *data2 = new _ParamData();
	char szText2[128] = {0};
	sprintf(szText2,"%s",strFilePathTo.c_str());
	data2->SetParam(szText2,"string",strlen(szText2));
	ParamIn.Push(data2);

	_ParamData *data3 = new _ParamData();;
	char szText3[128] = {0};
	sprintf(szText3,"%s",strFilePathtxt.c_str());
	data3->SetParam(szText3,"string",strlen(szText3));
	ParamIn.Push(data3);

	CParamGroup ParamOut;
	LuaManager::shareLuaManager()->callLuaFuncPar("DoFile",ParamIn,ParamOut);

	++m_nIndex;

	m_pProgressCtrl.SetPos(m_nIndex);


	if (m_nIndex == m_FileList.size())
	{
		//设置按钮锁定
		m_pButton1.EnableWindow(TRUE);
		m_pButton2.EnableWindow(TRUE);
		m_pStart.EnableWindow(TRUE);

		AfxMessageBox(_T("所有文件提取完成"),MB_OK);
	}
	return TRUE;
}
