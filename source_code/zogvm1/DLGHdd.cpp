// DLGHdd.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGHdd.h"
#include "video.h"
#include "usn.h"

extern CSqliteOperate SQLDB;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLGHdd dialog


CDLGHdd::CDLGHdd(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGHdd::IDD, pParent)
	, m_c_dir(FALSE)
{
	//{{AFX_DATA_INIT(CDLGHdd)
	m_check = FALSE;
	dirListChoose=-1;
	//}}AFX_DATA_INIT
}


void CDLGHdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLGHdd)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Check(pDX, IDC_CHECK, m_check);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_DIR, m_c_dir);
	DDX_Control(pDX, IDC_LIST_DIR, m_list_dir);
}


BEGIN_MESSAGE_MAP(CDLGHdd, CDialog)
	//{{AFX_MSG_MAP(CDLGHdd)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ADDDIR, &CDLGHdd::OnBnClickedButtonAdddir)
	ON_BN_CLICKED(IDC_BUTTON_DELDIR, &CDLGHdd::OnBnClickedButtonDeldir)
	ON_BN_CLICKED(IDC_CHECK_DIR, &CDLGHdd::OnBnClickedCheckDir)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CDLGHdd::OnBnClickedButtonClean)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DIR, &CDLGHdd::OnNMClickListDir)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CDLGHdd::OnLvnItemchangedList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DIR, &CDLGHdd::OnLvnItemchangedListDir)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SAMBA, &CDLGHdd::OnBnClickedButtonAddSamba)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLGHdd message handlers
DWORD CDLGHdd::GetHDDid(void)
{
	CString id; 
	char Name[MAX_PATH]=""; 
	DWORD serno; 
	DWORD length; 
	DWORD FileFlag; 
	char FileName[MAX_PATH]=""; 
	BOOL Ret; 
	Ret = GetVolumeInformation("c:\\\\", Name, MAX_PATH, &serno, &length, &FileFlag, FileName, MAX_PATH); 

	if (Ret) 
		return serno;
	else
		return 0;
}


void CDLGHdd::Add27HDDid(void)
{
	CString id; 
	char Name[MAX_PATH]=""; 
	DWORD serno; 
	DWORD length; 
	DWORD FileFlag; 
	char FileName[MAX_PATH]=""; 
	BOOL Ret; 
	char path[MAX_PATH]="";
	char serstr[MAX_PATH]="";

	unsigned long long i64FreeBytesToCaller;  
	unsigned long long i64TotalBytes;  
	unsigned long long i64FreeBytes;  

	char c;
	for(c='c';c<='z';c++)
	{
		sprintf(path,"%c:\\",c);
		//��ȡ����
		Ret = GetVolumeInformation(path, Name, MAX_PATH, &serno, &length, &FileFlag, FileName, MAX_PATH); 
		if (Ret) 
		{
			sprintf(serstr,"%X",serno);
			//��ȡ����
			i64FreeBytesToCaller=0;
			i64TotalBytes=0;

			GetDiskFreeSpaceEx (  
				path,  
				(PULARGE_INTEGER)&i64FreeBytesToCaller,  
				(PULARGE_INTEGER)&i64TotalBytes,  
				(PULARGE_INTEGER)&i64FreeBytes);  

			SQLDB.Hdd_Add(serstr,path,Name,FileName,i64TotalBytes,i64FreeBytesToCaller);

			//////////////////////////////////////////
			//USNBuildFullPathTest("c:","NTFS");
		}
	}
}


