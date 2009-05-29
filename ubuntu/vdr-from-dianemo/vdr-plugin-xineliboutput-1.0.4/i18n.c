/*
 * i18n.c: Internationalization
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: i18n.c,v 1.36.2.2 2008/10/17 20:19:50 phintuka Exp $
 *
 * Translations provided by:
 *
 * Finnish   Petri Hintukainen, Rolf Ahrenberg
 * Russian   Vladimir Monchenko
 * German    Udo Richter
 * Italian   Diego Pierotto
 *
 */

#include <vdr/config.h>
#include "i18n.h"

#if VDRVERSNUM < 10507

const tI18nPhrase Phrases[] = {
  { "X11/xine-lib output plugin", // English
    "X11/xine-lib Ausgabe-Plugin", // Deutsch
    "", // Slovenski
    "Plugin uscita X11/xine-lib", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "X11/xine-lib n‰yttˆlaite", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "X11/xine-lib “ÿ‘’ﬁ ‹ﬁ‘„€Ï", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Media Player", // English
    "Medien...", // Deutsch
    "", // Slovenski
    "Lettore multimediale", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Mediasoitin", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "Xine-lib", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "normal", // English
    "Normal", // Deutsch
    "", // Slovenski
    "normale", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "normaali", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "Ωﬁ‡‹–€Ï›ÎŸ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "inverted", // English
    "Invertiert", // Deutsch
    "", // Slovenski
    "invertito", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "k‰‰nteinen", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∏›“’‡‚ÿ‡ﬁ“–›ﬁ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Interlaced Field Order", // English
    "Interlaced Halbbild-Reihenfolge", // Deutsch
    "", // Slovenski
    "Ordine campo interlacciato", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Lomitettujen kenttien j‰rjestys", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "«’‡’◊·‚‡ﬁÁ›ÎŸ ﬂﬁ‡Ô‘ﬁ⁄ ﬂﬁ€’Ÿ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Brightness", // English
    "Helligkeit", // Deutsch
    "", // Slovenski
    "Luminosit‡", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Kirkkaus", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "œ‡⁄ﬁ·‚Ï", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Decoder", // English
    "Dekoder", // Deutsch
    "", // Slovenski
    "Decoder", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Dekooderi", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¥’⁄ﬁ‘’‡", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Audio", // English
    "Audio", // Deutsch
    "", // Slovenski
    "Audio", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "ƒ‰ni", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∞„‘ÿﬁ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "On-Screen Display", // English
    "On-Screen Display", // Deutsch
    "", // Slovenski
    "Messaggi in sovraimpressione (OSD)", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Kuvaruutun‰yttˆ", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "Õ⁄‡–››ﬁ’ ‹’›Ó", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Hide main menu", // English
    "Verstecke Hauptmen¸", // Deutsch
    "", // Slovenski
    "Nascondi voce nel menu principale", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Piilota valinta p‰‰valikossa", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¡⁄‡Î‚Ï ﬁ·›ﬁ“›ﬁ’ ‹’›Ó", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Window aspect", // English
    "Fenster-Seitenverh‰ltnis", // Deutsch
    "", // Slovenski
    "Aspetto finestra", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Ikkunan kuvasuhde", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¡ﬁﬁ‚›ﬁË’›ÿ’ ·‚ﬁ‡ﬁ›", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Scale to window size", // English
    "Skaliere auf Fenster-Grˆﬂe", // Deutsch
    "", // Slovenski
    "Scala a dimensione finestra", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Skaalaa ikkunan kokoiseksi", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "º–·Ë‚–—ÿ‡ﬁ“–‚Ï “ ‡–◊‹’‡ ﬁ⁄›–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Dynamic transparency correction", // English
    "Dynamische Transparenz-Korrektur", // Deutsch
    "", // Slovenski
    "Correzione trasparenza dinamica", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Dynaaminen l‰pin‰kyvyyden korjaus", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¥ÿ›–‹ÿÁ’·⁄–Ô ⁄ﬁ‡‡’⁄ÊÿÔ ﬂ‡ﬁ◊‡–Á›ﬁ·‚ÿ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Static transparency correction", // English
    "Statische Transparenz-Korrektur", // Deutsch
    "", // Slovenski
    "Correzione trasparenza statica", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "L‰pin‰kyvyyden korjaus", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¡‚–‚ÿÁ’·⁄–Ô ⁄ﬁ‡‡’⁄ÊÿÔ ﬂ‡ﬁ◊‡–Á›ﬁ·‚ÿ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Video", // English
    "Video", // Deutsch
    "", // Slovenski
    "Video", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Kuva", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "≤ÿ‘’ﬁ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Deinterlacing", // English
    "Deinterlacing", // Deutsch
    "", // Slovenski
    "Deinterlacciamento", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Lomituksen poisto", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¥’ÿ›‚’‡€’Ÿ·ÿ›”", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Remote Clients", // English
    "Entfernte Clients", // Deutsch
    "", // Slovenski
    "Client remoti", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Et‰k‰yttˆ", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "√‘–€’››Î’ ⁄€ÿ’›‚Î", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Allow remote clients", // English
    "Erlaube entfernte Clients", // Deutsch
    "", // Slovenski
    "Permetti client remoti", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Salli et‰k‰yttˆ", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¿–◊‡’Ëÿ‚Ï „‘–€’››ÎÂ ⁄€ÿ’›‚ﬁ“", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Listen port (TCP and broadcast)", // English
    "  Empfangender Port (TCP und Broadcast)", // Deutsch
    "", // Slovenski
    "  Porta in ascolto (TCP e broadcast)", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "  Kuuntele TCP-porttia", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  øﬁ‡‚ (TCP ÿ Ëÿ‡ﬁ⁄ﬁ“’Ë–‚’€Ï›ÎŸ)", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Listen address", // English
    "", // Deutsch
    "", // Slovenski
    "  Indirizzo in ascolto", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "  Kuuntele osoitteessa", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Remote keyboard", // English
    "  Tastaturfernsteuerung", // Deutsch
    "", // Slovenski
    "  Tastiera remota", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "  K‰yt‰ et‰n‰pp‰imistˆ‰", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  √‘–€’››–Ô ⁄€–“ÿ–‚„‡–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Buffer size", // English
    "Puffergrˆﬂe", // Deutsch
    "", // Slovenski
    "Dimensione buffer", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Puskurin koko", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¿–◊‹’‡ —„‰’‡–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Number of PES packets", // English
    "  Anzahl PES-Pakete", // Deutsch
    "", // Slovenski
    "  Numero di pacchetti PES", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "  PES-pakettien lukum‰‰r‰", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  PES ﬂ–⁄’‚ﬁ“", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "custom", // English
    "Benutzerdefiniert", // Deutsch
    "", // Slovenski
    "personalizza", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "oma", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "øﬁ€Ï◊ﬁ“–‚’€Ï", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "tiny", // English
    "Winzig", // Deutsch
    "", // Slovenski
    "molto piccolo", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "olematon", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "æÁ’›Ï ‹–€’›Ï⁄ÿŸ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "small", // English
    "Klein", // Deutsch
    "", // Slovenski
    "piccolo", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "pieni", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "º–€’›Ï⁄ÿŸ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "medium", // English
    "Mittel", // Deutsch
    "", // Slovenski
    "medio", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "keskikokoinen", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¡‡’‘›ÿŸ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "large", // English
    "Groﬂ", // Deutsch
    "", // Slovenski
    "grande", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "suuri", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "±ﬁ€ÏËﬁŸ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "very large", // English
    "", // Deutsch
    "", // Slovenski
    "molto grande", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "eritt‰in suuri", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "huge", // English
    "Riesig", // Deutsch
    "", // Slovenski
    "enorme", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "valtava", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "æÁ’›Ï —ﬁ€Ï›ﬁŸ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Display address", // English
    "Bildschirm-Adresse", // Deutsch
    "", // Slovenski
    "Mostra indirizzo", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "N‰ytˆn osoite", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∞‘‡’· ‘ÿ·ﬂ€’Ô", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Use keyboard", // English
    "Tastatur benutzen", // Deutsch
    "", // Slovenski
    "Utilizza tastiera", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "K‰yt‰ n‰pp‰imistˆ‰", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∏·ﬂﬁ€Ï◊ﬁ“–‚Ï ⁄€–“ÿ–‚„‡„", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Driver", // English
    "Treiber", // Deutsch
    "", // Slovenski
    "Driver", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Ohjain", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¥‡–Ÿ“’‡", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Port", // English
    "Port", // Deutsch
    "", // Slovenski
    "Porta", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Portti", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "øﬁ‡‚", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Delay", // English
    "Verzˆgerung", // Deutsch
    "", // Slovenski
    "Ritardo", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Viive", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∑–‘’‡÷⁄–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // ms -- milliseconds
  { "ms", // English
    "ms", // Deutsch
    "", // Slovenski
    "ms", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "ms", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ms", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // px - pixels
  { "px", // English
    "px", // Deutsch
    "", // Slovenski
    "px", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "px", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ﬂÿ⁄·’€’Ÿ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Window width", // English
    "  Fensterbreite", // Deutsch
    "", // Slovenski
    "  Larghezza finestra", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "  Ikkunan leveys", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  »ÿ‡ÿ›– ﬁ⁄›–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Window height", // English
    "  Fensterhˆhe", // Deutsch
    "", // Slovenski
    "  Altezza finestra", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "  Ikkunan korkeus", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  ≤Î·ﬁ‚– ﬁ⁄›–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "automatic", // English
    "Automatik", // Deutsch
    "", // Slovenski
    "automatica", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "automaattinen", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∞“‚ﬁ‹–‚ÿÁ’·⁄ÿ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "default", // English
    "Standard", // Deutsch
    "", // Slovenski
    "predefinita", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "oletus", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "øﬁ „‹ﬁ€Á–›ÿÓ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "4:3", // English
    "4:3", // Deutsch
    "4:3", // Slovenski
    "4:3", // Italiano
    "4:3", // Nederlands
    "4:3", // PortuguÍs
    "4:3", // FranÁais
    "4:3", // Norsk
    "4:3", // Suomi
    "4:3", // Polski
    "4:3", // EspaÒol
    "4:3", // Ellinika
    "4:3", // Svenska
    "4:3", // Romaneste
    "4:3", // Magyar
    "4:3", // Catala
    "4:3", // Russian
    "4:3", // Croatian
    "4:3", // Eesti
    "4:3", // Dansk
    "4:3", // Czech
  },
  { "16:9", // English
    "16:9", // Deutsch
    "16:9", // Slovenski
    "16:9", // Italiano
    "16:9", // Nederlands
    "16:9", // PortuguÍs
    "16:9", // FranÁais
    "16:9", // Norsk
    "16:9", // Suomi
    "16:9", // Polski
    "16:9", // EspaÒol
    "16:9", // Ellinika
    "16:9", // Svenska
    "16:9", // Romaneste
    "16:9", // Magyar
    "16:9", // Catala
    "16:9", // Russian
    "16:9", // Croatian
    "16:9", // Eesti
    "16:9", // Dansk
    "16:9", // Czech
  },
  { "Pan&Scan", // English
    "Pan&Scan", // Deutsch
    "Pan&Scan", // Slovenski
    "Pan&Scan", // Italiano
    "Pan&Scan", // Nederlands
    "Pan&Scan", // PortuguÍs
    "Pan&Scan", // FranÁais
    "Pan&Scan", // Norsk
    "Pan&Scan", // Suomi
    "Pan&Scan", // Polski
    "Pan&Scan", // EspaÒol
    "Pan&Scan", // Ellinika
    "Pan&Scan", // Svenska
    "Pan&Scan", // Romaneste
    "Pan&Scan", // Magyar
    "Pan&Scan", // Catala
    "Pan&Scan", // Russian
    "Pan&Scan", // Croatian
    "Pan&Scan", // Eesti
    "Pan&Scan", // Dansk
    "Pan&Scan", // Czech
  },
  { "HUE", // English
    "Farbton", // Deutsch
    "", // Slovenski
    "Tonalit‡", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "V‰ris‰vy", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "HUE", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Saturation", // English
    "S‰ttigung", // Deutsch
    "", // Slovenski
    "Saturazione", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Saturaatio", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "Ω–·ÎÈ’››ﬁ·‚Ï", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Contrast", // English
    "Kontrast", // Deutsch
    "", // Slovenski
    "Contrasto", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Kontrasti", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∫ﬁ›‚‡–·‚›ﬁ·‚Ï", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "off", // English
    "Aus", // Deutsch
    "", // Slovenski
    "disattivo", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "ei k‰ytˆss‰", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "≤Î⁄€.", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "no audio", // English
    "Kein Audio", // Deutsch
    "", // Slovenski
    "niente audio", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "ei ‰‰nt‰", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "Ω’‚ –„‘ÿﬁ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "no video", // English
    "Kein Video", // Deutsch
    "", // Slovenski
    "niente video", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "ei kuvaa", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "Ω’‚ “ÿ‘’ﬁ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Fullscreen mode", // English
    "Vollbild-Modus", // Deutsch
    "", // Slovenski
    "Mod. schermo intero", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Kokoruututila", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "øﬁ€›ﬁÌ⁄‡–››ÎŸ ‡’÷ÿ‹", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Local Frontend", // English
    "Lokale Anzeige", // Deutsch
    "", // Slovenski
    "Frontend locale", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Paikallinen n‰yttˆ", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ªﬁ⁄–€Ï›ÎŸ ‰‡ﬁ›‚’›‘", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Local Display Frontend", // English
    "Lokale Bildschirmanzeige", // Deutsch
    "", // Slovenski
    "Frontend visualizzazione locale", // Italiano
    "", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Paikallinen n‰yttˆ", // Suomi
    "", // Polski
    "", // EspaÒol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ƒ‡ﬁ›‚’›‘ €ﬁ⁄–€Ï›ﬁ”ﬁ Ì⁄‡–›–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Delete image ?", // English
    "Bild lˆschen?", // Deutsch
    "", // Slovenski
    "Cancellare immagine ?", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Poistetaanko kuva ?", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "√‘–€ÿ‚Ï ⁄–‡‚ÿ›⁄„ ?", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  TCP transport", // English
    "  TCP-‹bertragung", // Deutsch
    "", // Slovenski
    "  Protocollo TCP", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  TCP-siirto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "TCP ‚‡–›·ﬂﬁ‡‚", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  UDP transport", // English
    "  UDP-‹bertragung", // Deutsch
    "", // Slovenski
    "  Protocollo UDP", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  UDP-siirto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "UDP ‚‡–›·ﬂﬁ‡‚", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  RTP (multicast) transport", // English
    "  RTP (multicast) ‹bertragung", // Deutsch
    "", // Slovenski
    "  Protocollo RTP (multicast)", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  RTP (multicast) -siirto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  RTP (Ëÿ‡ﬁ⁄ﬁ“’È–‚’€Ï›ÎŸ) ‚‡–›·ﬂﬁ‡‚", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  PIPE transport", // English
    "  Pipe-‹bertragung", // Deutsch
    "", // Slovenski
    "  Protocollo PIPE", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  PIPE-siirto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  PIPE ‚‡–›·ﬂﬁ‡‚", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Server announce broadcasts", // English
    "  Server-Bekanntmachung Broadcast", // Deutsch
    "", // Slovenski
    "  Annuncio trasmissioni dal server", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Palvelimen broadcast-ilmoitukset", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "  ¡’‡“’‡ ÿ·ﬂﬁ€Ï◊„’‚ Ëÿ‡ﬁ⁄ﬁ“’È–›ÿ’", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Audio equalizer >>", // English
    "Audio-Equalizer >>", // Deutsch
    "", // Slovenski
    "Equalizzatore audio >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Taajuuskorjain >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∞„‘ÿﬁ Ì⁄“–€–Ÿ◊’‡ >>", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Audio Equalizer", // English
    "Audio Equalizer", // Deutsch
    "", // Slovenski
    "Equalizzatore audio", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Taajuuskorjain", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∞„‘ÿﬁ Ì⁄“–€–Ÿ◊’‡", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Grayscale", // English
    "Graustufen", // Deutsch
    "", // Slovenski
    "Scala di grigi", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Harmaas‰vy", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "æ‚‚’›⁄ÿ ·’‡ﬁ”ﬁ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Bitmap", // English
    "Bitmap", // Deutsch
    "", // Slovenski
    "Bitmap", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Bittikartta", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "±ÿ‚ﬁ“–Ô ⁄–‡‚–", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Button$Info", // English
    "Info", // Deutsch
    "", // Slovenski
    "Info", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Tiedot", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Audio Compression", // English
    "Audio-Komprimierung", // Deutsch
    "", // Slovenski
    "Compressione audio", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Voimista hiljaisia ‰‰ni‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∞„‘ÿﬁ ⁄ﬁ‹ﬂ‡’··ÿÔ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play file >>", // English
    "Datei abspielen >>", // Deutsch
    "", // Slovenski
    "Riproduci file >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista tiedosto >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ø‡ﬁÿ”‡–‚Ï ‰–Ÿ€ >>", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play music >>", // English
    "Musik abspielen >>", // Deutsch
    "", // Slovenski
    "Riproduci musica >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista musiikkia >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ø‡ﬁÿ”‡–‚Ï ‰–Ÿ€ >>", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "View images >>", // English
    "Bilder ansehen >>", // Deutsch
    "", // Slovenski
    "Visualizza immagini >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Katsele kuvia >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ø‡ﬁ·‹ﬁ‚‡’‚Ï ÿ◊ﬁ—‡–÷’›ÿÔ >>", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play file", // English
    "Datei abspielen", // Deutsch
    "", // Slovenski
    "Riproduci file", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista tiedosto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ø‡ﬁÿ”‡–‚Ï ‰–Ÿ€", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Images", // English
    "Bilder", // Deutsch
    "", // Slovenski
    "Immagini", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Kuvat", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "∏◊ﬁ—‡–÷’›ÿÔ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "CenterCutOut", // English
    "CenterCutOut", // Deutsch
    "", // Slovenski
    "CenterCutOut", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "CenterCutOut", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Test Images", // English
    "Testbilder", // Deutsch
    "", // Slovenski
    "Prova immagini", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Testikuvat", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "¬’·‚ﬁ“Î’ ÿ◊ﬁ—‡–÷’›ÿÔ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Visualization", // English
    "Visualisierung", // Deutsch
    "", // Slovenski
    "Visualizzazione", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Visualisointi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "≤ÿ◊„–€ÿ◊–ÊÿÔ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Upmix stereo to 5.1", // English
    "Stereo zu 5.1 hoch mischen", // Deutsch
    "", // Slovenski
    "Suono da Stereo a 5.1", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Miksaa stereo‰‰ni 5.1-kanavaiseksi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "ø‡’ﬁ—‡–◊ﬁ“–‚Ï ·‚’‡’ﬁ “ 5.1", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Downmix AC3 to surround", // English
    "AC3 zu Surround herunter mischen", // Deutsch
    "", // Slovenski
    "Suono da AC3 a surround", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Miksaa AC3-‰‰ni surroundiksi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Framebuffer device", // English
    "Framebuffer-Device", // Deutsch
    "", // Slovenski
    "Periferica framebuffer", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Framebuffer-laite", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "Framebuffer „·‚‡ﬁŸ·‚“ﬁ", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Use hardware for low-res video", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Laitteisto matalaresoluutioisella videolla", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },

  // 1.0.0pre2:
  { "Play remote DVD >>", // English
    "Entfernte DVD abspielen >>", // Deutsch
    "", // Slovenski
    "Riproduci DVD remoto >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista DVD-levy et‰koneesta >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play DVD disc >>", // English
    "DVD abspielen >>", // Deutsch
    "", // Slovenski
    "Riproduci disco DVD >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista DVD-levy >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Crop letterbox 4:3 to 16:9", // English
    "Schneide letterbox 4:3 zu 16:9", // Deutsch
    "", // Slovenski
    "Ritaglia letterbox 4:3 a 16:9", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Leikkaa 4:3-letterbox 16:9:ksi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play only audio", // English
    "Nur Audio spielen", // Deutsch
    "", // Slovenski
    "Riproduci solo audio", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista pelkk‰ ‰‰ni", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Off", // English
    "Aus", // Deutsch
    "", // Slovenski
    "Disattivo", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "ei k‰ytˆss‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "OSS", // English
    "OSS", // Deutsch
    "", // Slovenski
    "OSS", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "OSS", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Alsa", // English
    "Alsa", // Deutsch
    "", // Slovenski
    "Alsa", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Alsa", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Goom", // English
    "Goom", // Deutsch
    "", // Slovenski
    "Goom", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Goom", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Oscilloscope", // English
    "Oszilloskop", // Deutsch
    "", // Slovenski
    "Oscilloscopio", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Oskilloskooppi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "FFT Scope", // English
    "FFT Spektrum", // Deutsch
    "", // Slovenski
    "Spettro FFT", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Spektri", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "FFT Graph", // English
    "FFT Graph", // Deutsch
    "", // Slovenski
    "Grafico FFT", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Spektrogrammi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "X11 (sxfe)", // English
    "X11 (sxfe)", // Deutsch
    "", // Slovenski
    "X11 (sxfe)", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "X11 (sxfe)", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Framebuffer (fbfe)", // English
    "Framebuffer (fbfe)", // Deutsch
    "", // Slovenski
    "Framebuffer (fbfe)", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Framebuffer (fbfe)", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Xv", // English
    "Xv", // Deutsch
    "", // Slovenski
    "Xv", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Xv", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "XShm", // English
    "XShm", // Deutsch
    "", // Slovenski
    "XShm", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "XShm", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Bob", // English
    "Bob", // Deutsch
    "", // Slovenski
    "Bob", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Bob", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Weave", // English
    "Weave", // Deutsch
    "", // Slovenski
    "Weave", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Weave", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Greedy", // English
    "Greedy", // Deutsch
    "", // Slovenski
    "Greedy", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Greedy", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "One Field", // English
    "Ein Halbbild", // Deutsch
    "", // Slovenski
    "Un campo", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "One Field", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "One Field XV", // English
    "Ein Halbbild XV", // Deutsch
    "", // Slovenski
    "Un campo XV", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "One Field XV", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Linear Blend", // English
    "Linear mischen", // Deutsch
    "", // Slovenski
    "Trasparenza lineare", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Linear Blend", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "TvTime", // English
    "TvTime", // Deutsch
    "", // Slovenski
    "TvTime", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "TvTime", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "    Address", // English
    "    Multicast-Adresse", // Deutsch
    "", // Slovenski
    "    Indirizzo", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "    Osoite", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "    Port", // English
    "    Multicast-Port", // Deutsch
    "", // Slovenski
    "    Porta", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "    Portti", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "    TTL", // English
    "    Multicast-TTL", // Deutsch
    "", // Slovenski
    "    TTL", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "    TTL-aika", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "    Transmit always on", // English
    "    Immer senden", // Deutsch
    "", // Slovenski
    "    Trasmetti sempre", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "    Pid‰ l‰hetys aina p‰‰ll‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Speakers", // English
    "Lautsprecher", // Deutsch
    "", // Slovenski
    "Altoparlanti", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Kaiuttimet", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Headphones 2.0", // English
    "Kopfhˆhrer 2.0", // Deutsch
    "", // Slovenski
    "Cuffie 2.0", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Kuulokkeet 2.0", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Autodetect letterbox", // English
    "  Letterbox automatisch erkennen", // Deutsch
    "", // Slovenski
    "  Rileva letterbox in automatico", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Tunnista letterbox automaattisesti", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Soft start", // English
    "  Weich starten", // Deutsch
    "", // Slovenski
    "  Avvio leggero", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Portaittainen aloitus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Crop to", // English
    "  Schneide auf", // Deutsch
    "", // Slovenski
    "  Ritaglia a", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Leikkaa kokoon", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Detect subtitles", // English
    "  Erkenne Untertitel", // Deutsch
    "", // Slovenski
    "  Rileva sottotitoli", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Huomioi tekstitys", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },

  // 1.0.0pre4:
  { "Media", // English
    "Medien", // Deutsch
    "", // Slovenski
    "Media", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Media", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Video settings", // English
    "Video-Einstellungen", // Deutsch
    "", // Slovenski
    "Impostazioni video", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Videoasetukset", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Audio settings", // English
    "Audio-Einstellungen", // Deutsch
    "", // Slovenski
    "Impostazioni audio", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "ƒ‰niasetukset", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Overscan (crop image borders)", // English
    "Overscan (Bildr‰nder abschneiden)", // Deutsch
    "", // Slovenski
    "Overscan (ritaglia bordi immagine)", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Leikkaa kuvan reunoja (overscan)", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Smooth fast forward", // English
    "", // Deutsch
    "", // Slovenski
    "Avanzamento veloce leggero", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Tasainen kuvakelaus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // Missing texts 2006-09-20
  { "Post processing (ffmpeg)", // English
    "Nachbearbeitung (ffmpeg)", // Deutsch
    "", // Slovenski
    "Codifica (ffmpeg)", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "K‰yt‰ j‰lkik‰sittely‰ (ffmpeg)", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // ffmpeg post processing
  { "  Quality", // English
    "  Qualit‰t", // Deutsch
    "", // Slovenski
    "  Qualit‡", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Laatu", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Mode", // English
    "  Modus", // Deutsch
    "", // Slovenski
    "  Modalit‡", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Moodi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // tvtime de-interlacing
  { "  Method", // English
    "  Methode", // Deutsch
    "", // Slovenski
    "  Metodo", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Menetelm‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Cheap mode", // English
    "  einfacher Modus", // Deutsch
    "", // Slovenski
    "  Modo economico", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  K‰yt‰ Cheap-moodia", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Pulldown", // English
    "  Pulldown", // Deutsch
    "", // Slovenski
    "  Pulldown", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Pulldown-moodi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Frame rate", // English
    "  Bildrate", // Deutsch
    "", // Slovenski
    "  Frame rate", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Ruudunp‰ivitys", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Judder Correction", // English
    "  Ruckel-Korrektur", // Deutsch
    "", // Slovenski
    "  Correzione gamma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  K‰yt‰ t‰rin‰nkorjausta", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Use progressive frame flag", // English
    "  Nutze progressive frame flag", // Deutsch
    "", // Slovenski
    "  Utilizza flag frame progressivo", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Tunnista progressiivinen kuva", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Chroma Filter", // English
    "  Chrominanz-Filter", // Deutsch
    "", // Slovenski
    "  Filtro Chroma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  K‰yt‰ Chroma-suodinta", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Select subtitle track", // English
    "W‰hle Untertitel", // Deutsch
    "", // Slovenski
    "Seleziona traccia sottotitoli", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Valitse tekstityskieli", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Select subtitle track >>", // English
    "W‰hle Untertitel >>", // Deutsch
    "", // Slovenski
    "Seleziona traccia sottotitoli >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Valitse tekstityskieli >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Subtitles", // English
    "Untertitel", // Deutsch
    "", // Slovenski
    "Sottotitoli", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Tekstitys", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "External subtitle size", // English
    "Untertitel grˆﬂe", // Deutsch
    "", // Slovenski
    "Dimensione sottotitoli esterni", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Erillisen tekstityksen koko", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Aspect ratio", // English
    "Seitenverh‰ltnis", // Deutsch
    "", // Slovenski
    "Proporzioni", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Kuvasuhde", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play music", // English
    "Musik abspielen", // Deutsch
    "", // Slovenski
    "Riproduci musica", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista musiikkia", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Random play", // English
    "Zufallswiedergabe", // Deutsch
    "", // Slovenski
    "Riproduzione casuale", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Satunnaistoisto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Normal play", // English
    "Normale Wiedergabe", // Deutsch
    "", // Slovenski
    "Riproduzione normale", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Normaali toisto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Frontend initialization failed", // English
    "Initialisierung des Frontends fehlgeschlagen", // Deutsch
    "", // Slovenski
    "Inizializzazione frontend fallita", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "N‰yttˆlaitteen alustus ep‰onnistui", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Server initialization failed", // English
    "Initialisierung des Servers fehlgeschlagen", // Deutsch
    "", // Slovenski
    "Inizializzazione server fallita", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Palvelimen k‰ynnistys ep‰onnistui", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // Goom options
  { "  Width", // English
    "  Breite", // Deutsch
    "", // Slovenski
    "  Larghezza", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Leveys", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Height", // English
    "  Hˆhe", // Deutsch
    "", // Slovenski
    "  Altezza", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Korkeus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Speed", // English
    "  Bildrate", // Deutsch
    "", // Slovenski
    "  Velocit‡", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Nopeus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "    SAP announcements", // English
    "    SAP-Ank¸ndigungen", // Deutsch
    "", // Slovenski
    "    Annunci SAP", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "    SAP-ilmoitukset", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play remote CD >>", // English
    "Entfernte CD abspielen >>", // Deutsch
    "", // Slovenski
    "Riproduci CD remoto >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista CD-levy et‰koneesta >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Play audio CD >>", // English
    "Musik-CD abspielen >>", // Deutsch
    "", // Slovenski
    "Riproduci CD audio >>", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Toista CD-levy >>", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  HTTP transport for media files", // English
    "  HTTP-Verbindung f¸r Medien-Dateien", // Deutsch
    "", // Slovenski
    "  Protocollo HTTP per file multimediali", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  HTTP -siirto mediatiedostoille", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Additional network services", // English
    "Zus‰tzliche Netzwerk-Services", // Deutsch
    "", // Slovenski
    "Ulteriori servizi di rete", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Muut verkkopalvelut", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "HTTP server", // English
    "HTTP-Server", // Deutsch
    "", // Slovenski
    "Server HTTP", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "HTTP-palvelin", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "RTSP server", // English
    "RTSP-Server", // Deutsch
    "", // Slovenski
    "Server RTSP", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "RTSP-palvelin", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "HTTP clients can control VDR", // English
    "HTTP-Clients kˆnnen VDR kontrollieren", // Deutsch
    "", // Slovenski
    "I client HTTP possono controllare VDR", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Anna HTTP-asiakkaiden ohjata VDR:‰‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "RTSP clients can control VDR", // English
    "RTSP-Clients kˆnnen VDR kontrollieren", // Deutsch
    "", // Slovenski
    "I client RTSP possono controllare VDR", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Anna RTSP-asiakkaiden ohjata VDR:‰‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Button$Queue", // English
    "Warteschlange", // Deutsch
    "", // Slovenski
    "Coda", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Soittolistalle", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Button$Sort", // English
    "Sortiere", // Deutsch
    "", // Slovenski
    "Ordina", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "J‰rjest‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Button$Remove", // English
    "Entferne", // Deutsch
    "", // Slovenski
    "Rimuovi", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Poista", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Button$Add files", // English
    "F¸ge Dateien hinzu", // Deutsch
    "", // Slovenski
    "Aggiungi files", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Lis‰‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Button$Random", // English
    "Zufall", // Deutsch
    "", // Slovenski
    "Casuale", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Satunnaistoisto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Button$Normal", // English
    "Normal", // Deutsch
    "", // Slovenski
    "Normale", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Normaali toisto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "No subtitles available!", // English
    "Keine Untertitel verf¸gbar!", // Deutsch
    "", // Slovenski
    "Nessun sottotitolo disponibile!", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Ei tekstityst‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Playlist", // English
    "Wiedergabeliste", // Deutsch
    "", // Slovenski
    "Lista esecuzione", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Soittolista", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Add to playlist", // English
    "F¸ge zur Wiedergabeliste hinzu", // Deutsch
    "", // Slovenski
    "Aggiungi alla lista esecuzione", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Lis‰‰ soittolistalle", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Queued to playlist", // English
    "H‰nge an Wiedergabeliste an", // Deutsch
    "", // Slovenski
    "Accoda alla lista esecuzione", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Lis‰tty soittolistalle", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Volume control", // English
    "", // Deutsch
    "", // Slovenski
    "Controllo volume", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "ƒ‰nenvoimakkuuden s‰‰tˆ", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Hardware", // English
    "", // Deutsch
    "", // Slovenski
    "Hardware", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Laitteistolla", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Software", // English
    "", // Deutsch
    "", // Slovenski
    "Software", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Ohjelmallisesti", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "3D Denoiser", // English
    "", // Deutsch
    "", // Slovenski
    "3D Denoiser", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "3D kohinanpoisto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // 3D Denoiser
  { "  Spatial luma strength", // English
    "", // Deutsch
    "", // Slovenski
    "  Resistenza luma spaziale", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Luman tilavoimakkuus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Spatial chroma strength", // English
    "", // Deutsch
    "", // Slovenski
    "  Resistenza chroma spaziale", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Chroman tilavoimakkuus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Temporal strength", // English
    "", // Deutsch
    "", // Slovenski
    "  Resistenza temporale", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Ajallinen voimakkuus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Sharpen / Blur", // English
    "", // Deutsch
    "", // Slovenski
    "Nitido / Blur", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Ter‰vˆinti / Sumennus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  // Unsharp mask
  { "  Width of the luma matrix", // English
    "", // Deutsch
    "", // Slovenski
    "  Larghezza della matrice luma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Luma-matriisin leveys", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Height of the luma matrix", // English
    "", // Deutsch
    "", // Slovenski
    "  Altezza della matrice luma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Luma-matriisin korkeus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Amount of luma sharpness/blur", // English
    "", // Deutsch
    "", // Slovenski
    "  Valore di nitidezza/blur luma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Luma-ter‰vˆinti/-sumennus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Width of the chroma matrix", // English
    "", // Deutsch
    "", // Slovenski
    "  Larghezza della matrice chroma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Chroma-matriisin leveys", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Height of the chroma matrix", // English
    "", // Deutsch
    "", // Slovenski
    "  Altezza della matrice chroma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Chroma-matriisin korkeus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Amount of chroma sharpness/blur", // English
    "", // Deutsch
    "", // Slovenski
    "  Valore di nitidezza/blur chroma", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Chroma-ter‰vˆinti/-sumennus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Headphones 2.0", // English
    "", // Deutsch
    "", // Slovenski
    "Cuffie 2.0", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Kuulokkeet 2.0", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Pass Through", // English
    "", // Deutsch
    "", // Slovenski
    "Passa attraverso", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "L‰pivienti", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Show the track number", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "N‰yt‰ raidan numero", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Show the name of the artist", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "N‰yt‰ esitt‰j‰n nimi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Show the name of the album", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "N‰yt‰ levyn nimi", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Cache metainfo", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Tallenna metatieto", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },  
  { "Scan for metainfo", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Tutki kappaleiden metatiedot", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Playlist settings", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Soittolistan asetukset", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Video aspect ratio", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Videon kuvasuhde", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "square", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "neliˆ", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "anamorphic", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "anamorfinen", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Software scaling", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Skaalaus ohjelmistolla", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Change aspect ratio", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Muuta kuvasuhdetta", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Change video size", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Muuta videokuvan kokoa", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Fastest trick speed", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Suurin kelausnopeus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Blending method", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Piirtotapa", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Scaling method", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Skaalaus", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Show all layers", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "N‰yt‰ kaikki kerrokset", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Advanced", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Advanced settings", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { " * Following settings won't work with UDP/RTP *", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    " * Seuraavat asetukset eiv‰t toimi UDP/RTP:n kanssa *", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Sync to transponder in live mode", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "Tahdistu l‰hetteeseen live-tilassa", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "Adjust SCR", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "S‰‰d‰ SCR:‰‰", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Clock speed (Hz)", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Kellon nopeus (Hz)", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },
  { "  Clock Adjustment (%)", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu<EA>s
    "", // Fran<E7>ais
    "", // Norsk
    "  Kellon s‰‰tˆ (%)", // Suomi
    "", // Polski
    "", // Espa<F1>ol
    "", // Ellinika
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catala
    "", // Russian
    "", // Croatian
    "", // Eesti
    "", // Dansk
    "", // Czech
  },

  { NULL }
};

#endif // VDRVERSNUM < 10507

