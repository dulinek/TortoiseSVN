// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2008-2009 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "TortoiseProc.h"
#include "TreeConflictEditorDlg.h"

#include "SVN.h"
#include "SVNInfo.h"
#include "SVNStatus.h"
#include "SVNProgressDlg.h"
#include "MessageBox.h"
#include "UnicodeUtils.h"

// CTreeConflictEditorDlg dialog

IMPLEMENT_DYNAMIC(CTreeConflictEditorDlg, CDialog)

CTreeConflictEditorDlg::CTreeConflictEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeConflictEditorDlg::IDD, pParent)
	, src_left(NULL)
	, src_right(NULL)
	, m_bThreadRunning(false)
{

}

CTreeConflictEditorDlg::~CTreeConflictEditorDlg()
{
}

void CTreeConflictEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTreeConflictEditorDlg, CDialog)
	ON_BN_CLICKED(IDC_RESOLVEUSINGTHEIRS, &CTreeConflictEditorDlg::OnBnClickedResolveusingtheirs)
	ON_BN_CLICKED(IDC_RESOLVEUSINGMINE, &CTreeConflictEditorDlg::OnBnClickedResolveusingmine)
	ON_REGISTERED_MESSAGE(WM_AFTERTHREAD, OnAfterThread) 
END_MESSAGE_MAP()


// CTreeConflictEditorDlg message handlers

