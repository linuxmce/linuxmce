<?php
 #############################################################################
 # IMDBPHP                              (c) Giorgos Giagas & Itzchak Rehberg #
 # written by Giorgos Giagas                                                 #
 # extended & maintained by Itzchak Rehberg <izzysoft AT qumran DOT org>     #
 # http://www.izzysoft.de/                                                   #
 # ------------------------------------------------------------------------- #
 # This program is free software; you can redistribute and/or modify it      #
 # under the terms of the GNU General Public License (see doc/LICENSE)       #
 #############################################################################

 /* $Id: imdb.class.php 421 2010-10-16 23:28:45Z izzy $ */

 require_once (dirname(__FILE__)."/movie_base.class.php");

 #=============================================================================
 #=================================================[ The IMDB class itself ]===
 #=============================================================================
 /** Accessing IMDB information
  * @package IMDB
  * @class imdb
  * @extends movie_base
  * @author Georgos Giagas
  * @author Izzy (izzysoft AT qumran DOT org)
  * @copyright (c) 2002-2004 by Giorgos Giagas and (c) 2004-2009 by Itzchak Rehberg and IzzySoft
  * @version $Revision: 421 $ $Date: 2010-10-17 01:28:45 +0200 (So, 17. Okt 2010) $
  */
 class imdb extends movie_base {

 #======================================================[ Common functions ]===
 #-----------------------------------------------------------[ Constructor ]---
  /** Initialize the class
   * @constructor imdb
   * @param string id IMDBID to use for data retrieval
   */
  function __construct($id) {
    parent::__construct($id);
    $this->revision = preg_replace('|^.*?(\d+).*$|','$1','$Revision: 421 $');
    $this->setid($id);
  }

 #-------------------------------------------------------------[ Open Page ]---
  /** Define page urls
   * @method protected set_pagename
   * @param string wt internal name of the page
   * @return string urlname page URL
   */
  protected function set_pagename($wt) {
   switch ($wt){
    case "Title"       : $urlname="/"; break;
    case "Credits"     : $urlname="/fullcredits"; break;
    case "CrazyCredits": $urlname="/crazycredits"; break;
    case "Plot"        : $urlname="/plotsummary"; break;
    case "Synopsis"    : $urlname="/synopsis"; break;
    case "Taglines"    : $urlname="/taglines"; break;
    case "Episodes"    : $urlname="/episodes"; break;
    case "Quotes"      : $urlname="/quotes"; break;
    case "Trailers"    : $urlname="/trailers"; break;
    case "VideoSites"  : $urlname="/videosites"; break;
    case "Goofs"       : $urlname="/goofs"; break;
    case "Trivia"      : $urlname="/trivia"; break;
    case "Soundtrack"  : $urlname="/soundtrack"; break;
    case "MovieConnections" : $urlname="/movieconnections"; break;
    case "ExtReviews"  : $urlname="/externalreviews"; break;
    case "ReleaseInfo" : $urlname="/releaseinfo"; break;
    case "CompanyCredits" : $urlname="/companycredits"; break;
    case "ParentalGuide"  : $urlname="/parentalguide"; break;
    case "OfficialSites"  : $urlname="/officialsites"; break;
    case "Keywords"       : $urlname="/keywords"; break;
    case "Awards"      : $urlname="/awards"; break;
    case "Locations"   : $urlname="/locations"; break;
    default            :
      $this->page[$wt] = "unknown page identifier";
      $this->debug_scalar("Unknown page identifier: $wt");
      return false;
   }
   return $urlname;
  }

 #-----------------------------------------------[ URL to movies main page ]---
  /** Set up the URL to the movie title page
   * @method main_url
   * @return string url full URL to the current movies main page
   */
  public function main_url(){
   return "http://".$this->imdbsite."/title/tt".$this->imdbid()."/";
  }

 #======================================================[ Title page infos ]===
 #-------------------------------------------[ Movie title (name) and year ]---
  /** Setup title and year properties
   * @method private title_year
   */
  private function title_year() {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    if (@preg_match("/\<title\>(.*) \((.*)(\d{4}|\?{4}).*\)(.*)\<\/title\>/",$this->page["Title"],$match)) {
      $this->main_title = $match[1];
      if ($match[2]=="????") $this->main_year = "";
      else $this->main_year  = $match[3];
    }
  }


  /** Get movie title
   * @method title
   * @return string title movie title (name)
   * @see IMDB page / (TitlePage)
   */
  public function title() {
    if ($this->main_title == "") $this->title_year();
    return $this->main_title;
  }

  /** Get year
   * @method year
   * @return string year
   * @see IMDB page / (TitlePage)
   */
  public function year() {
    if ($this->main_year == -1) $this->title_year();
    return $this->main_year;
  }

  /** Get movie types (if any specified)
   * @method movieTypes
   * @return array [0..n] of strings (or empty array if no movie types specified)
   * @see IMDB page / (TitlePage)
   */
  public function movieTypes() {
    if ( empty($this->main_movietypes) ) {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (@preg_match("/\<title\>(.*)\<\/title\>/",$this->page["Title"],$match)) {
        if (preg_match_all('|\(([^\)]*)\)|',$match[1],$matches)) {
      for ($i=0;$i<count($matches[0]);++$i) if (!preg_match('|^\d{4}$|',$matches[1][$i])) $this->main_movietypes[] = $matches[1][$i];
        }
      }
    }
    return $this->main_movietypes;
  }

 #---------------------------------------------------------------[ Runtime ]---
  /** Get general runtime
   * @method private runtime_all
   * @return string runtime complete runtime string, e.g. "150 min / USA:153 min (director's cut)"
   */
  private function runtime_all() {
    if ($this->main_runtime == "") {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (@preg_match('!Runtime:</h4>\s*(.*)\s*</div!ms',$this->page["Title"],$match))
        $this->main_runtime = $match[1];
    }
    return $this->main_runtime;
  }

  /** Get overall runtime (first one mentioned on title page)
   * @method runtime
   * @return mixed string runtime in minutes (if set), NULL otherwise
   * @see IMDB page / (TitlePage)
   */
  public function runtime() {
    if (empty($this->movieruntimes)) $runarr = $this->runtimes();
    else $runarr = $this->movieruntimes;
    if (isset($runarr[0]["time"])) return $runarr[0]["time"];
    return NULL;
  }

  /** Retrieve language specific runtimes
   * @method runtimes
   * @return array runtimes (array[0..n] of array[time,country,comment])
   * @see IMDB page / (TitlePage)
   */
  public function runtimes(){
    if (empty($this->movieruntimes)) {
      if ($this->runtime_all() == "") return array();
      if (preg_match_all("/[\/ ]*((\D*?):|)([\d]+?) min( \((.*?)\)|)/",$this->main_runtime,$matches))
        for ($i=0;$i<count($matches[0]);++$i) $this->movieruntimes[] = array("time"=>$matches[3][$i],"country"=>$matches[2][$i],"comment"=>$matches[5][$i]);
    }
    return $this->movieruntimes;
  }

 #----------------------------------------------------------[ Movie Rating ]---
  /** Setup votes
   * @method private rate_vote
   */
  private function rate_vote() {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    if (preg_match("@class\=\"rating\-rating\">(\d{1,2}\.\d)@i",$this->page["Title"],$match)){
      $this->main_rating = $match[1];
    } else {
      $this->main_rating = 0;
    }
    if (preg_match("@href\=\"ratings\"\s*>([\d\,]+)@i",$this->page["Title"],$match)){
        $this->main_votes = $match[1];
    }else{
        $this->main_votes = 0;
    }
  }

  /** Get movie rating
   * @method rating
   * @return string rating current rating as given by IMDB site
   * @see IMDB page / (TitlePage)
   */
  public function rating () {
    if ($this->main_rating == -1) $this->rate_vote();
    return $this->main_rating;
  }

  /** Return votes for this movie
   * @method votes
   * @return string votes count of votes for this movie
   * @see IMDB page / (TitlePage)
   */
  public function votes() {
    if ($this->main_votes == -1) $this->rate_vote();
    return $this->main_votes;
  }

 #------------------------------------------------------[ Movie Comment(s) ]---
  /** Get movie main comment (from title page)
   * @method comment
   * @return string comment full text of movie comment from the movies main page
   * @see IMDB page / (TitlePage)
   */
  public function comment() {
    if ($this->main_comment == "") {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (@preg_match('!<div class\="user-comments">(.*?)<div class\="yn"!ms',$this->page["Title"],$match))
        $this->main_comment = preg_replace("/a href\=\"\//i","a href=\"http://".$this->imdbsite."/",$match[1]);
        $this->main_comment = str_replace("http://i.media-imdb.com/images/showtimes",$this->imdb_img_url."/showtimes",$this->main_comment);
    }
    return $this->main_comment;
  }

  /** Get movie main comment (from title page - split-up variant)
   * @method comment_split
   * @return array comment array[string title, string date, array author, string comment]; author: array[string url, string name]
   * @see IMDB page / (TitlePage)
   */
  public function comment_split() {
    if (empty($this->split_comment)) {
      if ($this->main_comment == "") $comm = $this->comment();
      if (@preg_match('!<strong>(.*?)</strong>.*<div class="comment-meta">\s*(.*?)\s*\|\s*by\s*(.*?)\s*&ndash;.*?<p>(.*?)</div!ims',$this->main_comment,$match)) {
        @preg_match('!href="(.*?)">(.*)</a!i',$match[3],$author);
        $this->split_comment = array("title"=>$match[1],"date"=>$match[2],"author"=>array("url"=>$author[1],"name"=>$author[2]),"comment"=>trim($match[4]));
      }
    }
    return $this->split_comment;
  }

 #--------------------------------------------------------------[ Keywords ]---
  /** Get the keywords for the movie
   * @method keywords
   * @return array keywords
   * @see IMDB page / (TitlePage)
   */
  public function keywords() {
    if (empty($this->main_keywords)) {
      if ($this->page["Title"] == "") $this->openpage("Title");
      if (preg_match_all('!<a href\="/keyword/[\w\-]+">(.*?)</a>!',$this->page["Title"],$matches))
        $this->main_keywords = $matches[1];
    }
    return $this->main_keywords;
  }

 #--------------------------------------------------------[ Language Stuff ]---
  /** Get movies original language
   * @method language
   * @return string language
   * @brief There is not really a main language on the IMDB sites (yet), so this
   *  simply returns the first one
   * @see IMDB page / (TitlePage)
   */
  public function language() {
   if ($this->main_language == "") {
     if (empty($this->langs)) $this->langs = $this->languages();
     $this->main_language = $this->langs[0];
   }
   return $this->main_language;
  }

  /** Get all langauges this movie is available in
   * @method languages
   * @return array languages (array[0..n] of strings)
   * @see IMDB page / (TitlePage)
   */
  public function languages() {
   if (empty($this->langs)) {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    if (preg_match_all('!<a href="/language/.*?">\s*(.*?)\s*<!m',$this->page["Title"],$matches))
      $this->langs = $matches[1];
   }
   return $this->langs;
  }


 #--------------------------------------------------------------[ Genre(s) ]---
  /** Get the movies main genre
   *  Since IMDB.COM does not really now a "Main Genre", this simply means the
   *  first mentioned genre will be returned.
   * @method genre
   * @return string genre first of the genres listed on the movies main page
   * @brief There is not really a main genre on the IMDB sites (yet), so this
   *  simply returns the first one
   * @see IMDB page / (TitlePage)
   */
  public function genre() {
   if (empty($this->main_genre)) {
    if (empty($this->moviegenres)) $genres = $this->genres();
    if (!empty($genres)) $this->main_genre = $this->moviegenres[0];
   }
   return $this->main_genre;
  }

  /** Get all genres the movie is registered for
   * @method genres
   * @return array genres (array[0..n] of strings)
   * @see IMDB page / (TitlePage)
   */
  public function genres() {
    if (empty($this->moviegenres)) {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (preg_match_all("@<a href\=\"/genre/[\w\-]+\"\>(.*?)\</a>@",$this->page["Title"],$matches))
        $this->moviegenres = $matches[1];
    }
    return $this->moviegenres;
  }


 #----------------------------------------------------------[ Color format ]---
  /** Get colors
   * @method colors
   * @return array colors (array[0..1] of strings)
   * @see IMDB page / (TitlePage)
   */
  public function colors() {
    if (empty($this->moviecolors)) {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (preg_match_all("|/search/title\?colors=.*?>\s*(.*?)<|",$this->page["Title"],$matches))
        $this->moviecolors = $matches[1];
    }
    return $this->moviecolors;
  }

 #---------------------------------------------------------------[ Creator ]---
  /** Get the creator of a movie (most likely for seasons only)
   * @method creator
   * @return array creator (array[0..n] of array[name,imdb])
   * @see IMDB page / (TitlePage)
   */
  public function creator() {
    if (empty($this->main_creator)) {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (@preg_match("/Creator:\<\/h5\>\s*\n(.*?)(<\/div|<a class=\"tn15more)/ms",$this->page["Title"],$match)) {
        if ( preg_match_all('|/name/nm(\d{7}).*?>(.*?)<|ims',$match[1],$matches) ) {
          for ($i=0;$i<count($matches[0]);++$i)
          $this->main_creator[] = array('name'=>$matches[2][$i],'imdb'=>$matches[1][$i]);
        }
      }
    }
    return $this->main_creator;
  }

 #---------------------------------------------------------------[ Tagline ]---
  /** Get the main tagline for the movie
   * @method tagline
   * @return string tagline
   * @see IMDB page / (TitlePage)
   */
  public function tagline() {
    if ($this->main_tagline == "") {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (@preg_match('!Taglines:</h4>\s*(.*?)\s*<!ims',$this->page["Title"],$match)) {
        $this->main_tagline = trim($match[1]);
      }
    }
    return $this->main_tagline;
  }

 #---------------------------------------------------------------[ Seasons ]---
  /** Get the number of seasons or 0 if not a series
   * @method seasons
   * @return int seasons number of seasons
   * @see IMDB page / (TitlePage)
   */
  public function seasons() {
    if ( $this->seasoncount == -1 ) {
      if ( $this->page["Title"] == "" ) $this->openpage("Title");
      if ( preg_match_all('|<a href="episodes#season-\d+">(\d+)</a>|Ui',$this->page["Title"],$matches) ) {
        $this->seasoncount = $matches[1][count($matches[0])-1];
      } else {
        $this->seasoncount = 0;
      }
      if ( preg_match_all('|<a href="episodes#season-unknown">unknown</a>|Ui',$this->page["Title"],$matches) ) {
        $this->seasoncount += count($matches[0]);
      }
    }
    return $this->seasoncount;
  }

 #-----------------------------------------------[ Is it part of a serial? ]---
  /** Try to figure out if this is a movie or part of a serie
   * @method is_serial
   * @return boolean
   * @see IMDB page / (TitlePage)
   */
  public function is_serial() {
    if ( $this->page["Title"] == "" ) $this->openpage("Title");
    return preg_match('|<h5>TV Series:</h5>|i',$this->page["Title"],$matches);
  }

 #--------------------------------------------------------[ Plot (Outline) ]---
  /** Get the main Plot outline for the movie
   * @method plotoutline
   * @param optional boolean fallback Fallback to storyline if we could not catch plotoutline? Default: FALSE
   * @return string plotoutline
   * @see IMDB page / (TitlePage)
   */
  public function plotoutline($fallback=FALSE) {
    if ($this->main_plotoutline == "") {
      if ($this->page["Title"] == "") $this->openpage("Title");
      if (preg_match('!<span class="rating-rating">.*?(<p>.*?)\s*<div!ims',$this->page['Title'],$match)) {
        $this->main_plotoutline = trim($match[1]);
      } elseif($fallback) {
        $this->main_plotoutline = $this->storyline();
      }
    }
    return $this->main_plotoutline;
  }

  /** Get the Storyline for the movie
   * @method storyline
   * @return string storyline
   * @see IMDB page / (TitlePage)
   */
  public function storyline() {
    if ($this->main_storyline == "") {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (@preg_match('!Storyline</h2>\s*(.*?)<h4!ims',$this->page["Title"],$match)) {
        if (preg_match('!(.*?)<em class="nobr">Written by!ims',$match[1],$det))
          $this->main_storyline = $det[1];
        elseif (preg_match('!(.*)\s<span class="see-more inline">!ims',$match[1],$det))
          $this->main_storyline = $det[1];
        elseif (preg_match('!(.*)\s\|!ims',$match[1],$det))
          $this->main_storyline = $det[1];
        else $this->main_storyine = trim($match[1]);
      }
    }
    return $this->main_storyline;
  }

 #--------------------------------------------------------[ Photo specific ]---
  /** Setup cover photo (thumbnail and big variant)
   * @method private thumbphoto
   * @return boolean success (TRUE if found, FALSE otherwise)
   * @see IMDB page / (TitlePage)
   */
  private function thumbphoto() {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    preg_match("!id\=\"img_primary\">[^<]*<a[^<]+<img src\=\"(.+?)\"!i",$this->page["Title"],$match);
    if (empty($match[1])) return FALSE;
    $this->main_thumb = $match[1];
    preg_match('|(.*\._V1).*|iUs',$match[1],$mo);
    $this->main_photo = $mo[1];
    return true;
  }


  /** Get cover photo
   * @method photo
   * @param optional boolean thumb get the thumbnail (100x140, default) or the
   *        bigger variant (400x600 - FALSE)
   * @return mixed photo (string url if found, FALSE otherwise)
   * @see IMDB page / (TitlePage)
   */
  public function photo($thumb=true) {
    if (empty($this->main_photo)) $this->thumbphoto();
    if (!$thumb && empty($this->main_photo)) return false;
    if ($thumb && empty($this->main_thumb)) return false;
    if ($thumb) return $this->main_thumb;
    return $this->main_photo;
  }

  /** Save the photo to disk
   * @method savephoto
   * @param string path where to store the file
   * @param optional boolean thumb get the thumbnail (100x140, default) or the
   *        bigger variant (400x600 - FALSE)
   * @return boolean success
   * @see IMDB page / (TitlePage)
   */
  public function savephoto($path,$thumb=true) {
    $req = new MDB_Request("");
    $photo_url = $this->photo ($thumb);
    if (!$photo_url) return FALSE;
    $req->setURL($photo_url);
    $req->sendRequest();
    if (strpos($req->getResponseHeader("Content-Type"),'image/jpeg') === 0
      || strpos($req->getResponseHeader("Content-Type"),'image/gif') === 0
      || strpos($req->getResponseHeader("Content-Type"), 'image/bmp') === 0 ){
    $fp = $req->getResponseBody();
    }else{
    $this->debug_scalar("<BR>*photoerror* ".$photo_url.": Content Type is '".$req->getResponseHeader("Content-Type")."'<BR>");
    return false;
    }
    $fp2 = fopen ($path, "w");
    if ((!$fp) || (!$fp2)){
      $this->debug_scalar("image error...<BR>");
      return false;
    }
    fputs ($fp2, $fp);
    return TRUE;
  }

  /** Get the URL for the movies cover photo
   * @method photo_localurl
   * @param optional boolean thumb get the thumbnail (100x140, default) or the
   *        bigger variant (400x600 - FALSE)
   * @return mixed url (string URL or FALSE if none)
   * @see IMDB page / (TitlePage)
   */
  public function photo_localurl($thumb=true){
    if ($thumb) $ext = ""; else $ext = "_big";
    if (!is_dir($this->photodir)) {
      $this->debug_scalar("<BR>***ERROR*** The configured image directory does not exist!<BR>");
      return false;
    }
    $path = $this->photodir.$this->imdbid()."${ext}.jpg";
    if ( @fopen($path,"r")) return $this->photoroot.$this->imdbid()."${ext}.jpg";
    if (!is_writable($this->photodir)) {
      $this->debug_scalar("<BR>***ERROR*** The configured image directory lacks write permission!<BR>");
      return false;
    }
    if ($this->savephoto($path,$thumb)) return $this->photoroot.$this->imdbid()."${ext}.jpg";
    return false;
  }

  /** Get URLs for the pictures on the main page
   * @method mainPictures
   * @return array [0..n] of [imgsrc, imglink, bigsrc], where<UL>
   *    <LI>imgsrc is the URL of the thumbnail IMG as displayed on main page</LI>
   *    <LI>imglink is the link to the <b><i>page</i></b> with the "big image"</LI>
   *    <LI>bigsrc is the URL of the "big size" image itself</LI>
   * @author moonface
   * @author izzy
   */
  public function mainPictures() {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    if (empty($this->main_pictures)) {
      preg_match('!<div class="mediastrip">\s*(.*?)\s*</div>!ims',$this->page["Title"],$match);
      if (@preg_match_all('!<a .*?href="(.*?)".*?<img.*?src="(.*?)"!ims',$match[1],$matches)) {
        for ($i=0;$i<count($matches[0]);++$i) {
          $this->main_pictures[$i]["imgsrc"] = $matches[2][$i];
          if (substr($matches[1][$i],0,4)!="http") $matches[1][$i] = "http://".$this->imdbsite.$matches[1][$i];
          $this->main_pictures[$i]["imglink"] = $matches[1][$i];
          preg_match('|(.*\._V1).*|iUs',$matches[2][$i],$big);
          $ext = substr($matches[2][$i],-3);
          $this->main_pictures[$i]["bigsrc"] = $big[1].".${ext}";
        }
      }
    }
    return $this->main_pictures;
  }

 #-------------------------------------------------[ Country of Production ]---
  /** Get country of production
   * @method country
   * @return array country (array[0..n] of string)
   * @see IMDB page / (TitlePage)
   */
  public function country() {
   if (empty($this->countries)) {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    $this->countries = array();
    if (preg_match_all("/\/country\/\w+\"\>(.*?)<\/a/m",$this->page["Title"],$matches))
      for ($i=0;$i<count($matches[0]);++$i) $this->countries[$i] = $matches[1][$i];
   }
   return $this->countries;
  }


 #------------------------------------------------------------[ Movie AKAs ]---
  /** Get movies alternative names
   * @method alsoknow
   * @return array aka array[0..n] of array[title,year,country,comment]; searching
   *         on akas.imdb.com will add "lang" (2-char language code) to the array
   *         for localized names, "country" may hold multiple countries separated
   *         by commas
   * @see IMDB page ReleaseInfo
   * @version Due to changes on the IMDB sites, neither the languages nor the year
   *          seems to be available anymore - so those array properties will always
   *          be empty, and kept for compatibility only (for a while, at least).
   *          Moreover, content has been moved from the title page to ReleaseInfo page.
   */
  public function alsoknow() {
   if (empty($this->akas)) {
    if ($this->page["ReleaseInfo"] == "") $this->openpage ("ReleaseInfo");
    $ak_s = strpos ($this->page["ReleaseInfo"], "<a name=\"akas\">");
    //if ($ak_s == 0) $ak_s = strpos ($this->page["ReleaseInfo"], "Alternativ:");
    if ($ak_s == 0) return array();
    $alsoknow_end = strpos ($this->page["ReleaseInfo"], "</table>", $ak_s);
    $alsoknow_all = substr($this->page["ReleaseInfo"], $ak_s, $alsoknow_end - $ak_s);
    preg_match_all("@<td>(.*?)</td>@i", $alsoknow_all, $matches);
    for($i=0;$i<count($matches[1]);$i+=2){
        $title = trim($matches[1][$i]);
        $countries = explode('/',$matches[1][$i+1]);
        foreach($countries as $country){
            $firstbracket = strpos($country, '(');
            if($firstbracket === false){
                $_country = trim($country);
                $comment = '';
            }else{
                $_country = trim(substr($country, 0, $firstbracket));
                preg_match_all("@\((.+?)\)@", $country, $matches3);
                $comment = implode(', ', $matches3[1]);
            }
            $this->akas[] = array(
                "title"=>$title,
                "year"=>'',
                "country"=>$_country,
                "comment"=>$comment,
                "lang"=>''
            );
        }
    }
   }
   return $this->akas;
  }


 #---------------------------------------------------------[ Sound formats ]---
  /** Get sound formats
   * @method sound
   * @return array sound (array[0..n] of strings)
   * @see IMDB page / (TitlePage)
   */
  public function sound() {
   if (empty($this->sound)) {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    if (preg_match_all("|/search/title\?sound_mixes=.*?>\s*(.*?)<|",$this->page["Title"],$matches))
      $this->sound = $matches[1];
   }
   return $this->sound;
  }

 #-------------------------------------------------------[ MPAA / PG / FSK ]---
  /** Get the MPAA data (also known as PG or FSK)
   * @method mpaa
   * @return array mpaa (array[country]=rating)
   * @see IMDB page / (TitlePage)
   */
  public function mpaa() {
   if (empty($this->mpaas)) {
    if ($this->page["ParentalGuide"] == "") $this->openpage("ParentalGuide");
    if (preg_match_all("|/search/title\?certificates=.*?>\s*(.*?):(.*?)<|",$this->page["ParentalGuide"],$matches)) {
      $cc = count($matches[0]);
      for ($i=0;$i<$cc;++$i) $this->mpaas[$matches[1][$i]] = $matches[2][$i];
    }
   }
   return $this->mpaas;
  }

  /** Get the MPAA data (also known as PG or FSK) - including historical data
   * @method mpaa_hist
   * @return array mpaa (array[country][0..n]=rating)
   * @see IMDB page / (TitlePage)
   */
  public function mpaa_hist() {
   if (empty($this->mpaas_hist)) {
    if ($this->page["ParentalGuide"] == "") $this->openpage("ParentalGuide");
    if (preg_match_all("|/search/title\?certificates=.*?>\s*(.*?):(.*?)<|",$this->page["ParentalGuide"],$matches)) {
      $cc = count($matches[0]);
      for ($i=0;$i<$cc;++$i) $this->mpaas_hist[$matches[1][$i]][] = $matches[2][$i];
    }
   }
   return $this->mpaas_hist;
  }

 #----------------------------------------------------[ MPAA justification ]---
  /** Find out the reason for the MPAA rating
   * @method mpaa_reason
   * @return string reason why the movie was rated such
   * @see IMDB page / (TitlePage)
   */
  public function mpaa_reason() {
   if (empty($this->mpaa_justification)) {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    if (preg_match('!href="/mpaa">.*?</h4>\s*(.*?)\s*<span!ims',$this->page["Title"],$match))
      $this->mpaa_justification = trim($match[1]);
   }
   return $this->mpaa_justification;
  }

 #------------------------------------------------------[ Production Notes ]---
  /** For not-yet completed movies, we can get the production state
   * @method prodNotes
   * @return array production notes [status,statnote,lastupdate[day,month,mon,year],more,note]
   * @see IMDB page / (TitlePage)
   */
  public function prodNotes() {
   if (empty($this->main_prodnotes)) {
    if ($this->page["Title"] == "") $this->openpage ("Title");
    if (!preg_match('!(<h2>Production Notes.*?)\s*</div!ims',$this->page["Title"],$match)) return $this->main_prodnotes; // no info available
    if ( preg_match('!<b>Status:\s*</b>\s*(.*?)\s*\|!ims',$match[1],$tmp) )
        $status = trim($tmp[1]); $statnote = trim($tmp[2]);
    if ( preg_match('!<b>Updated:\s*</b>\s*(\d+)\s*(\D+)\s+(\d{4})!ims',$match[1],$tmp) )
        $update = array("day"=>$tmp[1],"month"=>$tmp[2],"mon"=>$this->monthNo($tmp[2]),"year"=>$tmp[3]);
    if ( preg_match('!<b>More Info:\s*</b>\s*(.*)!ims',$match[1],$tmp) )
        $more = preg_replace('!\s*onclick=".*?"!ims','',trim($tmp[1]));
        $more = preg_replace('!href="/!ims','href="http://'.$this->imdbsite.'/',$more);
    if ( preg_match('!<b>Note:\s*</b>\s*(.*?)</!ims',$match[1],$tmp) )
        $note = trim($tmp[1]);
    $this->main_prodnotes = array("status"=>$status,"statnote"=>$statnote,"lastUpdate"=>$update,"more"=>$more,"note"=>$note);
   }
   return $this->main_prodnotes;
  }

 #----------------------------------------------[ Position in the "Top250" ]---
  /** Find the position of a movie in the top 250 ranked movies
   * @method top250
   * @return int position a number between 1..250 if the movie is listed, 0 otherwise
   * @author abe
   * @see http://projects.izzysoft.de/trac/imdbphp/ticket/117
   */
  public function top250() {
    if ($this->main_top250 == -1) {
      if ($this->page["Title"] == "") $this->openpage ("Title");
      if (@preg_match("/\<a href\=\"\/chart\/top\?tt(.*?)\"\>Top 250: #(.*?)\<\/a\>/",$this->page["Title"],$match)) {
        $this->main_top250 = $match[2];
      } else {
        $this->main_top250 = 0;
      }
    }
    return $this->main_top250;
  }


 #=====================================================[ /plotsummary page ]===
 #--------------------------------------------------[ Full Plot (combined) ]---
  /** Get the movies plot(s)
   * @method plot
   * @return array plot (array[0..n] of strings)
   * @see IMDB page /plotsummary
   */
  public function plot() {
   if (empty($this->plot_plot)) {
    if ( $this->page["Plot"] == "" ) $this->openpage ("Plot");
    if ( $this->page["Plot"] == "cannot open page" ) return array(); // no such page
    if (preg_match_all("/p class=\"plotpar\">(.*?)<\/p>/",str_replace("\n"," ",$this->page["Plot"]),$matches))
      for ($i=0;$i<count($matches[0]);++$i)
        $this->plot_plot[$i] = preg_replace('/<a href=\"\/SearchPlotWriters/i','<a href="http://'.$this->imdbsite.'/SearchPlotWriters/',$matches[1][$i]);
   }
   return $this->plot_plot;
  }

 #-----------------------------------------------------[ Full Plot (split) ]---
  /** Get the movie plot(s) - split-up variant
   * @method plot_split
   * @return array array[0..n] of array[string plot,array author] - where author consists of string name and string url
   * @see IMDB page /plotsummary
   */
  public function plot_split() {
    if (empty($this->split_plot)) {
      if (empty($this->plot_plot)) $plots = $this->plot();
      for ($i=0;$i<count($this->plot_plot);++$i) {
        if (preg_match("/(.*?)<i>.*<a href=\"(.*?)\">(.*?)<\/a>/",$this->plot_plot[$i],$match))
          $this->split_plot[] = array("plot"=>$match[1],"author"=>array("name"=>$match[3],"url"=>$match[2]));
      }
    }
    return $this->split_plot;
  }

 #========================================================[ /synopsis page ]===
 #---------------------------------------------------------[ Full Synopsis ]---
  /** Get the movies synopsis
   * @method synopsis
   * @return string synopsis
   * @see IMDB page /synopsis
   */
  public function synopsis() {
    if (empty($this->synopsis_wiki)) {
    if ( $this->page["Synopsis"] == "" ) $this->openpage ("Synopsis");
    if ( $this->page["Synopsis"] == "cannot open page" ) return $this->synopsis_wiki; // no such page
    if (preg_match('|<div id="swiki\.2\.1">(.*?)</div>|ims',$this->page["Synopsis"],$match))
      $this->synopsis_wiki = trim($match[1]);
    }
    return $this->synopsis_wiki;
  }

 #========================================================[ /taglines page ]===
 #--------------------------------------------------------[ Taglines Array ]---
  /** Get all available taglines for the movie
   * @method taglines
   * @return array taglines (array[0..n] of strings)
   * @see IMDB page /taglines
   */
  public function taglines() {
   if (empty($this->taglines)) {
    if ( $this->page["Taglines"] == "" ) $this->openpage ("Taglines");
    if ( $this->page["Taglines"] == "cannot open page" ) return array(); // no such page
    if (preg_match_all("/<p>(.*?)<\/p><hr/",$this->page["Taglines"],$matches))
      $this->taglines = $matches[1];
   }
   return $this->taglines;
  }

 #=====================================================[ /fullcredits page ]===
 #-----------------------------------------------------[ Helper: TableRows ]---
  /** Get rows for a given table on the page
   * @method private get_table_rows
   * @param string html
   * @param string table_start
   * @return mixed rows (FALSE if table not found, array[0..n] of strings otherwise)
   * @see used by the methods director, cast, writing, producer, composer
   */
  private function get_table_rows( $html, $table_start ) {
   $row_s = strpos ( $html, ">".$table_start."<");
   $row_e = $row_s;
   if ( $row_s == 0 )  return FALSE;
   $endtable = strpos($html, "</table>", $row_s);
   if (preg_match_all("/<tr>(.*?)<\/tr>/",substr($html,$row_s,$endtable - $row_s),$matches)) {
     $mc = count($matches[1]);
     for ($i=0;$i<$mc;++$i) if ( strncmp( trim($matches[1][$i]), "<td valign=",10) == 0 ) $rows[] = $matches[1][$i];
   }
   return $rows;
  }

 #------------------------------------------------[ Helper: Cast TableRows ]---
  /** Get rows for the cast table on the page
   * @method private get_table_rows_cast
   * @param string html
   * @param string table_start
   * @return mixed rows (FALSE if table not found, array[0..n] of strings otherwise)
   * @see used by the method cast
   */
  private function get_table_rows_cast( $html, $table_start, $class="nm" ) {
   $row_s = strpos ( $html, '<table class="cast">');
   $row_e = $row_s;
   if ( $row_s == 0 )  return FALSE;
   $endtable = strpos($html, "</table>", $row_s);
   if (preg_match_all("/<tr.*?(<td class=\"$class\".*?)<\/tr>/",substr($html,$row_s,$endtable - $row_s),$matches))
     return $matches[1];
   return array();
  }

 #------------------------------------------------[ Helper: Awards TableRows ]---
  /** Get rows for the awards table on the page
   * @method private get_table_rows_awards
   * @param string html
   * @param string table_start
   * @return mixed rows (FALSE if table not found, array[0..n] of strings otherwise)
   * @see used by the method awards
   * @author Qvist
   */
  private function get_table_rows_awards( $html ) {
   $row_s = strpos ( $html, '<table style="margin-top:' );
   $row_e = $row_s;
   if ( $row_s == 0 )  return FALSE;
   $endtable = strpos($html, "</table>", $row_s);
   $table_string = substr($html,$row_s,$endtable - $row_s);
   if (preg_match_all("/<tr>(.*?)<\/tr>/ims",$table_string,$matches)) {
     return $matches[1];
   }
   return $rows;
  }

 #------------------------------------------------------[ Helper: RowCells ]---
  /** Get content of table row cells
   * @method private get_row_cels
   * @param string row (as returned by imdb::get_table_rows)
   * @return array cells (array[0..n] of strings)
   * @see used by the methods director, cast, writing, producer, composer
   */
  private function get_row_cels( $row ) {
   if (preg_match_all("/<td.*?>(.*?)<\/td>/ims",$row,$matches)) return $matches[1];
   return array();
  }

 #-------------------------------------------[ Helper: Get IMDBID from URL ]---
  /** Get the IMDB ID from a names URL
   * @method private get_imdbname
   * @param string href url to the staff members IMDB page
   * @return string IMDBID of the staff member
   * @see used by the methods director, cast, writing, producer, composer
   */
  private function get_imdbname($href) {
   if ( strlen( $href) == 0) return $href;
   $name_s = 17;
   $name_e = strpos ( $href, '"', $name_s);
   if ( $name_e != 0) return substr( $href, $name_s, $name_e -1 - $name_s);
   else	return $href;
  }

 #-------------------------------------------------------------[ Directors ]---
  /** Get the director(s) of the movie
   * @method director
   * @return array director (array[0..n] of arrays[imdb,name,role])
   * @see IMDB page /fullcredits
   */
  public function director() {
   if (empty($this->credits_director)) {
    if ( $this->page["Credits"] == "" ) $this->openpage ("Credits");
    if ( $this->page["Credits"] == "cannot open page" ) return array(); // no such page
   }
   $director_rows = $this->get_table_rows($this->page["Credits"], "Directed by");
   if($director_rows==null) $director_rows = $this->get_table_rows($this->page["Credits"], "Series Directed by");
   for ( $i = 0; $i < count ($director_rows); $i++){
    $cels = $this->get_row_cels ($director_rows[$i]);
    if (!isset ($cels[0])) return array();
    $dir = array();
    $dir["imdb"] = $this->get_imdbname($cels[0]);
    $dir["name"] = strip_tags($cels[0]);
    $role = trim(strip_tags($cels[2]));
    if ( $role == "") $dir["role"] = NULL;
    else $dir["role"] = $role;
    $this->credits_director[$i] = $dir;
   }
   return $this->credits_director;
  }

 #----------------------------------------------------------------[ Actors ]---
  /** Get the actors
   * @method cast
   * @return array cast (array[0..n] of arrays[imdb,name,role,thumb,photo])
   * @see IMDB page /fullcredits
   */
  public function cast() {
   if (empty($this->credits_cast)) {
    if ( $this->page["Credits"] == "" ) $this->openpage ("Credits");
    if ( $this->page["Credits"] == "cannot open page" ) return array(); // no such page
   }
   $cast_rows = $this->get_table_rows_cast($this->page["Credits"], "Cast", "hs");
   for ( $i = 0; $i < count ($cast_rows); $i++){
    $cels = $this->get_row_cels ($cast_rows[$i]);
    if (!isset ($cels[0])) return array();
    $dir = array();
    $dir["imdb"] = $this->get_imdbname($cels[1]);
    $dir["name"] = strip_tags($cels[1]);
    $role = strip_tags($cels[3]);
    if ( $role == "") $dir["role"] = NULL;
    else $dir["role"] = $role;
    $dir["thumb"] = preg_replace('|.*<img src="(.*?)".*|is','$1',$cels[0]);
    if (strpos($dir["thumb"],'._V1'))
      $dir["photo"] = preg_replace('|(.*._V1)\..+\.(.*)|is','$1.$2',$dir["thumb"]);
    $this->credits_cast[$i] = $dir;
   }
   return $this->credits_cast;
  }


 #---------------------------------------------------------------[ Writers ]---
  /** Get the writer(s)
   * @method writing
   * @return array writers (array[0..n] of arrays[imdb,name,role])
   * @see IMDB page /fullcredits
   */
  public function writing() {
   if (empty($this->credits_writing)) {
    if ( $this->page["Credits"] == "" ) $this->openpage ("Credits");
    if ( $this->page["Credits"] == "cannot open page" ) return array(); // no such page
   }
   $this->credits_writing = array();
   $writing_rows = $this->get_table_rows($this->page["Credits"], "Writing credits");
   for ( $i = 0; $i < count ($writing_rows); $i++){
     $cels = $this->get_row_cels ($writing_rows[$i]);
     if ( count ( $cels) > 2){
       $wrt = array();
       $wrt["imdb"] = $this->get_imdbname($cels[0]);
       $wrt["name"] = strip_tags($cels[0]);
       $role = strip_tags($cels[2]);
       if ( $role == "") $wrt["role"] = NULL;
       else $wrt["role"] = $role;
       $this->credits_writing[$i] = $wrt;
     }
   }
   return $this->credits_writing;
  }

 #-------------------------------------------------------------[ Producers ]---
  /** Obtain the producer(s)
   * @method producer
   * @return array producer (array[0..n] of arrays[imdb,name,role])
   * @see IMDB page /fullcredits
   */
  public function producer() {
   if (empty($this->credits_producer)) {
    if ( $this->page["Credits"] == "" ) $this->openpage ("Credits");
    if ( $this->page["Credits"] == "cannot open page" ) return array(); // no such page
   }
   $this->credits_producer = array();
   $producer_rows = $this->get_table_rows($this->page["Credits"], "Produced by");
   for ( $i = 0; $i < count ($producer_rows); $i++){
    $cels = $this->get_row_cels ($producer_rows[$i]);
    if ( count ( $cels) > 2){
     $wrt = array();
     $wrt["imdb"] = $this->get_imdbname($cels[0]);
     $wrt["name"] = strip_tags($cels[0]);
     $role = strip_tags($cels[2]);
     if ( $role == "") $wrt["role"] = NULL;
     else $wrt["role"] = $role;
     $this->credits_producer[$i] = $wrt;
    }
   }
   return $this->credits_producer;
  }

 #-------------------------------------------------------------[ Composers ]---
  /** Obtain the composer(s) ("Original Music by...")
   * @method composer
   * @return array composer (array[0..n] of arrays[imdb,name,role])
   * @see IMDB page /fullcredits
   */
  public function composer() {
   if (empty($this->credits_composer)) {
    if ( $this->page["Credits"] == "" ) $this->openpage ("Credits");
    if ( $this->page["Credits"] == "cannot open page" ) return array(); // no such page
   }
   $this->credits_composer = array();
   $composer_rows = $this->get_table_rows($this->page["Credits"], "Original Music by");
   for ( $i = 0; $i < count ($composer_rows); $i++){
    $cels = $this->get_row_cels ($composer_rows[$i]);
    if ( count ( $cels) > 2){
     $wrt = array();
     $wrt["imdb"] = $this->get_imdbname($cels[0]);
     $wrt["name"] = strip_tags($cels[0]);
     $role = strip_tags($cels[2]);
     if ( $role == "") $wrt["role"] = NULL;
     else $wrt["role"] = $role;
     $this->credits_composer[$i] = $wrt;
    }
   }
   return $this->credits_composer;
  }

 #====================================================[ /crazycredits page ]===
 #----------------------------------------------------[ CrazyCredits Array ]---
  /** Get the Crazy Credits
   * @method crazy_credits
   * @return array crazy_credits (array[0..n] of string)
   * @see IMDB page /crazycredits
   */
  public function crazy_credits() {
    if (empty($this->crazy_credits)) {
      if (empty($this->page["CrazyCredits"])) $this->openpage("CrazyCredits");
      if ( $this->page["CrazyCredits"] == "cannot open page" ) return array(); // no such page
      $tag_s = strpos ($this->page["CrazyCredits"],"<li><tt>");
      $tag_e = strpos ($this->page["CrazyCredits"],"</ul>",$tag_s);
      $cred  = str_replace ("<br>"," ",substr ($this->page["CrazyCredits"],$tag_s, $tag_e - $tag_s));
      $cred  = str_replace ("  "," ",str_replace ("\n"," ",$cred));
      if (preg_match_all("/<li><tt>(.*?)<\/tt><\/li>/",$cred,$matches))
        $this->crazy_credits = $matches[1];
    }
    return $this->crazy_credits;
  }

 #========================================================[ /episodes page ]===
 #--------------------------------------------------------[ Episodes Array ]---
  /** Get the series episode(s)
   * @method episodes
   * @return array episodes (array[0..n] of array[0..m] of array[imdbid,title,airdate,plot])
   * @see IMDB page /episodes
   */
  public function episodes() {
    if ( $this->seasons() == 0 ) return $this->season_episodes;
    if ( empty($this->season_episodes) ) {
      if ( $this->page["Episodes"] == "" ) $this->openpage("Episodes");
      if ( $this->page["Episodes"] == "cannot open page" ) return array(); // no such page
      if ( preg_match_all('!<h3>Season (\d+), Episode (\d+): <a href="/title/tt(\d{7})/">(.*)</a></h3><span.*>Original Air Date.*<strong>(.*)</strong></span><br>\s*(.*)(<h5>|</td>)!Ui',$this->page["Episodes"],$matches) ) {
	for ( $i = 0 ; $i < count($matches[0]); $i++ ) {
          $this->season_episodes[$matches[1][$i]][$matches[2][$i]] = array("imdbid" => $matches[3][$i],"title" => $matches[4][$i], "airdate" => $matches[5][$i], "plot" => $matches[6][$i]);
        }
      }
    }
    return $this->season_episodes;
  }

 #===========================================================[ /goofs page ]===
 #-----------------------------------------------------------[ Goofs Array ]---
  /** Get the goofs
   * @method goofs
   * @return array goofs (array[0..n] of array[type,content]
   * @see IMDB page /goofs
   */
  public function goofs() {
    if (empty($this->goofs)) {
      if (empty($this->page["Goofs"])) $this->openpage("Goofs");
      if ($this->page["Goofs"] == "cannot open page") return array(); // no such page
      $tag_s = strpos($this->page["Goofs"],'<ul class="trivia">');
      $tag_e = strrpos($this->page["Goofs"],'<ul class="trivia">'); // maybe more than one
      $tag_e = strrpos($this->page["Goofs"],"</ul>");
      $goofs = substr($this->page["Goofs"],$tag_s,$tag_e - $tag_s);
      if (preg_match_all("/<li>.*?<b>(.*?)<\/b>(.*?)<br>\s*<br><\/li>/ims",$goofs,$matches)) {
        $gc = count($matches[1]);
        for ($i=0;$i<$gc;++$i) $this->goofs[] = array("type"=>$matches[1][$i],"content"=>$matches[2][$i]);
      }
    }
    return $this->goofs;
  }

 #==========================================================[ /quotes page ]===
 #----------------------------------------------------------[ Quotes Array ]---
  /** Get the quotes for a given movie
   * @method quotes
   * @return array quotes (array[0..n] of string)
   * @see IMDB page /quotes
   */
  public function quotes() {
    if ( empty($this->moviequotes) ) {
      if ( $this->page["Quotes"] == "" ) $this->openpage("Quotes");
      if ( $this->page["Quotes"] == "cannot open page" ) return array(); // no such page
      if (preg_match_all('|<a name="qt.*?</a>\s*(.*?)<p class="linksoda"|ims',str_replace("\n"," ",$this->page["Quotes"]),$matches))
        foreach ($matches[1] as $match) $this->moviequotes[] = str_replace('href="/name/','href="http://'.$this->imdbsite.'/name/',$match);
    }
    return $this->moviequotes;
  }

 #========================================================[ /trailers page ]===
 #--------------------------------------------------------[ Trailers Array ]---
  /** Get the trailer URLs for a given movie
   * @method trailers
   * @param optional boolean full Retrieve all available data (TRUE), or stay compatible with previous IMDBPHP versions (FALSE, Default)
   * @return mixed trailers either array[0..n] of string ($full=FALSE), or array[0..n] of array[lang,title,url,restful_url ($full=TRUE)
   * @see IMDB page /trailers
   */
  public function trailers($full=FALSE) {
    if ( empty($this->trailers) ) {
      if ( $this->page["Trailers"] == "" ) $this->openpage("Trailers");
      if ( $this->page["Trailers"] == "cannot open page" ) return array(); // no such page
      $tag_s = strpos($this->page["Trailers"], '<div id="search-results">');
      if (!empty($tag_s)) { // trailers on the IMDB site itself
        $tag_e = strpos($this->page["Trailers"],"</ol>",$tag_s);
        $trail = substr($this->page["Trailers"], $tag_s, $tag_e - $tag_s +1);
        if (preg_match_all('|<a href="{0,1}(/video/screenplay/vi\d*/).*?title="(.*?)"|ims',$trail,$matches))
          for ($i=0;$i<count($matches[0]);++$i) {
            $trailer = "http://".$this->imdbsite.$matches[1][$i];
            if ( $full ) $this->trailers[] = array("lang"=>'',"title"=>$matches[2][$i],"url"=>$trailer,"restful_url"=>'');
            else $this->trailers[] = $trailer;
        }
      }
    }
    return $this->trailers;
  }

 #===========================================================[ /videosites ]===
 #------------------------------------------[ Off-site trailers and videos ]---
  /** Get the off-site videos and trailer URLs
   * @method videosites
   * @return array videosites array[0..n] of array(site,url,type,desc)
   * @see IMDB page /videosites
   */
  public function videosites() {
    if ( empty($this->video_sites) ) {
      if ( $this->page["VideoSites"] == "" ) $this->openpage("VideoSites");
      if ( $this->page["VideoSites"] == "cannot open page" ) return array(); // no such page
      if ( preg_match('|<h3[^>]*>\s*Trailers on Other Sites\s*</h3>(.*?)<hr|ims',$this->page["VideoSites"],$match) ) {
        preg_match_all('!<p[^>]*>(.*?)\s*\((.*?)\)\s*\(<a href="(.*?)">(.*?)</a>\)\s*</p!ims',$match[1],$matches);
        for ($i=0;$i<count($matches[0]);++$i) {
          $this->video_sites[] = array("site"=>$matches[4][$i],"url"=>$matches[3][$i],"type"=>$matches[2][$i],"desc"=>$matches[1][$i]);
        }
      }
    }
    return $this->video_sites;
  }


 #==========================================================[ /trivia page ]===
 #----------------------------------------------------------[ Trivia Array ]---
  /** Get the trivia info
   * @method trivia
   * @return array trivia (array[0..n] string
   * @see IMDB page /trivia
   */
  public function trivia() {
    if (empty($this->trivia)) {
      if (empty($this->page["Trivia"])) $this->openpage("Trivia");
      if ($this->page["Trivia"] == "cannot open page") return array(); // no such page
      if ( preg_match_all('!<div class="sodatext">\s*(.*?)\s<br>!ims',$this->page["Trivia"],$matches) ) {
        $gc = count($matches[1]);
        for ($i=0;$i<$gc;++$i) $this->trivia[] = str_replace('href="/','href="http://'.$this->imdbsite."/",$matches[1][$i]);
      }
    }
    return $this->trivia;
  }

 #======================================================[ /soundtrack page ]===
 #------------------------------------------------------[ Soundtrack Array ]---
  /** Get the soundtrack listing
   * @method soundtrack
   * @return array soundtracks (array[0..n] of array(soundtrack,array[0..n] of credits)
   * @brief Usually, the credits array should hold [0] written by, [1] performed by.
   *  But IMDB does not always stick to that - so in many cases it holds
   *  [0] performed by, [1] courtesy of
   * @see IMDB page /soundtrack
   */
  public function soundtrack() {
   if (empty($this->soundtracks)) {
    if (empty($this->page["Soundtrack"])) $this->openpage("Soundtrack");
    if ($this->page["Soundtrack"] == "cannot open page") return array(); // no such page
    if (preg_match('|<p class="disclaimer">(.*?)</script|ims',$this->page["Soundtrack"],$match)) {
      if (preg_match_all("/<li>(.*?)<\/b><br>(.*?)<br>(.*?)<br>.*?<\/li>/",str_replace("\n"," ",$match[1]),$matches)) {
        $mc = count($matches[0]);
        for ($i=0;$i<$mc;++$i) $this->soundtracks[] = array("soundtrack"=>$matches[1][$i],"credits"=>array(
                                                             str_replace('href="/','href="http://'.$this->imdbsite.'/',$matches[2][$i]),
                                                             str_replace('href="/','href="http://'.$this->imdbsite.'/',$matches[3][$i])));
      }
    }
   }
   return $this->soundtracks;
  }

 #=================================================[ /movieconnection page ]===
 #----------------------------------------[ Helper: ConnectionBlock Parser ]---
  /** Parse connection block (used by method movieconnection only)
   * @method private parseConnection
   * @param string conn connection type
   * @return array [0..n] of array mid,name,year,comment - or empty array if not found
   */
  private function parseConnection($conn) {
    $tag_s = strpos($this->page["MovieConnections"],"<h5>$conn</h5>");
    if (empty($tag_s)) return array(); // no such feature
    $tag_e = strpos($this->page["MovieConnections"],"<h5>",$tag_s+4);
    if (empty($tag_e)) $tag_e = strpos($this->page["MovieConnections"],"<hr/><h3>",$tag_s);
    $block = substr($this->page["MovieConnections"],$tag_s,$tag_e-$tag_s);
    if (preg_match_all("/\<a href=\"(.*?)\"\>(.*?)\<\/a\> \((\d{4})\)(.*\<br\/\>\&nbsp;-\&nbsp;(.*))?/",$block,$matches)) {
      $mc = count($matches[0]);
      for ($i=0;$i<$mc;++$i) {
        $mid = substr($matches[1][$i],9,strlen($matches[1][$i])-10); // isolate imdb id from url
        $arr[] = array("mid"=>$mid, "name"=>$matches[2][$i], "year"=>$matches[3][$i], "comment"=>$matches[5][$i]);
      }
    }
    return $arr;
  }

 #-------------------------------------------------[ MovieConnection Array ]---
  /** Get connected movie information
   * @method movieconnection
   * @return array connections (versionOf, editedInto, followedBy, spinOff,
   *         spinOffFrom, references, referenced, features, featured, spoofs,
   *         spoofed - each an array of mid, name, year, comment or an empty
   *         array if no connections of that type)
   * @see IMDB page /movieconnection
   */
  public function movieconnection() {
    if (empty($this->movieconnections)) {
      if (empty($this->page["MovieConnections"])) $this->openpage("MovieConnections");
      if ($this->page["MovieConnections"] == "cannot open page") return array(); // no such page
      $this->movieconnections["editedFrom"] = $this->parseConnection("Edited from");
      $this->movieconnections["editedInto"] = $this->parseConnection("Edited into");
      $this->movieconnections["featured"]   = $this->parseConnection("Featured in");
      $this->movieconnections["features"]   = $this->parseConnection("Features");
      $this->movieconnections["followedBy"] = $this->parseConnection("Followed by");
      $this->movieconnections["follows"]    = $this->parseConnection("Follows");
      $this->movieconnections["references"] = $this->parseConnection("References");
      $this->movieconnections["referenced"] = $this->parseConnection("Referenced in");
      $this->movieconnections["remadeAs"]   = $this->parseConnection("Remade as");
      $this->movieconnections["remakeOf"]   = $this->parseConnection("Remake of");
      $this->movieconnections["spinOff"]    = $this->parseConnection("Spin off");
      $this->movieconnections["spinOffFrom"] = $this->parseConnection("Spin off from");
      $this->movieconnections["spoofed"]    = $this->parseConnection("Spoofed in");
      $this->movieconnections["spoofs"]     = $this->parseConnection("Spoofs");
      $this->movieconnections["versionOf"]  = $this->parseConnection("Version of");
    }
    return $this->movieconnections;
  }

 #=================================================[ /externalreviews page ]===
 #-------------------------------------------------[ ExternalReviews Array ]---
  /** Get list of external reviews (if any)
   * @method extReviews
   * @return array [0..n] of array [url, desc] (or empty array if no data)
   * @see IMDB page /externalreviews
   */
  public function extReviews() {
    if (empty($this->extreviews)) {
      if (empty($this->page["ExtReviews"])) $this->openpage("ExtReviews");
      if ($this->page["ExtReviews"] == "cannot open page") return array(); // no such page
      if (preg_match_all("/\<li\>\<a href=\"(.*?)\"\>(.*?)\<\/a\>/",$this->page["ExtReviews"],$matches)) {
        $mc = count($matches[0]);
        for ($i=0;$i<$mc;++$i) {
          $this->extreviews[$i] = array("url"=>$matches[1][$i], "desc"=>$matches[2][$i]);
        }
      }
    }
    return $this->extreviews;
  }

 #=====================================================[ /releaseinfo page ]===
 #-----------------------------------------------------[ ReleaseInfo Array ]---
  /** Obtain Release Info (if any)
   * @method releaseInfo
   * @return array release_info array[0..n] of strings (country,day,month,mon,
             year,comment) - "month" is the month name, "mon" the number
   * @see IMDB page /releaseinfo
   */
  public function releaseInfo() {
    if (empty($this->release_info)) {
      if (empty($this->page["ReleaseInfo"])) $this->openpage("ReleaseInfo");
      if ($this->page["ReleaseInfo"] == "cannot open page") return array(); // no such page
      $tag_s = strpos($this->page["ReleaseInfo"],'<th class="xxxx">Country</th><th class="xxxx">Date</th>');
      $tag_e = strpos($this->page["ReleaseInfo"],'</table',$tag_s);
      $block = substr($this->page["ReleaseInfo"],$tag_s,$tag_e-$tag_s);
      preg_match_all('!<tr><td><b>(.*?)</b></td>\s*<td[^>]*>(.*?)</td>\s*<td>(.*?)</td>!ims',$block,$matches);
      $mc = count($matches[0]);
      for ($i=0;$i<$mc;++$i) {
        $country = strip_tags($matches[1][$i]);
        if ( preg_match('!href="/date/(\d{2})-(\d{2})/">\d+ (.*?)</a>\s*<a href="/year/(\d{4})/">!is',$matches[2][$i],$match) ) { // full info
          $this->release_info[] = array('country'=>$country,'day'=>$match[2],'month'=>$match[3],'mon'=>$match[1],'year'=>$match[4],'comment'=>$matches[3][$i]);
        } elseif ( !preg_match('|a href=|i',$matches[2][$i],$match) ) { // no links within
          if ( preg_match('!^(.+?)\s(\d{4})$!s',trim($matches[2][$i]),$match) ) { // month and year
            $this->release_info[] = array('country'=>$country,'day'=>'','month'=>$match[1],'mon'=>$this->monthNo(trim($match[1])),'year'=>$match[2],'comment'=>$matches[3][$i]);
          } elseif ( preg_match('!(\d{4})!',trim($matches[2][$i]),$match) ) { // year at least
            $this->release_info[] = array('country'=>$country,'day'=>'','month'=>'','mon'=>'','year'=>$match[1],'comment'=>$matches[3][$i]);
          }
        } else {
          $this->debug_scalar("NO MATCH ON<pre>".htmlentities($matches[2][$i])."</pre>");
        }
      }
    }
    return $this->release_info;
  }

 #=======================================================[ /locations page ]===
  /** Obtain filming locations
   * @method locations
   * @return array locations array[0..n] of array[name,url] with name being the
   *               name of the location, and url a relative URL to list other
   *               movies sharing this location
   * @see IMDB page /locations
   */
  public function locations() {
    if ( empty($this->locations) ) {
      if ( empty($this->page['Locations']) ) $this->openpage("Locations");
      if ( $this->page['Locations'] == "cannot open page" ) return array(); // no such page
      $tag_s = strpos($this->page['Locations'],'<div id="tn15adrhs">');
      $tag_e = strpos($this->page['Locations'],'</dl>');
      $block = substr($this->page['Locations'],$tag_s,$tag_e-$tag_s);
      $block = substr($block,strpos($block,'<dl>'));
      if ( preg_match_all('!<dt>(<a href="(.*?)">|)(.*?)(</a>|</dt>)!ims',$block,$matches) ) {
        for ($i=0;$i<count($matches[0]);++$i)
          $this->locations[] = array('name'=>$matches[3][$i], 'url'=>$matches[2][$i]);
      }
    }
    return $this->locations;
  }

 #==================================================[ /companycredits page ]===
 #---------------------------------------------[ Helper: Parse CompanyInfo ]---
  /** Parse company info
   * @method private companyParse
   * @param ref string text to parse
   * @param ref array parse target
   */
  public function companyParse(&$text,&$target) {
    preg_match_all('|<li><a href="(.*)">(.*)</a>(.*)</li>|iUms',$text,$matches);
    $mc = count($matches[0]);
    for ($i=0;$i<$mc;++$i) {
      $target[] = array("name"=>$matches[2][$i], "url"=>$matches[1][$i], "notes"=>$matches[3][$i]);
    }
  }

 #---------------------------------------------------[ Producing Companies ]---
  /** Info about Production Companies
   * @method prodCompany
   * @return array [0..n] of array (name,url,notes)
   * @see IMDB page /companycredits
   */
  public function prodCompany() {
    if (empty($this->compcred_prod)) {
      if (empty($this->page["CompanyCredits"])) $this->openpage("CompanyCredits");
      if ($this->page["CompanyCredits"] == "cannot open page") return array(); // no such page
      if (preg_match('|<h2>Production Companies</h2><ul>(.*?)</ul>|ims',$this->page["CompanyCredits"],$match)) {
        $this->companyParse($match[1],$this->compcred_prod);
      }
    }
    return $this->compcred_prod;
  }

 #------------------------------------------------[ Distributing Companies ]---
  /** Info about distributors
   * @method distCompany
   * @return array [0..n] of array (name,url,notes)
   * @see IMDB page /companycredits
   */
  public function distCompany() {
    if (empty($this->compcred_dist)) {
      if (empty($this->page["CompanyCredits"])) $this->openpage("CompanyCredits");
      if ($this->page["CompanyCredits"] == "cannot open page") return array(); // no such page
      if (preg_match('|<h2>Distributors</h2><ul>(.*?)</ul>|ims',$this->page["CompanyCredits"],$match)) {
        $this->companyParse($match[1],$this->compcred_dist);
      }
    }
    return $this->compcred_dist;
  }

 #---------------------------------------------[ Special Effects Companies ]---
  /** Info about Special Effects companies
   * @method specialCompany
   * @return array [0..n] of array (name,url,notes)
   * @see IMDB page /companycredits
   */
  public function specialCompany() {
    if (empty($this->compcred_special)) {
      if (empty($this->page["CompanyCredits"])) $this->openpage("CompanyCredits");
      if ($this->page["CompanyCredits"] == "cannot open page") return array(); // no such page
      if (preg_match('|<h2>Special Effects</h2><ul>(.*?)</ul>|ims',$this->page["CompanyCredits"],$match)) {
        $this->companyParse($match[1],$this->compcred_special);
      }
    }
    return $this->compcred_special;
  }

 #-------------------------------------------------------[ Other Companies ]---
  /** Info about other companies
   * @method otherCompany
   * @return array [0..n] of array (name,url,notes)
   * @see IMDB page /companycredits
   */
  public function otherCompany() {
    if (empty($this->compcred_other)) {
      if (empty($this->page["CompanyCredits"])) $this->openpage("CompanyCredits");
      if ($this->page["CompanyCredits"] == "cannot open page") return array(); // no such page
      if (preg_match('|<h2>Other Companies</h2><ul>(.*?)</ul>|ims',$this->page["CompanyCredits"],$match)) {
        $this->companyParse($match[1],$this->compcred_other);
      }
    }
    return $this->compcred_other;
  }

 #===================================================[ /parentalguide page ]===
 #-------------------------------------------------[ ParentalGuide Details ]---
  /** Detailed Parental Guide
   * @method parentalGuide
   * @return array of strings; keys: Alcohol, Sex, Violence, Profanity,
   *         Frightening - and maybe more; values: arguments for the rating
   * @see IMDB page /parentalguide
   */
  public function parentalGuide() {
    if (empty($this->parental_guide)) {
      if (empty($this->page["ParentalGuide"])) $this->openpage("ParentalGuide");
      if ($this->page["ParentalGuide"] == "cannot open page") return array(); // no such page
      if (preg_match_all('/<div class="section">(.*)<div id="swiki(\.\d+\.\d+|_last)">/iUms',$this->page["ParentalGuide"],$matches)) {
        $mc = count($matches[0]);
	for ($i=0;$i<$mc;++$i) {
	  if ( !preg_match('|<span>(.*)</span>|iUms',$matches[1][$i],$match) ) continue;
	  $section = $match[1];
	  preg_match('|<p id="swiki\.\d+\.\d+\.\d+">(.*)</p>|iUms',$matches[1][$i],$match);
	  $content = trim($match[1]);
	  preg_match('/^(.*)(\s|\/)/U',$section,$match);
	  $sgot = $match[1]; if (empty($sgot)) $sgot = $section;
	  switch($sgot) {
	    case "Alcohol"    : $this->parental_guide["Drugs"] = trim($content); break;
	    case "Sex"        :
	    case "Violence"   :
	    case "Profanity"  :
	    case "Frightening":
	    default           : $this->parental_guide[$sgot] = trim($content); break;
	  }
	}
      }
    }
    return $this->parental_guide;
  }

 #===================================================[ /officialsites page ]===
 #---------------------------------------------------[ Official Sites URLs ]---
  /** URLs of Official Sites
   * @method officialSites
   * @return array [0..n] of url, name
   * @see IMDB page /officialsites
   */
  public function officialSites() {
    if (empty($this->official_sites)) {
      if (empty($this->page["OfficialSites"])) $this->openpage("OfficialSites");
      if ($this->page["OfficialSites"] == "cannot open page") return array(); // no such page
      preg_match('|official sites for(.*?)related links|ims',$this->page["OfficialSites"],$match);
      if (preg_match_all('|<li><a href="(.*?)">(.*?)</a></li>|ims',$match[1],$matches)) {
        $mc = count($matches[0]);
        for ($i=0;$i<$mc;++$i) {
          $this->official_sites[] = array("url"=>$matches[1][$i],"name"=>$matches[2][$i]);
        }
      }
    }
    return $this->official_sites;
  }

 #========================================================[ /keywords page ]===
 #--------------------------------------------------------------[ Keywords ]---
  /** Get the complete keywords for the movie
   * @method keywords_all
   * @return array keywords
   * @see IMDB page /keywords
   */
  function keywords_all() {
    if (empty($this->all_keywords)) {
      if ($this->page["Keywords"] == "") $this->openpage("Keywords");
      if (preg_match_all('|<li><b class="keyword">\s*<a href\="/keyword\/[\w\-]+/">(.*?)</a>|',$this->page["Keywords"],$matches))
        $this->all_keywords = $matches[1];
    }
    return $this->all_keywords;
  }

  #========================================================[ /awards page ]===
  #--------------------------------------------------------------[ Awards ]---
  /** Get the complete awards for the movie
   * @method awards
   * @return array awards array[festivalName]['entries'][0..n] of array[year,won,category,award,people,comment]
   * @see IMDB page /awards
   * @author Qvist
   * @brief array[festivalName] is array[name,entries] - where name is a string,
   *        and entries is above described array
   */
  public function awards() {
    if (empty($this->awards)) {
      if ($this->page["Awards"] == "") $this->openpage("Awards");
      $award_rows = $this->get_table_rows_awards($this->page["Awards"]);
      $rowcount = count ($award_rows);
      $festival = ""; $year = 0; $won = false; $award = ""; $comment = ""; $people = array(); $nr = 0;
      for ( $i = 0; $i < $rowcount; $i++){
        $cels = $this->get_row_cels ($award_rows[$i]);
        if( count ($cels) == 0 ){ continue; }
        if( count ($cels) == 1 && preg_match( '|<big><a href\="/Sections/Awards/([^\/]+)/">(.*?)</a></big>|s', $cels[0], $matches ) ){
            $festival = $matches[1];
            $this->awards[$festival]['name'] = $matches[2];
            $nr = 0;
        }
        if( count ($cels) == 4 && preg_match( '|<a href\="/Sections/Awards/'.quotemeta( $festival ).'/[\d-]+">(\d{4}) </a>|s', $cels[0], $matches ) ){
            $year = $matches[1];
            array_shift( $cels );
        }
        if( count ($cels) == 3 && preg_match( '|<b>(.*?)</b>|s', $cels[0], $matches ) ){
            $won = ($matches[1]=="Won")?true:false;
            array_shift( $cels );
        }
        if( count ($cels) == 2 && strpos( $cels[0], "<" ) === false ){
            $award = $cels[0];
            array_shift( $cels );
        }
        if( count ($cels) == 1 && preg_match( '|([^<]*)<br>(.*)<small>|s', $cels[0], $matches ) ){
            $category = trim( $matches[1] );
            preg_match_all( '|<a href\="/name/nm(\d{7})/">(.*?)</a>|s', $matches[2], $matches );
            $people = isset( $matches[0][0] )?array_combine( $matches[1], $matches[2] ):array();
            preg_match( '|<small>(.*?)</small>|s', $cels[0], $matches );
            $comment = isset( $matches[1] )?strip_tags( $matches[1] ):'';
            array_shift( $cels );
            $nr++;
        }
        if( count ($cels) == 0 ){
            $this->awards[$festival]['entries'][$nr] = array(
                'year' => $year, 'won' => $won, 'category' => $category, 'award' => $award, 'people' => $people, 'comment' => $comment );
        }
      }
    }
    return $this->awards;
  }


 } // end class imdb

?>