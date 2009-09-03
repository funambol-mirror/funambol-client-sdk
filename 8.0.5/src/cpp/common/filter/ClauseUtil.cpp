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

/**
 * This class is a collection of utility methods used to manage Clause
 * and related classes.
 */

#include "base/util/StringBuffer.h"
#include "filter/ClauseUtil.h"
#include "filter/AllClause.h"
#include "filter/FieldClause.h"
#include "filter/LogicalClause.h"
#include "filter/WhereClause.h"
#include "syncml/core/Property.h"
#include "syncml/core/PropParam.h"
#include "base/globalsdef.h"

USE_NAMESPACE


/**
 * Converts a WhereClause to a CGI query string. The returned
 * string is allocated with the new [] operator and must be
 * deleted by the caller with the delete [] operator.
 *
 * @param clause the clause to convert
 *
 * @return the corresponding CGI query string
 */
char* ClauseUtil::toCGIQuery(Clause& clause) {
    StringBuffer query;

    ArrayList operands;
    WhereClause* whereClause = NULL;

    if (clause.type == WHERE_CLAUSE) {
        operands.add(clause);
    } else {
        operands = *((LogicalClause&)clause).getOperands();
    }

    for (int i=0; i<operands.size(); ++i) {
        whereClause = (WhereClause*)operands.get(i);
        if (i) {
            switch (((LogicalClause&)clause).getOperator()) {
                case AND:
                    query.append("&AND;");
                    break;
                case OR:
                    query.append("&OR;");
                    break;
                default:
                    // Nothing to do in these cases
                    break;
            }
        }
        query.append(whereClause->getProperty());
        switch (whereClause->getOperator()) {
            case EQ:
                if (whereClause->isCaseSensitive()) {
                    query.append("&EQ;");
                } else {
                    query.append("&iEQ;");
                }
                break;
            case NE:
                if (whereClause->isCaseSensitive()) {
                    query.append("&NE;");
                } else {
                    query.append("&iNE;");
                }
                break;
            case LT:
                if (whereClause->isCaseSensitive()) {
                    query.append("&LT;");
                } else {
                    query.append("&iLT;");
                }
                break;
            case GT:
                if (whereClause->isCaseSensitive()) {
                    query.append("&GT;");
                } else {
                    query.append("&iGT;");
                }
                break;
            case LE:
                if (whereClause->isCaseSensitive()) {
                    query.append("&LE;");
                } else {
                    query.append("&iLE;");
                }
                break;
            case GE:
                if (whereClause->isCaseSensitive()) {
                    query.append("&GE;");
                } else {
                    query.append("&iGE;");
                }
                break;
            case CONTAIN:
                if (whereClause->isCaseSensitive()) {
                    query.append("&CON;");
                } else {
                    query.append("&iCON;");
                }
                break;
            case NCONTAIN:
                if (whereClause->isCaseSensitive()) {
                    query.append("&NCON;");
                } else {
                    query.append("&iNCON;");
                }
                break;
            default:
                // This is either UNKNOWN or an error. In both cases
                // there is nothing to do
                break;
        }
        query.append(whereClause->getValue());
    }

    return stringdup(query.c_str());
}

/**
 * Converts a Clause to a Filter object. The Filter is allocated
 * with the new operator and must be deleted by the caller with
 * the delete operator.
 *
 * A Filter is created by an AND LogicalClause where the first operand
 * is the field filter and the second operand is the record filter. The
 * first operand must be of type FieldClause; the latter of type
 * WhereClause.
 *
 * @param clause the clause to convert
 *
 * @return the corresponding filter; NULL is returned in case it was
 *         not possible to create the filter
 */
Filter* ClauseUtil::toFilter(SourceFilter& sourceFilter) {
    LogicalClause* clause = sourceFilter.getClause();

    //
    // A filter is created by an AND LogicaFilter...
    //
    if (clause->getOperator() != AND) {
        return NULL;
    }

    //
    // A AND Clause must have at least two operands
    //
    ArrayList* operands = clause->getOperands();
    if (operands->size() < 2) {
        return NULL;
    }



    //
    // The first operand must be a FieldClause or AllClause
    //
    Clause* c = (Clause*)operands->get(0);

    if ((c->type != FIELD_CLAUSE) && (c->type != ALL_CLAUSE)) {
        return NULL;
    }

    FieldClause* fieldClause = (c->type == ALL_CLAUSE)
                             ? NULL
                             : (FieldClause*)c;

    //
    // The second operand mustbe a WhereClause
    //
    c = (Clause*)operands->get(1);

    if (  (c->type != LOGICAL_CLAUSE)
       && (c->type != ALL_CLAUSE)
       && (c->type != WHERE_CLAUSE)) {
        return NULL;
    }

    Clause* recordClause = (c->type == ALL_CLAUSE)
                         ? NULL
                         : c;

    //
    // Now we can start to create the filter...
    //
    Meta meta;
    meta.setType(sourceFilter.getType());

    Filter* filter = new Filter();
    filter->setMeta(&meta);

    if (fieldClause) {

        Item field;
        Meta fieldMeta;
        ComplexData fieldData;

        fieldMeta.setType("application/vnd.syncml-devinf+xml");
        fieldData.setProperties(fieldClause->getProperties());

        field.setMeta(&fieldMeta);
        field.setData(&fieldData);

        filter->setField(&field);
    }

    if (recordClause) {
        Item record;
        Meta recordMeta;
        ComplexData recordData;

        recordMeta.setType("syncml:filtertype-cgi");
		
		char* query = toCGIQuery(*recordClause);
        recordData.setData(query);
        safeDelete(&query);

        record.setMeta(&recordMeta);
        record.setData(&recordData);

        filter->setRecord(&record);
    }

    if (sourceFilter.isInclusive()) {
        filter->setFilterType("INCLUSIVE");
    }

    return filter;
}

 /**
  * Creates the filter clause given download age, body size and attach size.
  * since, bodySize and attachSize have the following meaning:
  *
  * since
  * -----
  * Since when we want download new emails?
  *
  * NULL: since ever
  * x : emails earlier than the date x
  *
  * bodySize
  * --------
  * How much body shall be downloaded?
  *
  * 0 : only headers (e.g. subject)
  * -1: no limits
  * x : up to x Kb
  *
  * attachSize
  * ----------
  * If there are attachments, which ones shall be downloaded?
  *
  * 0 : no attachments
  * -1: all attachments
  * x : attachments up to x kb
  *
  ***************************************************************************
  * SINCE 2006-04-18: changed the meaning of the flag
  *
  * if since == -1, bodySize == -1, attachSize == -1 -> no filter
  *
  * since: keeps the same meaning
  *
  * bodySize: == 0  there is only the header of the mail. No attach is consider
  *           == 1  there is the header and the body. AttachSize regards only the
                    amount in byte of the Body. Actually it doesn't refer to attachSize.
              == -1 permits to consider the attachments too. If attachsize > 0 the filter
                    contains the amount of header + body + attachment until to attachSize.
  * attachSize: 1   it is consider only if bodySize is -1 or 1.

  *
  *
  */
