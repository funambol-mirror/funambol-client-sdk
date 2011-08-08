/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

package com.funambol.storage;

import java.util.Vector;
import java.util.Hashtable;

/**
 */
public class QueryFilter {

    public static final int EQUAL = 0;
    public static final int NOT_EQUAL = 1;
    public static final int GREAT = 2;
    public static final int GREAT_EQUAL = 3;
    public static final int LESS = 4;
    public static final int LESS_EQUAL = 5;

    private static final String SQL_COMPARISON_OPERATOR[] = {"=","<>",">",">=","<","<="};

    // We may define lexycographical operators in the future

    private Vector filterKeys = null;
    private Hashtable filterValues = null;

    public QueryFilter() {
    }

    public QueryFilter(Object key) {
        filterKeys = new Vector();
        filterKeys.addElement(key);
    }

    public QueryFilter(Vector keys) {
        filterKeys = keys;
    }

    /**
     * Defines a simple filter for a given field. For each field only one filter
     * can be defined and the operator being used is one among EQUAL, NOT_EQUAL,
     * NUMERIC_GREAT, NUMERIC_GREAT_EQUAL, NUMERIC_LESS, NUMERIC_LESS_EQUAL.
     * Values condition can only be set if no keys filtering has been defined,
     * otherwise an IllegalArgumentException is raised.
     * The condition specified in this method can be put in OR or AND with other
     * conditions.
     *
     * @param idx the index of the field (0 is not allowed as it is the key)
     * @param and specifies if the condition is in AND or OR with other
     * conditions
     * @param operator the comparison operator to be used
     * @param value the value to be used in the comparison
     */
    public void setValueFilter(int idx, boolean and, int operator, Object value) {

        if (filterKeys != null) {
            throw new IllegalArgumentException("Cannot define a value filter on top of a key filter");
        }

        if (idx == 0) {
            throw new IllegalArgumentException("Cannot define a value filter on the key");
        }

        if (filterValues == null) {
            filterValues = new Hashtable();
        }
        FieldFilter f = new FieldFilter(and, operator, value);
        filterValues.put(new Integer(idx), f);
    }

    /**
     * This method checks if the given row shall be included in the query
     * result.
     *
     * @param value the table row to be checked
     * @return true if the row shall be included in the result
     */
    protected boolean filterRow(Tuple value) {
        // Keys are in "OR"
        if (filterKeys != null) {
            Object key = value.getKey();
            if (!filterKeys.contains(key)) {
                return false;
            }
            // Note that if a key filter is set, then no value filter can be
            // defined.
            return true;
        }

        if (filterValues != null) {
            boolean res = false;
            boolean resInitialized = false;
            for(int i=0;i<value.getArity();++i) {
                Object v = value.getField(i);
                // Is there a filter on this value?
                // TODO as an optimization we can shortcut the evaluation
                FieldFilter f = (FieldFilter)filterValues.get(new Integer(i));
                if (f != null) {
                    int operator = f.getOperator();
                    Object filterValue = f.getValue();
                    boolean currentCond = compareValues(value.getType(i), operator, v, filterValue);
                    if(!resInitialized) {
                        res = f.getAnd() ? true : false;
                        resInitialized = true;
                    }
                    if (f.getAnd()) {
                        res = res && currentCond;
                    } else {
                        res = res || currentCond;
                    }
                }
            }
            return res;
        }
        return true;
    }

    // This method returns a select clause equivalent to this filter. This can
    // be used by StringTable implementations based on a SQL database
    protected String getSQLWhereClause(Table table) {
        StringBuffer clause = new StringBuffer();

        if (filterKeys != null) {
            // Keys are in "OR"
            for(int i=0;i<filterKeys.size();++i) {
                Object key = (Object)filterKeys.elementAt(i);
                if (i > 0) {
                    clause.append(" OR ");
                }
                clause.append(table.getColName(0)).append("=").append("'").append(key.toString()).append("'");
            }
            return clause.toString();
        }

        if (filterValues != null) {
            boolean firstCond = true;
            for(int i=0;i<table.getArity();++i) {
                // Is there a filter on this value?
                FieldFilter f = (FieldFilter)filterValues.get(new Integer(i));
                if (f != null) {
                    int operator = f.getOperator();
                    Object filterValue = f.getValue();

                    if (!firstCond) {
                        if (f.getAnd()) {
                            clause.append(" AND ");
                        } else {
                            clause.append(" OR ");
                        }
                    }

                    String sqlOp = getSqlOperator(operator);

                    clause.append(table.getColName(i)).append(sqlOp).append("'")
                          .append(filterValue.toString()).append("'");
                    firstCond = false;
                }
            }
        }
        return clause.toString();
    }

    private String getSqlOperator(int operator) {
        return SQL_COMPARISON_OPERATOR[operator];
    }

    private boolean compareValues(int type, int operator, Object v1, Object v2) {

        if (Table.TYPE_STRING == type) {
            return compareStringValues(operator, (String)v1, (String)v2);
        } else if (Table.TYPE_LONG == type) {
            return compareLongValues(operator, (Long)v1, (Long)v2);
        } else {
            throw new IllegalArgumentException("Unknown field type " + type);
        }
    }

    // The Comparable interface does not exist in java microedition, so we
    // cannot use it here
    private boolean compareStringValues(int operator, String v1, String v2) {
        int c = v1.compareTo(v2);

        if (operator == EQUAL) {
            if (c == 0) {
                return true;
            } else {
                return false;
            }
        } else if (operator == NOT_EQUAL) {
            if (c == 0) {
                return false;
            } else {
                return true;
            }
        } else if (operator == GREAT) {
            if (c > 0) {
                return true;
            } else {
                return false;
            }
        } else if (operator == GREAT_EQUAL) {
            if (c >= 0) {
                return true;
            } else {
                return false;
            }
        } else if (operator == LESS) {
            if (c < 0) {
                return true;
            } else {
                return false;
            }
        } else if (operator == LESS_EQUAL) {
            if (c <= 0) {
                return true;
            } else {
                return false;
            }
        } else {
            throw new IllegalArgumentException("Invalid operator " + operator);
        }
    }

    // The Comparable interface does not exist in java microedition, so we
    // cannot use it here
    private boolean compareLongValues(int operator, Long v1l, Long v2l) {
        long v1 = v1l.longValue();
        long v2 = v2l.longValue();

        if (operator == EQUAL) {
            return v1 == v2;
        } else if (operator == NOT_EQUAL) {
            return v1 != v2;
        } else if (operator == GREAT) {
            return v1 > v2;
        } else if (operator == GREAT_EQUAL) {
            return v1 >= v2;
        } else if (operator == LESS) {
            return v1 < v2;
        } else if (operator == LESS_EQUAL) {
            return v1 <= v2;
        } else {
            throw new IllegalArgumentException("Invalid operator " + operator);
        }
    }


    protected class FieldFilter {
        private int operator;
        private Object value;
        private boolean and;

        public FieldFilter(boolean and, int operator, Object value) {
            this.and = and;
            this.operator = operator;
            this.value    = value;
        }

        public int getOperator() {
            return operator;
        }

        public Object getValue() {
            return value;
        }

        public boolean getAnd() {
            return and;
        }
    }
}

