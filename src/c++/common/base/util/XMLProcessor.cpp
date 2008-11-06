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


#include <stdlib.h>

#include "base/util/utils.h"
#include "base/util/XMLProcessor.h"
#include "base/util/StringBuffer.h"
#include "base/Log.h"
#include "base/globalsdef.h"

USE_NAMESPACE

//--------------------------------------------------------------- Static functions

static const char *findElementContent(const char *xml,
                          const char *openTag, const char *closeTag,
                          unsigned int* pos     ,
                          unsigned int* startPos,
                          unsigned int* endPos  )
{
    const char *p1, *p2, *xmlptr = xml;

    if (pos) {
        *pos = 0;
    }

    do {
        p1 = strstr(xmlptr, openTag);
        p2 = NULL;

        if (!p1) {
            // Tag not found
            //LOG.debug("XMLProcessor: tag %s not found", openTag);
            return 0;
        }

        p1 += strlen(openTag); // move to end of tag

        // Check the tag type
        switch( *p1 ){
            case ' ':   // <tag attr=xxx>
                // Find the end of the tag (TODO: should check for invalid chars?)
                for (p1++; *p1 != '>'; p1++) {
                    if(*p1 == 0 || *p1 == '<'){
                        LOG.info("XMLProcessor: incomplete tag");
                        return 0;
                    }
                }
                if(*(p1-1) == '/'){ // <tag attr=xxx />
                    // Like case '/' below:
                    p1++;
                    p2=p1;
                    closeTag=0;
                    break;
                }
                // The break is not missing!
                // After the for, we are in the same case of the tag
                // without attributes
            case '>':   // <tag>
                p1++;   // this is the beginning of content
                if (!p1[0]) {
                    LOG.info("XMLProcessor: tag at end of file");
                    return 0;
                }
                // Find the closing tag
                p2 = strstr(p1, closeTag);
                break;
            case '/':
                p1++;
                if(*p1 != '>'){
                    LOG.info("XMLProcessor: invalid empty tag");
                    return 0;
                }
                p1++;
                // The tag is already closed, no content: make end = start
                p2=p1;
                // Invalidate closeTag
                closeTag=0;
                break;
            case '\n':
                p1++;
                p2 = strstr(p1, closeTag);
                break;
            default:
                // This is not the searched tag, search again
                //LOG.debug("XMLProcessor: this is not tag %s", openTag);
                xmlptr = p1;
                p1 = 0;
        }
    }
    while (!p1); // If p1 is null and we are here, it means that the 'default'
                 // case was hit.

    // Closing tag not found
    if (!p2) {
        //
        // This is abc<tag>xyz\0
        //
        p1 = NULL;
        return 0;
    }

    // Okay, if we are here, the tag content has been found
    if (startPos) {
        *startPos = p1 - xml;
    }
    if (endPos) {
        *endPos = p2 - xml ;
    }
    if (pos) {
        *pos = p2-xml;
        if (closeTag){
            *pos += strlen(closeTag);
        }
    }

    return p1;
}

const char* XMLProcessor::getElementContent(const char* xml,
                                            const char* tag,
                                            unsigned int* pos,
                                            unsigned int* startPos,
                                            unsigned int* endPos  )
{
    char *openTag = 0;
    char *closeTag = 0;

    if (!xml) {
        return 0;
    }

    size_t l = strlen(tag);

    if(strcmp(tag, "CDATA") == 0) {
        openTag = stringdup("<![CDATA[");
        closeTag = stringdup("]]>");
    }
    else {
        openTag = new char[l+10];
        closeTag = new char[l+10];
        sprintf(openTag, "<%s", tag);
        sprintf(closeTag, "</%s>", tag);
    }

    const char *ret = findElementContent(xml, openTag, closeTag, pos, startPos, endPos);

    if (openTag)
        delete [] openTag;
    if (closeTag)
        delete [] closeTag;

    return ret;
}

