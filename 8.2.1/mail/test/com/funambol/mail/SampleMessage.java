/*
 * Copyright (C) 2006-2007 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package com.funambol.mail;

import java.util.Date;

/**
 * Represents the input to be passed to the parser in order to build a
 * <code>Message</code> object
 */
public class SampleMessage {
    
    private SampleMessage() {
    }
    
    public static final String RFC2822_MSGID = 
                  "c96b5e8f0610060157g5223301cy60f9ece072a7467a@mail.gmail.com";

    public static final String RFC2822_RECEIVED =
                                               "Fri, 6 Oct 2006 8:57:2 +0000";
    
    public static final String RFC2822_FROM = 
                   "\"Giuseppe Garibaldi\" <giuseppe.garibaldi@googlemail.com>";
    
    public static final String RFC2822_TO = 
                            "\"demoname@funambol.com\" <demoname@funambol.com>";
                   
    public static final String RFC2822_SUBJECT = 
                                        "Testo, HTML e c'è anche un attachment";
    
    public static final String RFC2822_CONTENT_TYPE = "text/plain";
   
    public static final String RFC2822_CONTENT = 
               "\r\nTesto\r\n--\r\nGiuseppe Garibaldi\r\nViale Teano 26\r\nI " +
               "- 26845 Vienna (Lodi)\r\n";
    
    /**
     * A RFC 2822 / MIME message as received from the server through the email
     * connector
     */
    public static final String RFC_2822 =
        "Return-Path: <giuseppe.garibaldi@googlemail.com>\r\n"
                + "Received: by 10.49.64.2 with HTTP; Fri, 6 Oct 2006 01:57:02 -0700 (PDT)\r\n"
                + "Message-ID: <c96b5e8f0610060157g5223301cy60f9ece072a7467a@mail.gmail.com>\r\n"
                + "Date: Fri, 6 Oct 2006 10:57:02 +0200\r\n"
                + "From: \"Giuseppe Garibaldi\" <giuseppe.garibaldi@googlemail.com>\r\n"
                + "To: \"demoname@funambol.com\" <demoname@funambol.com>\r\n"
                + "Subject: =?ISO-8859-1?Q?Testo,_HTML_e_c'=E8_anche_un_attachment?=\r\n"
                + "MIME-Version: 1.0\r\n"
                + "Content-Type: text/plain; charset=iso-8859-1; format=flowed\r\n"
                + "Content-Transfer-Encoding: 7bit\r\n"
                + "Delivered-To: 205-demoname@funambol.com\r\n"
                + "X-Spam-Checker-Version: SpamAssassin 3.1.3 (2006-06-01) on rs1000.servadmin.com\r\n"
                + "X-Spam-Level: X-Spam-Status: No, score=0.4 required=7.0 tests=AWL,HTML_MESSAGE, HTML_SHORT_LENGTH autolearn=no version=3.1.3\r\n"
                + "Received-SPF: pass (rs1000.servadmin.com: SPF record at _spf.google.com designates 64.233.182.188 as permitted sender) DomainKey-Signature: a=rsa-sha1; q=dns; c=nofws; s=beta; d=googlemail.com; h=received:message-id:date:from:to:subject:mime-version:content-type; b=HJkYzKwuIFP0XK/uuv0FINtgO+N6Dqza9QMizy52thDzgp8TTxuVrSW+aLc2RMlgAarWyaLJdNhc4FqXodn6QI2YKlllpaP0KPfG8c9wz+jW1GSW1G057dudG0P+DDEzJm6hAlqCfYwNmnfogmqzh9Xm8zff8i6ynfAvmPp1TlQ=\r\n"
                + "\r\n" + "Testo\r\n" + "--\r\n" + "Giuseppe Garibaldi\r\n"
                + "Viale Teano 26\r\n" + "I - 26845 Vienna (Lodi)\r\n";

    public static final String THUNDERBIRD_MSGID = 
                                     "017701c6eedf$f4673500$1600a8c0@Delicious";

    public static final String THUNDERBIRD_RECEIVED =
                                              "Fri, 13 Oct 2006 15:53:41 +0000";
    
    public static final String THUNDERBIRD_FROM = 
                                    "\"Marco Cardinali\" <stragi@funambol.com>";
    
    public static final String THUNDERBIRD_TO_0 = 
                                  "\"Giuseppe Garibaldi\" <ponty@funambol.com>";
    
    public static final String THUNDERBIRD_TO_1 = 
                                 "\"Lorenzo Balocchi\" <balocchi@funambol.com>";
    
    public static final String THUNDERBIRD_CC_0 = 
                          "\"Andrea Tartarughini\" <tartarughini@funambol.com>";
    
    public static final String THUNDERBIRD_SUBJECT = "Re: Calcetto!";
    
    public static final String THUNDERBIRD_CONTENT_TYPE = "text/plain";
   
    public static final String THUNDERBIRD_CONTENT = 
               "\r\n"
                + "mi sembra scontato che ci sia in ogni caso confermo.. :)\r\n"
                + "ciao\r\n"
                + "m\r\n"
                + "\r\n"
                + "----- Original Message ----- \r\n"
                + "From: \"Giuseppe Garibaldi\" <ponty@funambol.com>\r\n"
                + "To: \"Lorenzo Balocchi\" <balocchi@funambol.com>\r\n"
                + "Cc: \"Andrea Tartarughini\" <tartarughini@funambol.com>; \"Andrea Cristofori\" \r\n"
                + "\r\n"
                + "<andrea.cristofori@funambol.com>; <trova@funambol.com>; \"Renato Brosio\" \r\n"
                + "<brosio@funambol.com>; \"Fabio Raggi\" <moebius@funambol.com>; \"Gabriele Rana\" \r\n"
                + "<gabriele@funambol.com>; \"Glauco Babini\" <paolo@funambol.com>; \r\n"
                + "<pietro.gaggi@funambol.com>; \"Stefano Ezechiele\" <ezechielei@funambol.com>; \r\n"
                + "\"Stefano Monari\" <stefano.monari@funambol.com>; \"Cristiano\" \r\n"
                + "<magalli@funambol.com>; <luigino@funambol.com>; <minichetti@funambol.com>; \r\n"
                + "\"Gilberto Poggibonzi\" <gilberto.poggibonzi@funambol.com>; \"Marco \r\n"
                + "Cardinali\" <stragi@funambol.com>; \"Adriano Brugnoli\" \r\n"
                + "<brugnoli@funambol.com>\r\n"
                + "Sent: Friday, October 13, 2006 5:50 PM\r\n"
                + "Subject: Re: Calcetto!\r\n"
                + "\r\n"
                + "\r\n"
                + "> Presente.\r\n"
                + ">   G\r\n"
                + ">\r\n"
                + "> Lorenzo Balocchi wrote:\r\n"
                + ">> Ciao a tutti,\r\n"
                + ">>\r\n"
                + ">> il buon Magi proponeva di fare una partita di calcetto per mercoledi' \r\n"
                + ">> prossimo. Come orario direi o 19 o 19.30.\r\n"
                + ">> Fatemi sapere se ci siete cosi', raggiunto il numero minimo, prenoto il \r\n"
                + ">> campo.\r\n"
                + ">>\r\n"
                + ">> Ciao\r\n"
                + ">>\r\n"
                + ">>    Lorenzo\r\n"
                + ">\r\n"
                + ">\r\n"
                + "> -- \r\n"
                + "> Giuseppe Garibaldi\r\n"
                + ">\r\n"
                + "> funambol :: mobile open source :: http://www.funambol.com\r\n"
                + ">\r\n" + "> \r\n" + "\r\n" + "\r\n";
    
    public static final String THUNDERBIRD =
        "X-Mozilla-Status: 0001\r\n"
                + "X-Mozilla-Status2: 00000000\r\n"
                + "Return-Path: <stragi@funambol.com>\r\n"
                + "Delivered-To: 205-ponty@funambol.com\r\n"
                + "X-Spam-Checker-Version: SpamAssassin 3.1.3 (2006-06-01) on \r\n"
                + "\trs1000.servadmin.com\r\nX-Spam-Level: \r\n"
                + "X-Spam-Status: No, score=0.0 required=7.0 tests=none autolearn=ham \r\n"
                + "\tversion=3.1.3\r\n"
                + "Received: (qmail 6837 invoked from network); 13 Oct 2006 10:53:41 -0500\r\n"
                + "Received: from 85-18-37-27.ip.fastwebnet.it (HELO Delicious) (85.18.37.27)\r\n"
                + "  by rs1000.servadmin.com with SMTP; 13 Oct 2006 10:53:40 -0500\r\n"
                + "Message-ID: <017701c6eedf$f4673500$1600a8c0@Delicious>\r\n"
                + "From: \"Marco Cardinali\" <stragi@funambol.com>\r\n"
                + "To: \"Giuseppe Garibaldi\" <ponty@funambol.com>,\r\n"
                + "\t\"Lorenzo Balocchi\" <balocchi@funambol.com>\r\n"
                + "Cc: \"Andrea Tartarughini\" <tartarughini@funambol.com>,\r\n"
                + "\t\"Andrea Cristofori\" <andrea.cristofori@funambol.com>,\r\n"
                + "\t<trova@funambol.com>,\r\n"
                + "\t\"Renato Brosio\" <brosio@funambol.com>,\r\n"
                + "\t\"Fabio Raggi\" <moebius@funambol.com>,\r\n"
                + "\t\"Gabriele Rana\" <gabriele@funambol.com>,\r\n"
                + "\t\"Glauco Babini\" <paolo@funambol.com>,\r\n"
                + "\t<pietro.gaggi@funambol.com>,\r\n"
                + "\t\"Stefano Ezechiele\" <ezechiele@funambol.com>,\r\n"
                + "\t\"Stefano Monari\" <stefano.monari@funambol.com>,\r\n"
                + "\t\"Cristiano\" <magalli@funambol.com>,\r\n"
                + "\t<luigino@funambol.com>,\r\n"
                + "\t<minichetti@funambol.com>,\r\n"
                + "\t\"Gilberto Poggibonzi\" <gilberto.poggibonzi@funambol.com>,\r\n"
                + "\t\"Adriano Brugnoli\" <brugnoli@funambol.com>\r\n"
                + "References: <452FAD22.7000404@funambol.com> <452FB5B5.7060703@funambol.com>\r\n"
                + "Subject: Re: Calcetto!\r\n"
                + "Date: Fri, 13 Oct 2006 17:54:33 +0200\r\n"
                + "MIME-Version: 1.0\r\n"
                + "Content-Type: text/plain;\r\n"
                + "\tformat=flowed;\r\n"
                + "\tcharset=\"ISO-8859-15\";\r\n"
                + "\treply-type=response\r\n"
                + "Content-Transfer-Encoding: 7bit\r\n"
                + "X-Priority: 3\r\n"
                + "X-MSMail-Priority: Normal\r\n"
                + "X-Mailer: Microsoft Outlook Express 6.00.2900.2869\r\n"
                + "X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2900.2962\r\n"
                + "\r\n"
                + "mi sembra scontato che ci sia in ogni caso confermo.. :)\r\n"
                + "ciao\r\n"
                + "m\r\n"
                + "\r\n"
                + "----- Original Message ----- \r\n"
                + "From: \"Giuseppe Garibaldi\" <ponty@funambol.com>\r\n"
                + "To: \"Lorenzo Balocchi\" <balocchi@funambol.com>\r\n"
                + "Cc: \"Andrea Tartarughini\" <tartarughini@funambol.com>; \"Andrea Cristofori\" \r\n"
                + "\r\n"
                + "<andrea.cristofori@funambol.com>; <trova@funambol.com>; \"Renato Brosio\" \r\n"
                + "<brosio@funambol.com>; \"Fabio Raggi\" <moebius@funambol.com>; \"Gabriele Rana\" \r\n"
                + "<gabriele@funambol.com>; \"Glauco Babini\" <paolo@funambol.com>; \r\n"
                + "<pietro.gaggi@funambol.com>; \"Stefano Ezechiele\" <ezechielei@funambol.com>; \r\n"
                + "\"Stefano Monari\" <stefano.monari@funambol.com>; \"Cristiano\" \r\n"
                + "<magalli@funambol.com>; <luigino@funambol.com>; <minichetti@funambol.com>; \r\n"
                + "\"Gilberto Poggibonzi\" <gilberto.poggibonzi@funambol.com>; \"Marco \r\n"
                + "Cardinali\" <stragi@funambol.com>; \"Adriano Brugnoli\" \r\n"
                + "<brugnoli@funambol.com>\r\n"
                + "Sent: Friday, October 13, 2006 5:50 PM\r\n"
                + "Subject: Re: Calcetto!\r\n"
                + "\r\n"
                + "\r\n"
                + "> Presente.\r\n"
                + ">   G\r\n"
                + ">\r\n"
                + "> Lorenzo Balocchi wrote:\r\n"
                + ">> Ciao a tutti,\r\n"
                + ">>\r\n"
                + ">> il buon Magi proponeva di fare una partita di calcetto per mercoledi' \r\n"
                + ">> prossimo. Come orario direi o 19 o 19.30.\r\n"
                + ">> Fatemi sapere se ci siete cosi', raggiunto il numero minimo, prenoto il \r\n"
                + ">> campo.\r\n"
                + ">>\r\n"
                + ">> Ciao\r\n"
                + ">>\r\n"
                + ">>    Lorenzo\r\n"
                + ">\r\n"
                + ">\r\n"
                + "> -- \r\n"
                + "> Giuseppe Garibaldi\r\n"
                + ">\r\n"
                + "> funambol :: mobile open source :: http://www.funambol.com\r\n"
                + ">\r\n" + "> \r\n" + "\r\n" + "\r\n";

    public static final String GMAIL_MSGID = 
                  "BAY105-DAV1446B8F84CA4DEC28F52E2A22F0@phx.gbl";

