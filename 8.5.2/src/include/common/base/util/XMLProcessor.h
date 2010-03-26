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

#ifndef INCL_XML_PROCESSOR
#define INCL_XML_PROCESSOR
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "base/util/KeyValuePair.h"
#include "base/util/ArrayList.h"

#define DIM_TAG 32
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/*
 * This class implements a minimalist XML processor used to extract
 * the content between a known tag, and to make an XML element with
 * a specified value.
 *
 */

class XMLProcessor  {

public:

    /*
     * Extracts the content of a tag into an XML message. It is supposed that the
     * message is a valid XML message. It returns NULL in case the tag is not
     * found or the XML fragment is not in the expected form.
     * The returned pointer (if not NULL) is allocated with the new operator and
     * must be discarded with the operator delete.
     *
     * @param xml the xml fragment
     * @param tag the tag we want the content
     * @param pos (OUTPUT) the position where the tag is found (ignored if NULL)
     *
     */
    static char* copyElementContent(const char*  xml, const char*  tag,
                                    unsigned int* pos = NULL);
    static void copyElementContent(StringBuffer& res, const char*  xml, const char*  tag,
                                   unsigned int* pos = NULL);

    /*
     * It's like copyElementContent but it doesn't allocate new memory.
     *
     * @param xml the xml fragment
     * @param tag the tag we want the content
     * @param pos (OUTPUT) the position where the tag is found (ignored if NULL)
     * @param startPos (OUTPUT) the start position of the tag content (ignored if NULL)
     * @param endPos (OUTPUT) the end position of the tag content (ignored if NULL)
     */
    static const char*  getElementContent(const char*  xml, const char*  tag,
                    unsigned int* pos, unsigned int* startPos, unsigned int* endPos);


    static void getElementContent(const char*  xml, const char*  tag,
                                  unsigned int* pos, unsigned int* startPos,
                                  unsigned int* endPos, StringBuffer& res);

    /*
    * It returns the number of the tag in the xml string
    */
    static int countElementTag(const char*  xml, const char*  tag);

    static int countAnd(const char*  token);

    static int countChar(const char*  token, const char*  element);

    static const char* getNextTag(const char* xml, int* pos);
   /*
    * it's as copyElementContent but it doesn't get the content of a tag if
    * the parent match except.
    * The parent can be more than one. They have to be separated by &
    * i.e.
    *
    * copyElementContentExcept(xmlPtr, "Add", "Sync&Atomic", &post)
    *
    * The function returns "... to keep ... " content only
    *
    * <SyncBody>
    *   <Sync>
    *     <Add>... to avoid ...</Add>
    *   </Sync>
    *   <Add>... to keep ...</Add>
    *   <Sync>
    *     <Add>... to avoid ...</Add>
    *   </Sync>
    *    <Atomic>
    *     <Add>... to avoid ...</Add>
    *   </Atomic>
    * </SyncBody>
    */
    static char* copyElementContentExcept(const char* xml       ,
                                          const char* tag       ,
                                          const char* except    ,
                                          unsigned int* pos);


    static char* copyElementContentLevel(const char* xml,
                                         const char* tag,
                                         unsigned int* pos,
                                         int           lev = 0 ,  // the root value level
                                         int* startLevel   = NULL);

    static void copyElementContentLevel(StringBuffer& res,
                                        const char* xml,
                                        const char* tag,
                                        unsigned int* pos,
                                        int           lev = 0 ,  // the root value level
                                        int* startLevel   = NULL);



    /*
     * It returns the content of the buffer specified by startPos (initial position)
     * and and endPos (the end position)
     *
     * It allocates new memory that has to be freed by caller with delete [].
     *
     * @param xml the xml fragment
     * @param startPos the start position of the tag content
     * @param endPos  the end position of the tag content
     *
     */

    static char* copyContent(const char*  xml, unsigned int startPos, unsigned int endPos);
    static void copyContent(const char*  xml, unsigned int startPos,
                            unsigned int endPos, StringBuffer& res);

    /**
     * Create an XML element with the specified tag and value.
     *
     * @param tag - the tag we want the content
     * @param val - the element value
     */
    //static StringBuffer makeElement(const WCHAR* tag, const WCHAR* val);

    /**
     * Create an XML element with the specified tag, value and attribute list.
     *
     * @param tag - the tag we want the content
     * @param val - the element value
     * @param attr - attribute list, already formated - ex: enc="base64"
     */
    static StringBuffer makeElement(const char*  tag,
                                    const char*  val,
                                    const char*  attr = NULL);

    /**
     * Create an XML element with the specified tag, value and attribute list.
     *
     * @param tag - the tag we want the content
     * @param val - the element value
     * @param attr - attribute list, pairs of name and value
     */
    static StringBuffer makeElement(const char*  tag,
                                    const char*  val,
                                    ArrayList attrList);

    /**
     * Same with bool value
     */
    static StringBuffer makeElement(const char*  tag, bool val) {
        return makeElement( tag, (val?"true":"false") ) ;
    }

    /**
     * Same with integer value
     */
    static StringBuffer makeElement(const char*  tag, int val) {
        return makeElement( tag, StringBuffer().append(val) ) ;
    }

    /**
     * Extracts the attributes list from a tag into an XML message. It is supposed that the
     * message is a valid XML message. It returns NULL in case the tag is not
     * found or the XML fragment is not in the expected form. The calling function must
     * further parse the resultto extract attribute name and value
     *
     * @param xml the xml fragment
     * @param tag the tag whom attributes we want
     * @param startPos (OUTPUT) the start position of the tag attributes (ignored if NULL)
     * @param endPos (OUTPUT) the end position of the tag attributes (ignored if NULL)
     *
    */
    static const char*  getElementAttributes(
                                         const char*  xml,
                                         const char*  tag,
                                         unsigned int*  startPos,
                                         unsigned int*  endPos,
                                         bool escaped = false);

};


END_NAMESPACE

/** @endcond */
#endif