BOOL CDLGHdd::OnInitDialog()
{
	CDialog::OnInitDialog();
	hddList.clear();

	SQLDB.Begin();
	SQLDB.Hdd_SetNonsert();
	Add27HDDid();
	SQLDB.Commit();

	SQLDB.Hdd_Read(hddList);

	m_list.InsertColumn(0, ConvertString("�������к�") , LVCFMT_LEFT, 90);
	m_list.InsertColumn(1, ConvertString("����") , LVCFMT_LEFT, 40);
	m_list.InsertColumn(2, ConvertString("��ǩ") , LVCFMT_LEFT, 150);
	m_list.InsertColumn(3, ConvertString("�ܿռ�") , LVCFMT_LEFT, 75);
	m_list.InsertColumn(4, ConvertString("ʣ��ռ�") , LVCFMT_LEFT, 70);
	m_list.InsertColumn(5, ConvertString("�Ƿ�Ӳ�") , LVCFMT_LEFT, 60);
	m_list.InsertColumn(6, ConvertString("nid") , LVCFMT_LEFT, 0);
	m_list.InsertColumn(7, ConvertString("��ʽ") , LVCFMT_LEFT, 50);

	//����ѡ�� LVS_EX_CHECKBOXES
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

	long long tb=0;

	char str[32];
	int i=0;
	list<struct HDD_ST>::iterator beglist;

	for(beglist=hddList.begin();beglist!=hddList.end();beglist++)
	{
		m_list.InsertItem(i,beglist->serno);

		m_list.SetItemText(i,1,beglist->area);
		m_list.SetItemText(i,2,beglist->mark);

		sprintf(str,"%.3fG",(double)(beglist->TotalBytes/1024/1024)/1024.0);
		m_list.SetItemText(i,3,str);
		sprintf(str,"%.3fG",(double)(beglist->FreeBytes/1024/1024)/1024.0);
		m_list.SetItemText(i,4,str);

		if(beglist->insertflag)
			m_list.SetItemText(i,5,ConvertString("����<--"));
		else
			m_list.SetItemText(i,5,ConvertString("�ѻ�-->"));

		sprintf(str,"%d",beglist->hdd_nid);
		m_list.SetItemText(i,6,str);

		m_list.SetItemText(i,7,beglist->format);

		m_list.SetCheck(i,beglist->enable);
		i++;

		tb+=beglist->TotalBytes;
	}
	sprintf(str,ConvertString("��������%.3f T"),(double)(tb/1024/1024/1024)/1024.0);
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText(str);

	////////////////////

	m_list_dir.InsertColumn(0, ConvertString("�������к�") , LVCFMT_LEFT, 90);
	m_list_dir.InsertColumn(1, ConvertString("����") , LVCFMT_LEFT, 40);
	m_list_dir.InsertColumn(2, ConvertString("�Ƿ�Ӳ�") , LVCFMT_LEFT, 60);
	m_list_dir.InsertColumn(3, ConvertString("·��") , LVCFMT_LEFT, 350);
	m_list_dir.InsertColumn(4, ConvertString("nid") , LVCFMT_LEFT, 0);

	//����ѡ�� LVS_EX_CHECKBOXES
	m_list_dir.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

	DisplayDir();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDLGHdd::DisplayDir() 
{
	m_list_dir.DeleteAllItems();
	dirList.clear();

	dirListChoose=-1;
	GetDlgItem(IDC_BUTTON_DELDIR)->EnableWindow(FALSE);

	SQLDB.Dir_Read(dirList);
	int i;
	bool flag;

	char str[32];

	if(dirList.size()>0)
	{
		i=0;

		list<struct DIR_ST>::iterator dirbeglist;
		for(dirbeglist= dirList.begin();dirbeglist!=dirList.end();dirbeglist++)
		{
			list<struct HDD_ST>::iterator beglist;
			flag=false;
			for(beglist=hddList.begin();beglist!=hddList.end();beglist++)
			{
				if(dirbeglist->hdd_nid == beglist->hdd_nid)
				{
					m_list_dir.InsertItem(i,beglist->serno);

					m_list_dir.SetItemText(i,1,beglist->area);

					if(beglist->insertflag)
						m_list_dir.SetItemText(i,2,ConvertString("����<--"));
					else
						m_list_dir.SetItemText(i,2,ConvertString("�ѻ�-->"));

					flag=true;
					break;
				}
			}
			if(false == flag)
			{
				MessageBox("����ʿ�����Ŷ,����(������)������:��ֵĴ���A,������ղ�����������һ��~лл");
				continue;
			}
			m_list_dir.SetItemText(i,3,dirbeglist->path);
			sprintf(str,"%d",dirbeglist->dir_nid);
			m_list_dir.SetItemText(i,4,str);

			m_list_dir.SetCheck(i,dirbeglist->enable);
			i++;
		}
	}

}
void CDLGHdd::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	int i;

	char str[32];
	int check;

	SQLDB.Begin();
	for( i=0;i<m_list.GetItemCount();i++)
	{
		m_list.GetItemText(i,6,str,32);
		check=m_list.GetCheck(i);
		SQLDB.Hdd_ModifyEnable(str,check);
	}
	SQLDB.Commit();

	hddList.clear();
	//////////////////////////
	SQLDB.Begin();
	for( i=0;i<m_list_dir.GetItemCount();i++)
	{
		m_list_dir.GetItemText(i,4,str,32);
		check=m_list_dir.GetCheck(i);
		SQLDB.Dir_ModifyEnable(str,check);
	}
	SQLDB.Commit();

	dirList.clear();

	CDialog::OnOK();
}

