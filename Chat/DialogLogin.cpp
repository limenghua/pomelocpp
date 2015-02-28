// DialogLogin.cpp : 实现文件
//

#include "stdafx.h"
#include "Chat.h"
#include "DialogLogin.h"
#include "afxdialogex.h"


// CDialogLogin 对话框

IMPLEMENT_DYNAMIC(CDialogLogin, CDialogEx)

CDialogLogin::CDialogLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogLogin::IDD, pParent)
	, m_room(_T(""))
	, m_user(_T(""))
	, m_ip(_T(""))
{

}

CDialogLogin::~CDialogLogin()
{
}

void CDialogLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ROOM, m_room);
	DDX_Text(pDX, IDC_USERNAME, m_user);
	DDX_Text(pDX, IDC_REMOTE_IP, m_ip);
}


BEGIN_MESSAGE_MAP(CDialogLogin, CDialogEx)
END_MESSAGE_MAP()


// CDialogLogin 消息处理程序
