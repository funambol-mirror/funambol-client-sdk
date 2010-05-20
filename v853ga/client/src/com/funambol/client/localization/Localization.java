package com.funambol.client.localization;

public interface Localization {

    /**
     * Return a localized string for the given key. The method shall never
     * return null. If the key is not found, the key itself shall be returned.
     * @param key the string value key
     */
    public String getLanguage(String key);

    /**
     * Return a date in the given locale format.
     * @param date is a UTC date
     */
    public String getDate(long date);

    /**
     * Return a time in the given locale format.
     * @param date is a UTC date
     */
    public String getTime(long date);
}
