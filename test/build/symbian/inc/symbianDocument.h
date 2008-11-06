/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#ifndef __SYMBIANDOCUMENT_h__
#define __SYMBIANDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CsymbianAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CsymbianDocument application class.
 * An instance of class CsymbianDocument is the Document part of the
 * AVKON application framework for the symbian example application.
 */
class CsymbianDocument : public CAknDocument
    {
public:
    // Constructors and destructor

    /**
     * NewL.
     * Two-phased constructor.
     * Construct a CsymbianDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CsymbianDocument.
     */
    static CsymbianDocument* NewL(CEikApplication& aApp);

    /**
     * NewLC.
     * Two-phased constructor.
     * Construct a CsymbianDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CsymbianDocument.
     */
    static CsymbianDocument* NewLC(CEikApplication& aApp);

    /**
     * ~CsymbianDocument
     * Virtual Destructor.
     */
    virtual ~CsymbianDocument();

public:
    // Functions from base classes

    /**
     * CreateAppUiL
     * From CEikDocument, CreateAppUiL.
     * Create a CsymbianAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CsymbianDocument.
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    CsymbianDocument(CEikApplication& aApp);

    };

#endif // __SYMBIANDOCUMENT_h__
// End of File
