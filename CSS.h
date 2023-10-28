String webpage = ""; //String to save the html code

void append_page_header() {
  webpage  = F("<!DOCTYPE html><html>");
  webpage += F("<head>");
  webpage += F("<title>"); // NOTE: 1em = 16px
  webpage += servername; // NOTE: 1em = 16px
  webpage += F(" - "); // NOTE: 1em = 16px
  webpage += sName; // NOTE: 1em = 16px
  webpage += F("</title>"); // NOTE: 1em = 16px
  webpage += refreshSite;
  webpage += F("<meta name='viewport' content='user-scalable=yes,initial-scale=1.0,width=device-width'>");
  webpage += F("<style>");//From here style:
  webpage += F("body{max-width:600px;margin:0.2em auto;font-family:arial;font-size:100%;background-color:white;border:0.1em solid #000000;border-radius:0.375em;padding:0.2em 0.2em;}");
  webpage += F("ul{list-style-type:none;margin:0.2em 0em 0.2em 0em;padding:0;border:0.06em solid #000000;border-radius:0.375em;overflow:hidden;background-color:#d90707;font-size:1em;text-align:center;}");
  webpage += F("li{float:left;border-radius:0.375em;border:0.06em solid #000;}");
  webpage += F("li a{color:white; display: block;border-radius:0.375em;padding:0.2em 0.2em;text-decoration:none;font-size:100%}");
  webpage += F("li a:hover{background-color:#e86b6b;border-radius:0.375em;border:0.06em solid #000;font-size:100%}");
  webpage += F("h1{font-size:1.5em;text-align:center;color:white;margin:0em 0em 0em 0em;border:0.06em solid #000000;border-radius:0.375em;padding:0em 0em;background:#d90707;}");
  webpage += F("h2{text-align:center;margin:0em 0em 0.2em 0em;background-color:yellow;border:0.06em solid #000000;border-radius:0.375em;padding:0.2em 0.2em;font-size:0.9em;}");
  webpage += F("h3{margin:margin:0em 0em 0.2em 0em;;}");
  webpage += F("table{font-family:arial,sans-serif;font-size:0.9em;border:0.06em solid #000000;border-radius:0.375em;width:85%;}"); 
  webpage += F("th {text-align:left;padding:0.1em;}"); 
  webpage += F("td {border-radius:0.375em;border:0.06em solid #000000;text-align:left;padding:0.3em;}"); 
  webpage += F("td.test {text-align:left;padding:0.1em;width:10%;border:0em solid #000000;}"); 
  webpage += F("tr:nth-child(odd) {background-color:#eeeeee;}");
  webpage += F(".rcorners_n {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:20%;color:white;font-size:75%;}");
  webpage += F(".rcorners_m {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:50%;color:white;font-size:75%;}");
  webpage += F(".rcorners_w {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:70%;color:white;font-size:75%;}");
  webpage += F(".column{float:left;width:50%;height:45%;}");
  webpage += F(".row:after{content:'';display:table;clear:both;}");
  webpage += F("*{box-sizing:border-box;}");
  webpage += F("a{color:black;font-weight: normal;padding:0.2em 0.2em;text-decoration:none;background-color:#cccccc;border:0.06em solid #000000;border-radius:0.375em;}");
  webpage += F("a:hover{color:black;font-weight: normal;padding:0.2em 0.2em;text-decoration:none;background-color:#eeeeee;border:0.06em solid #000000;border-radius:0.375em;}");
  webpage += F("button{font-size:1em;color:black;font-weight: normal;padding:0.2em 0.2em;text-decoration:none;background-color:#cccccc;border:0.06em solid #000000;border-radius:0.375em;}");
  webpage += F("button:hover{font-size:1em;color:black;font-weight: normal;padding:0.2em 0.2em;text-decoration:none;background-color:#eeeeee;border:0.06em solid #000000;border-radius:0.375em;}");
  webpage += F("p{}");
  webpage += F("h1.leiste{font-size:1em;font-weight: normal;text-align:center;color:#000000;margin:0em 0em 0em 0em;border:0.06em solid #000000;border-radius:0.375em;padding:0.2em 0.2em;background:#cccccc;}");
  webpage += F("h1.footer{position: relative;width:100%;font-size:1em;font-weight: normal;text-align:right;color:#000000;margin:0em 0em 0em 0em;border:0.06em solid #000000;border-radius:0.375em;padding:0.2em 0.2em;background:#cccccc;}");
  webpage += F("p.status{position: absolute;top: -28px;right:0px;width:40px;text-align:center;margin:0em 0em 0em 0em;background-color:yellow;border:0.06em solid #000000;border-radius:0.375em;padding:0.2em 0.2em;font-size:0.9em;}");
  webpage += F("p.footer{font-size:1em;position: absolute;top: 0px;left:-0.2em;text-align:center;margin:0em 0em 0em 0em;background-color:#cccccc;padding:0.2em 0.2em;}");
  webpage += F("p.status1{text-align:center;margin:0em 0em 0em 0em;padding:0.2em 0.2em;font-size:0.9em;}");
  webpage += F("div.inhalt{position: relative;width:100%;height:500px;text-align:center;margin:0em 0em 0em 0em;background-color:#f6f5f4;border:0.06em solid #000000;border-radius:0.375em;padding:0.2em 0.2em;}");
  webpage += F("a.lileiste{border:0.00em solid #000000;color:red;font-weight: normal;padding:0.2em 0.2em;text-decoration:none;}");
  webpage += F("a:hover.lileiste{border:0.00em solid #000000;color:red;font-weight: normal;padding:0.2em 0.2em;text-decoration:underline;}");
  webpage += F("a.foot{color:red;font-weight: normal;padding:0.2em 0.2em;text-decoration:none;border:0.06em solid #000000;border-radius:0.375em;}");
  webpage += F("a:hover.foot{color:red;font-weight: normal;padding:0.2em 0.2em;text-decoration:underline;border:0.06em solid #000000;border-radius:0.375em;}");
  webpage += F("</style></head><body><h1>");
  webpage += servername; // NOTE: 1em = 16px
  webpage += F(" - "); // NOTE: 1em = 16px
  webpage += sName; // NOTE: 1em = 16px
  webpage += F("</h1>");
//  webpage += F("<ul>");
//  webpage += F("<li><a href='/'>Start</a></li>");
//  webpage += F("<li><a href='/files'>Files</a></li>"); //Menu bar with commands
//  webpage += F("<li><a href='/upload'>Upload</a></li>"); 
//  webpage += F("</ul>");
  webpage += F("<h1 class='leiste'>");
  webpage += F("<a class='lileiste' href='/'>Start</a> | ");
  webpage += F("<a class='lileiste' href='/files'>Files</a> | "); //Menu bar with commands
  webpage += F("<a class='lileiste' href='/upload'>Upload</a>"); 
  webpage += F("</h1>");
}
//Saves repeating many lines of code for HTML page footers
void append_page_footer()
{ 
  webpage += F("<h1 class='footer'><p class='footer'><a class='foot' href='/config'>System Config</a>  |  <a class='foot' href='/newstart'>!RESTART SYSTEM!</a></p>");
  webpage += F("&copy;opyleft by zero&trade;");
  webpage += F("</h1>");
  webpage += F("</body></html>");
}
