/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include <symbian_0xEEBB239B.rsg>

#include "symbian_0xEEBB239B.hlp.hrh"
#include "symbian.hrh"
#include "symbian.pan"
#include "symbianApplication.h"
#include "symbianAppUi.h"
#include "symbianAppView.h"

#include "cppunit/extensions/FCppUnit.h"

_LIT(KFileName, "C:\\private\\EEBB239B\\symbian.txt");
_LIT(KText, "FunambolAPITest");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CsymbianAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CsymbianAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Create view object
    iAppView = CsymbianAppView::NewL(ClientRect() );

    // Create a file to write the text to
    TInt err = CCoeEnv::Static()->FsSession().MkDirAll(KFileName);
    if ( (KErrNone != err) && (KErrAlreadyExists != err))
        {
        return;
        }

    RFile file;
    err = file.Replace(CCoeEnv::Static()->FsSession(), KFileName, EFileWrite);
    CleanupClosePushL(file);
    if (KErrNone != err)
        {
        CleanupStack::PopAndDestroy(1); // file
        return;
        }

    RFileWriteStream outputFileStream(file);
    CleanupClosePushL(outputFileStream);
    outputFileStream << KText;

    CleanupStack::PopAndDestroy(2); // outputFileStream, file

    }
// -----------------------------------------------------------------------------
// CsymbianAppUi::CsymbianAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CsymbianAppUi::CsymbianAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CsymbianAppUi::~CsymbianAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CsymbianAppUi::~CsymbianAppUi()
    {
    if (iAppView)
        {
        delete iAppView;
        iAppView = NULL;
        }

    }

// -----------------------------------------------------------------------------
// CsymbianAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CsymbianAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case ECommand1:
        {
            CppUnit::TestManager testManager;
            testManager.run();
        }
        break;
        case ECommand2:
            {
            RFile rFile;

            //Open file where the stream text is
            User::LeaveIfError(rFile.Open(CCoeEnv::Static()->FsSession(),
                                                KFileName, EFileStreamText));//EFileShareReadersOnly));// EFileStreamText));
            CleanupClosePushL(rFile);

            // copy stream from file to RFileStream object
            RFileReadStream inputFileStream(rFile);
            CleanupClosePushL(inputFileStream);

            // HBufC descriptor is created from the RFileStream object.
            HBufC* fileData = HBufC::NewLC(inputFileStream, 32);

            CAknInformationNote* informationNote;

            informationNote = new ( ELeave ) CAknInformationNote;
            // Show the information Note
            informationNote->ExecuteLD( *fileData);

            // Pop loaded resources from the cleanup stack
            CleanupStack::PopAndDestroy(3); // filedata, inputFileStream, rFile
            }
            break;
        case EHelp:
            {

            CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
            }
            break;
        case EAbout:
            {

            CAknMessageQueryDialog* dlg = new (ELeave)CAknMessageQueryDialog();
            dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
            HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
            dlg->QueryHeading()->SetTextL(*title);
            CleanupStack::PopAndDestroy(); //title
            HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
            dlg->SetMessageTextL(*msg);
            CleanupStack::PopAndDestroy(); //msg
            dlg->RunLD();
            }
            break;
        default:
            Panic(EsymbianUi);
            break;
        }
    }
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CsymbianAppUi::HandleStatusPaneSizeChange()
    {
    iAppView->SetRect(ClientRect() );
    }

CArrayFix<TCoeHelpContext>* CsymbianAppUi::HelpContextL() const
    {
#warning "Please see comment about help and UID3..."
    // Note: Help will not work if the application uid3 is not in the
    // protected range.  The default uid3 range for projects created
    // from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
    // can be self signed and installed on the device during testing.
    // Once you get your official uid3 from Symbian Ltd. and find/replace
    // all occurrences of uid3 in your project, the context help will
    // work.
    CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(array);
    array->AppendL(TCoeHelpContext(KUidsymbianApp, KGeneral_Information));
    CleanupStack::Pop(array);
    return array;
    }

// End of File
