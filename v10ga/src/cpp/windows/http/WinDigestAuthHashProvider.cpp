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

#include "base/util/StringBuffer.h"
#include "http/WinDigestAuthHashProvider.h"
#include <Wincrypt.h>

USE_NAMESPACE

StringBuffer WinDigestAuthHashProvider::getHash(const StringBuffer str) const {
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	DWORD size = 0;
	char rgbDigits[] = "0123456789abcdef";
	#define MD5LEN 16
	BYTE hashval[100];
	char* final;

	if ( !CryptAcquireContext(
        &hProv,  // variable to hold the returned handle
        NULL,           // use default key container
        NULL,           // use default provider 
        PROV_RSA_FULL,  // type of context to acquire
        CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) {
		//error
        return NULL;
    }

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
		return NULL;
	}

	if (!CryptHashData(hHash, (BYTE*)str.c_str(), str.length(), 0)) {
		return NULL;
	}

	size = MD5LEN;
	final = new char[((MD5LEN*2)+1)];
	if (CryptGetHashParam(hHash, HP_HASHVAL, hashval, &size, 0)) {
		for (DWORD i=0; i<MD5LEN; i++) {
			final[2*i] = rgbDigits[hashval[i] >> 4];
			final[2*i + 1] = rgbDigits[hashval[i] & 0xf];
		}
		final[(MD5LEN*2)] = '\0';
	}
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

    StringBuffer result(final);
    delete [] final;

	return result;
}
