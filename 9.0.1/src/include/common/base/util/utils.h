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

#ifndef INCL_BASE_UTILS
#define INCL_BASE_UTILS
/** @cond DEV */

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/md5.h"
#include "base/base64.h"
#include "base/util/StringBuffer.h"

// Default len for stringdup (means: use source string len)
#define STRINGDUP_NOLEN 0xFFFFFFFF

#define B64_ENCODING        "b64"
#define TEXT_PLAIN_ENCODING "text/plain"

BEGIN_NAMESPACE

/*
 * Deletes the given char* [] buffer if it is not NULL
 * and sets the pointer to NULL
 *
 */
void safeDelete(char*  p[]);

void safeDel(char** p);

/**
 * Convert an unsigned long to an anchor.
 *
 * @param timestamp the timestamp to convert into an anchor
 * @param anchor where the anchor will be written (has to be at least 21 characters long)
 */
void timestampToAnchor(unsigned long timestamp, char anchor[21]);

/**
 * inverse operation for timestampToAnchor(), returns 0 if not a valid anchor
 */
unsigned long anchorToTimestamp(const char* anchor);

char* stringdup(const char* s, size_t len = STRINGDUP_NOLEN) ;
WCHAR* wstrdup(const WCHAR* s, size_t len = STRINGDUP_NOLEN);

char*  strtolower(const char *s);

char*  wcstoupper(const char *s);

/**
 * find a substring from the end, with optional string lenght
 */
const char *brfind(const char *s1, const char *s2, size_t len=STRINGDUP_NOLEN) ;

/**
 * Returns true is the given string is NULL or zero-length
 */
inline bool isEmpty(const char*  s) {
    return ((s == NULL) || (strlen(s) == 0));
}

/**
 * Returns true is the given string is NULL or zero-length
 */
inline bool isNotEmpty(const char*  s) {
    return (s && (strlen(s) > 0));
}


/*
* compare two char array ignoring the case of the char
*/
bool wcscmpIgnoreCase(const char*  p, const char*  q);

/*
* Converts a integer into a char*
*/
char*  itow(int i);

/*
* Converts a integer into a char*
*/
char*  ltow(long i);


/*
* Method to create the cred data given the username, password and nonce
* It uses the calculateMD5 to calculate the MD5 using the alghoritm.
*/
char*  MD5CredentialData(const char* userName, const char* password, const char* nonce);

/*
* Calculates the digest given the token and its lenght
*/
char* calculateMD5(const void* token, int len, char* wdigest);

/*
 * Return a filename composed by the system temp dir and the name given
 * in input. If the file exists, try to add a digit 0-9.
 * If this fails too, return NULL (there's must be something wrong in
 * the calling program)
 *
 * @param name - a file name, without path
 * @return - a full pathname, allocated with new[], or NULL on error
 */
char *mkTempFileName(const char *name);

/*
 * Write len bytes from buffer to the file 'filename'.
 *
 * @param name - the file name
 * @param buffer - pointer to the buffer to write
 * @param len - the number of bytes to write
 * @param binary - if true the file will be opened in binary mode
 *
 * @return - true if file is successfully saved
 */
bool saveFile(const char *filename, const char *buffer, size_t len,
              bool binary = false );

/*
 * Get the size of the file, in bytes
 *
 * @param f - the file to evaluate
 * @return - the length of the file
 */
size_t fgetsize(FILE *f);

/**
 * Get the size of the file, in bytes
 *
 * @param fileName - the file name (full path)
 * @return - the length of the file
 */
size_t fgetsize(const char* fileName);

/**
 * Returns the file name, given its full name (path+name).
 * It just gets the last part of the string, after the last "/" or "\".
 * @param fullName  the full path of the file
 * @return          the file name
 */
StringBuffer getFileNameFromPath(const StringBuffer& fullName);

/**
 * Utility function to handle file names.
 * Returns the full path+name of a file, given its name and the dir where the file is located.
 */
StringBuffer getCompleteName(const char* dir, const StringBuffer& name);

/// Wchar version of getCompleteName()
StringBuffer getCompleteName(const char *dir, const WCHAR *name);

/**
 * Returns true if the fileName extension is equal to one passed.
 * @param fileName          the file name
 * @param extension         the extension to check
 * @param caseInsensitive   true = case insensitive compare (default = true)
 * @return                  true if the extension matches
 */
bool checkFileExtension(const StringBuffer& fileName, const StringBuffer& extension, bool caseInsensitive = true);

/**
 * Returns true if the fileName extension is equal to one passed, WCHAR version.
 * @see checkFileExtension(const StringBuffer&, const StringBuffer&, bool).
 */
bool checkFileExtension(const WCHAR* wfileName, const WCHAR* wextension, bool caseInsensitive);

/**
 * Test the presence of a file.
 *
 * @param the path to the file to test
 * @return true if the file exists.
 */
bool fileExists(const char *pathname);

