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

package com.funambol.mail;

import com.funambol.util.Log;

import com.funambol.util.StringUtil;
import java.util.Vector;

/**
 * A class describing the RFC 2822 address specification as per par. 3.4
 */
public class Address {

    // -------------------------------------------------------------- Constants

    /** Address is used in a FROM recipient */
    public static final int FROM = 1;
    
    /** Address is used in a TO recipient */
    public static final int TO = 2;
    
    /** Address is used in a CC recipient */
    public static final int CC = 3;
    
    /** Address is used in a BCC recipient */
    public static final int BCC = 4;

    /** Address is used in a REPLYTO recipient */
    public static final int REPLYTO = 5;


    private static final char QUOTE  = '\"';
    private static final char ESCAPE = '\\';
    private static final char COMMA  = ',';
    private static final char LT     = '<';
    private static final char GT     = '>';
    private static final char P_BEGIN     = '(';
    private static final char P_END     = ')';

    // ----------------------------------------------------------- Private Data
    
    /** The type of the address */
    private int type;
    /** The optional display name (part 1 of the mailbox) */
    private String name;
    /** The email address (part 2 of the mailbox) */
    private String email;
    public static final String INVALID_ADDRESS="Invalid_email_address@sender";
    
    // ----------------------------------------------------------- Constructors

    /**
     * Default Constructor
     */
    public Address() {
    }
    
    /**
     * Initializes a new instance of Address, parsing <code>address<code/>.
     * If <code>address<code/> contains more than one email address,
     * only the first one is used and the others are ignored.
     * Use Adress.parse to get an array of Address from an address list.
     *
     * @param type is the type of this address
     * @param address is the String representation of the address to be parsed
     * @throws MailException when some error occurs parsing address 
     */
    public Address(int type, String address) throws MailException {
        this.type = type;
        Vector vnames = new Vector();
        Vector addresses = new Vector();

        parseAddrList(address, vnames, addresses);
        if(addresses.size() == 0) {
            Log.error("Address: invalid address '"+address+"'");
            throw new MailException(
                        MailException.INVALID_ADDRESS,
                        "Invalid empty address");
        }
        this.name = (String)vnames.elementAt(0);
        this.email = checkAddr((String)addresses.elementAt(0));
    }
    
    
    /**
     * Initializes a new instance of Address
     * @param type is the type of address
     * @param name is the name of the Address
     * @param address is the email of this address  
     */
    public Address(int type, String name, String address) throws MailException {
        this.type = type;
        this.name = name;
        this.email= checkAddr(address);
    }
    
    
    // --------------------------------------------------------- Public Methods
    
    /**
     * Accessor method to get the type of this address
     * @return int representation of this address' type
     */
    public int getType() {
        return type;
    }
    
    /**
     * Accessor method to get this address' visible name representation:
     * @return this address name if it is not null, this address email otherwise
     */
    public String getVisibleName() {
        return this.name != null ? this.name : this.email;
    }
    
    /**
     * Accessor method to get this address name field
     * @return String representation of this address name field
     */
    public String getName() {
        return name;
    }
    
    /**
     * Accessor method to get this address email field
     * @return String representation of this address email field
     */
    public String getEmail() {
        return email;
    }
    
    /**
     * Accessor method to set the type of this address
     * @param type is the type to be set for this address
     */
    public void setType(int type) {
        this.type = type;
    }
    
    /**
     * Accessor method to set the name of this address
     * @param name is the name to be set for this address
     */
    public void setName(String name) {
        this.name = name;
    }
    
    /**
     * Accessor method to set this address' email
     * @param email is the email string value to be set
     * @throws MailException if some error occurs while check address' email 
     */
    public void setEmail(String email) throws MailException {
        this.email = checkAddr(email);
    }
    
    
    /**
     * Parses an address list and return an array of <code>Address</code>
     * objects, each representing a recipient.
     *
     * Use: Address [] list = Address.parse( Address.TO, "John
     * Doe <john.doe@mail.com>"
     *
     * @param addresslist
     *            the comma separated list of addresses
     * @return an array of <code>Address</code> containing the result, or an
     * empty array if addresslist is empty
     *
     * @throws MailException
     *             if the address is malformed
     *
     */
    public static Address[] parse(int type, String addresslist)
    throws MailException {
        if (addresslist == null || addresslist.trim().length() == 0) {
            return new Address[0];
        }
        
        // TODO: handle groups.
        if (addresslist.trim().indexOf("undisclosed-recipients:") != -1) {
            return new Address[0];
        }
        
        Vector vnames = new Vector();
        Vector addresses = new Vector();

        parseAddrList(addresslist, vnames, addresses);
        
        int len = addresses.size();
        Address[] ret = new Address[len];

        for (int i = 0; i < len; i++) {
            ret[i] = new Address(type, 
                                 (String)vnames.elementAt(i),
                                 (String)addresses.elementAt(i));
        }
        return ret;
    }


    /**
     * Overloads toString() Object' method to return this address representation
     * in RFC2822 format
     * @return the string representation of the Address
     */
    public String toString() {
        // If a name is defined, compose the '"name" <address>' form
        if (name != null) {
            StringBuffer ret = new StringBuffer("");
            
            ret.append("\"").append(name).append("\"").
                    append(" <").append(email).append(">");
            
            return ret.toString();
        }
        
        // Otherwise return the address
        return email;
    }
    