void CDLGHdd::OnCancel() 
{
	// TODO: Add extra cleanup here
	hddList.clear();
	CDialog::OnCancel();
}

void CDLGHdd::OnCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list.GetItemCount();i++)
		m_list.SetCheck(i,m_check);
}


void CDLGHdd::OnBnClickedButtonAdddir()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO   bi;                           //����BROWSEINFO�ṹ��
	TCHAR   Buffer[512]= " ";
	TCHAR   FullPath[512]= " ";
	bi.hwndOwner   =   GetSafeHwnd();               //���ھ��
	bi.pidlRoot   =   NULL;
	bi.pszDisplayName   =   Buffer;            //����ѡ���Ŀ¼���Ļ�����
	bi.lpszTitle   =   "Selection ";           //�����Ĵ��ڵ�������ʾ
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS   ;  //ֻ����Ŀ¼��������־��MSDN
	bi.lpfn   =   NULL;               //�ص���������ʱ������
	bi.lParam   =   0;
	bi.iImage   =   0;
	ITEMIDLIST*   pidl   =   ::SHBrowseForFolder(&bi);   //��ʾ�������ڣ�ITEMIDLIST����Ҫ
	if(::SHGetPathFromIDList(pidl,FullPath)) //��ITEMIDLIST�еõ�Ŀ¼��������·��
	{
		memset(Buffer,0,512*sizeof(TCHAR));
		memcpy(Buffer,FullPath,3*sizeof(TCHAR));
		strlwr(Buffer);

		list<struct HDD_ST>::iterator beglist;

		for(beglist=hddList.begin();beglist!=hddList.end();beglist++)
		{
			if(0==strcmp(Buffer,beglist->area ) && beglist->insertflag)
			{
				memcpy(Buffer,&(FullPath[3]),500*sizeof(TCHAR));
				//����ֻ��Ŀ¼����
				if(0==strlen(Buffer))
					return ;

				SQLDB.Dir_Add(beglist->hdd_nid,Buffer);
				DisplayDir() ;
				return ;
			}
		}
	}

}

void CDLGHdd::OnBnClickedButtonDeldir()
{
	// TODO: Add your control notification handler code here
	if(dirListChoose>=0)
	{
		char str[32];
		long long dir_nid;
		m_list_dir.GetItemText(dirListChoose,4,str,32);
		sscanf(str,"%I64u",&dir_nid);

		SQLDB.Dir_DeleteforDir(dir_nid);
		DisplayDir() ;
	}
}

void CDLGHdd::OnBnClickedCheckDir()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list_dir.GetItemCount();i++)
		m_list_dir.SetCheck(i,m_c_dir);
}

void CDLGHdd::OnBnClickedButtonClean()
{
	// TODO: Add your control notification handler code here
	SQLDB.Dir_DeleteAll();
	m_list_dir.DeleteAllItems();
	dirList.clear();
}

void CDLGHdd::OnNMClickListDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* p=	(NM_LISTVIEW *)pNMHDR;
	//ѡ�еĸ�ֵ
	dirListChoose= p->iItem;
	//û��
	if(-1 == dirListChoose)
	{
		GetDlgItem(IDC_BUTTON_DELDIR)->EnableWindow(FALSE);
		return ;
	}

	GetDlgItem(IDC_BUTTON_DELDIR)->EnableWindow(TRUE);

	*pResult = 0;
}