/**
 * Just mapped to stdio fopen().
 * On Windows platform, using the _wfopen() in order to correctly
 * manage file names with special chars.
 */
FILE* fileOpen(const char* filename, const char* mode);

/*
 * Read the content
 *
 * @param name - the file name
 * @param message (out) - new allocated buffer with the file content
 * @param len - length of the read content
 * @param binary - if true the file has to be opened in binary mode
 *
 * @return - true if file is successfully read
 */
bool readFile(const char* name, char **message, size_t *len, bool binary = false );

/**
 * Read the content of a directory
 *
 * @param name - the dir name
 * @param count (out) - number of files in dir
 * @param onlyCount - optional, if true only set the nuber of files (count)
 *
 * @return - new allocated array of fileNames (NULL if errors)
 */
char** readDir(const char* name, int *count, bool onlyCount = false);

/**
* Remove the named file in the directory. It doesn't remove the subdirectory
* and the file inside them. If the filename is NULL all files are removed
*
* @param dir    the directory in which delete all the files
* @param filename the name of the file to be deleted. If NULL all
*        the files in dir will be deleted
*/
bool removeFileInDir(const char* dir, const char* filename = NULL);

/**
 * Returns the most recent time stamp of file content or attribute
 * modifications, 0 in case of error. The time stamp must use the same
 * time base as time().
 */
unsigned long getFileModTime(const char* name);

/**
 * Converts a timestamp from unix format (seconds from 1970-01-01) 
 * into a string like "20091022T192200" or "20091022T192100Z".
 * No local/UTC conversion is done inside this function.
 *
 * @param unixTime  the input timestamp in unix format
 * @param isUTC     if true, will place the "Z" at the end of the returned string
 * @return          a string formatted like "20091022T192200Z", empty string in case of error
 */
StringBuffer unixTimeToString(const unsigned long unixTime, const bool isUTC);

/**
 * Creates a folder.
 * If the folder already exists, the method returns success and does nothing.
 *
 * @return 0 on success, -1 otherwise.
 */
int createFolder(const char *path);

long int getLenEncoding(const char*  s, const char* encoding);
char *toMultibyte(const WCHAR *wc, const char *encoding = 0 );
WCHAR *toWideChar(const char *mb, const char *encoding = 0 );


// Wide Char Convert: inline function used to convert a wchar
// in char, using a static buffer to store the converted string.
//
// BEWARE: the string is deleted and re-used at each call.
inline const char *_wcc(const WCHAR *wc, const char *enc=0) {
    StringBuffer s;
    s.convert(wc);
    static char* encodeBuf = 0;

    if (encodeBuf){
        delete [] encodeBuf;
        encodeBuf = 0;
    }
    if (wc) {
        encodeBuf = toMultibyte(wc, enc);
    }
    return encodeBuf;
}

/**
* Calculates the CRC of an array given its length.
* If len is <= 0 it returns 0.
*
* @param s      the pointer to the byte array.
* @param len    the len of the byte array
* 
*/
inline long calculateCRC(const void *s, size_t len) {
    
    if (len <= 0) {
        return 0;
    }
    static const unsigned long s_arrdwCrc32Table1[256] =
    {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
        0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
        0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
        0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
        0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
        0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
        0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
        0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
        0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
        0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
        0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
        0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
        0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
        0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
        0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
        0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
        0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
        0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
        0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
        0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
        0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
        0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
    };
        
    unsigned long dwCrc32 = 0;

    unsigned char byte = 0;

    dwCrc32 = 0xFFFFFFFF;

    size_t i = 0;
    unsigned char* p = (unsigned char*) s;

    while (i < len) {
        byte = p[i];
        dwCrc32 = ((dwCrc32) >> 8) ^ s_arrdwCrc32Table1[(byte) ^ ((dwCrc32) & 0x000000FF)];        
        i++;

    }    
    dwCrc32 = ~dwCrc32;
    return dwCrc32;
}

/**
* Calculates the CRC of a given array of char NULL terminated.
*
* @param s  the pointer to the array of char NULL terminated 
*/
inline long calculateCRC(const char* s) {
    
    return calculateCRC(s, strlen(s));
}

/**
* Read the files into a directory in recursive way it recursive is true.
* Returns an array list of files with full path dirname/filename, dirname/dir1/fileinside
*
* @param dirname the dir to look into
* @param recursive if true it looks inside the inner dirs
*
* @return an ArrayList of Stringbuffer containing the file names 
*/

ArrayList readFilesInDirRecursive(const char* dirname, bool recursive);

/**
 * Read the dir into a directory in recursive way it recursive is true.
 * Returns an array list of dir with full path dirname/dir1, dirname/dir1/dir2
 *
 * @param dirname the dir to look into
 * @param recursive if true it looks inside the inner dirs
 *
 * @return an ArrayList of Stringbuffer containing the dir names 
 */
ArrayList readDirsInDirRecursive(const char* dirname, bool recursive);


END_NAMESPACE

/** @endcond */
#endif