char* XMLProcessor::copyContent(const char* xml,
                                unsigned int startPos,
                                unsigned int endPos  ) {

    char * ret = NULL;

    if (!xml) {
        return 0;
    }
    if (endPos < startPos) {
        return 0;
    }
    if (strlen(xml) < endPos - startPos) {
        return 0;
    }

    // figure out whether the text that we are about to copy
    // contains further elements; if not, treat it as a leaf
    // element and decode entities
    bool isLeaf = true;
    unsigned int pos = startPos;
    while (pos < endPos) {
        if (xml[pos] == '<') {
            isLeaf = false;
            break;
        }
        pos++;
    }

    const char cdataStart[] = "<![CDATA[";
    const int cdataStartLen = sizeof(cdataStart) - 1;
    const char cdataEnd[] = "]]>";
    const int cdataEndLen = sizeof(cdataEnd) - 1;

    // strip CDATA markers at start and end?
    if (!isLeaf &&
        endPos - pos > cdataStartLen + cdataEndLen &&
        !strncmp(xml + pos, cdataStart, cdataStartLen)) {
        // yep, copy content verbatim;
        // search real end of data first
        pos += cdataStartLen;
        unsigned int cdataEndPos = endPos;
        while (cdataEndPos - cdataEndLen > pos) {
            if (!strncmp(xml + cdataEndPos - cdataEndLen,
                          cdataEnd,
                          cdataEndLen)) {
                // found "]]>"
                cdataEndPos -= cdataEndLen;
                break;
            }
            cdataEndPos--;
        }

        ret = new char[cdataEndPos - pos + 1];
        strncpy(ret, xml + pos, cdataEndPos - pos);
        ret[cdataEndPos - pos] = 0;
    } else if (isLeaf) {
        // Decode content of final element:
        // might contain escaped special characters.
        //
        // This must _not_ be done for tags which contain other
        // tags because then we might destroy the content of e.g.
        // <Add><Data><![CDATA[ literal entity &amp; ]]></Data></Add>
        //
        StringBuffer tmp(xml+startPos, endPos - startPos);
        tmp.replaceAll("&lt;", "<");
        tmp.replaceAll("&gt;", ">");
        tmp.replaceAll("&amp;", "&");
        ret = stringdup(tmp.c_str());
    } else {
        size_t len = endPos - startPos;
        ret = new char [len + 1];
        memcpy( ret, xml + startPos, len * sizeof(char));
        ret[len] = 0;
    }

    return ret;
}

char* XMLProcessor::copyElementContent(const char* xml,
                                       const char* tag,
                                       unsigned int* pos)
{
    unsigned int start, end;

    if( getElementContent (xml, tag, pos, &start, &end) ) {
        return copyContent(xml, start, end);
    }
    return 0;
}

/*
* It returns the number of the tag in the xml string
*/

int XMLProcessor::countElementTag(const char* xml, const char* tag) {

    unsigned int count = 0, pos = 0, previous = 0;

    while (getElementContent(&xml[pos], tag , &pos, NULL, NULL) != NULL) {
        pos += previous;
        previous = pos;
        count ++;
    }
    return count;
}


/*
* Returns the next tag found in the xml string. It looks at the < and > tags to retrieve
* the name of the token.
* If <tag xmlns...> it returns "tag"
* The "pos" argument will contain the position of the close <tag/>
* The return value is a new char* and must be fred by the caller. If no tag is found, NULL is returned
*/
const char* XMLProcessor::getNextTag(const char*xml, int* pos) {

    const char* p1, *p2, *p4, *p3 = NULL;
    char* ret = NULL;
    p1 = p2 = p4 = xml;
    int i = 0, k = 0, len = 0;
    bool found = false;
    len = strlen(xml);

    for (i = 0; i < len; i++) {
        if (found) {
            if (p4[i] != '/' && p4[i] != '!' && p4[i] != '-' ) {
                break; // the element found is right!
            } else {
                found = false;
            }
        }
        if (p4[i] == '<') {
            p1 = &p4[i];
            found = true;
        }
    }

    if (found) {
        p2 = p1;
        for (k = 0; k < len; k++) {
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == ' ') {
                p3 = p1;
            }
            else if (*p1 == '>') {
                *pos = p1 -xml + 1;
                if (p3) {
                    p1 = p3;
                }
                ret = new char[(p1)-(p2)];
                strncpy(ret, p2+1, (p1)-(p2+1));
                ret[(p1)-(p2+1)] = 0;
                return ret;
                break;
            }
            p1 = p1 + 1;
        }
    }
    return ret;

}