BOOL CTreeConflictEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_CONFLICTINFO, m_sConflictInfo);
	SetDlgItemText(IDC_RESOLVEUSINGTHEIRS, m_sUseTheirs);
	SetDlgItemText(IDC_RESOLVEUSINGMINE, m_sUseMine);

	if (conflict_reason == svn_wc_conflict_reason_deleted)
	{
		// start thread to find the possible copyfrom item
		InterlockedExchange(&m_bThreadRunning, TRUE);
		if (AfxBeginThread(StatusThreadEntry, this)==NULL)
		{
			InterlockedExchange(&m_bThreadRunning, FALSE);
			CMessageBox::Show(NULL, IDS_ERR_THREADSTARTFAILED, IDS_APPNAME, MB_OK | MB_ICONERROR);
		}
	}
	else
	{
		SetDlgItemText(IDC_INFOLABEL, CString(MAKEINTRESOURCE(IDC_TREECONFLICT_HOWTORESOLVE)));
		GetDlgItem(IDC_RESOLVEUSINGMINE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESOLVEUSINGTHEIRS)->ShowWindow(SW_SHOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTreeConflictEditorDlg::OnBnClickedResolveusingtheirs()
{
	if (m_bThreadRunning)
		return;

	INT_PTR retVal = IDC_RESOLVEUSINGTHEIRS;
	SVN svn;

	if (conflict_reason == svn_wc_conflict_reason_deleted)
	{
		if ((!m_copyfromPath.IsEmpty())&&(src_left->peg_rev))
		{
			CTSVNPath url1 = CTSVNPath(CUnicodeUtils::GetUnicode(src_left->repos_url));
			url1.AppendPathString(CUnicodeUtils::GetUnicode(src_left->path_in_repos));
			CTSVNPath url2 = CTSVNPath(CUnicodeUtils::GetUnicode(src_right->repos_url));
			url2.AppendPathString(CUnicodeUtils::GetUnicode(src_right->path_in_repos));

			CSVNProgressDlg progDlg;
			progDlg.SetCommand(CSVNProgressDlg::SVNProgress_Merge);
			progDlg.SetPathList(CTSVNPathList(m_copyfromPath));
			progDlg.SetUrl(url1.GetSVNPathString());
			progDlg.SetSecondUrl(url2.GetSVNPathString());
			progDlg.SetRevision(src_left->peg_rev);
			progDlg.SetRevisionEnd(src_right->peg_rev);
			if (url1.IsEquivalentTo(url2))
			{
				SVNRevRange range;
				range.SetRange(src_left->peg_rev, src_right->peg_rev);
				SVNRevRangeArray array;
				array.AddRevRange(range);
				progDlg.SetRevisionRanges(array);
			}
			int options = ProgOptIgnoreAncestry;
			progDlg.SetOptions(options);
			progDlg.DoModal();
			return;
		}
		else
		{
			// get the file back from the repository:
			// revert the deletion
			svn.Revert(CTSVNPathList(m_path), CStringArray(), false);
		}
	}

	if (!svn.Resolve(m_path, svn_wc_conflict_choose_theirs_full, false))
	{
		CMessageBox::Show(m_hWnd, svn.GetLastErrorMessage(), _T("TortoiseSVN"), MB_ICONERROR);
		retVal = IDCANCEL;
	}
	else
	{
		// Subversion conflict resolving does *not* remove files/dirs automatically but
		// (currently?) simply marks the conflict as resolved.
		// We try to do the deletion here ourselves since that's what the dialog button
		// suggested
		if ((m_sUseTheirs.Compare(CString(MAKEINTRESOURCE(IDS_TREECONFLICT_RESOLVE_REMOVEFILE))) == 0)||
			(m_sUseTheirs.Compare(CString(MAKEINTRESOURCE(IDS_TREECONFLICT_RESOLVE_REMOVEDIR))) == 0))
		{
			if (m_path.Exists())
			{
				if (!svn.Remove(CTSVNPathList(m_path), TRUE, FALSE))
				{
					CMessageBox::Show(m_hWnd, svn.GetLastErrorMessage(), _T("TortoiseSVN"), MB_ICONERROR);
					retVal = IDCANCEL;
				}
			}
		}
	}
	EndDialog(retVal);
}

void CTreeConflictEditorDlg::OnBnClickedResolveusingmine()
{
	if (m_bThreadRunning)
		return;

	INT_PTR retVal = IDC_RESOLVEUSINGMINE;
	SVN svn;
	if (!svn.Resolve(m_path, svn_wc_conflict_choose_mine_full, false))
	{
		CMessageBox::Show(m_hWnd, svn.GetLastErrorMessage(), _T("TortoiseSVN"), MB_ICONERROR);
		retVal = IDCANCEL;
	}
	else
	{
		// Subversion conflict resolving does *not* remove files/dirs automatically but
		// (currently?) simply marks the conflict as resolved.
		// We try to do the deletion here ourselves since that's what the dialog button
		// suggested
		if ((m_sUseMine.Compare(CString(MAKEINTRESOURCE(IDS_TREECONFLICT_RESOLVE_REMOVEFILE))) == 0)||
			(m_sUseMine.Compare(CString(MAKEINTRESOURCE(IDS_TREECONFLICT_RESOLVE_REMOVEDIR))) == 0))
		{
			if (m_path.Exists())
			{
				if (!svn.Remove(CTSVNPathList(m_path), TRUE, FALSE))
				{
					CMessageBox::Show(m_hWnd, svn.GetLastErrorMessage(), _T("TortoiseSVN"), MB_ICONERROR);
					retVal = IDCANCEL;
				}
			}
		}
	}
	EndDialog(retVal);
}


//this is the thread function which calls the subversion function
UINT CTreeConflictEditorDlg::StatusThreadEntry(LPVOID pVoid)
{
	return ((CTreeConflictEditorDlg*)pVoid)->StatusThread();
}


//this is the thread function which calls the subversion function
UINT CTreeConflictEditorDlg::StatusThread()
{
	InterlockedExchange(&m_bThreadRunning, TRUE);
	m_progressDlg.SetTitle(IDS_REPOBROWSE_WAIT);
	m_progressDlg.SetShowProgressBar(false);
	m_progressDlg.ShowModeless(m_hWnd, false);
	SVNInfo info;
	const SVNInfoData * pData = info.GetFirstFileInfo(m_path, SVNRev(), SVNRev());
	CTSVNPath statPath = m_path.GetContainingDirectory();
	CTSVNPath copyFromPath;
	bool bFound = false;
	m_copyfromPath.Reset();
	if (pData)
	{
		CString url = pData->url;

		SVNStatus stat;
		do 
		{
			svn_wc_status2_t * s = stat.GetFirstFileStatus(statPath, copyFromPath, false, svn_depth_infinity, true, true);
			if (s)
			{
				while ((s = stat.GetNextFileStatus(copyFromPath)) != NULL)
				{
					if ((s->entry)&&(s->entry->copyfrom_url))
					{
						if (CUnicodeUtils::GetUnicode(s->entry->copyfrom_url).Compare(url) == 0)
						{
							bFound = true;
							m_copyfromPath = copyFromPath;
							break;	// found the copyfrom path!
						}
					}
				}
			}
			statPath = statPath.GetContainingDirectory();
		} while (!bFound && statPath.HasAdminDir());
	}

	PostMessage(WM_AFTERTHREAD);
	m_progressDlg.Stop();
	InterlockedExchange(&m_bThreadRunning, FALSE);
	return 0;
}

LRESULT CTreeConflictEditorDlg::OnAfterThread(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if ((!m_copyfromPath.IsEmpty())&&(src_left->peg_rev))
	{
		m_sUseTheirs.Format(IDS_TREECONFLICT_RESOLVE_MERGECHANGES, (LPCTSTR)m_copyfromPath.GetFileOrDirectoryName());
		SetDlgItemText(IDC_RESOLVEUSINGTHEIRS, m_sUseTheirs);
	}

	GetDlgItem(IDC_RESOLVEUSINGMINE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RESOLVEUSINGTHEIRS)->ShowWindow(SW_SHOW);
	SetDlgItemText(IDC_INFOLABEL, CString(MAKEINTRESOURCE(IDC_TREECONFLICT_HOWTORESOLVE)));

	return 0;
}
