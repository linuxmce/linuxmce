/*
 * iso639.h: iso-639-1 <-> iso-639-2 language code translations
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: iso639.h,v 1.6 2011-03-19 17:03:56 phintuka Exp $
 *
 */

#ifndef __ISO_639_H
#define __ISO_639_H

static const struct {
  const char iso639_2[4];
  const char iso639_1[4];
} ISO639_map[] =
{
  {"???", "??"},
  {"abk", "ab"},
  {"aar", "aa"},
  {"afr", "af"},
  {"alb", "sq"},
  {"amh", "am"},
  {"ara", "ar"},
  {"arm", "hy"},
  {"ast", "as"},
  {"aym", "ay"},
  {"aze", "az"},
  {"bak", "ba"},
  {"baq", "eu"},
  {"bel", "be"},
  {"ben", "bn"},
  {"bih", "bh"},
  {"bis", "bi"},
  {"bre", "br"},
  {"bul", "bg"},
  {"bur", "my"},
  {"cat", "ca"},
  {"chi", "zh"},
  {"cos", "co"},
  {"scr", "hr"},
  {"cze", "cs"},
  {"dan", "da"},
  {"dut", "nl"},
  {"dzo", "dz"},
  {"eng", "en"},
  {"epo", "eo"},
  {"est", "et"},
  {"fao", "fo"},
  {"fij", "fj"},
  {"fin", "fi"},
  {"fre", "fr"},
  {"fry", "fy"},
  {"glg", "gl"},
  {"geo", "ka"},
  {"ger", "de"},
  {"gre", "el"},
  {"grn", "gn"},
  {"guj", "gu"},
  {"hau", "ha"},
  {"heb", "he"},
  {"heb", "iw"},
  {"hin", "hi"},
  {"hun", "hu"},
  {"ice", "is"},
  {"ind", "id"},
  {"ina", "ia"},
  {"iku", "iu"},
  {"ipk", "ik"},
  {"gle", "ga"},
  {"ita", "it"},
  {"jpn", "ja"},
  {"jav", "jv"},
  {"kal", "kl"},
  {"kan", "kn"},
  {"kas", "ks"},
  {"kaz", "kk"},
  {"khm", "km"},
  {"kin", "rw"},
  {"kir", "ky"},
  {"kor", "ko"},
  {"kur", "ku"},
  {"lao", "lo"},
  {"lat", "la"},
  {"lav", "lv"},
  {"lin", "ln"},
  {"lit", "lt"},
  {"mac", "mk"},
  {"mlg", "mg"},
  {"may", "ms"},
  {"mlt", "ml"},
  {"mao", "mi"},
  {"mar", "mr"},
  {"mol", "mo"},
  {"mon", "mn"},
  {"nau", "na"},
  {"nep", "ne"},
  {"nor", "no"},
  {"oci", "oc"},
  {"ori", "or"},
  {"orm", "om"},
  {"per", "fa"},
  {"pol", "pl"},
  {"por", "pt"},
  {"pus", "ps"},
  {"que", "qu"},
  {"roh", "rm"},
  {"rum", "ro"},
  {"run", "rn"},
  {"rus", "ru"},
  {"smo", "sm"},
  {"sag", "sg"},
  {"san", "sa"},
  {"srp", "sr"},
  {"scr", "sh"},
  {"sna", "sn"},
  {"snd", "sd"},
  {"sin", "si"},
  {"slo", "sk"},
  {"slv", "sl"},
  {"som", "so"},
  {"sot", "st"},
  {"spa", "es"},
  {"sun", "su"},
  {"swa", "sw"},
  {"ssw", "ss"},
  {"swe", "sv"},
  {"tgl", "tl"},
  {"tgk", "tg"},
  {"tam", "ta"},
  {"tat", "tt"},
  {"tel", "te"},
  {"tha", "th"},
  {"tib", "bo"},
  {"tir", "ti"},
  {"tog", "to"},
  {"tso", "ts"},
  {"tsn", "tn"},
  {"tur", "tr"},
  {"tuk", "tk"},
  {"twi", "tw"},
  {"uig", "ug"},
  {"ukr", "uk"},
  {"urd", "ur"},
  {"uzb", "uz"},
  {"vie", "vi"},
  {"vol", "vo"},
  {"wel", "cy"},
  {"wol", "wo"},
  {"xho", "xh"},
  {"yid", "yi"},
  {"yor", "yo"},
  {"zha", "za"},
  {"zul", "zu"},
};

static const char *iso639_1_to_iso639_2(const char *lang)
{
  if (lang && lang[0]) {
    if (lang[1] && !lang[2]) {
      unsigned int i;
      for (i = 0 ; i < sizeof(ISO639_map) / sizeof(ISO639_map[0]); i++)
	if (!memcmp(ISO639_map[i].iso639_1, lang, 2))
	  return ISO639_map[i].iso639_2;
      LOGMSG("Unknown iso639-1 code: %s", lang);
    }
    return lang;
  }
  return NULL;
}

static const char *iso639_2_to_iso639_1(const char *lang)
{
  if (lang && lang[0]) {
    if (lang[1] && lang[2] && !lang[3]) {
      unsigned int i;
      for (i = 0 ; i < sizeof(ISO639_map) / sizeof(ISO639_map[0]); i++)
	if (!memcmp(ISO639_map[i].iso639_2, lang, 3))
	  return ISO639_map[i].iso639_1;
      LOGMSG("Unknown iso639-2 code: %s", lang);
    }
    return lang;
  }
  return NULL;
}

#endif