/*
* count the number of "&" (passed as a string) in the token.
*/
int XMLProcessor::countAnd(const char* token) {
    return countChar(token, "&");
}

int XMLProcessor::countChar(const char* token, const char* element) {

    const char* p1, *p2;
    p1 = p2 = token;
    int i = 0, k = 0, len = 0;

    while (strstr(p1, element) != NULL) {
        len = strlen(p2);
        for (k = 0; k < len; k++) {
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == '&') {
                p1 = p1 + 1;
                i++;
                break;
            }
            p1 = p1 + 1;
        }
    }
    return i;
}


/*
* it's as getElementContent but it doesn't get the content of a tag if the parent match except.
* The parent can be more than one. They have to be separated by &
* i.e.
*
* getElementContentExcept(xmlPtr, "Add", "Sync&Atomic", &post)
*
* The function returns "... to keep ... " content only
*
* <SyncBody>
   <Sync>
     <Add>... to avoid ...</Add>
   </Sync>
   <Add>... to keep ...</Add>
   <Sync>
     <Add>... to avoid ...</Add>
   </Sync>
    <Atomic>
     <Add>... to avoid ...</Add>
   </Atomic>
 </SyncBody>
*/

char* XMLProcessor::copyElementContentExcept(const char*xmlPtr    ,
                                             const char*tag       ,
                                             const char*except    ,
                                             unsigned int* post) {

    char*  ret    = NULL;
    const char*  found  = NULL;
    const char*  xml    = NULL;
    char** array = NULL;
    int*  validElement = NULL;
    int count        = 0, countTag = 0;
    bool notValid  = false;

    unsigned int pos      = 0, previous         = 0,
                 position = 0, previousPosition = 0,
                 startPos = 0, endPos           = 0;

    xml = xmlPtr;

    if (xml == NULL) {
        return NULL;
    }

    if (except == NULL) {
        ret = copyElementContent(xml, tag, &pos);
        if (post) {
            *post = pos;
        }
        return ret;
    }
    count = countAnd(except);
    count++;

    array = new char*[count + 1];
	int l;
    for (l = 0; l <= count; l++) {
        array[l] = NULL;
    }

    // represent a element found that can be used properly
    countTag = countElementTag(xml, tag);
    if (countTag > 0) {
        validElement = new int[countTag];
        for (l = 0; l < countTag; l++) {
            validElement[l] = 1;
        }
    }

    char* internal = stringdup(except);
    char* p1, *p2;
    p1 = p2 = internal;
    int i = 0, k = 0, len = 0;

    while (strstr(p2, "&") != NULL) {
        len = strlen(p2);
        for (k = 0; k < len; k++) {
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == '&') {
                *p1 = 0;
                array[i] = stringdup(p2);
                p1 = p1 + 1;
                p2 = p1;
                i++;
                break;
            }
            p1 = p1 + 1;
        }
    }

    if (i == 0 || k < len) {
        if (array[i]) { delete [] array[i]; array[i] = NULL; }
        array[i] = stringdup(p2);
        i++;
    }

    for (int s = 0; s < count; s ++) {
        i = 0;

        do {
            safeDel(&ret);
            k = 0;

            pos = 0, previous = 0;
            while ((ret = copyElementContent(&xml[pos], tag, &pos)) != NULL) {

                if (validElement && validElement[k] == 1) {
                    pos += previous;
                    position = 0;
                    previousPosition = 0;
                    startPos = 0;
                    endPos   = 0;
                    while ((found = getElementContent(&xml[position], array[i], &position, &startPos, &endPos)) != NULL ) {

                        startPos += previousPosition;
                        endPos   += previousPosition;
                        position += previousPosition;
                        if (startPos < pos && pos < endPos) {
                            notValid = true;
                            break;
                        }
                        previousPosition = position;

                    }

                    if (notValid) {
                        notValid = false;
                        safeDel(&ret);
                        validElement[k] = 0;
                    } else {
                        if (post) {
                            *post = pos;
                        }
                        break;
                    }
                    previous = pos;
                    k++;
                } else {
                    pos += previous;
                    previous = pos;
                    k++;
                    safeDel(&ret); // if (ret) { delete [] ret; ret = NULL; }
                }
            }
            i++;

        } while(array[i] != NULL);

        if (count > 1) {
            char* tmp = stringdup(array[0]);

            for (int m = 0; m < count - 1; m++) {
                if (array[m]) { delete [] array[m]; array[m] = NULL; }
                //safeDel(&array[m]);
                array[m] = stringdup(array[m+1]);
            }
            //safeDel(&array[count-1]);
            if (array[count-1]) { delete [] array[count-1]; array[count-1] = NULL; }
            array[count-1] = stringdup(tmp);
            safeDel(&tmp);
        }
    }

    if (notValid) {
        if (ret) {
            safeDel(&ret);
        }
        if (post) {
            *post = 0;
        }
    } else {
        ;
    }

    safeDel(&internal);
    if (validElement) {
        delete [] validElement; validElement = NULL;
    }

    for (l = 0; l <= count; l++) {
        if (array[l]) { delete [] array[l]; array[l] = NULL; }
    }
    delete [] array; array = NULL;
    //safeDelete(array);

    return ret;
}

