
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
   speedcheck.c \
   cookie.c \
   strequal.c \
   inet_ntop.c \
   strtoofft.c \
   getenv.c \
   http_chunks.c \
   hash.c \
   mprintf.c \
   http.c \
   sslgen.c \
   hostthre.c \
   krb4.c \
   ftp.c \
   amigaos.c \
   memdebug.c \
   http_negotiate.c \
   share.c \
   easy.c \
   hostsyn.c \
   hostip4.c \
   parsedate.c \
   strdup.c \
   hostip.c \
   connect.c \
   timeval.c \
   llist.c \
   http_ntlm.c \
   nss.c \
   base64.c \
   getinfo.c \
   splay.c \
   if2ip.c \
   qssl.c \
   progress.c \
   nwlib.c \
   hostasyn.c \
   version.c \
   http_digest.c \
   ssh.c \
   hostip6.c \
   sendf.c \
   strerror.c \
   gtls.c \
   inet_pton.c \
   transfer.c \
   dict.c \
   tftp.c \
   ssluse.c \
   file.c \
   escape.c \
   strtok.c \
   content_encoding.c \
   socks.c \
   ldap.c \
   nwos.c \
   telnet.c \
   select.c \
   security.c \
   krb5.c \
   formdata.c \
   netrc.c \
   multi.c \
   hostares.c \
   md5.c \
   url.c \

LOCAL_CFLAGS += -Wall -Wmissing-prototypes -Wstrict-prototypes -fexceptions -DHAVE_CONFIG_H 

LOCAL_CFLAGS += -I sources/include
LOCAL_CFLAGS += -I sources/include/curl
LOCAL_CFLAGS += -I sources

ifeq ($(HOST_OS),darwin)
	LOCAL_CFLAGS += -fno-common
endif

LOCAL_MODULE:= libcurl

include $(BUILD_SHARED_LIBRARY)