    public static final String GMAIL_RECEIVED = "Wed, 4 Jan 2006 9:36:23 +0000";
    
    public static final String GMAIL_FROM = 
                          "\"Sebastian Borkert\" <johannsebastian@hotmail.com>";
                
    public static final String GMAIL_TO = 
                   "\"Giuseppe Garibaldi\" <giuseppe.garibaldi@googlemail.com>";
                   
    public static final String GMAIL_SUBJECT = 
                             "=?iso-8859-1?Q?Viele_Gr=FC=DFe_aus_G=F6ttingen?=";
    
    public static final String GMAIL_CONTENT_TYPE = "multipart/alternative";
   
    public static final String GMAIL =
        "X-Gmail-Received: 92810b6a24b5e92ff80aa077ec9aeec15f04f8d2\r\n"
                + "Delivered-To: giuseppe.garibaldi@gmail.com\r\n"
                + "Received: by 10.64.131.8 with SMTP id e8cs97678qbd;\r\n"
                + "        Wed, 4 Jan 2006 01:38:37 -0800 (PST)\r\n"
                + "Received: by 10.54.114.7 with SMTP id m7mr2316356wrc;\r\n"
                + "        Wed, 04 Jan 2006 01:38:37 -0800 (PST)\r\n"
                + "Return-Path: <johannsebastian@hotmail.com>\r\n"
                + "Received: from hotmail.com (bay105-dav14.bay105.hotmail.com [65.54.224.86])\r\n"
                + "        by mx.gmail.com with ESMTP id d7si8272818wra.2006.01.04.01.38.36;\r\n"
                + "        Wed, 04 Jan 2006 01:38:37 -0800 (PST)\r\n"
                + "Received-SPF: pass (gmail.com: domain of johannsebastian@hotmail.com designates 65.54.224.86 as permitted sender)\r\n"
                + "Received: from mail pickup service by hotmail.com with Microsoft SMTPSVC;\r\n"
                + "\t Wed, 4 Jan 2006 01:38:35 -0800\r\n"
                + "Message-ID: <BAY105-DAV1446B8F84CA4DEC28F52E2A22F0@phx.gbl>\r\n"
                + "Received: from 84.132.147.186 by BAY105-DAV14.phx.gbl with DAV;\r\n"
                + "\tWed, 04 Jan 2006 09:38:35 +0000\r\nX-Originating-IP: [84.132.147.186]\r\n"
                + "X-Originating-Email: [johannsebastian@hotmail.com]\r\n"
                + "X-Sender: johannsebastian@hotmail.com\r\n"
                + "From: \"Sebastian Borkert\" <johannsebastian@hotmail.com>\r\n"
                + "To: <giuseppe.garibaldi@gmail.com>\r\n"
                + "Subject: =?iso-8859-1?Q?Viele_Gr=FC=DFe_aus_G=F6ttingen?=\r\n"
                + "Date: Wed, 4 Jan 2006 10:36:23 +0100\r\n"
                + "MIME-Version: 1.0\r\n"
                + "Content-Type: multipart/alternative;\r\n"
                + "\tboundary=\"----=_NextPart_000_0011_01C6111A.B5505340\"\r\n"
                + "X-Priority: 3\r\n"
                + "X-MSMail-Priority: Normal\r\n"
                + "X-Mailer: Microsoft Outlook Express 6.00.2800.1409\r\n"
                + "X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2800.1409\r\n"
                + "X-OriginalArrivalTime: 04 Jan 2006 09:38:35.0994 (UTC) FILETIME=[A27ECBA0:01C61112]\r\n\r\nThis is a multi-part message in MIME format.\r\n"
                + "\r\n"
                + "------=_NextPart_000_0011_01C6111A.B5505340\r\n"
                + "Content-Type: text/plain;\r\n"
                + "\tcharset=\"iso-8859-1\"\r\n"
                + "Content-Transfer-Encoding: quoted-printable\r\n"
                + "\r\n"
                + "Ciao Giuseppe,\r\n"
                + "mittlerweile bin ich wieder in G=F6ttingen. Hier wartet am Freitag einer =\r\n"
                + "Klausur in Mikrobiologie auf mich. Demnach kommt keine Langeweile und =\r\n"
                + "mir bleibt leider nur wenig Zeit, die Tage in Mailand zu reflektieren =\r\n.. F=FCr mich waren es sch=F6ne Tage, die wie im Fluge vergangen sind =\r\n"
                + "und ich nehme die vielen sch=F6nen Erinnerungen gerne mit in das Jahr =\r\n"
                + "2006. Ganz besonders m=F6chte ich dir an dieser Stelle danken. Du hast =\r\n"
                + "im besonderen Ma=DFe besonders dazu beigetragen, dass ich die Zeit in =\r\n"
                + "Codogno als sehr angenehm empfunden habe - nicht zuletzt auch wegen =\r\n"
                + "deinen detaillierten Ausf=FChrungen zur Organisation, zur Tombola und =\r\n"
                + "zur italienischen Musik (Ich habe mir eine CD von Paolo Conte gekauft, =\r\n"
                + "sie ist wirklich gro=DFartig!).\r\n"
                + "Herzliche Gr=FC=DFe von Sebastian\r\n"
                + "------=_NextPart_000_0011_01C6111A.B5505340\r\n"
                + "Content-Type: text/html;\r\n"
                + "\tcharset=\"iso-8859-1\"\r\n"
                + "Content-Transfer-Encoding: quoted-printable\r\n"
                + "\r\n"
                + "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\r\n"
                + "<HTML><HEAD>\r\n"
                + "<META http-equiv=3DContent-Type content=3D\"text/html; =\r\n"
                + "charset=3Diso-8859-1\">\r\n"
                + "<META content=3D\"MSHTML 6.00.2800.1528\" name=3DGENERATOR>\r\n"
                + "<STYLE></STYLE>\r\n"
                + "</HEAD>\r\n"
                + "<BODY bgColor=3D#ffffff>\r\n"
                + "<DIV><FONT face=3DArial size=3D2>Ciao Giuseppe,</FONT></DIV>\r\n"
                + "<DIV><FONT face=3DArial size=3D2>mittlerweile bin ich wieder in =\r\n"
                + "G=F6ttingen. Hier=20\r\n"
                + "wartet am Freitag einer Klausur in Mikrobiologie auf mich. Demnach kommt =\r\n"
                + "keine=20\r\n"
                + "Langeweile und mir bleibt leider nur wenig Zeit, die Tage in Mailand zu=20\r\n"
                + "reflektieren ... F=FCr mich waren es sch=F6ne Tage, die wie im Fluge =\r\n"
                + "vergangen sind=20\r\n"
                + "und ich nehme&nbsp;die vielen sch=F6nen Erinnerungen gerne&nbsp;mit in =\r\n"
                + "das Jahr=20\r\n"
                + "2006. Ganz besonders m=F6chte ich&nbsp;dir an =\r\n"
                + "dieser&nbsp;Stelle&nbsp;danken. Du=20\r\n"
                + "hast im besonderen Ma=DFe besonders dazu beigetragen, dass ich die Zeit =\r\n"
                + "in Codogno=20\r\n"
                + "als sehr angenehm empfunden habe - nicht zuletzt auch wegen deinen =\r\n"
                + "detaillierten=20\r\n"
                + "Ausf=FChrungen zur Organisation, zur Tombola und&nbsp;zur italienischen=20\r\n"
                + "Musik&nbsp;(Ich habe mir eine CD von Paolo Conte gekauft, sie ist =\r\n"
                + "wirklich=20\r\n"
                + "gro=DFartig!).</FONT></DIV>\r\n"
                + "<DIV><FONT face=3DArial size=3D2>Herzliche Gr=FC=DFe von=20\r\n"
                + "Sebastian</FONT></DIV></BODY></HTML>\r\n" + "\r\n"
                + "------=_NextPart_000_0011_01C6111A.B5505340--\r\n";

    
    
    public static final String QUOTED_PRINTABLE = "From - Mon Nov 06 17:20:18 2006\r\n" +
            "X-Mozilla-Status: 0001\r\n" +
            "X-Mozilla-Status2: 00000000\r\n" +
            "Return-Path: <giuseppe.garibaldi@frooglemail.com>\r\n" +
            "Delivered-To: 205-ponty@bambool.com\r\n" +
            "X-Spam-Checker-Version: SpamAssassin 3.1.3 (2006-06-01) on \r\n" +
            "\trs1000.servadmin.com\r\n" +
            "X-Spam-Level: \r\n" +
            "X-Spam-Status: No, score=0.0 required=7.0 tests=none autolearn=ham \r\n" +
            "\tversion=3.1.3\r\n" +
            "Received: (qmail 30162 invoked from network); 6 Nov 2006 10:19:55 -0600\r\n" +
            "Received: from ug-out-1314.google.com (66.249.92.168)\r\n" +
            "  by rs1000.servadmin.com with SMTP; 6 Nov 2006 10:19:55 -0600\r\n" +
            "Received-SPF: pass (rs1000.servadmin.com: SPF record at _spf.google.com designates 66.249.92.168 as permitted sender)\r\n" +
            "Received: by ug-out-1314.google.com with SMTP id y2so897459uge\r\n" +
            "        for <ponty@bambool.com>; Mon, 06 Nov 2006 08:19:52 -0800 (PST)\r\n" +
            "DomainKey-Signature: a=rsa-sha1; q=dns; c=nofws;\r\n" +
            "        s=beta; d=googlemail.com;\r\n" +
            "        h=received:message-id:date:from:to:subject:mime-version:content-type:content-transfer-encoding:content-disposition;\r\n" +
            "        b=gTA1u6Sl8NCf+BVT4ZUbST5Nqed8xIAClz7DPMU1dyWCEYIvl25o2pH3/fmfvQwR0BLY3ONo6cmpiIjc5/ajBvsDykCg//Fw/wC97FvHVgQcipyEc8lTj2UjpPYHfSNOhiu7d7EVcSViAG99thvaKHVRyzZM9CLZKLV5bpZGqVQ=\r\n" +
            "Received: by 10.78.201.2 with SMTP id y2mr6892824huf.1162829992097;\r\n" +
            "        Mon, 06 Nov 2006 08:19:52 -0800 (PST)\r\n" +
            "Received: by 10.78.186.10 with HTTP; Mon, 6 Nov 2006 08:19:47 -0800 (PST)\r\n" +
            "Message-ID: <c96b5e8f0611060819t3a600d4dhb0ed08052e61444e@mail.gmail.com>\r\n" +
            "Date: Mon, 6 Nov 2006 17:19:47 +0100\r\n" +
            "From: \"Giuseppe Garibaldi\" <giuseppe.garibaldi@frooglemail.com>\r\n" +
                    "To: \"Giuseppe Garibaldi\" <ponty@bambool.com>\r\n" +
                    "Subject: =?ISO-8859-1?Q?Questo_header_=E8_in_italiano?=\r\n" +
                    "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1; format=flowed\r\n" +
                    "Content-Transfer-Encoding: quoted-printable\r\n" +
                    "Content-Disposition: inline\r\n" +
                    "\r\n" +
                    "Questo text body =E8 in italiano, e il messaggio =E8 single-part!\r\n" +
                    "\r\n" +
                    "--=20\r\n" +
                    "Giuseppe Garibaldi\r\n" +
                    "Trientallee 45\r\n" +
                    "A - 21236 Lienz\r\n";
    