/*
* It returns getElementContent value but it depends on the level that is specified.
* It return only ... to keep ... independently on the tag in which it is contained if lev is zero.
*
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
*
* The parameters are:
* xml: the xml to analize
* tag: the name of the tag to use
* lev: the inner level in which discover the tag
* startLevel: the starting level from which start the search
* pos: the position index search end
*
* The follow piece of code explain how get the value tag "... to keep ..." that is in the root (0) level.
* Note the startLevel declaration and initialization to -1 value
*
*
*    char* p = NULL;
*    unsigned int pos = 0, previous = 0;
*    int startLevel = -1;
*    while ((p = XMLProcessor::getElementContentLevel(&xml[pos], "Add", 0, &startLevel, &pos)) != NULL) {
*        pos += previous;
*        previous = pos;
*    }
*
*/

char* XMLProcessor::copyElementContentLevel(const char*xml   ,
                                            const char*tag   ,
                                            unsigned int* pos,
                                            int           lev ,
                                            int*          startLevel)  {

    const char* p1 = NULL;
    const char* p2 = NULL;
    char* ret = NULL;
    bool openBracket  = false;  // <
    bool closeBracket = false;  // >
    bool preCloseBracket = false;  //<.../
    bool openTag      = false;
    bool closeTag     = false;

    char tagNameFound[40];

    int level               = -1;
    unsigned int xmlLength  = (unsigned int)-1;
    unsigned int l          = (unsigned int)-1;
    unsigned int previousIndex = (unsigned int)-1;
    unsigned int i          =  0;

    if (xml == NULL) {
        goto finally;
    }

    if (lev < 0) {
        return copyElementContent(xml, tag, pos);
    }

    xmlLength = strlen(xml);
    l = strlen(tag);

    if (pos != NULL) {
        *pos = 0;
    }
    if (startLevel != NULL) {
       level = *startLevel;
    }

    p1 = p2 = xml;

    for (i = 0; i < xmlLength; i ++) {
        if (!strncmp(p1 + i, "<![CDATA[", strlen("<![CDATA["))) {
            // skip over content
            while(p1[i]) {
                i++;
                if (!strcmp(p1 + i, "]]>")) {
                    i += strlen("]]>");
                    break;
                }
            }
        }
        if (p1[i] == '<') {
            openBracket = true;
            previousIndex = i;
            p2 = &p1[i];

        } else if (p1[i] == '/') {
            if (previousIndex == (i - 1)) {
                // </...>
                preCloseBracket = true;
            } else {
                // might be <.../>, which will be checked below
                // with p1[i - 1] == '/'
            }

        } else if (p1[i] == '>') {

            if (openBracket == false) {
                closeBracket = false;
                preCloseBracket = false;
            } else {
                if (preCloseBracket) {
                    closeTag = true;
                }
                else if (openBracket && p1[i - 1] == '/') {
                    // <.../>: do not change levels or open tag,
                    // it has been closed already
                } else {
                    openTag = true;
                }
                closeBracket = true;

                if (closeTag) {
                    level--;
                    openBracket  = false;
                    closeBracket = false;
                    preCloseBracket = false;
                    openTag      = false;
                    closeTag     = false;

                } else if (openTag) {
                    level++;
                } else {
                    openBracket  = false;
                    closeBracket = false;
                    preCloseBracket = false;
                    openTag      = false;
                    closeTag     = false;

                }
            }
        }
          if (openTag && openBracket && closeBracket) {
            int n = (&p1[i] - p2 - 1);
            strncpy(tagNameFound, p2 + 1, n);
            tagNameFound[n] = 0;
            if (strcmp(tagNameFound, tag) == 0 && (level == lev)) {
                unsigned int internalPos;
                ret = copyElementContent(p2, tag, &internalPos);
                if (pos) {
                    *pos = p2 - xml + internalPos;
                }
                if (startLevel) {
                    *startLevel = level - 1;
                }
                break;
            }
            openBracket  = false;
            closeBracket = false;
        }
    }

finally:
    openBracket  = false;
    closeBracket = false;
    preCloseBracket = false;
    openTag      = false;
    closeTag     = false;
    return ret;

}