SourceFilter* ClauseUtil::createSourceFilter(const WCHAR* since, int bodySize, int attachSize) {
    if ((since == NULL) && (bodySize == -1) && (attachSize == -1)) {
        //
        // No filter is needed
        //
        return NULL;
    }

    ArrayList operands;

    //
    // FIELD CLAUSE(S)
    // ---------------
    //

    ArrayList properties;

    if (bodySize >= 0) {
        Property p;

        p.setPropName("emailitem");

        if (bodySize > 0) {
            ArrayList params;
            PropParam textParam;
            textParam.setParamName("texttype");

            params.add(textParam);
            p.setPropParams(params);

            if (attachSize > 0) {
                p.setMaxSize(attachSize*1024);
            }
        }
        properties.add(p);

    } else {

        if (attachSize == -1) {
            // do nothing
        }
        else if (attachSize > 0) {
            Property p;

            p.setPropName("emailitem");
            ArrayList params;

            PropParam textParam;
            textParam.setParamName("texttype");
            params.add(textParam);

            PropParam attachParam;
            attachParam.setParamName("attachtype");
            params.add(attachParam);

            p.setPropParams(params);

            p.setMaxSize(attachSize*1024);
            properties.add(p);
        }

    }

/*
    if (bodySize >= 0) {
        Property p;

        p.setPropName("emailitem");
        if (bodySize > 0) {
            ArrayList params;
            PropParam textParam;
            textParam.setParamName("texttype");

            params.add(textParam);

            p.setPropParams(params);
            p.setMaxSize(bodySize*1024);
        }
        properties.add(p);
    }

    if (attachSize >= 0) {
        ArrayList params;
        PropParam attachParam;
        attachParam.setParamName("attachtype");

        params.add(attachParam);

        Property p;

        p.setPropName("emailitem");
        p.setPropParams(params);
        p.setMaxSize(attachSize*1024);

        properties.add(p);
    }
*/
    if (properties.size() == 0) {
        AllClause all;
        operands.add(all);
    } else {
        FieldClause fieldClause(properties);
        operands.add(fieldClause);
    }

    //
    // RECORD CLAUSE
    // -------------
    //
    //
    // If since is not NULL we need to add a Record clause
    //
    if (since) {
        char *s = toMultibyte(since);
        WhereClause recordClause("modified", s, GE, false);
        operands.add(recordClause);
        delete [] s;
    } else {
        AllClause all;
        operands.add(all);
    }

    LogicalClause clause(AND, operands);

    SourceFilter* filter = new SourceFilter();
    filter->setClause(clause);

    return filter;
}

SourceFilter* ClauseUtil::createSourceFilterInclusive(const char* luid, int size) {

    if (!luid) {
        //
        // No filter is needed
        //
        return NULL;
    }

    ArrayList operands;

    //
    // FIELD CLAUSE(S)
    // ---------------
    //

    ArrayList properties;

    Property p;
    p.setPropName("emailitem");
    ArrayList params;

    PropParam textParam;
    textParam.setParamName("texttype");
    params.add(textParam);

    PropParam attachParam;
    attachParam.setParamName("attachtype");
    params.add(attachParam);
    p.setPropParams(params);

    p.setMaxSize(size*1024);
    properties.add(p);

    if (properties.size() == 0) {
        AllClause all;
        operands.add(all);
    } else {
        FieldClause fieldClause(properties);
        operands.add(fieldClause);
    }

    //
    // RECORD CLAUSE
    // -------------
    //

    if (luid) {
        WhereClause recordClause("&LUID", luid, EQ, false);
        operands.add(recordClause);
    } else {
        AllClause all;
        operands.add(all);
    }

    LogicalClause clause(AND, operands);

    SourceFilter* filter = new SourceFilter();
    filter->setClause(clause);
    filter->setInclusive(true);

    return filter;
}