    public static final String MIME_SIGNED =
        "X-Gmail-Received: d67ef7b1396ce07ad22204ad2647c15669d25fb6\r\n"
                + "Delivered-To: giuseppe.garibaldi@gmail.com\r\n"
                + "Received: by 10.49.64.2 with SMTP id r2cs94773nfk;\r\n"
                + "        Fri, 29 Sep 2006 22:58:08 -0700 (PDT)\r\n"
                + "Received: by 10.67.101.10 with SMTP id d10mr3163934ugm;\r\n"
                + "        Fri, 29 Sep 2006 22:58:08 -0700 (PDT)\r\n"
                + "Return-Path: <direkt@wuppbank.de>\r\n"
                + "Received: from mailrelay2.bonn.wuppbank.de (mailrelay2.bonn.wuppbank.de [62.180.72.121])\r\n"
                + "        by mx.gmail.com with ESMTP id y7si401136ugc.2006.09.29.22.58.07;\r\n"
                + "        Fri, 29 Sep 2006 22:58:08 -0700 (PDT)\r\n"
                + "Received-SPF: pass (gmail.com: domain of direkt@wuppbank.de designates 62.180.72.121 as permitted sender)\r\n"
                + "Received: from unknown (HELO ls0004bz.wuppbank.de) ([10.252.68.174])\r\n"
                + "  by mailrelay2.bonn.wuppbank.de with ESMTP; 30 Sep 2006 07:58:07 +0200\r\n"
                + "Received: by ls0004bz.wuppbank.de (\"Sendmail++\", from userid 2)\r\n"
                + "\tid 71BEF2338F; Sat, 30 Sep 2006 07:58:07 +0200 (CEST)\r\n"
                + "Received: from GW033BN.Z1.bonn.wuppbank.de (unknown [10.252.66.158])\r\n"
                + " \tby ls0004bz.wuppbank.de (\"Sendmail++\") with ESMTP id 4095A2338F\r\n"
                + " \tfor <giuseppe.garibaldi@googlemail.com>; Sat, 30 Sep 2006 07:58:07 +0200 (CEST)\r\n"
                + "Received: from MS106BN (unverified) by GW031BN.Z1.bonn.wuppbank.de\r\n"
                + "    (Content Technologies SMTPRS 4.3.20) with ESMTP id\r\n"
                + "    <T7b0a23a8bf0afc429c3f4@GW031BN.Z1.bonn.wuppbank.de> for\r\n"
                + "    <giuseppe.garibaldi@googlemail.com>; Sat, 30 Sep 2006 07:58:06 +0200\r\n"
                + "Received: from DOM_GWIA1-MTA by MS106BN with Novell_GroupWise; Sat, 30 Sep\r\n"
                + "    2006 07:58:06 +0200\r\nMessage-Id: <451E23780200007500053B35@MS106BN>\r\n"
                + "X-Mailer: Novell GroupWise Internet Agent 7.0.1 Beta\r\n"
                + "Date: Sat, 30 Sep 2006 07:57:44 +0200\r\n"
                + "From: direkt@wuppbank.de\r\n"
                + "To: \"Giuseppe Garibaldi\" <giuseppe.garibaldi@googlemail.com>\r\n"
                + "Subject: Antw: Meine Adressenaenderung\r\n"
                + "References: <c96b5e8f0609291506p2cc960c6me098ea1aceb825d1@mail.gmail.com>\r\n"
                + "In-Reply-To: <c96b5e8f0609291506p2cc960c6me098ea1aceb825d1@mail.gmail.com>\r\n"
                + "Mime-Version: 1.0\r\n"
                + "Sender: direkt@wuppbank.de\r\n"
                + "X-Sender: direkt@wuppbank.de\r\n"
                + "X-From: direkt@wuppbank.de\r\n"
                + "Gecos: direkt@wuppbank.de\r\n"
                + "X-JULIA-VERIFICATION-STATUS-0: ok (0)  \r\n"
                + "MIME-Version: 1.0\r\n"
                + "Content-Type: multipart/signed; protocol=\"application/x-pkcs7-signature\"; micalg=sha1; boundary=\"----60CE79F562685C2989DF913B3608E96E\"\r\n"
                + "\r\n"
                + "This is an S/MIME signed message\r\n"
                + "\r\n"
                + "------60CE79F562685C2989DF913B3608E96E\r\n"
                + "Content-Type: text/plain; charset=ISO-8859-1\r\n"
                + "Content-Transfer-Encoding: quoted-printable\r\n"
                + "Content-Disposition: inline\r\n"
                + "\r\n"
                + "Guten Tag Herr Garibaldi,\r\n"
                + "\r\n"
                + "vielen Dank f=FCr Ihre E-Mail.\r\n"
                + "\r\n"
                + "Leider kann ich den Eingang Ihrer legitimierten Anschriften=E4nderung bis z=\r\n"
                + "um heutigen Tage nicht feststellen.\r\n"
                + "\r\n"
                + "Bitte teilen Sie uns Ihre neue Anschrift auf einem der folgenden Wege mit:\r\n"
                + "\r\n"
                + "- per Brief oder Fax (0180) 30 40 800 (9 Cent/ Minute) mit Ihrer rechtsverb=\r\n"
                + "indlichen Unterschrift.\r\n"
                + "\r\n"
                + "- telefonisch 7 x 24 Stunden =FCber unseren Direktservice (0180) 30 40 700 =\r\n"
                + "(9 Cent/ Minute). Bitte halten Sie hierf=FCr Ihre Telefon-Banking PIN berei=\r\n"
                + "t.\r\n"
                + "\r\n"
                + "- per Online-Banking: Senden Sie uns Ihren =C4nderungswunsch per Online-Mit=\r\n"
                + "teilung (Men=FC - Einstellungen - Kontakt). Bitte best=E4tigen Sie Ihre Mit=\r\n"
                + "teilung mit einer g=FCltigen TAN. Oder nutzen Sie folgenden Link: www.postb=\r\n"
                + "ank.de/kontaktformular=20\r\n"
                + "\r\n"
                + "Diese Vorgehensweise dient Ihrer Sicherheit, da Sie sich als Kontoinhaber l=\r\n"
                + "egitimieren. Eine E-Mail unterliegt nicht diesem Sicherheitsstandard.\r\n"
                + "\r\n"
                + "Ein Hinweis f=FCr Sie:\r\n"
                + "W=FCnschen Sie eine schriftliche Best=E4tigung =FCber die Anschriften=E4nde=\r\n"
                + "rung, berechnen wir ein Entgelt in H=F6he von 3,50 Euro.\r\n"
                + "\r\n"
                + "Fehlen Ihnen Kontoausz=FCge? Bitte geben Sie uns dies bei Ihrer legitimiert=\r\n"
                + "en Anschriften=E4nderung noch einmal bekannt.\r\n"
                + "\r\n"
                + "Mit freundlichen Gr=FC=DFen\r\n"
                + "\r\n"
                + "Ihr Wuppbank E-Mail Team\r\n"
                + "Birgit Mannesmann\r\n"
                + "\r\n"
                + "\r\n"
                + "\r\n"
                + "\r\n"
                + ">>> \"Giuseppe Garibaldi\" <giuseppe.garibaldi@googlemail.com> 30.9.06  0.0=\r\n"
                + "6 >>>\r\n"
                + "Sehr geehrte Damen und Herren,\r\n"
                + "\r\n"
                + "vor einigen Monaten hatte ich auf der Homepage der Wuppbank eine\r\n"
                + "Adressen=E4nderung beantragt.\r\n"
                + "\r\n"
                + "Seit damals als Hinweis zur Adress=E4nderung steht auf der obengenannten\r\n"
                + "Homepage (Mein Profil - Adress- & Kontaktdaten) folgender Text: \"Ihre\r\n"
                + "Daten wurden =FCbermittelt. Da die komplette Korrespondenz mit der\r\n"
                + "Wuppbank umgestellt wird, kann die Aktualisierung einige Tage in\r\n"
                + "Anspruch nehmen. Sobald alle Systeme der Wuppbank aktualisiert wurden,\r\n"
                + "wird der Hinweis wieder entfernt und die ge=E4nderten Daten werden\r\n"
                + "angezeigt\".\r\n"
                + "\r\n"
                + "Diese Aktualisierung nimmt wohl nicht \"einige Tage\" in Anspruch,\r\n"
                + "sondern mehrere Monate.\r\n"
                + "\r\n"
                + "Ich m=F6chte gerne den Stand dieses Prozesses erfahren.\r\n"
                + "\r\n"
                + "Mit freundlichem Gru=DF,\r\n"
                + "\r\n"
                + "    Giuseppe Garibaldi\r\n"
                + "\r\n"
                + "--=20\r\n"
                + "Giuseppe Garibaldi\r\n"
                + "Viale Teano 26\r\n"
                + "I - 26845 Vienna (Lodi)\r\n"
                + "\r\n"
                + "Mobile: +39 340 1234567\r\n"
                + "\r\n"
                + "\r\n"
                + "\r\n"
                + "Diese Nachricht ist nur f=FCr den vorgesehenen Empf=E4nger bestimmt. Sollte=\r\n"
                + "n Sie nicht der vorgesehene Empf=E4nger dieser E-Mail und ihres Inhalts sei=\r\n"
                + "n oder diese E-Mail irrt=FCmlich erhalten haben, bitten wir Sie, den Absend=\r\n"
                + "er unverz=FCglich dar=FCber zu informieren und diese Nachricht und all ihre=\r\n"
                + " Anh=E4nge vollst=E4ndig von Ihrem Computer zu l=F6schen.=20\r\n"
                + "Jede Form der unbefugten Nutzung, Ver=F6ffentlichung, des Kopierens oder de=\r\n"
                + "r Offenlegung des Inhalts dieser E-Mail ist nicht gestattet.=20\r\n"
                + "\r\n"
                + "This message is intended for the addressee only. If you are not the intende=\r\n"
                + "d recipient of this e-mail message and its content or have received this e-=\r\n"
                + "mail in error, please notify the sender immediately and delete this message=\r\n"
                + " and all its attachments.=20\r\n"
                + "Any form of unauthorized use, publication, copying or disclosure of the con=\r\n"
                + "tent of this e-mail is prohibited.\r\n"
                + "\r\n"
                + "\r\n"
                + "------60CE79F562685C2989DF913B3608E96E\r\n"
                + "Content-Type: application/x-pkcs7-signature; name=\"smime.p7s\"\r\n"
                + "Content-Transfer-Encoding: base64\r\n"
                + "Content-Disposition: attachment; filename=\"smime.p7s\"\r\n"
                + "\r\n"
                + "MIIHHgYJKoZIhvcNAQcCoIIHDzCCBwsCAQExCzAJBgUrDgMCGgUAMAsGCSqGSIb3\r\n"
                + "DQEHAaCCBDswggQ3MIIDoKADAgECAg5XagABAAJvfq3pKO4HmjANBgkqhkiG9w0B\r\n"
                + "AQUFADCBvDELMAkGA1UEBhMCREUxEDAOBgNVBAgTB0hhbWJ1cmcxEDAOBgNVBAcT\r\n"
                + "B0hhbWJ1cmcxOjA4BgNVBAoTMVRDIFRydXN0Q2VudGVyIGZvciBTZWN1cml0eSBp\r\n"
                + "biBEYXRhIE5ldHdvcmtzIEdtYkgxIjAgBgNVBAsTGVRDIFRydXN0Q2VudGVyIENs\r\n"
                + "YXNzIDMgQ0ExKTAnBgkqhkiG9w0BCQEWGmNlcnRpZmljYXRlQHRydXN0Y2VudGVy\r\n"
                + "LmRlMB4XDTA1MTAyNzExMDc1MFoXDTA2MTAyNzExMDc1MFowgasxCzAJBgNVBAYT\r\n"
                + "AkRFMRwwGgYDVQQIExNOb3JkcmhlaW4tV2VzdGZhbGVuMQ0wCwYDVQQHEwRCb25u\r\n"
                + "MREwDwYDVQQKEwhQb3N0YmFuazEXMBUGA1UECxMORGlyZWt0dmVydHJpZWIxIDAe\r\n"
                + "BgNVBAMTF1Bvc3RiYW5rIERpcmVrdHZlcnRyaWViMSEwHwYJKoZIhvcNAQkBFhJk\r\n"
                + "aXJla3RAcG9zdGJhbmsuZGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\r\n"
                + "AQCxZyTzr8Qphel+6lf+CYGJaSkbt/YjSua8LF4g49WP3/cHnPim6+pG/lgxHaK7\r\n"
                + "Cmak8Rr+clh3Z4QWMvv8XsltuFcz2vHzxIxFmz9us/Fb0KqC3rbuihZ8VaRrs9Xk\r\n"
                + "t95jGcH8LChTw2ErAP7Sl4TRwQdNrijLNYxaTnXVgIPeiHwqlI+bAe5dbkEgG4NQ\r\n"
                + "DN+Uy8QpvK0mRXDtKyBmbVfP0NsbTmByZazOA0nuMbPqC/W9eOA+3Kd3iyor1nlu\r\n"
                + "DhD+/l2GBfksWaW9iFobpVgP1bZ/tIRqnj2uQDTKUYQoy/RhrzCzpi/pdJvLVt2E\r\n"
                + "MQmFza7moXW0+++IOForkU3/AgMBAAGjgcYwgcMwDAYDVR0TAQH/BAIwADAOBgNV\r\n"
                + "HQ8BAf8EBAMCBeAwPgYJYIZIAYb4QgEIBDEWL2h0dHA6Ly93d3cudHJ1c3RjZW50\r\n"
                + "ZXIuZGUvZ3VpZGVsaW5lcy9pbmRleC5odG1sMBEGCWCGSAGG+EIBAQQEAwIFoDBQ\r\n"
                + "BglghkgBhvhCAQMEQxZBaHR0cHM6Ly9ucnUudGNjbGFzczMudHJ1c3RjZW50ZXIu\r\n"
                + "ZGUvNTc2QTAwMDEwMDAyNkY3RUFERTkyOEVFMDc5QT8wDQYJKoZIhvcNAQEFBQAD\r\n"
                + "gYEANb0/L1f7JQrjac1mLzzIAZBMkJ3+t2BiXXn+ySd3pp2B87zLenICuGXOz0ZQ\r\n"
                + "bz6sj7i6NH27wrjxTTZUDt27IDopXPjaMRZuOlulI4ML8+j/NWstBjHjhuSGRRF5\r\n"
                + "8manaF9EY/G7OaxWiBaLHcDm4Z/2H9gi/Ju6ZBDvopkGaOYxggKrMIICpwIBATCB\r\n"
                + "zzCBvDELMAkGA1UEBhMCREUxEDAOBgNVBAgTB0hhbWJ1cmcxEDAOBgNVBAcTB0hh\r\n"
                + "bWJ1cmcxOjA4BgNVBAoTMVRDIFRydXN0Q2VudGVyIGZvciBTZWN1cml0eSBpbiBE\r\n"
                + "YXRhIE5ldHdvcmtzIEdtYkgxIjAgBgNVBAsTGVRDIFRydXN0Q2VudGVyIENsYXNz\r\n"
                + "IDMgQ0ExKTAnBgkqhkiG9w0BCQEWGmNlcnRpZmljYXRlQHRydXN0Y2VudGVyLmRl\r\n"
                + "Ag5XagABAAJvfq3pKO4HmjAJBgUrDgMCGgUAoIGxMBgGCSqGSIb3DQEJAzELBgkq\r\n"
                + "hkiG9w0BBwEwHAYJKoZIhvcNAQkFMQ8XDTA2MDkzMDA1NTgwN1owIwYJKoZIhvcN\r\n"
                + "AQkEMRYEFExOF0eWJqEXacmrzohtp4XiQJkeMFIGCSqGSIb3DQEJDzFFMEMwCgYI\r\n"
                + "KoZIhvcNAwcwDgYIKoZIhvcNAwICAgCAMA0GCCqGSIb3DQMCAgFAMAcGBSsOAwIH\r\n"
                + "MA0GCCqGSIb3DQMCAgEoMA0GCSqGSIb3DQEBAQUABIIBAJoBGkP33d/ms5LpYB9o\r\n"
                + "SZk/fP6AFvsM0Akt1Yeh3PNN3ybQiixCu69wjtArWST65arugUw7WZ1YJ75qsm6n\r\n"
                + "gorSmuDgq1kfA2xWnB8eMYr2ekz328D7uFyAoBmh0UEt3LHf9iLSC3krBpJxJqoW\r\n"
                + "Ue3b4lbK5i2qOpQu7kqOuZcOX4PWdSBp0dasrdVPcJa3h8W3vmrUsPS6KLJWW/pF\r\n"
                + "GMtSpklH0vDAKTkI7FsoJzqRpeDJGjY0oDkrtYPv38OpZR+voeJpDQPShvo3KSyH\r\n"
                + "FBUeK1woI+/7i0zRcdWZHCp+XjXxsMfa8uhZ0YC6tNkzQBVcC0aQGaq9thP8JbrR\r\n"
                + "yCA=\r\n" + "\r\n"
                + "------60CE79F562685C2989DF913B3608E96E--\r\n";
    