/**
 * Get the attribute list of the forst element 'tag', returning a pointer
 * to the beginning of the string in the original buffer 'xml', and the
 * starting and ending position of the substring.
 *
 * @param xml - the XML document to process.
 * @param tag - the tag name to find
 * @param startPos - return value - the start pos of the attribute list
 * @param endPos - return value - the end position of the attribute list
 */
const char* XMLProcessor::getElementAttributes(const char* xml,
                                          const char* tag,
                                          unsigned int* startPos,
                                          unsigned int* endPos,
                                          bool escaped) {

    const char* p1 = NULL;
    const char* p2 = NULL;
    unsigned int l = strlen(tag);

    // example ot tag with attribute list
    // <body enc="base64">
    char *openTag = 0; //<tag

    if (!xml) {
        goto finally;
    }

    if(strcmp(tag, "CDATA") == 0) {
        goto finally;
    }
    else {
        openTag = new char[l+10];
        if (escaped){
            sprintf(openTag, "&lt;%s ", tag);
        }
        else{
            sprintf(openTag, "<%s ", tag);
        }
    }

    p1 = strstr(xml, openTag);

    if (!p1) {
        LOG.debug("XMLProcessor: tag %s not found", tag);
        goto finally;
    }
    // move to the beginning of the attribute list
    p1 += strlen(openTag);

    // find the end of the tag
    for (p2 = p1; *p2 != '>'; p2++) {
        if(*p2 == 0 || *p2 == '<'){
            LOG.info("XMLProcessor: incomplete tag");
            goto finally;
        }
    }
    // set the return parameters
    if (startPos != NULL) {
        *startPos = p1 - xml;
    }
    if (endPos != NULL) {
        *endPos = p2 - xml ;
    }

    finally:

    if (openTag)
        delete [] openTag;

    return p1;

}


StringBuffer XMLProcessor::makeElement(const char* tag, const char* val, const char* attr)
{
    StringBuffer s;

    if (!val)
        return s;
    if (!val[0])
        return s;

    size_t len = strlen(tag);
    char* t1 = new char[len + 4]; // <  >  0, whitout closing >
    char* t2 = new char[len + 6]; // </ > \n 0

    sprintf(t1, "<%s", tag);
    sprintf(t2, "</%s>\n", tag);

    s = t1;
    if (attr != NULL)
    {
        s += " ";
        s += attr;
    }
    s += ">";
    s += val; s += t2;

    delete [] t1;
    delete [] t2;

    return s;
}


StringBuffer XMLProcessor::makeElement(const char* tag,
                                    const char* val,
                                    ArrayList attrList) {

    StringBuffer s;

    for (int i = 0; i < attrList.size(); i++)
    {
        KeyValuePair* item = (KeyValuePair*)attrList[i];
        if (i > 0)
            s += " ";
        s += item->getKey();
        s += "=\"";
        s += item->getValue();
        s += "\"";
    }
    s = makeElement(tag, val, s.c_str());

    return s;
}