void CDLGHdd::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	 NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) /* old state : unchecked */ 
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2)) /* new state : checked */ 
		) 
	{ 
		//TRACE("Item %d is checked\n", pNMLV->iItem); 
		char hddserno[32];
		char dirserno[32];

		m_list.GetItemText(pNMLV->iItem,0,hddserno,32);

		for(int i=0;i<m_list_dir.GetItemCount();i++)
		{
			m_list_dir.GetItemText(i,0,dirserno,32);
			if(0==strcmp(hddserno,dirserno))	
				m_list_dir.SetCheck(i,false);
		}
	} 
	else if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) /* old state : checked */ 
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)) /* new state : unchecked */ 
		) 
	{ 
		TRACE("Item %d is unchecked\n", pNMLV->iItem); 
	} 
	else 
	{ 
		TRACE("Item %d does't change the check-status\n", pNMLV->iItem); 
	} 
}


void CDLGHdd::OnLvnItemchangedListDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	 NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) /* old state : unchecked */ 
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2)) /* new state : checked */ 
		) 
	{ 
		//TRACE("Item %d is checked\n", pNMLV->iItem); 
		char hddserno[32];
		char dirserno[32];

		m_list_dir.GetItemText(pNMLV->iItem,0,dirserno,32);

		for(int i=0;i<m_list.GetItemCount();i++)
		{
			m_list.GetItemText(i,0,hddserno,32);
			if(0==strcmp(hddserno,dirserno))	
				m_list.SetCheck(i,false);
		}
	} 
	else if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) /* old state : checked */ 
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)) /* new state : unchecked */ 
		) 
	{ 
		TRACE("Item %d is unchecked\n", pNMLV->iItem); 
	} 
	else 
	{ 
		TRACE("Item %d does't change the check-status\n", pNMLV->iItem); 
	} 
}

void CDLGHdd::OnBnClickedButtonAddSamba()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO   bi;                           //����BROWSEINFO�ṹ��
	TCHAR   Buffer[512]= " ";
	TCHAR   FullPath[512]= " ";
	bi.hwndOwner   =   GetSafeHwnd();               //���ھ��
	bi.pidlRoot   =   NULL;
	bi.pszDisplayName   =   Buffer;            //����ѡ���Ŀ¼���Ļ�����
	bi.lpszTitle   =   "Selection ";           //�����Ĵ��ڵ�������ʾ
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS   ;  //ֻ����Ŀ¼��������־��MSDN
	bi.lpfn   =   NULL;               //�ص���������ʱ������
	bi.lParam   =   0;
	bi.iImage   =   0;
	ITEMIDLIST*   pidl   =   ::SHBrowseForFolder(&bi);   //��ʾ�������ڣ�ITEMIDLIST����Ҫ
	if(::SHGetPathFromIDList(pidl,FullPath)) //��ITEMIDLIST�еõ�Ŀ¼��������·��
	{

		memset(Buffer,0,512*sizeof(TCHAR));

		TCHAR   *strp=NULL;

		strp=strstr(FullPath+2,"\\");
		if(strp==NULL)
		{
			strcpy(Buffer,&(FullPath[2]));
		}
		else
		{
			memcpy(Buffer,&(FullPath[2]),strp-(FullPath+2));
		}

		list<struct HDD_ST>::iterator beglist;

		for(beglist=hddList.begin();beglist!=hddList.end();beglist++)
		{
			if(0==strcmp(Buffer,beglist->serno))
			{
				SQLDB.Dir_Add(beglist->hdd_nid,&(FullPath[1]));
				DisplayDir() ;
				return ;
			}
		}

		//�����ٱ���
		SQLDB.Hdd_Add(Buffer,"\\","","SAMBA",0,0);
		SQLDB.Hdd_Read(hddList);

		for(beglist=hddList.begin();beglist!=hddList.end();beglist++)
		{
			if(0==strcmp(Buffer,beglist->serno))
			{
				SQLDB.Dir_Add(beglist->hdd_nid,&(FullPath[1]));
				DisplayDir() ;
				return ;
			}
		}

	}
}
