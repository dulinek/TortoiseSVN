// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2011 - TortoiseSVN

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
#pragma once
#include "resource.h"
#include "EditPropBase.h"
#include "StandAloneDlg.h"
#include "UserProperties.h"


// EditPropUserMultiLine dialog

class EditPropUserMultiLine : public CStandAloneDialog, public EditPropBase
{
    DECLARE_DYNAMIC(EditPropUserMultiLine)

public:
    EditPropUserMultiLine(CWnd* pParent, const UserProp * p);   // standard constructor
    virtual ~EditPropUserMultiLine();

    void SetUserProp(UserProp * p) {m_userprop = p;}

    // Dialog Data
    enum { IDD = IDD_EDITPROPUSERMULTILINE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()

    INT_PTR DoModal() { return CStandAloneDialog::DoModal(); }
private:
    CString             m_sLine;
    CString             m_sLabel;
    const UserProp *    m_userprop;
};