    public static final String MULTIPART_JPEG = "From - Tue Nov 07 14:13:58 2006\r\n" +
            "X-Mozilla-Status: 0001\r\n" +
            "X-Mozilla-Status2: 00000000\r\n" +
            "Return-Path: <giuseppe.garibaldi@platsch.com>\r\n" +
            "Delivered-To: 205-gaudy@bambool.com\r\nX-Spam-Checker-Version: SpamAssassin 3.1.3 (2006-06-01) on \r\n" +
            "\trs1000.servadmin.com\r\n" +
            "X-Spam-Level: \r\n" +
            "X-Spam-Status: No, score=0.0 required=7.0 tests=none autolearn=ham \r\n" +
            "\tversion=3.1.3\r\n" +
            "Received: (qmail 5071 invoked from network); 7 Nov 2006 07:13:30 -0600\r\n" +
            "Received: from ug-out-1314.google.com (66.249.92.170)\r\n" +
            "  by rs1000.servadmin.com with SMTP; 7 Nov 2006 07:13:30 -0600\r\n" +
            "Received-SPF: pass (rs1000.servadmin.com: SPF record at _spf.google.com designates 66.249.92.170 as permitted sender)\r\n" +
            "Received: by ug-out-1314.google.com with SMTP id m2so1355157ugc\r\n" +
            "        for <gaudy@bambool.com>; Tue, 07 Nov 2006 05:13:28 -0800 (PST)\r\n" +
            "DomainKey-Signature: a=rsa-sha1; q=dns; c=nofws;\r\n" +
            "        s=beta; d=platsch.com;\r\n" +
            "        h=received:message-id:date:from:to:subject:mime-version:content-type;\r\n" +
            "        b=KMiUGTZa55KJUfg4aoIacKsk174uWvhk6x58ie1Q/sAIXzwMHZNb5Oh40ItRqSspN4o+yMECt89OaGNpO5ZPQg7R5/a/H7fJk832wyL4ipze3kcu0sMDKeD5qlZHcTYm0h26ddtPPqV1bW/25NsYY8m8HargqLcH6Oxre9sZPcw=\r\n" +
            "Received: by 10.78.97.7 with SMTP id u7mr8211532hub.1162905206843;\r\n" +
            "        Tue, 07 Nov 2006 05:13:26 -0800 (PST)\r\n" +
            "Received: by 10.78.186.10 with HTTP; Tue, 7 Nov 2006 05:13:26 -0800 (PST)\r\n" +
            "Message-ID: <c96b5e8f0611070513o3ec40fcbt24fdfed718856167@mail.gmail.com>\r\n" +
            "Date: Tue, 7 Nov 2006 14:13:26 +0100\r\n" +
            "From: \"Giuseppe Garibaldi\" <giuseppe.garibaldi@platsch.com>\r\n" +
            "To: \"Giuseppe Garibaldi\" <gaudy@bambool.com>\r\n" +
            "Subject: =?ISO-8859-1?Q?Questo_subject_=E8_in_italiano?=\r\n" +
            "MIME-Version: 1.0\r\n" +
            "Content-Type: multipart/mixed; \r\n" +
            "\tboundary=\"----=_Part_3944_4636740.1162905206437\"\r\n" +
            "\r\n" +
            "------=_Part_3944_4636740.1162905206437\r\n" +
            "Content-Type: text/plain; charset=ISO-8859-1; format=flowed\r\n" +
            "Content-Transfer-Encoding: quoted-printable\r\n" +
            "Content-Disposition: inline\r\n" +
            "\r\n" +
            "Questo content =E8 italiano text/plain, ma c'=E8 anche un attachment grafic=\r\n" +
            "o.\r\n" +
            "\r\n" +
            "--=20\r\n" +
            "Giuseppe Garibaldi\r\n" +
            "Trientallee 26\r\n" +
            "A - 22347 Lienz\r\n" +
            "\r\n" +
            "------=_Part_3944_4636740.1162905206437\r\n" +
            "Content-Type: image/jpeg; name=\"Oval Lingotto Torino.jpg\"\r\n" +
            "Content-Transfer-Encoding: base64\r\n" +
            "X-Attachment-Id: f_eu8bozhj\r\n" +
            "Content-Disposition: attachment; filename=\"Oval Lingotto Torino.jpg\"\r\n" +
            "\r\n" +
            "/9j/4AAQSkZJRgABAQEASABIAAD/4RZ2RXhpZgAASUkqAAgAAAALAA8BAgAJAAAAkgAAABABAgAP\r\n" +
            "AAAAmwAAABIBAwABAAAAAAAAABoBBQABAAAAqgAAABsBBQABAAAAsgAAACgBAwABAAAAAgAAADEB\r\n" +
            "AgAlAAAAugAAADIBAgAUAAAA3wAAABMCAwABAAAAAgAAAJiCAgAFAAAA8wAAAGmHBAABAAAA+AAA\r\n" +
            "AGoEAABGVUpJRklMTQBGaW5lUGl4IFM1MDAwIABIAAAAAQAAAEgAAAABAAAARGlnaXRhbCBDYW1l\r\n" +
            "cmEgRmluZVBpeCBTNTAwMCBWZXIzLjAwADIwMDU6MTI6MTAgMTg6Mjc6MDkAICAgIAAkAJqCBQAB\r\n" +
            "AAAArgIAAJ2CBQABAAAAtgIAACKIAwABAAAAAgAAACeIAwABAAAAkAEAAACQBwAEAAAAMDIyMAOQ\r\n" +
            "AgAUAAAAvgIAAASQAgAUAAAA0gIAAAGRBwAEAAAAAQIDAAKRBQABAAAA5gIAAAGSCgABAAAA7gIA\r\n" +
            "AAKSBQABAAAA9gIAAAOSCgABAAAA/gIAAASSCgABAAAABgMAAAWSBQABAAAADgMAAAeSAwABAAAA\r\n" +
            "AQAAAAiSAwABAAAAAAAAAAmSAwABAAAAEAAAAAqSBQABAAAAFgMAAHySBwAeAQAAHgMAAACgBwAE\r\n" +
            "AAAAMDEwMAGgAwABAAAA//8AAAKgBAABAAAAIAMAAAOgBAABAAAAWAIAAAWgBAABAAAATAQAAA6i\r\n" +
            "BQABAAAAPAQAAA+iBQABAAAARAQAABCiAwABAAAAAwAAABeiAwABAAAAAgAAAACjBwABAAAAAwAA\r\n" +
            "AAGjBwABAAAAAQAAAAGkAwABAAAAAAAAAAKkAwABAAAAAAAAAAOkAwABAAAAAQAAAAakAwABAAAA\r\n" +
            "AAAAAAqkAwABAAAAAAAAAAykAwABAAAAAAAAAAAAAAAKAAAASwAAABgBAABkAAAAMjAwNToxMjox\r\n" +
            "MCAxODoyNzowOQAyMDA1OjEyOjEwIDE4OjI3OjA5AB4AAAAKAAAAAAABIgAAAGQsAQAAZAAAAP//\r\n" +
            "/5UAAABkAAAAAAAAAGQsAQAAZAAAADoCAABkAAAARlVKSUZJTE0MAAAAFQAAAAcABAAAADAxMzAA\r\n" +
            "EAIACAAAAA4BAAABEAMAAQAAAAMAAAACEAMAAQAAAAAAAAADEAMAAQAAAAAAAAAQEAMAAQAAAAIA\r\n" +
            "AAAREAoAAQAAABYBAAAgEAMAAQAAAAAAAAAhEAMAAQAAAAAAAAAiEAMAAQAAAAEAAAAjEAMAAgAA\r\n" +
            "AIAC4AEwEAMAAQAAAAAAAAAxEAMAAQAAAAYAAAAyEAMAAQAAAAEAAAAAEQMAAQAAAAEAAAABEQMA\r\n" +
            "AQAAAAUAAAAAEgMAAQAAAAAAAAAQEgMAAQAAAAAAAAAAEwMAAQAAAAEAAAABEwMAAQAAAAAAAAAC\r\n" +
            "EwMAAQAAAAAAAAAAAAAATk9STUFMIAAAAAAACgAAAGUJAAABAAAAZQkAAAEAAAACAAEAAgAEAAAA\r\n" +
            "Ujk4AAIABwAEAAAAMDEwMAAAAAAGAAMBAwABAAAABgAAABoBBQABAAAAuAQAABsBBQABAAAAwAQA\r\n" +
            "ACgBAwABAAAAAgAAAAECBAABAAAAyAQAAAICBAABAAAAphEAAAAAAABIAAAAAQAAAEgAAAABAAAA\r\n" +
            "/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAYEBQYFBAYGBQYHBwYIChAKCgkJChQODwwQFxQYGBcU\r\n" +
            "FhYaHSUfGhsjHBYWICwgIyYnKSopGR8tMC0oMCUoKSj/2wBDAQcHBwoIChMKChMoGhYaKCgoKCgo\r\n" +
            "KCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCj/xAGiAAABBQEBAQEB\r\n" +
            "AQAAAAAAAAAAAQIDBAUGBwgJCgsQAAIBAwMCBAMFBQQEAAABfQECAwAEEQUSITFBBhNRYQcicRQy\r\n" +
            "gZGhCCNCscEVUtHwJDNicoIJChYXGBkaJSYnKCkqNDU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVm\r\n" +
            "Z2hpanN0dXZ3eHl6g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS\r\n" +
            "09TV1tfY2drh4uPk5ebn6Onq8fLz9PX29/j5+gEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL\r\n" +
            "EQACAQIEBAMEBwUEBAABAncAAQIDEQQFITEGEkFRB2FxEyIygQgUQpGhscEJIzNS8BVictEKFiQ0\r\n" +
            "4SXxFxgZGiYnKCkqNTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqCg4SFhoeI\r\n" +
            "iYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2dri4+Tl5ufo6ery\r\n" +
            "8/T19vf4+fr/wAARCAB4AKADASIAAhEBAxEB/9oADAMBAAIRAxEAPwD5XxRilpasY3FGKkiieVts\r\n" +
            "SM59AM1qw6HIAGvJY7dT2PLH8KzlUjDdlxpylsjGxVq0sLm6/wBRC7D+9jAH410NvaWltj7Pbec/\r\n" +
            "/PSfp+Aqy3mOP3spx/dT5RXPLEv7K+82VBL4n9xl2Xhx5Z1iklDTNnEUXLHFWWtrCzfyvsrzNnDD\r\n" +
            "HX2yf8K6rwTbKupvcmI+XChG5eoZu/5ZrqNb0bT9VXCMy3DJueVEAA54BH8XTrWsKUqsOds5quJj\r\n" +
            "Sn7NL5nkmqaErRNd6QWltxzJCeZIfqO49658ivQ77RbzRLrzIAsIH3Zsl9309fpVG/0iDWV821QW\r\n" +
            "l+Rna42R3B9V9D+lWpX0e4ktLrY4rFGKnuLeW3meG4jaOVDhlYYIqLFUA3FGKdikNACYoxS0GgBM\r\n" +
            "UmKdRigBpFGKdSGgC9badcTjcF2J/efgVqW2l2y4JD3Le3yp+dakduowXy59X5/Sp9uRXDPESex3\r\n" +
            "xoxRUSJlXapWFP7sQ/rT0iVTkLz/AHicmpsY9BRt/GsbmliP/PFAWpNv4U+KFppEjQEs7BR9TxSu\r\n" +
            "Kx23hCzlg0lJkCsZmLlTwcdBg/h3robeBJGJi/dT91I5/Ed/qKu2NkkNtFEowI1Cj8BVw2qugWRQ\r\n" +
            "wHQjqP8ACvWg+SKij5+o/aTcu5z00RghkW4QMzMWwRlHJPA9v51Qu/CNtPdHyfLW4PzktmVumflH\r\n" +
            "Q/z9a7FrWRFxjz4j1Dfe/wADUMVjAJGnjY4j+9GxwVA5OPQ1NRKaNKU5U3pscPrHg3+0oVg1JZFu\r\n" +
            "QP3N1w0p9PkUcr7ZyK8t8R+GtR0C8MF/Ayg8pIoJRx6g19k+BbaG4t44Lq1luZGG6SIYDFz/AHz0\r\n" +
            "wM4/Csj4kTaRpkd3pWnaTZXMs4cTlY2f7NuGGEZ7N6scAHtXLCrJb7I9edKDtbdnxky4pMV13inw\r\n" +
            "hdaVG15axzTadnBZl+aE/wB18cfiOK5QiupO+pxtNOzI8UEU/FGKZIzFGKdijFADaTFPoIpAd2o4\r\n" +
            "6U7GfenKvHA/OnY9/wAq8ds9exFt57UmB3P5VKV9vzppH0ouKw0L6Ct3wZZfatfgz92IGU/h0/U1\r\n" +
            "j7c54/Cu/wDhnYfubq6Kf6xhGvHYcn9T+la0FzTRz4mXLTbO1t7ckAEAir0dtnp+RrQsNPLrlM4r\r\n" +
            "SSwOPmTI9RXc6h5Cgc+8GOowfUVVurL7WYrc5DzOI/MT7yg9T+QrqJbH5fl59jVbT7fZqTzsNi2s\r\n" +
            "RYk9AW/+sD+dOD5tAUbMe+u3WgeHri3so3mkEhiN2oAwWH3sZzuGcE/kK8+3XEFybq2cTAY4PPOe\r\n" +
            "pHf+ddV4iZks7FJFxLKTcSMD1zzyPxH5VgSRDYZB8rn+Jf6+tbRggr15NryI5Jl1KNnudReP5Suw\r\n" +
            "KGDr3B6Z/wA8V5T438EQiZrnQEmU/ee2eMgH3jPT/gP5V6heWqyxqJBsduPMTp17jtQ9xJbhrfVI\r\n" +
            "Wu7TGyKUOVwe2SP61k6bhrD7jWni+f3av3nzQ8ZRirAhgcEEcg0zFe+eI/hje+Iy9zYWeycNsWZB\r\n" +
            "wzdlf1/3h+NeNeIdEvNA1afTtSjEdzCcMFYMD9COCPcURmpbG8oOJkEUYp5FGKogjxQRT8UYoA75\r\n" +
            "V46fnTscdQKcFyO5p2PYCvEbPYsQkfWkK9cYqUj3oK8Hgmi4WEC56c+le3+B9Ie10+0tiMSAZdcf\r\n" +
            "xHk15R4Utlu/EdhG4BXzQ7Z6EL82PxxivoDwsi/bY3kQ/eye9dWGvrI4Ma78sT0i20OG20ZUZA0u\r\n" +
            "3J+tPOhRrbIq5VyeavtfwyPEiP1IJq4Zk3hdyk9a6VSg+pkY0ugRM/QFQP1rjDp+bK48tsPfXOwB\r\n" +
            "hxsB2/yBrv8AVdQS30+4kQ/vACqD1Y8D9TXKtusSV81RHZWzMxc/xEYGPwB/Ot8PSjGTaE9TzXxa\r\n" +
            "/m67cDCgRYiGw8cdf51nXMJDQRAfMQM4681bsLaTU9Wjjx888nOPc81cjjW51u7nHMNuryfgvA/p\r\n" +
            "WzlZnmSXO2+5kCPdePjlYgSfwpoty0cccY+aY524yGJ4GRWlDbFdFlnI/eXMwhT3xy381Fauj6cs\r\n" +
            "viXy+fLtF5PbKjA5/wB40X1sJRbKGsXNxp2kx6dpaRRXJj2zzFslweCiH+EH8zmvK9Z8MjVD9j8i\r\n" +
            "SeT+GMcSRk/3Ceo9u/617NqVt5s00gU7GJxu54HA+vArHfS/MuomRcvGdyk9QfY9q5+VdDp9tJS1\r\n" +
            "2PmjxT4R1bw3OBqFpMsD/wCrmKEK3sfQ+1c/t5r6z8RXS6rFLo91b7tEkCxEqQZCR0L9Nwz0I6V4\r\n" +
            "j8RPhvdeF0W+triK50yY5j+b96B6leu3/aoT6Pc67RkrwZ5xtoxU5TmkKVRB3ijjuaXb7VIq8dac\r\n" +
            "V+teDc9qxAQaCOOtSlfY0m0+lCYmdT8N7Pz9alnbgQx8H3bj+Wa9K1HxVp3hW3+0X8rMwXckMYBd\r\n" +
            "+ccf/Xri/h8htdOknGN00h/Icf410up2Wna0p/tCyhlnWNo4pX58vPOcdDyAea9ajTapLzPGxFWL\r\n" +
            "rO/Q1G+L9gmrWVpYW73nm4aWTdsEKlN/ocntjj610XhDxfq+u63q0V7pUdpY2xUQSq+4yE843A4b\r\n" +
            "jnI4HSvJLfw/q8fh8W8M1naX0RaQz29uqsckAjeoDYPGB2r0688USW2g2NvBaRx3QCo1xn5jt65X\r\n" +
            "pzjmq5LrQG1sjsSXvtRt7cFwisZn9MKP8SK57xPftHol23mYa7n2KCvJVeP6VQh8T6jb2UjRiGWa\r\n" +
            "VSpcD51B54A+g5rk/GesvZtawuxYJB5pEmRjK5H+fetqU1FWYVE4xaW5p+GtatNGvJZ7qIyOYnWI\r\n" +
            "q33XIwCRT9O1Owj8Pakvmn7fcuibSOAmck5+oFeIG8mnu2kmkZ2Y5OTxWrDFcPps17FnyYZUR1DH\r\n" +
            "guGIP/jhrVNT1scfLKOlz3iyS0lvNHt7e4ilgsrdrmUq3Bk5cj3PCitDwVZvJBdXQOJLmURhj+pH\r\n" +
            "4sPyrw/SJruV4jaPeMSMlYT5m38DXt/hW/H9g6ajNtn5bLr5bMM43EZwMkn8qTaia04t+9Y6PV9B\r\n" +
            "QzwRWy/IRjNZN3owtHvGYcICAfYDr/P8hW1q3iSLS7J2uwI2jUgknkY6/j/jXlWo/Ea5uIZtu5fN\r\n" +
            "lZiJQCAp6KB19Kwpw1HNw7FptLJlUsOFBkb2x/8AXP61gavpyxXLajqUTTNMpO2Qkh8cLn29x2Fa\r\n" +
            "eneOIpQ8VzbMZXwA6KeeSela+uzaZrTW8UV7GpSJY0ST5DnoQAR2OaqabdiaatrE+cfiTpGlWGoR\r\n" +
            "y6c/lT3CiWSyRPkhB6Yb39K4sx12/im1bVvEmqT2OZbdJfLjPcrnauB3z1/GsKbS5of9ZGynJHI7\r\n" +
            "0rnW03qzp1jOO1IVA6sKra3eNY+M5NLm0+1RQUjclGZgQOSMnuT1x6VZvYzNB5kK5YLhcp8hPqcd\r\n" +
            "voa8evQ9jPlkz1aUnWjzJEbPEv35UX6sBTQ8bBijhtoLHac4HrWfbHUjd7U1DTInkXbtlWNwvr94\r\n" +
            "ED9K5GEy2urMm9C0LkFlOVIHp7cV1xwCkrxlc5ZYlxdpKx7bputaXZWcMDahbKyKFI8wfe7/AK0u\r\n" +
            "peLtOtdOnlt7+CScIfLVXzlu1eIKzAgsAxwXPHc027WWKO3YrgPnHHvXqXSVjyPY88nJs+mdKupb\r\n" +
            "nToJJZXLSQxlz/eJAb+dXorSCeYGXzDjPRsV5r4C8SzXFpBFfeVHFskUNnGPL2Ko59jXVrr8Mctw\r\n" +
            "BKg8v7hzw/FGlrmz0Z7LYwtpvh2Oe2vEg8vpuQsxAOAORjvmvNPF2jW2u396WmuJ7sxMWnQEhQMB\r\n" +
            "SR/dziny+MLJ2i265IloIGDWrLuCSYGCPXnnPavKPEV1PN4gvdQtdfMSTEDeJthP94bQemPpzXPz\r\n" +
            "6aaG01d66ncwfCDUjpsOorrFsIHZo3KxEmMgcbueMnj2yK2Jfhdqmj6aLG48Q2yQahdxBo3tgWyu\r\n" +
            "drZ3cfePHfIrjvBXj+x8N2OoQXTHUPtVq8JSWdtpZsbi2AcnAXB9qS++LE97Z+Vd/Z5JUube4WYR\r\n" +
            "tvLQY2ZOenGTWinCwKmrnpWlfDSXRr4+X4ihmjWOSUuIlQKiMq5JLED7x/75Nbk11Zi9i0u/uI7a\r\n" +
            "K2hVJJFYAxrnhnHUZPcdSa8Du/iZdSWV/YxTLBZXcLQvDDajBVnZyMsSerdawYfE8CLsMcjK0Swv\r\n" +
            "+7UFkX7oPrg+tJzhYSjZqx9QeKtJGraci3d9DbRF3xMIwRhRluc88lR/+quFg8Cfa7a8vDq8C29o\r\n" +
            "7RSSlcjcBnGAcmvLtR+JF/eWiwM92QpJX95gAk5JwB3rPPja8WJ0SGXa7FyDK3LHvxjmplVXRB7K\r\n" +
            "J7x4c0MLbXckGpaVJ9lRJ38yEEjIDDGTzwcH8qoapHpuu6/Z3F1qmjW8kkptxCkflRgBS+9l469M\r\n" +
            "+teHxeL7+OIILcEBduWd+n51z+s6heX1yt0UWPYMFUJ5/MmpjK7s1ZFtJLTU9yj0vwm3iqzjn1+J\r\n" +
            "hdHe5tISBGc4xk/TPSt7xLo/gSyvbqTVtdvbogeY8kUatuPsfWvlw3l1FIrZIZBt44xzWt4g1u5u\r\n" +
            "beEOYgOMlFwTx3qmnzaC9orO5tarBqGrahJf6nqUk95IQXmkO5jjgc1RWwkt4xGupyxop4VZCAKz\r\n" +
            "TdvIP3kx/Om+YpPMp/76pyjF7o057bEsmj2xcsZwzE5JJ70i2MVu++OVQ3qKhaSP/nofzqGSSPu1\r\n" +
            "GpDsy+ZH6faGP0pjSA8SMWwQfm55FUN8R/izRuj/ALw5pMmy6G8blE0uFlx/rZA34hT/AEqp9tiH\r\n" +
            "VV/75rMyCuNxx1xmm7Af4qlxTBmr9vh/uL+VNN9CP4R+VZZSL+8fzpDGhHyqfxNHKguzTa9i9PyF\r\n" +
            "RvewnsT9azTGPVR+NHlr320WFcuNeqT8oFRm6z6D9arkKO60mF/vLRYCz5/+2RQJQejGqx2/3qNy\r\n" +
            "+posMn8zH8RpyzAK/JzwRn61VLL7/nTWYdjik1cadmSzur3DPg4I7nvUVwyvEeuR700t/tCmPJkE\r\n" +
            "E+1NC3HCbgDrjpzR5xqClFbDJjN70ecfU1X71I1JiH+ac5yeaUuT61F3FSdqTAN596CzetJ3FDdB\r\n" +
            "SsMXcfXNGM9XFMXv9KB1osIdx2NHTvTO9KaQC5H+RSGl7UdhTSEJjNLik70CgBSM+lKV/wB2kHQU\r\n" +
            "dqQAV+lNYYpxpj0IQP/Z/+0A3lBob3Rvc2hvcCAzLjAAOEJJTQQEAAAAAADBHAJQAAtQaWNhc2Eg\r\n" +
            "Mi4wABwCdQCYPGhlbGxvc3RhbXA+CiA8Z2lkPjAtMC03ZmZmZmZmZi0wPC9naWQ+CiA8bWQ1PjAt\r\n" +
            "MC0wLTA8L21kNT4KIDxvcmlnV2lkdGg+MDwvb3JpZ1dpZHRoPgogPG9yaWdIZWlnaHQ+MDwvb3Jp\r\n" +
            "Z0hlaWdodD4KIDxvcmlnU2l6ZT4wPC9vcmlnU2l6ZT4KPC9oZWxsb3N0YW1wPgocAnYADzxwaWNh\r\n" +
            "c2FzdGFtcC8+CgD/2wBDAAYEBQYFBAYGBQYHBwYIChAKCgkJChQODwwQFxQYGBcUFhYaHSUfGhsj\r\n" +
            "HBYWICwgIyYnKSopGR8tMC0oMCUoKSj/2wBDAQcHBwoIChMKChMoGhYaKCgoKCgoKCgoKCgoKCgo\r\n" +
            "KCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCj/wAARCADhASwDASIAAhEBAxEB/8QA\r\n" +
            "HAAAAAcBAQAAAAAAAAAAAAAAAAECAwQFBgcI/8QAQRAAAQMCBAMGBAUCBAUEAwAAAQACAwQRBRIh\r\n" +
            "MQZBURMiYXGBkQcUMqEjQlKxwRViM4LR4RZTkvDxNENjoiRy0v/EABoBAAIDAQEAAAAAAAAAAAAA\r\n" +
            "AAABAgMEBQb/xAAnEQACAgICAgICAgMBAAAAAAAAAQIRAyESMQRBE1EiMmFxFFKBQv/aAAwDAQAC\r\n" +
            "EQMRAD8A8soeSMIWUxhBCyO2qCAC+yBRmyCACQRoJAEhZGnoKaad1oY3O5aBDdbYJNvQxZGr+l4c\r\n" +
            "lfZ1VKyEfpGrlYNocNoAHPDS79UhufQLNLyoJ1HZfHxptXLRl6eiqKj/AAonEdbaK1psCOhnk9Gr\r\n" +
            "T4JF/Wa00tO1zGtjLy4tyiw6D+U3jcNXht43RiAjZ5GbN43UHPNNWlSHWGD4t2yvh4ehnyR5BE1x\r\n" +
            "A7Qmx9Lqlx7AqrCJfxRngJsyVo0Pgeh8FMkp6qc9pKXH/wCSV1h7n+PurzDccijp/k8VeKqB4y3y\r\n" +
            "EgDxJ3H3VuH8VUnZTlfJ3FUYHmgtZj3CxjiNbg5+YpCM2QHM5o8Oo+6ytle40VxdiUEdkCEhhc0E\r\n" +
            "fVBABckEdkEAEUSUAggBNkaNBACUEZQ5IEEhbRGEEDCQRoFAgkQSrIrIGEgUaCAFIbbhBHbVSJBI\r\n" +
            "0EOSBA8UFLpcOqai2SIhv6naBW9JgMTLOrJbjezTYe6qnnhDtlsMM5dIzzGOe4BjS4nkArWkwKrn\r\n" +
            "IL29kzq7f2V5FU0dIOzoYQ94/wCWL+7kRnrpSe+2naf0953us0/Im/1Vf2XxwQj+zv8AoTDhWHUD\r\n" +
            "Q+pHaOA/9x1h7Jb8SBbko6e7Btlblb7pttLHnzuBkk/U83KdNtL2VDXLcnbLuVKoqhomqk+uURg/\r\n" +
            "liH8lHFTxRnMGDN+o6n3TpPQe6J18pJ5a2CLrSIf2av4e0jJJK6skLmvFooXNNiCDclbUYbNijHQ\r\n" +
            "PoZKptrl8URdbxIG3mNFQ4LhhpMMphC7s5sgc8flcTqb+PirGXE6qjpKhjTNHI5hGVjjZ/tuF2sU\r\n" +
            "UsaiefzzcszkjJcQ8KThva0RbUwHUXPeaPA9FlKjDYaZpfM+SbXaJuUN8ydft68l1yhgNPTRMp32\r\n" +
            "LWgHm1x5kqvxXBqHE6gXAgrvqy5rCT23VOTC1+pfi8q9SObYbjE+HyBlJC0RE3dFcuLvXr5D/RWW\r\n" +
            "LcPU2NwGrw9nyleRmdA8Zc3mOXnspmJ4PiMExp6aF1ObEkRANYR1zf7+yp204w2pEkla4VLD9NOM\r\n" +
            "x9XGw/cKmM3HRrcE1aMdVU8tJO+GojdHKw2LXDUJnkumyHDeKIxT1zBTVw0hlvcn109liMdwOswW\r\n" +
            "p7KrZ3Se5IPpd/30Vqpq0Raa0ypsglWQt0SASi5pSKyYARFGggAih6o+qFkAEgEaJCAAQQQQIBQR\r\n" +
            "lBCAJElWRIGEiRoBAhVuSdhhkmflijc4+AVxT4XTwtz1Tw7wJsFYRS93JRwEt6kZW/7qmeeukao4\r\n" +
            "P9mVtLgUjtamQRjo3UqeyHD6HQND5R17zvZPfLTSj/8AImNv0R90f6p2KGKFtomAeQWaWRy7ZfGM\r\n" +
            "Y/qhl81VOR2cYhb1fqfZJ+Ua92aoe+Z39x09lKNz4IFtzqq0660TavsQ0Nb3WgAcgEZvyFvNKFuS\r\n" +
            "K55BKxCSL7nRC1koi/NACxSEEPAXUvCqR1ZiVNANMztdL2A1UY2/8LT8CUplrqiqyktiYGDzP+ys\r\n" +
            "xx5TSIZZ8IORqGzyU+lbGGsG00YJZ6jdv3HipzGMnaPpexwv1BTjG9EhmH5HGSkead51IAuxx8W/\r\n" +
            "yLLr2ee4+xJpZoLupnZgd2O39Dz9fdQ6YdtWVLqlgaSGxtY7oNb+5+ytmVPY92tj7A8pAbxn/Ny8\r\n" +
            "inKikiqG/iNueThoR5FLn9kuJV1ZbHE2GtjNRA9wa0EXe0naxVZUcOUbmPqGRvrouTS+xj8CBqff\r\n" +
            "05q2lo6iCeGR5M8EVyLDvA2tqPdSMRxB0hZVQOAnaxkTcoADraAOA381VkxqaLcWaWJmQfguLsph\r\n" +
            "NAIMPonjSUAU4IP/ANz6XS2tof6eaHF6r5+N2mZkJtH5udYnzAW9o4aWulYydnYTk95jXWZL4Xsf\r\n" +
            "2UuiwE17TJgFJS0oa0udJLGHHQ2uHvvrfoAsTk8bOxiUc8bR5/4r4KqcJZ83RXqcOf3myN1LR/I8\r\n" +
            "Vkcq9V/0WopmyzVLq/FGlv4kbI+5bzdqLf8A6jzXOuKPhm3E7V/DMNQS91paYQuJYetgNvH9ldDy\r\n" +
            "Iz77Iz8aUdro4sRqiIV7xVw9V8N4q6gxBuWYMbJaxBsdrg7KlIVyZmaG7dELaJVkRCBBIrJQCLom\r\n" +
            "IJDxR2Qt7JDCQR2QtzTEFzQR2QAQASBGqVZFZIYlDzR2QTQjYQUsTCHFpe/9TtVLsfJE07WCVY3X\r\n" +
            "Lk23s6qVCQOuvmgdtEsNAGu3igSLHp4BRsBvXogW3tcpVjtYBAtN9SUWAkWHgi5aBOWASdybICgr\r\n" +
            "E+HkgGi9zqlAHwCPL4pCE7HWy6PwLSCHBI3kd+dxkI8Dt9guexwmaSOFg78rgwadTZdkoqZsMEcL\r\n" +
            "R3Y2ho9Atfixt8jB58qgo/Y4IWu3Gqcjjcxv6h906GOAAbr4FPsaLWdpfqtjZzEhgMa9paQCDoQU\r\n" +
            "x/T3Q60LxGOcTtYz6bt9PZWXYhxF9xzS8haNe8PulyJLRVRVAEgZUsNPKTYBx7rj/a7Y/v4IVOFw\r\n" +
            "1MrXgCOVrswcBz6kKzdFHNG5j2te0jVrhcHzCjCkmpnXpH54/wDkynT/ACu3HkbjyRYIgVwFJhlc\r\n" +
            "2ra/tnxGOmfGdDK4gNv4a6rf4JDHDBS0cTnGlZExm/0i1jbzN/VYw1UNRidDSVDXM7xlkhkAu4NG\r\n" +
            "g10NyRtdbvhPBn1cDpKqolipXvLWU7HfiyBuwzH6W/crL5MPkWjqeA1jt/ZtqSow5oNBg9CaiRoy\r\n" +
            "OadGMH9zjp6brMcU49U8GQGnwqGSuxCYl7o2/h01K3w5X8L+Oit+JcfpuDcGZTUscLK+YHsoGDSM\r\n" +
            "fqPX13K4Zic8WLYiHYveSV7i5wfIRHL59DdP45KNrslLyIKXGXTMvx7TU3GdXJV4pVUcGPyGwMU4\r\n" +
            "ka+2wNiQLdCVx3F8LqsKq3U9bEY5BseTh1BXpWPhiiAY6Oelw9ztRDHS9rLp0cb+d7hQOIeHcHqa\r\n" +
            "H5LGHT5N21lU9kb2eIbe5HkSnjy1qRLJh5flFnmohFbVaTirheowObtGSMqsPefwqmLVrh49Cs6Q\r\n" +
            "tBkehCCUUDsnYhNkEaFkhhAIkrmiQIJHZH5IITASha6NAboAK3shZGUWqEDN0zbQeqVqg25/3Sw0\r\n" +
            "nc6dAuQzr0JDRa+/mhpa1r+SXkFuqFxy+yjfsQ3Z3Ie6BaeZStTrb3Qsbb+ydgJygItAdDdKDPVG\r\n" +
            "G7kctTZFiE6nW3ujsb7pQ30BQAPgiwLng2i+a4ghzC7IWmU39h9yuqRQnTI70Oqx3w4oj8tVVZOr\r\n" +
            "3iNum4bv9yt7Txuvq3TqF0cC44zj+XLlkr6BGw3s5p05jUKUyMO6FPQMDhopLKcHUCx6hTciiiI2\r\n" +
            "nse4bfsgWltg8eo2Vg2BzQPzfYojGCbEWPQqNhxK50Ydr02ISQxzf7v3U6SDU27pPMKPOexifI8d\r\n" +
            "1oJJCknYmqIeE4NR4/W4hJWA2gyQREOs5jvqcQPVo/701GG427A6KrpGRVNbVUgaGTAfU0i4bz7w\r\n" +
            "38fZNcK0dPHgNO+ZjHSyAzyPb9TXOJJ1Gu2ltuXUpnDHzUuDz172iZtW99Q65/EDdh4O7oHTfxU3\r\n" +
            "js1YsrxqkZPEcXHEmJT1U8j5XNAbd7cjwfK+lvZU81HJHUOmB7ZhaG2tqB5c1PcyKtz1Dw4SSvdI\r\n" +
            "H3s8X21HhZIZ8xB9YM8f6mizx5jn6eysiqRiyZOUiJR10z4DHJNanuQ0OcWuY3bRw1HkrCkwHDJ4\r\n" +
            "5HTCOOaE5ZJOxE0ma+oc1xtfxCi1NNBXsLo3AEkXc3nbkQo9QKmCoD5HOiAaQJWHQnx/0Khkwqa0\r\n" +
            "Tw+VPE77RLxGnweijkgrKhlRFIMroqqoYdOrWRgrkPGfBcMDpK7hx8lRQ3JMT2Oa9g8AQCQuqYFW\r\n" +
            "Ucz+zrmCKOYm3Zu7JshOxzDvNOnVPV2BUzHSz4PSzFzCQXNa1h8sz3Fzh5X8lnXLE6fR04zh5Mbj\r\n" +
            "pnmoi3+6Ihdb4s4HdiQfVUtMKGv/ADRnM2OX1c1ve9FyyrpZqSofBUxOjmYcrmOFiFcmpbRTKLjp\r\n" +
            "kayFkotQy9E7IiLIWSiECEAJsgQjt7oWPJABIDdHYoEapgJQIPJKsiy32NkgZvGXI290qxO5sPBE\r\n" +
            "y9tksA7k+y47ezsBBoAuholBo3O6BsNiFGxDepGgPXVFrrewTl+gKLU72TsBGXrcoM0OiO29yUpo\r\n" +
            "AJsN/VFgE2ztAb2F0YJAJLTYapYHTkpuC0vz2J0kNrtfK3N5DU/shbdEW62dN4WonUODUkDmd4MD\r\n" +
            "nW6nUrVUbA5o199FDo2tJABA8FrMCww1cjW5Mw8QupOShE4aXyTb+yJFRXb3gn20jgAGn0Oq1tRg\r\n" +
            "nYQhzbtty5KJS0Er5D3Q5vhos8cvJWXPE1opOyIHeZbxGqLsWvZcWIKv56UMIDgWk9Qjkws9kHFv\r\n" +
            "qh5BfGzLTUpB7twByVPisTpWQ0ZFvmpWwki5s0nvba7ArcuwyTI5w1H9wVDSRl/FcYIc1tNEXXAu\r\n" +
            "A5/dGvLTN466K7DNTlSIOFNWDiNubCJGQ3fJNaniMZs4Fxy8vXTwt1Kh8Rvkw7AnQ0+Qx5WwsGxa\r\n" +
            "DoLdef79FaYjDFXcQUVPe4pw+okcw2AsMrb28z5WsOaz/HMrzXwUweHsZeQ3FiOQvyvv5aDktjWx\r\n" +
            "T/FNma7uUMH5RZOgFsRO4Tdg5wBGvipNSwxRNaDuL2KZi7K807ZZ87CY5f1N39Rz9UdVM6MFlUwG\r\n" +
            "O1i9ou31G4+4UmlaHOLnC1tdU24uMumo+6dbI3ohTYRE6jMlO1hJsWAnQeSj4LjNbgt6d/aXka7M\r\n" +
            "3MQ5g/tPTYWVvPAYw0U7uycdS23dJ8R/IR14hmp4qesiDHEaE7XPR38KEly0ycW4vlEhVdLUYtHF\r\n" +
            "UtmDRL/hnIG362NszbAjXNZUeM8EnGoI48Ra9s7u7BWMYXOB5B9r3HjcrdcMUVQ+UUkfa1McVmwx\r\n" +
            "NbctcbEm3iA0X2sNVuuIMdPD9GY8brIqrEpBmhw5uVsUXQyFo+3NYnGam+J2sWWM8aeQ8e8a8C4z\r\n" +
            "wh8ucYpzEyoc5sTi0jNltci/LUarKEWXd+LcTqsaqKhnEbXVjJXEua+wLD1YRsuV8ScNS4Y01VI5\r\n" +
            "1ThzjpLls5h6PHI+OxV0ZqRVNJPXRmiNUVk4Qit4KVEBCFkq1keyYCECNUq1kRQIIBFr0S7IrXSH\r\n" +
            "ZvGh1xpbzSspO5t5IxmI2CFidz7Li2dkIMHmjIA2tdKyoFoCTYqG7jl+yF73zadErQ31ui+rcEef\r\n" +
            "NMBIB1ufZHHdpOt76apQBtrppogwZSb6308kWAYbr3dD4LZ/DnDXS1s1e9vcjHZMJ/UdT9re6xzW\r\n" +
            "d7c+67HwvRNpeGcJiI/EkiNTJfq86D/pDFZg3lijN5MuOJmiw+APLbgHzXUuDsMZBTduWkOIsNVz\r\n" +
            "vBYLyN1Puuv4Ozs8Ohbe+i6LjznTOdhVKw8SZmpnZQCm8NpRHCHPZZ5T9U/vMjsTc62UhNY4ym2X\r\n" +
            "Xogz0jH1LdBYa2Ts1LG+PKBbyUiwvfmjR8Ed2HJkCppGMpHAWNm81ieH42iOsq3xSMdPM5wkOg7N\r\n" +
            "vdBB5DfXfXRbHiaqNFgdXM0XfkLWDq46D7rM00/9IwKzmkwU8RvpcFzRqQeZvz2HJWYMUYTbiJu9\r\n" +
            "sg0dDHV4titaJHlokEDHRk3yxjW3Le++1rrneNvdLjFW8ydqGu7Np8G729brpdVLFhnCkTomNt2O\r\n" +
            "eQg95zyMzrnpcnzXKO/l7xu46m/VaWZPJaUUvsVSx9rO1pB1KXigHblrDoNPBWnDlPmnlme05IWF\r\n" +
            "xIVROe2qHOG5N9FBO5f0ZWqiOwMMdBJI4aHQJighM9UwMOhKtMWidS0FJCR3nNzkeaPA4Gspa2se\r\n" +
            "MoiZlB/uOyXL8Ww4/kkV8je2rslvzWBCFTZ9Q4OAdHsb9FLwiFz5Kic6iKMuv47D7pnDqOSvr4aW\r\n" +
            "K4knkEY9Tqfa59EXuhVrXsv+HoZMBoTWUM57Z9M6ofHJtC4kBoHPUACxuNLrIYrQNxaWSSaR7K54\r\n" +
            "vI5+peed+vmF0PiWN4p3drCB2k3ZRPbt2cYtbrus1NTCRtpGh7b3HVV0os05Jy4qD9GGrIhRwCHE\r\n" +
            "Y5HAC4cdQSTsw+A5FQoMDxGee2DxfMskbrE2P6221Dm+XLVbipp3PjLHATw82uHe/wB1DwalNBib\r\n" +
            "qihle9rmlhhcTcE7W9TsVCcFJWuwwZXCSi+jk/Fvw2xGCCTEcNw6eNjQXTUtsxZ1LOo8DqFzhzC0\r\n" +
            "2cCD0K9uwzxYfh1M7H5H08srgIC6oLZpzfYNFrDxcbea4/8AF7hlnElWKvDqOkw+qiaWMhgdmjlF\r\n" +
            "ydXAaO8b28lGMn/6OlLGpK4Hn8jVAhSaqmmpah8FRG6KZhyuY8WIKaIvurCgashbknC26INsgQiy\r\n" +
            "K3mnLIiNUmJm8bm5AepR5STv7BG2/QD1SgHdQFxGztsTl8/dDKOQ1SsuupKGUC1hqhsQg26hELE6\r\n" +
            "X0CXltyCIWPMHyQgEgE/ltppdBuhOcDbS3VKHeva9gL7I263zAjS48SgQujiM1VDFcEyPDNdtTb+\r\n" +
            "V3YRt7ciJxEbLMYOQa0WH2AXH+DqQ1eP0jbaxu7U9LNH+tl2OmDhY5QT5rb4kNuf/Dn+dPSh/wBL\r\n" +
            "7Bw9rmkFp156LouH4l2dK1rmHTobrnOGyFrm9x37rSQVjQ2xJHmCFfJ0zLB0jSQVokqg52YeYVsa\r\n" +
            "iIMzZ2keBWNpsUphOIvmYBKdQztGhxA8L3UXivjPAuGWQjHq1tO6a/Zs7Nz3OA3IDQSpRk4lidm6\r\n" +
            "hna8EmwTRqR2x73dtayzlDiNNXUkNXQziWnmYHxyMd3XNOxCOSd7RpISSeYuhyfQckK4jqRW4lh1\r\n" +
            "AXM7MyGZ9+jBcac9cqhY+xkrqPD3PLWzzNDmgfkb3nFx9LWHVRcLfJU41X1V7tga2nbbT+53eO35\r\n" +
            "fZCGqE3EMsjmudFSQhjcouA95uco5mwGviteFfjv2NbX9lf8R5qVlBFTRg9rK4Zid3De56DQWC56\r\n" +
            "1t3jUq840xBtXi5aIzG2Iah25J6+gCrMKp/m6qOONwzucAATurHq2YM75TpGmpYHUHBlTUOaM9VK\r\n" +
            "Im8jYalZvCaQ1+KU9O0G8kgbt4rd/EZkeHYThuHROGdrS4j+VX/DbD3GrqcQe3u0kbng/wB1jZUR\r\n" +
            "bSb+yUoXNQKDiuRsuO1DISDFGezbzFhorHEYv6fwXQwlv4tdI6d1v0N0H8qpgpziWNNjjBLpZLX6\r\n" +
            "klXvHrs2OxUUB/CpYmQNHkNfupekvor/ANpFdDCKXhV0v0vq5sjfFrRc/chTuBae2JTVskWeKjgd\r\n" +
            "IbC/ecLN0/6kvi2I0sOGYcB/6enBcB+t/eP8K64MgZTYKySRxaaqpzkHnHH5+I+6cFfZOMfzS+iL\r\n" +
            "jELnVkNOxzi2nha3K43s46lVVRTZAdC0+C3FFhT6qJ1ZKxo7cumsOQ5fZUVXSntbAnfYrNLJcmWT\r\n" +
            "g+2ZiWAhhNr+IRU+HCdrXZX5mgyZ2aObrYeevLpdX+I0Do4mANIdJ9Nud9AnX0YippnMAJzdkwtf\r\n" +
            "YjKMoJ8CST6KUZWinju0YTidkeMVLnV7p5QyPuTE3e3oD4eCLBJ6zC5DQVzzWYa1pcwtDXbbt71w\r\n" +
            "N91OfBme6UasdI53+SMf6pugp3NkmmDnMyWzENzA8zcHdSnFNUSx5pRychrjLgPhfHsLGJYrVTUD\r\n" +
            "WtsyoIvJf9AGVoePLbqvNGOYVLhda+GVpMZJ7OQbSNvoQV3/AImirMYxb551U/tmgNLJB+G5vJob\r\n" +
            "s3TkEHcKYRW4dE3HO0hjqXOdDE5zQZbc4/zA/wBx081XfBV2dBZI53rs82lvghZari3hepwTEJsk\r\n" +
            "bn0eY5HjvZRyDrbHx5rNuap2QlFxdMYy2Ou6LZPFvRFlQ2I27M3Jo90YDuoCNl7bD3SrOvyXDb2d\r\n" +
            "picp3LiECwAbn3S8ptv9kMvUkqNiGsg5BC1tvVLyjpdDIANAE0xCbjkdtUkEE6mw32S9BsbIAAuA\r\n" +
            "uNfFMRtvhjS3qayqLSbNbG3Ta+p/hdKhNjqHD0WJ4Ii+TwOnc4hr5rzHXqdPsFr6etYR3nDzuuvg\r\n" +
            "hxxo4nk5FLK/4MVxlx7VULsTw7D4y2WnZeSZrSySMHm0HR1uZ8dlncF+JeIUPC1fBW1r6mt7aOen\r\n" +
            "fUVAc9kZBJIOt7EDunquj1HCnDWJ19TW4jQx1NRUgB7pHuI02sL2Houa0FZU0HFsmHNwvBu2w8te\r\n" +
            "yBlG13zOSwzPcdSC0kkaD2RVPZZjcZLikR+EaqoxfjyixmtljqnwXlmkhY6V7nAaNygX10G1rE7r\r\n" +
            "U4xwRiWOU+GzYSa9tU/K2vdVMc1offvOZmNwACe7YbaKN8QsfxoNqK2khNBFVOhjIwyLKQ5jT3cz\r\n" +
            "Rpcku032uuwfCTFKMcIUrOI7NnDDI+aocTK7MbjMeZt/Cbxqel6H8lfl9ltw5hwwnCKTDo5nvbTx\r\n" +
            "hhkIAznmbcrnkptU9zXOJfZrWlxuOgVHxRxTQwYjA3h9zJI2sJlLmktJvpa6i0XEclT2vz8Mbad7\r\n" +
            "Swllw43FtFCUZKVsioNqzacPUslJwrFJMGCaoJqHl+gBcb7eAt7KkweWSKgqcQkGV88jpBK5wvlO\r\n" +
            "jco8rKJj/GsVVQfKUUboC1mTMXX0tbRZ/GqyWiwVz2kNGWKLM55c67481gOVhv5rZHLFaJOX0Uld\r\n" +
            "Vmqqp6l2a8ry/vam2wv6AKtmrYID35WMd4usVX4vjUcGGTSUwPaA9kzMNMxH+guufuqJe0L3y3cd\r\n" +
            "zrcqTypfyc34pZG5PR0+r4kFa2EVOIGXsW9mxz33Ibfa+59VeYLxpPR4PW0cckMtJO3IZNnMcdu9\r\n" +
            "/BXI4Khhj78s4f4DRL7QuaXB85aBcnLeyd2ukTUWpWpOzsPCPENNhWJx1lTTmqay5aInDNe3jorL\r\n" +
            "D8Ro8Q4nbW4o8wRPm7RxLbgC97Gy4nRyRO0NVMx/IBnNW1K7EGvApKpxPQnKUSj2wgnSinZ1riiq\r\n" +
            "+fx+rmhcHAvyx2NweQ16HRb003ytAyjBa5sETKVh55nWzH9lyTgafFBjWHnGKOR1E2ZuacNFmn8t\r\n" +
            "z0vZdSrZGZqN4a4PkzVDvXQfb9lBTSi2jRCLjcpGwYyPsTFHoxoyC3QKhGGGWsN7Fu5uOSfwmtYI\r\n" +
            "3B7zmNhryUitxCKKCR8b2l7nZW68gsslyLbTVsiY1QtNTTysb/h3cAP7RoPchUmNsNJhkpuxwpor\r\n" +
            "bWJfb/8Apyg43xK9/ENLBTTuiihaDLlcLZR3je+m4Cz2Ocag1L6V5p54GSMeS52Uvc03IJG4JWiG\r\n" +
            "PVpFE82O2PzUIhidE4aRNjpiT4DtZT+wTclAWYTBG5tpaghxJG19Tr5E+yj0/EtFWROZV54y4PLn\r\n" +
            "sOcOdI8F5tuO6MoV9ilXS1U84glaDkysYQWu71m3sfAuPqEpxaZCPCXTM9Ngr48DZWubYVLiQDyb\r\n" +
            "ew/n2WHxM55RaJhEdhGLbW2K7FxfHI/D4WMLY4SwCJh00AAA87G/usDJgM01dDDGwuc659gLn7hV\r\n" +
            "RdJtlji06ic4+JvF9TRxx4DhT2tpBCGzvLu0dMDyJsBl8APO65KWm60HFExrcfrpRqztXMb5NNh+\r\n" +
            "yqTFopuVmlt+yIWeCQWkHb7Kd2OiQYtdkhPRrWAn8vulWPgltBA+k3SspN7NHuuC39HZGrHqLeSL\r\n" +
            "KdO99k8GnwsiLDzI9krAZy+JRBgvsnsh5n7IFniSixDGQDYD2RiMvc2OPR8hDB5k2ThZpzU7AoBL\r\n" +
            "i8DTq1l5T6aD7lW4485qJXkmoRcn6OhUgbDDHEwWaxoaPQKV2gtbSyq2EW2N/NLzAkbjnuvScK0j\r\n" +
            "yznbtlvFK0WuB7JbqWmlqoKx0UQmY9sfa5RnAJAsDvbVVDpWxsL3EgAdVU8F4zNilPXVM07zEytY\r\n" +
            "I230azO3/wAqqcDR475TRsvl2xYNVvE84kdL2TDnO50/780ukkp6EvY6Sd92EtOe1iOdgFCe0ujc\r\n" +
            "wyy5WvLgMxte+6WYWvcLvk2I3R8dF1ESplilxGeSIERl1mguudNN/crQ4TTTVMEb46Bta0uLHNc9\r\n" +
            "zMpG2rT4qtpcJpXuAL5BY20P+y3fCmHSU7ZWQxVFTTxuBLRKGkF3PUa7bKE4/RpxdUygl4KxesdK\r\n" +
            "YKKmpgW5i1879G8tx1B9lQ/FB78PjoqSSaN5y9s4Rtt+VrW63N9Gj3XRqt2PQvLWVNT2c92ta5oL\r\n" +
            "so0/ToFg+I8Djr5H1GJukkfYsDr6AM00t0sovHVNtJEZvTUVs5DjNWHU1JCNAS6Y663Ogv7fdUpc\r\n" +
            "HPAGi3XF/BlY2Ohq8Hp3VUUt49JWaAbaXuNiqGbgriSOgqK6TD2sp4L5nGUX0AOg56EJuFOilXS0\r\n" +
            "Qo2uDGXBIWz4JoG4hgPGLnD/ANNhrHNO+U9qHH7NVO34dcXSPYwUkbSYRO0fMjVnUK+4e4P+ImBY\r\n" +
            "biDaCkgggr3CnqGzPbd24G+w726tm5NVHsWKFSuRj4/w6hjm2J0OvOxWioKBmLVOaV7os3JuoHun\r\n" +
            "sM+GvGGJYUcQhp6AQNzEAzd5wadS0DcePNXGF8D8VUFXTw1hw6lM8T5oi8udnayxdo3Y2cCOqlOT\r\n" +
            "eokMeOttFtw5wxJS8QUVPSYjUSMeS8tALQbXI0v16rfS4m9+J1XzXdhp2BoygaNbpbU8/wCQq3he\r\n" +
            "nrsBqJayeSkqZnQhkfZNcMrL3J15mw62TkYZi08lM2eaMTnshFKchcL3/cb81RwdWzTyh1Vp/wA9\r\n" +
            "GkFVSuwynqKeWUySH8VhAcWgfUbDxt7rJ8VcSjD+1Y0dqY2ixJFsxG3ur5rK3BQ2kiMYbTRE5S9w\r\n" +
            "zBzsx1532WY4u4axjGq6ljjhpW1FQ0yCFstrN1Od1xop/Gq6KMke+K9nMXyyOcZqmSSWV5LnOvum\r\n" +
            "n4hLmLYJJWN6F+b+Fa1HC2Lwns5hTtLQ4O/EvYggam3UhJi4Lxt0cskbKd5Y7LZrnPDj5geSjGl+\r\n" +
            "wOLf6la0VMjmlhne42PdgB+4WgwT+qyVZy1NY2VpA/Eo3P0211uFIwnBsdhljlhhp4hmy5QZCQWi\r\n" +
            "7r6aWsT6LV0FXxLSVmH1rKCjbLlLGtzvHa21LjfwP2UXNsshjT77KrEeLsRpiJ8S+UqJIHZGxlr4\r\n" +
            "y4bXAIGniolbx02owjE5aSkmjxJ1I6GnEYLg0uN3Ov7n0CsONKjHeN2Mp6mkp6YUb3lzWZnO00Lj\r\n" +
            "zsFVcP4LiVPhxr6PCo6xkUjT8zE93cLTd2osb8vBDa6RZGFS10cbwSkpRjFL/VIXy0YkAmYHFpLe\r\n" +
            "eoVpgvCM2MU1bJRRySSU7O17Njb9z8xJ5W091usS+HmMQ1cZqKNlOamQiMyytAJOtrk6nVa/hn4V\r\n" +
            "VdRgdPXU+ImGSpYc0bRYFt/1A8+lkpRatLZbFLlclo4BUYRkia5rsz7kOYGnujkb876+ygnD33/w\r\n" +
            "3f8ASV6Qd8ISKh0c+K07Mtt39fBOP+D2GQvLK3HoIJh+RxF7cjqQnGL+iEk29I4DWVkFC1hq3GIP\r\n" +
            "vlzNOtkI6tkkrY445i517XjIGm4uU5x5j1Xh1FQUlLBFTEgioZJFnMtiHAuDwcuvJttEzgXFGKY/\r\n" +
            "LVvxavzxmzxTjKyIO1ALY22AsBbQLF5Ph48UZSTujbgzyyyS6slPbM0HLTyOPgFGdO/tMjoHtObL\r\n" +
            "qee9lKqImVVK2KZ2YAAudGMpLv4CpMXdV0McUeFNe6nYXOlcw5nA9Dz6+yxYFGckpGnKnFNosHPq\r\n" +
            "C4MhgbLMSB2LZB2n/TukY6zHcCjilxnh+uoYJZDCySoYWAvGpA8Vn34fjklTnhZWE3BaXSEHKQCD\r\n" +
            "clRsVwvHI6I1NYZzCxxcWvmzZTtmtf7rqrxMDVxRzv8AIyLUmT6fieE1EQqoXtgzDtDGQXBvPKDp\r\n" +
            "fzVi/iGTCmzVVI1t5HiKNswuQ0am9ueq5615uNduasqt8r6Wjje4uOV0huf1H/ZTh40YSUororyZ\r\n" +
            "nKDjL2ax3H+LiNhZFS3dc/4Z/wBfNHJx5jLS0NjpbloJ/COh91j3FwqBG3YWalCQy1Qttmv7bLU5\r\n" +
            "MxLHH6NJinHGMSskgDoMru6S2PXx5rX/AAunAwDEgToyRht5Af6LkFY+SGbK152PeGmZaXhLG6ql\r\n" +
            "qqehiOWnqZQ6U83WaRbyVc5WrZfixqMklrZ6EkeAZQCdHHl4pWfUfUstwvjcmKyYzFUOPbUtZIzQ\r\n" +
            "Adwk5f2VliVeKKOIvzd9+QW5XWiVLZUr6NJSSWcdTvddD4QrmQwVMwq2wsfIxhLhfUZgfLULkE9X\r\n" +
            "2NHJLd2gvofBaSvpIcZwCGelqTTyxwuDmstZ5f187HXxWbK3CWkacK5LZreIayqrsWoWUs3bTxvI\r\n" +
            "ie1gHeG+pHn7LlHGXFNHR4kYKmpIqZXF7iI+RO9x6rQUGF1NLwxS01RNJHJHLKexc+zmMuLHU7HM\r\n" +
            "uLcf4VK7Ge0p3id/ZxNyhpc4jUON9tLfdQvmlfsjOKi3s6tW45wpSw0lVRYwYqCRti8SNe9jvzWY\r\n" +
            "NRqem3kuhcA/EbB8Rw+WiqWlpbUinc6QHLIHkNaTmAJuLHbYheUX4G6CtrH2l+WbLpZt3OBNt/W6\r\n" +
            "3/C9VS4fUQ/O01XW07X5+0D7guYYnMB6AZXi3iOiMSUNJ6/knyT9HfaCnpMJ4qh4bxCLNCKeolop\r\n" +
            "XgEGAZCGEn8zO8PIAp3H62CuwnG6Gkw/sqbD2F9XIRYSRBoeGMP6nj2G+65h8QviZV4riuG1GH4b\r\n" +
            "DQvw975YZqiZrnOD2FpBaNLEeJWcwf4lYzDglTSmegLKyLs5jI0ZiMpbfU72JWhZorbYV/B03hLj\r\n" +
            "/wCV4BwDEamjLmYniQwwlpBLWuzAOIGgsbCw5Ba/hypGPcTtqAxpgwyCWjZobuc7sjmIO2xHovNO\r\n" +
            "H8X1mE4OzDsLqsKpII6xlXGcpkdG9otcXJ6k+ZUKPivEziFVM/iepbM8tvIJ3N7QBtgNLbWVKyRS\r\n" +
            "7J5Em3xR1+f4nUeM4jieGnBaqDFaX5hlKcoLH9nGXAONxvldfoAOqy3DfGoraKrxmow51JVU0Mc9\r\n" +
            "PC1xzSAuIzi41A0NvBcvp8ShpcTFUMVeJWSSPDgHEkvADtepA1TtNjlPFPCHVkk7WwmHMY9QLgga\r\n" +
            "8rAolnsq4K74o9IcKcWu4josJqMbdT3nJkmeR2bmta9zQC0Hc7+RQx/immwziLEpqWeEwudTRsnI\r\n" +
            "LmtjJGZu+xvr4Arz3DxZR092xy1ZZyDAGKDifEtHURNbFFVOc0tIMsugsb7KHz/SLFe26OyT8U9r\r\n" +
            "xPhZmiphCyiMM7W2DX95rrgH81rBarjHj+ililp8Bqm0zAwGKQZGWkuDpfUCwIK80s4mpo75MMh/\r\n" +
            "zSEopeLWn6MPo2/9R/lReab9BGCXs7A3jZ8keCzx1FOxlFTuY9jpTeR7iLkgb7ey1fFfxRpq+hpW\r\n" +
            "UXZRTROzF9ybOtbu2Gy82UnFM1NGGRU9Na5N3NJOpvZOu40ruzNo6QDqI1B5MpNV2dIl+J9bgEna\r\n" +
            "wzteDTSQNzRaC5Bt481AwX4xV2G8G1WGwsaySSZ8omiOozDVtunO65XjuOVmLwNinyHKS5oay2uy\r\n" +
            "o2ySMZJG67SOR0WjB+SqfZTknKNOPR1Liz4kYvjbaJldVySNYA4Z9s3Wy3Hw7+KlbhlI2mhvUShh\r\n" +
            "zMtdu+/mvOklTJJFF2hLg0ZQrHAsRnhxCN0M7obb5dLqEk612CyO7fR2nHeOKk1L56h0rHOJcS59\r\n" +
            "lznFOJ66ur5agzzyBx0c8lxI81WY9jWI1FM9tXNeNxtkB0WdbK625RDG3tjlmbemdd+PH9Ar8WoZ\r\n" +
            "eHMXmxEOa4zma5MbgGgd52pvYlc/wedmG9tI/sZPwi1rS7d2425KxOCmU3fmPi4lD/h9hJ+n0CMs\r\n" +
            "fkTUvZdjbxu0S8M4hwyWlHzjBHUDR4YDl8xqiqcdwlvbupn2la27WkGzzbTVQ/6EzmNPJD+hRgXt\r\n" +
            "9lk/wcd3s1f5uWqKeo4kq5S1kkdOcg3a0m591M/4wxCTDpaPsqJkcoIf2dMA8i1vqN7bp52ExNv3\r\n" +
            "mg+SScMA/N7BbYVjXGKMWRzyO5MyscMmgIHTdX8lJFJUiQ1kDYwGgDUmwCkGgaDo77bps0oGoe6y\r\n" +
            "kshTLHYy2gp2vLzWhztfpjO6OlpKaCVr+3lcR/8AHb90owtbu8+rkA2Nv5v/ALJOd6oSx17IuL0k\r\n" +
            "dVI+opmyNc9wtG4DQdVKoKE09fQSNeHuEouG8ggHMGzgpFJK35iK1h3xz8VXJtRosirlbNXHjFVh\r\n" +
            "VbjDIdO3qe0ADRexaNblNVfElXVxiOenc9oIcM0jRY+iqOIZgzGJ9rkMO/VjVXfNW0JHupTnK2r0\r\n" +
            "QilSZqH8SVrgAYLjoZzZNHiCuyFraeIA6EGRxFvdZ35to/MPdD54dW+6rfJklSNBNj9e5oPYwCYm\r\n" +
            "7pLFxPTdMDHcUY4lr4diNY9Bfn5qmNaNszUn5xtz3mqPF+x2Tpq/FJQO0rHkHlfRR5X1kjiZJy4k\r\n" +
            "3JJ3Uf5xnNwv5IjVxjd/2T4jUhwsqDYmXlbVNFkuwkLii+div9R9kTqyLldOhJsBhkPN102ad1+8\r\n" +
            "SEHVYI0CafVf3D3QK2LMIBsSUBEP7j6pg1JJ0uQk9q47usmBKyi2zhbxQHZDftCegKi3afqc53mj\r\n" +
            "BaOZQDJIMX/LP+Z6B7HS7fZyYuNb390XdvqPul0NEgSQNHdib6klH20R/wDbjHoSolm3vYepQzN6\r\n" +
            "CyYqZKbKzOLNZe/RKeyOaue+UDRp0UMPYHA2GienkEdaTclpGhvuoSu9FkV+LTIMrmskYw3s190q\r\n" +
            "meIZS65GumiOpf8Ajtc1lxzR9od7e6tcrVlNU6F1tR20WTW11V5iDYahWJkJBsB6lVs3dkcABbwU\r\n" +
            "oSE4nQH4uy2h9LXUaXFyLBlreSpo7W/NZOtLeWnopNl/Jk2TE3OO0mu+UJh1c4jvGX1CaJA1c5qV\r\n" +
            "mAtqCPNKwsP5zXQO9kXzbjsXeyS6QEfkHhdIdIy2zfQpEbYb6qS+mYjxTZmed2k80l0jfBJzx/md\r\n" +
            "b1SQCu1kto0W8QiMslvpF/JJMkQH1EnzRhzD+U28ShitADn87X8kBI5rmnTQ33QJib+lFmbuAPZA\r\n" +
            "IkYnXOrKvtzHlJa1tgb7C38KFn/tF/NO3vsERA6nyARdiaGXPt+VJL/D7KSWtFvrPsjBaNmG3iQi\r\n" +
            "yJF7V3JpQ7R++UqSXtbew28URlv9LD7pJjojZ5Cbi6Mvk/Nf1TpD3EnbwukmLr90ANOkedERMl7a\r\n" +
            "DzThjtsdfBGI+rihCGi2Q7uHohktzTwjCBaPE+qBjfe/UisbfWnMoO+nqiLGg6lACRcD6yknfdxT\r\n" +
            "ncHK/qiPZ7WCQwtL63R5wP8AyiuwaWCVnaNmlAJhZ7nkiDx11RmQG3cRZ9u4hIGwi7nf7IswtuEd\r\n" +
            "zzYEnKNiAEUFh5hz/dJ0vr+6FgP0pJNje7UBsVZvI39VDqdJTZSM56hIc4X1sfNNa2HfZMjlu12Y\r\n" +
            "d47HohmPhbzUYPNr3QzlWobZJLrkf+UDIBbVRc56lGX6XOqQiRnbrqgJGab3Uftdd7ohJ01SHokF\r\n" +
            "7DfMCSk/hkaNN002Q8kRkKLYtDokaNC0+iXduuhUcyHS5Qz+aG7BDxc3Sw1QMlhzTV0L6aBJDHu1\r\n" +
            "IOhNkBKeTimQfBANJGyVAPds62ryk9oD+c6pvLrqAhdrRyTEOCS3O6HzDgLZjZI1d0A80eVl9Tcp\r\n" +
            "AAzvOzih2j9y4kI+4PJJuNeiAFdo+31EIs7j+You7bf2RXb4oAVmNtz7oZj1SMzeQKGYBwOU2SAV\r\n" +
            "d1/90WZw/wDKInX6UR3+ke6dWIMuNuSBcf8AsIha+jQjt5IAAcUeY+KGvUIEf3fZAAzHXUpOp1Sg\r\n" +
            "LjcosovugQC0nkiLTpeyXkHW480A1t0DEFvUpJHinMotyQt0shCbGyByKK7eqUTzSSRdCQWJOwRn\r\n" +
            "+EEFaTDbsgeSCCgACnI9iggpMigH6h5Im7FBBIb6DdunOnmggoggyknf0QQQg9CBuEHblBBN9hEb\r\n" +
            "O/og36UEE0HoX+VqLl6oIKKEGfoCS7dBBDGLbulDYoIIF7C6oH6UEEgEcylfp8kEE0AHIgggkC7C\r\n" +
            "5oxyQQUvYAGxR9fJBBIXoI7eiBQQS9AEd0YQQSQMS7dEUEFNdjR//9k=\r\n" +
            "------=_Part_3944_4636740.1162905206437--\r\n";
}
