// DLGset.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGset.h"


extern CSqliteOperate SQLDB;
extern struct SYSTEM_SET_ST systemset;

// DLGset dialog

IMPLEMENT_DYNAMIC(DLGset, CDialog)

DLGset::DLGset(CWnd* pParent /*=NULL*/)
	: CDialog(DLGset::IDD, pParent)
	
	, m_c_mediainfo(FALSE)
{
	m_c_net_proxy=systemset.checkProxy;
	m_e_net_proxy=systemset.Proxy;
	if(systemset.subsrc ==SUBSRC_SV)
		m_c_sub_sv=TRUE;
	else
		m_c_sub_sv=FALSE;

	m_c_svsub_onlyone=systemset.checkOnlyOne;
	m_c_svsub_onlychn=systemset.checkOnlyChn;
	m_c_mediainfo=systemset.c_mediainfo;
}

DLGset::~DLGset()
{
}

void DLGset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_NET_PROXY, m_c_net_proxy);
	DDX_Text(pDX, IDC_EDIT_NET_PROXY, m_e_net_proxy);
	DDX_Check(pDX, IDC_CHECK_SUB_SV, m_c_sub_sv);
	DDX_Check(pDX, IDC_CHECK_SVSUB_ONLEYONE, m_c_svsub_onlyone);
	DDX_Check(pDX, IDC_CHECK_SVSUB_ONLYCHN, m_c_svsub_onlychn);
	DDX_Check(pDX, IDC_CHECK_MEDIAINFO, m_c_mediainfo);
}


BEGIN_MESSAGE_MAP(DLGset, CDialog)
END_MESSAGE_MAP()


BOOL DLGset::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_INFO)->SetWindowText("��������:\n\
�����������Դ���ɱ�����ṩ�������������з����뽻����\n\
��������г��ĸ���Դ�ļ���û�б��������ߵķ������ϡ�\n\
��������������ӣ����ǶԱ������ļ�������һ����֪�������޷����û����κη���������Ϊ������\n\
֣������:\n\
������ṩ��������Դ�����������Ѽ��������������ݿ���ѧϰʹ�á�\n\
�κ��漰��ҵӯ��Ŀ�ľ�����ʹ�ã����������һ�к���������Լ��е���\n\
���߽����Ա�������κ����ݸ��κη������Σ�\n\
���������ݽ����������ʹ�ã��������غ�24Сʱ��ɾ������֧�ֲ��������棡");

	return TRUE;  // return TRUE  unless you set the focus to a control
}
// DLGset message handlers

void DLGset::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	systemset.checkProxy=m_c_net_proxy;
	strcpy(systemset.Proxy,m_e_net_proxy.GetBuffer(0));

	if(	m_c_sub_sv)
		systemset.subsrc =SUBSRC_SV;

	systemset.checkOnlyOne=m_c_svsub_onlyone;
	systemset.checkOnlyChn=	m_c_svsub_onlychn;

	systemset.c_mediainfo=m_c_mediainfo;

	SQLDB.SystemSet_Modify(systemset);

	CDialog::OnOK();
}

void DLGset::OnCancel() 
{
	
	CDialog::OnCancel();
}
