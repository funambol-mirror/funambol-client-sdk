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
 *
 *
 */

package com.funambol.util;

/*
 * This class is intended to function as a container for blackberry API specific methods. 
 */

//#ifndef isBlackberry
//# public class BlackberryHelper{}
//#else

//# import net.rim.device.api.servicebook.ServiceBook;
//# import net.rim.device.api.servicebook.ServiceRecord;
//# import net.rim.device.api.system.DeviceInfo;
//# import net.rim.device.api.util.StringUtilities;
//# 
//# public class BlackberryHelper {
//# 
//#     //Static list of APNs. Only a couple for now. 
//#     //Beware of case sensitivity
//#     private static WapGateway[] apnList = {
//#     //ATT Orange (Formerly Cingular)
//#             new WapGateway( "wap.cingular", "WAP@CINGULARGPRS.COM", "CINGULAR1" ),
//#             //T-Mobile US1
//#             new WapGateway( "internet2.voicestream.com", null, null ),
//#             //T-Mobile US2
//#             new WapGateway( "wap.voicestream.com", "", "" ) };
//#     
//#     static public boolean isSimulator() {
//# 
//#         return DeviceInfo.isSimulator();
//#     }
//# 
//#     static public String getHttpOptions() {
//# 
//#         //get only active service records
//#         ServiceBook sb = ServiceBook.getSB();
//#         ServiceRecord[] records = sb.findRecordsByType( ServiceRecord.SRT_ACTIVE );
//# 
//#         //Obtain WAP2 ServiceBook Record
//#         for (int i = 0; i < records.length; i++) {
//#             //get the record
//#             ServiceRecord sr = records[i];
//# 
//#             //check if CID is WPTCP and UID. I think UID can be different per carrier. We could build a list. 
//#             if (StringUtilities.strEqualIgnoreCase( sr.getCid(), "WPTCP" ) && StringUtilities.strEqualIgnoreCase( sr.getUid(), "WAP2 trans" )) {
//#                 return ( ";deviceside=true;ConnectionUID=" + records[i].getUid() );
//#             }
//#         }
//# 
//#         //If We are here, no service record was found. Set deviceside true and the phone will
//#         //use the phone settings under Options>advanced>TCP
//#         return ";deviceside=true";
//#     }
//# 
//#     static public String getTcpOptions() {
//# 
//#         StringBuffer options = new StringBuffer( ";deviceside=true" );
//#         String serviceBookApn = getServiceBookApn();
//#         Log.debug( "Trying to find gateway for APN: " + serviceBookApn );
//#         System.out.println( "Trying to find gateway for APN: " + serviceBookApn );
//#         if (serviceBookApn != null) {
//#             WapGateway gateway = findGatewayByApn( serviceBookApn );
//#             if (gateway != null) {
//#                 //We matched with a gateway in our list. Build connection options
//#                 options.append( ";apn=" + gateway.getApn() );
//#                 if (gateway.getUsername() != null) {
//#                     options.append( ";TunnelAuthUsername=" + gateway.getUsername() );
//#                 }
//#                 if (gateway.getPassword() != null) {
//#                     options.append( ";TunnelAuthPassword=" + gateway.getPassword() );
//#                 }
//#             }
//#         }
//# 
//#         Log.debug( "Return TCP Options: " + options.toString() );
//#         return options.toString();
//#     }
//# 
//#     static public String getServiceBookApn() {
//# 
//#         //get only active service records
//#         ServiceBook sb = ServiceBook.getSB();
//#         ServiceRecord[] records = sb.findRecordsByType( ServiceRecord.SRT_ACTIVE );
//#         String apn = null;
//#         //Obtain WAP2 ServiceBook Record
//#         for (int i = 0; i < records.length; i++) {
//#             //get the record
//#             ServiceRecord sr = records[i];
//#             //check if CID is WPTCP and UID. I think UID can be different per carrier. We could build a list. 
//#             if (StringUtilities.strEqualIgnoreCase( sr.getCid(), "WPTCP" ) && StringUtilities.strEqualIgnoreCase( sr.getUid(), "WAP2 trans" )) {
//#                 apn = records[i].getAPN();
//#             }
//#         }
//# 
//#         return apn;
//#     }
//# 
//#     static WapGateway findGatewayByApn( String apn ) {
//# 
//#         for (int i = 0; i < apnList.length; i++) {
//#             if (StringUtilities.strEqualIgnoreCase( apnList[i].getApn(), apn )) {
//#                 return apnList[i];
//#             }
//#         }
//# 
//#         //Return null if not found
//#         return null;
//#     }
//# 
//#     static final class WapGateway {
//# 
//#         private String apn;
//#         private String username;
//#         private String password;
//# 
//#         public WapGateway(String apn, String username, String password) {
//# 
//#             super();
//#             this.apn = apn;
//#             this.username = username;
//#             this.password = password;
//#         }
//# 
//#         public String getApn() {
//# 
//#             return apn;
//#         }
//# 
//#         public String getUsername() {
//# 
//#             return username;
//#         }
//# 
//#         public String getPassword() {
//# 
//#             return password;
//#         }
//#     }
//# }
//#endif