    // -------------------------------------------------------- Private Methods

    /*
     * Parse an address list, returning the visible names and addresses in
     * the given Vectors.
     *
     * The string is parsed char by char to find special characters:
     *  ", \, <, >
     *
     * These characters change the state of the parser. The states are 4:
     *
     * Normal: at the beginning, characters are appended to the 'name' buffer.
     *         Special characters are allowed. If a '<' is found, the name is
     *         used for the visible name and the address is processed in the
     *         state 'Inside address'. Otherwise, the characters processed in
     *         this state are actually the address
     * Inside quotes: the other characters have no special meaning, ends with
     *                another quote
     * After escape: the single character has no special meaning, ends immediately
     *
     * Inside address: the special characters are not allowed, all the others are
     *                 appended to 'addr'.
     *
     * When a comma or the end of the string is found being in the Normal state,
     * the Address is complete and the name and address are appended to the output
     * Vectors.
     *
     * @param addresslist the address list to parse
     * @param vnames (out) the visible names. If an address has no visible name, its
     *               position in the vector is null.
     * @param addresses (out) the mail addresses.
     */
    private static void parseAddrList(
                            String addresslist,
                            Vector vnames,
                            Vector addresses) throws MailException {
        
        
       
        if (addresslist == null) {
            throw new MailException(
                        MailException.INVALID_ADDRESS,
                        "Invalid address (null)");
        }

        final int STATE_NORMAL = 0;     // beginning of the parsing
        final int STATE_QUOTE  = 1;     // inside quotes
        final int STATE_ESCAPE = 2;     // after an ESCAPE
        final int STATE_ADDR   = 3;     // inside address
        final int STATE_PARENTHESIS   = 4;     // inside address

        int state = STATE_NORMAL;
        StringBuffer name = new StringBuffer();
        StringBuffer addr = new StringBuffer();
        addresslist.trim();
        String tmp = addresslist.trim();
        String[] ret = StringUtil.split(tmp, " ");
        for (int i=0; i<ret.length; i++) {
            Log.debug("SPLITTED LIST: " + ret[i]);
        }
        
        for(int i=0, l=addresslist.length(); i<l; i++) {
            char ch = addresslist.charAt(i);
            switch(state) {
                case STATE_ESCAPE:
                    name.append(ch);
                    state = STATE_NORMAL;
                    break;

                case STATE_QUOTE:
                    if (ch == QUOTE) {
                        state = STATE_NORMAL;
                    }
                    else {
                        name.append(ch);
                    }
                    break;

                case STATE_ADDR:
                    switch(ch) {
                        case QUOTE:
                            break;
                        case ESCAPE:
                        case COMMA:
                        case LT:
                            // read the MIME rfc and use the invalid characters
                            throw new
                                MailException(MailException.INVALID_ADDRESS,
                                              "Invalid address.");
                        case GT:
                            state = STATE_NORMAL;
                            break;
                        default:
                            addr.append(ch);
                    }
                    break;

                case STATE_NORMAL:
                    switch(ch) {
                        case QUOTE:
                            state = STATE_QUOTE;
                            break;
                        case P_BEGIN:
                            name.append(ch);
                            break;
                        case P_END:
                            state = STATE_ADDR;
                            name.append(ch);
                            break;
                        case ESCAPE:
                            state = STATE_ESCAPE;
                            break;
                        case LT:
                            state = STATE_ADDR;
                            break;
                        case COMMA:
                            //fix bug for addresslist with last char=','
                            if(i == addresslist.length()-1){
                                break;
                            }else{
                                addNames(name, addr, vnames, addresses);
                                break;
                            }
                           
                        default:
                            name.append(ch);
                    }
                    break;
            }
        }

        addNames(name, addr, vnames, addresses);

    }

    // Add visible name and email address to the vectors used by parseAddrList.
    private static void addNames(
                            StringBuffer name,
                            StringBuffer addr,
                            Vector vnames,
                            Vector addresses) throws MailException {

        // If we had name and address, save both
        // if addr is null, name is actually the address
        String vn = name.toString().trim();
        String ad = addr.toString().trim();
        if (ad.length() > 0) {
            vnames.addElement((vn.length() > 0) ? vn : null);
            addresses.addElement(ad);
        }
        else if (vn.length() > 0) {
            vnames.addElement(null);
            addresses.addElement(vn);
        }
        else {
            Log.error("addnames: empty address");
            throw new MailException(MailException.INVALID_ADDRESS, "Invalid address.");
        }
        name.setLength(0);
        addr.setLength(0);
    }

    /**
     * Check if the given address is a valid address
     * @param address is the adress to be checked 
     * @throws MailException when the given address is invalid 
     */
    private String checkAddr(String address) throws MailException {
        int position=address.indexOf("@");
        if ( (position!=-1) && (position!=0) && (position!=address.length())) {

            // If the address contains trailing characters, just ignore the
            // trailer. It may happen with sender using the obsolete syntax:
            //     "user@host.domain (User Name)"
            int pos = address.indexOf(" ");
            if (pos > 0) {
                address = address.substring(0, pos);
            }

            return address;
            
        } else {
            return (INVALID_ADDRESS);            
        }
    }
}

