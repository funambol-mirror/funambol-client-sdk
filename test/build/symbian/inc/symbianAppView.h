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

#ifndef __SYMBIANAPPVIEW_h__
#define __SYMBIANAPPVIEW_h__
/** @cond DEV */

// INCLUDES
#include <coecntrl.h>

// CLASS DECLARATION
class CsymbianAppView : public CCoeControl
    {
public:
    // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * Create a CsymbianAppView object, which will draw itself to aRect.
     * @param aRect The rectangle this view will be drawn to.
     * @return a pointer to the created instance of CsymbianAppView.
     */
    static CsymbianAppView* NewL(const TRect& aRect);

    /**
     * NewLC.
     * Two-phased constructor.
     * Create a CsymbianAppView object, which will draw itself
     * to aRect.
     * @param aRect Rectangle this view will be drawn to.
     * @return A pointer to the created instance of CsymbianAppView.
     */
    static CsymbianAppView* NewLC(const TRect& aRect);

    /**
     * ~CsymbianAppView
     * Virtual Destructor.
     */
    virtual ~CsymbianAppView();

public:
    // Functions from base classes

    /**
     * From CCoeControl, Draw
     * Draw this CsymbianAppView to the screen.
     * @param aRect the rectangle of this view that needs updating
     */
    void Draw(const TRect& aRect) const;

    /**
     * From CoeControl, SizeChanged.
     * Called by framework when the view size is changed.
     */
    virtual void SizeChanged();

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CsymbianAppView object.
     * @param aRect The rectangle this view will be drawn to.
     */
    void ConstructL(const TRect& aRect);

    /**
     * CsymbianAppView.
     * C++ default constructor.
     */
    CsymbianAppView();

    };

#endif // __SYMBIANAPPVIEW_h__
/** @endcond */
// End of File

