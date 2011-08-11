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

#ifndef INCL_SAPI_CONTENT_TYPE
#define INCL_SAPI_CONTENT_TYPE
/** @cond DEV */

#include "base/fscapi.h"


BEGIN_FUNAMBOL_NAMESPACE

/**
 *
 */
class SapiContentType {
    
public:

    /**
    * Static method to retrieve the content-type of a file given the extension.
    * @param val - the extension to retrieve the content-type, i.e. "jpg"
    * @return the content type associated to the extension i.e. image/ipeg
    */
    static StringBuffer getContentTypeByExtension(const char* val) {        

        if (val == NULL || strcmp(val, "") == 0) {
            return "application/octet-stream";
        }

        StringBuffer value(val);
        size_t pos = value.find(".");
        
        if (pos != StringBuffer::npos && pos > 0) { // at least the extension must be .something otherwise is not valid
            return "application/octet-stream";
        }
        if (pos == 0 && value.length() >= 2) {
            value = value.substr(1);
        }
        
        value.lowerCase();
        
        //
        // PICTURES
        //
        if (value == "bmp")  { return "image/bmp"               ; }     
        if (value == "cod" ) { return "image/cis-cod"           ; }     
        if (value == "gif" ) { return "image/gif"               ; }     
        if (value == "ief" ) { return "image/ief"               ; }     
        if (value == "jpe" ) { return "image/jpeg"              ; }     
        if (value == "jpeg") { return "image/jpeg"              ; }     
        if (value == "jpg" ) { return "image/jpeg"              ; }
        if (value == "png" ) { return "image/png"               ; }     
        if (value == "jfif") { return "image/pipeg"             ; }     
        if (value == "svg" ) { return "image/svg+xml"           ; }     
        if (value == "tif" ) { return "image/tiff"              ; }     
        if (value == "tiff") { return "image/tiff"              ; }     
        if (value == "ras" ) { return "image/x-cmu-raster"      ; }     
        if (value == "cmx" ) { return "image/x-cmx"             ; }     
        if (value == "ico" ) { return "image/x-icon"            ; }     
        if (value == "pnm" ) { return "image/x-portable-anymap" ; }     
        if (value == "pbm" ) { return "image/x-portable-bitmap" ; }     
        if (value == "pgm" ) { return "image/x-portable-graymap"; }     
        if (value == "ppm" ) { return "image/x-portable-pixmap" ; }     
        if (value == "rgb" ) { return "image/x-rgb"             ; }     
        if (value == "xbm" ) { return "image/x-xbitmap"         ; }     
        if (value == "xpm" ) { return "image/x-xpixmap"         ; }     
        if (value == "xwd" ) { return "image/x-xwindowdump"     ; }     
        
        //
        // VIDEO
        //
        
        if (value == "mp2"   ) { return "video/mpeg"            ; }
        if (value == "mpa"   ) { return "video/mpeg"            ; }
        if (value == "mpe"   ) { return "video/mpeg"            ; }
        if (value == "mpeg"  ) { return "video/mpeg"            ; }
        if (value == "mpg"   ) { return "video/mpeg"            ; }
        if (value == "mpv2"  ) { return "video/mpeg"            ; }
        if (value == "mov"   ) { return "video/quicktime"       ; }
        if (value == "qt"    ) { return "video/quicktime"       ; }
        if (value == "lsf"   ) { return "video/x-la-asf"        ; }
        if (value == "lsx"   ) { return "video/x-la-asf"        ; }
        if (value == "asf"   ) { return "video/x-ms-asf"        ; }
        if (value == "asr"   ) { return "video/x-ms-asf"        ; }
        if (value == "asx"   ) { return "video/x-ms-asf"        ; }
        if (value == "avi"   ) { return "video/x-msvideo"       ; }
        if (value == "movie" ) { return "video/x-sgi-movie"     ; }
        if (value == "dif"   ) { return "video/x-dv"            ; }
        if (value == "dv"    ) { return "video/x-dv"            ; }
        if (value == "m4u"   ) { return "video/vnd.mpegurl"     ; }
        if (value == "m4v"   ) { return "video/x-m4v"           ; }
        if (value == "mp4"   ) { return "video/mp4"             ; }
        if (value == "mxu"   ) { return "video/vnd.mpegurl"     ; }
        if (value == "3g2"   ) { return "video/3gpp2"           ; }
        if (value == "3gp"   ) { return "video/3gpp"            ; }
        if (value == "3gpp"  ) { return "video/3gpp"            ; }
        if (value == "swf"   ) { return "application/x-shockwave-flash"; }
        if (value == "flv"   ) { return "video/x-flv"           ; }
        
        return "application/octet-stream";                                                      

    }

};

END_NAMESPACE

/** @endcond */
#endif
