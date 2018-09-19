#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
 
const char* ssid = "******";
const char* password = "*******";
IPAddress ip(192, 168, 1, 69); // IP do aparelho
IPAddress gateway(192, 168, 1, 254); // IP do router
IPAddress subnet(255, 255, 255, 0);
 
const int router = 2; // D4 // Pin do Relay
const int BInLED = LED_BUILTIN;
const int maxTentativas = 4;
int tentativas = 0;
ESP8266WebServer server(80);

unsigned long Timer;
bool block = false;

const char* html = "<!doctype html>\n\
<html>\n\
<head>\n\
  <meta charset=\"utf-8\">\n\
  <meta http-equiv=\"x-ua-compatible\" content=\"ie=edge\">\n\
  <title>Home Lock</title>\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=0, shrink-to-fit=no\">\n\
  <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Roboto:400,100,300,500,700,900\">\n\
  <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/normalize/8.0.0/normalize.min.css\">\n\
  <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/animate.css/3.2.3/animate.min.css\">\n\
  <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.3.1/css/regular.css\" integrity=\"sha384-ZlNfXjxAqKFWCwMwQFGhmMh3i89dWDnaFU2/VZg9CvsMGA7hXHQsPIqS+JIAmgEq\" crossorigin=\"anonymous\">\n\
  <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.3.1/css/fontawesome.css\" integrity=\"sha384-1rquJLNOM3ijoueaaeS5m+McXPJCGdr5HcA03/VHXxcp2kX2sUrQDmFc3jR5i/C7\" crossorigin=\"anonymous\">\n\
  <style>\n\
    html, body { width: 100%; height: 100%;}.patt-holder{background:#3382c0;-ms-touch-action:none}.patt-wrap{position:relative;cursor:pointer}.patt-wrap li,.patt-wrap ul{list-style:none;margin:0;padding:0}.patt-circ{position:relative;float:left;box-sizing:border-box;-moz-box-sizing:border-box}.patt-circ.hovered{border:3px solid #090}.patt-error .patt-circ.hovered{border:3px solid #BA1B26}.patt-hidden .patt-circ.hovered{border:0}.patt-dots,.patt-lines{border-radius:5px;height:10px;position:absolute}.patt-dots{background:#FFF;width:10px;top:50%;left:50%;margin-top:-5px;margin-left:-5px}.patt-lines{background:rgba(255,255,255,.7);transform-origin:5px 5px;-ms-transform-origin:5px 5px;-webkit-transform-origin:5px 5px}.patt-hidden .patt-lines{display:none}.mhn-ui-date-time,.text-center{text-align:center}*,:after,:before{box-sizing:border-box}.pull-left{float:left}.pull-right{float:right}.clearfix:after,.clearfix:before{content:'';display:table}.clearfix:after{clear:both;display:block}body{margin:0;color:#fff;background:#eee;font:300 14px/18px Roboto,sans-serif}a{color:inherit;text-decoration:none}a:hover{text-decoration:underline}.mhn-ui-wrap{width:100%;height:100%;overflow:hidden;position:relative;margin:0 auto;background:#2c3e50}.mhn-ui-wrap:before{top:0;left:0;right:0;bottom:0;content:'';position:absolute;background:rgba(0,0,0,.4)}.mhn-lock-wrap{z-index:100;position:relative;margin-top:15%}.mhn-lock-wrap .mhn-lock-title{text-align:center;text-shadow:0 1px 1px rgba(0,0,0,.5);height:20px;line-height:20px;font-size:1.2em;margin-bottom:15px}.mhn-lock-wrap .mhn-lock-success{color:#64ff69;text-shadow:none}.mhn-lock-wrap .mhn-lock-failure{color:#f34235}.mhn-lock{margin:auto;background:0 0}.patt-wrap{margin:auto;overflow:hidden}.patt-wrap li{transition:all .4s ease-in-out 0s}.patt-dots,.patt-lines{transition:background .1s ease-in-out 0s}.patt-circ{border:3px solid transparent}.patt-dots{background:rgba(255,255,255,.8)}.patt-lines{background:rgba(255,255,255,.4)}.patt-circ.hovered{border-color:#ddd;background:rgba(255,255,255,.2)}.patt-error .patt-circ.hovered{background:rgba(243,66,53,.4);border-color:rgba(243,66,53,.8)}.patt-error .patt-lines{background:rgba(243,66,53,.5)}.patt-success .patt-circ.hovered{background:rgba(75,174,79,.4);border-color:rgba(75,174,79,.8)}.patt-success .patt-lines{background:rgba(75,174,79,.5)}.mhn-lock-failure{animation:zoomIn .4s}.patt-circ:nth-child(1),.patt-circ:nth-child(2),.patt-circ:nth-child(3),.patt-circ:nth-child(4){animation:fadeInUp .4s}.patt-circ:nth-child(5),.patt-circ:nth-child(6),.patt-circ:nth-child(7),.patt-circ:nth-child(8){animation:fadeInUp .6s}.patt-circ:nth-child(10),.patt-circ:nth-child(11),.patt-circ:nth-child(12),.patt-circ:nth-child(9){animation:fadeInUp .8s}.patt-circ:nth-child(13),.patt-circ:nth-child(14),.patt-circ:nth-child(15),.patt-circ:nth-child(16){animation:fadeInUp 1s}.mhn-lock-title,.patt-circ:nth-child(17),.patt-circ:nth-child(18),.patt-circ:nth-child(19),.patt-circ:nth-child(20){animation:fadeInUp 1.2s}.invis .patt-circ.hovered,.invis .patt-lines{background:0 0!important;border-color:transparent!important;color:transparent!important}#toggle{position:fixed;color:#d0d2d5;font-size:30px;top:5px;right:10px;z-index:999;padding:10px;cursor:pointer}\n\
  </style>\n\
</head>\n\
<body>\n\
    <span id=\"toggle\"><i class=\"far fa-eye\"></i></span>\n\
    <div class=\"mhn-ui-wrap\">\n\
        <div class=\"mhn-lock-wrap\">\n\
            <div class=\"mhn-lock-title\" data-title=\"Desenhe o padrão\"></div>\n\
            <div id=\"lock\" class=\"mhn-lock\"></div>\n\
        </div>\n\
    </div>\n\
  <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>\n\
  <script>\n\
    !function(t,e,n,a){\"use strict\";function r(t){for(var e=t.holder,n=t.option,a=n.matrix,r=n.margin,i=n.radius,o=['<ul class=\"patt-wrap\" style=\"padding:'+r+'px\">'],s=0,l=a[0]*a[1];l>s;s++)o.push('<li class=\"patt-circ\" style=\"margin:'+r+\"px; width : \"+2*i+\"px; height : \"+2*i+\"px; -webkit-border-radius: \"+i+\"px; -moz-border-radius: \"+i+\"px; border-radius: \"+i+'px; \"><div class=\"patt-dots\"></div></li>');o.push(\"</ul>\"),e.html(o.join(\"\")).css({width:a[1]*(2*i+2*r)+2*r+\"px\",height:a[0]*(2*i+2*r)+2*r+\"px\"}),t.pattCircle=t.holder.find(\".patt-circ\")}function i(t,e,n,a){var r=e-t,i=a-n;return{length:Math.ceil(Math.sqrt(r*r+i*i)),angle:Math.round(180*Math.atan2(i,r)/Math.PI)}}function o(){}function s(e,n){var a=this,i=a.token=Math.random(),h=p[i]=new o,u=h.holder=t(e);if(0!=u.length){h.object=a,n=h.option=t.extend({},s.defaults,n),r(h),u.addClass(\"patt-holder\"),\"static\"==u.css(\"position\")&&u.css(\"position\",\"relative\"),u.on(\"touchstart mousedown\",function(t){d.call(this,t,a)}),h.option.onDraw=n.onDraw||l;var c=n.mapper;h.mapperFunc=\"object\"==typeof c?function(t){return c[t]}:\"function\"==typeof c?c:l,h.option.mapper=null}}var l=function(){},p={},d=function(e,a){e.preventDefault();var r=p[a.token];if(!r.disabled){r.option.patternVisible||r.holder.addClass(\"patt-hidden\");var i=\"touchstart\"==e.type?\"touchmove\":\"mousemove\",o=\"touchstart\"==e.type?\"touchend\":\"mouseup\";t(this).on(i+\".pattern-move\",function(t){h.call(this,t,a)}),t(n).one(o,function(){u.call(this,e,a)});var s=r.holder.find(\".patt-wrap\"),l=s.offset();r.wrapTop=l.top,r.wrapLeft=l.left,a.reset()}},h=function(e,n){e.preventDefault();var a=e.pageX||e.originalEvent.touches[0].pageX,r=e.pageY||e.originalEvent.touches[0].pageY,o=p[n.token],s=o.pattCircle,l=o.patternAry,d=o.option.lineOnMove,h=o.getIdxFromPoint(a,r),u=h.idx,c=o.mapperFunc(u)||u;if(l.length>0){var f=i(o.lineX1,h.x,o.lineY1,h.y);o.line.css({width:f.length+10+\"px\",transform:\"rotate(\"+f.angle+\"deg)\"})}if(u){if(-1==l.indexOf(c)){var v,m=t(s[u-1]);if(o.lastPosObj){for(var g=o.lastPosObj,x=g.i,w=g.j,b=Math.abs(h.i-x),j=Math.abs(h.j-w);(0==b&&j>1||0==j&&b>1||j==b&&j>1)&&(w!=h.j||x!=h.i);){x=b?Math.min(h.i,x)+1:x,w=j?Math.min(h.j,w)+1:w,b=Math.abs(h.i-x),j=Math.abs(h.j-w);var M=(w-1)*o.option.matrix[1]+x,y=o.mapperFunc(M)||M;-1==l.indexOf(y)&&(t(s[M-1]).addClass(\"hovered\"),l.push(y))}v=[],h.j-g.j>0?v.push(\"s\"):h.j-g.j<0?v.push(\"n\"):0,h.i-g.i>0?v.push(\"e\"):h.i-g.i<0?v.push(\"w\"):0,v=v.join(\"-\")}m.addClass(\"hovered\"),l.push(c);var P=o.option.margin,k=o.option.radius,C=(h.i-1)*(2*P+2*k)+2*P+k,O=(h.j-1)*(2*P+2*k)+2*P+k;if(1!=l.length){var D=i(o.lineX1,C,o.lineY1,O);o.line.css({width:D.length+10+\"px\",transform:\"rotate(\"+D.angle+\"deg)\"}),d||o.line.show()}v&&(o.lastElm.addClass(v+\" dir\"),o.line.addClass(v+\" dir\"));var E=t('<div class=\"patt-lines\" style=\"top:'+(O-5)+\"px; left:\"+(C-5)+'px\"></div>');o.line=E,o.lineX1=C,o.lineY1=O,o.holder.append(E),d||o.line.hide(),o.lastElm=m}o.lastPosObj=h}},u=function(t,e){t.preventDefault();var n=p[e.token],a=n.patternAry.join(\"\");n.holder.off(\".pattern-move\").removeClass(\"patt-hidden\"),a&&(n.option.onDraw(a),n.line.remove(),n.rightPattern&&(a==n.rightPattern?n.onSuccess():(n.onError(),e.error())))};o.prototype={constructor:o,getIdxFromPoint:function(t,e){var n=this.option,a=n.matrix,r=t-this.wrapLeft,i=e-this.wrapTop,o=null,s=n.margin,l=2*n.radius+2*s,p=Math.ceil(r/l),d=Math.ceil(i/l),h=r%l,u=i%l;return p<=a[1]&&d<=a[0]&&h>2*s&&u>2*s&&(o=(d-1)*a[1]+p),{idx:o,i:p,j:d,x:r,y:i}}},s.prototype={constructor:s,option:function(t,e){var n=p[this.token],i=n.option;return e===a?i[t]:(i[t]=e,void((\"margin\"==t||\"matrix\"==t||\"radius\"==t)&&r(n)))},getPattern:function(){return p[this.token].patternAry.join(\"\")},setPattern:function(t){var e=p[this.token],n=e.option,a=n.matrix,r=n.margin,i=n.radius;if(n.enableSetPattern){this.reset(),e.wrapLeft=0,e.wrapTop=0;for(var o=0;o<t.length;o++){var s=t[o]-1,d=s%a[1],u=Math.floor(s/a[1]),c=d*(2*r+2*i)+2*r+i,f=u*(2*r+2*i)+2*r+i;h.call(null,{pageX:c,pageY:f,preventDefault:l,originalEvent:{touches:[{pageX:c,pageY:f}]}},this)}}},enable:function(){var t=p[this.token];t.disabled=!1},disable:function(){var t=p[this.token];t.disabled=!0},reset:function(){var t=p[this.token];t.pattCircle.removeClass(\"hovered dir s n w e s-w s-e n-w n-e\"),t.holder.find(\".patt-lines\").remove(),t.patternAry=[],t.lastPosObj=null,t.holder.removeClass(\"patt-error patt-success\")},error:function(){p[this.token].holder.addClass(\"patt-error\")},checkForPattern:function(t,e,n){var a=p[this.token];a.rightPattern=t,a.onSuccess=e||l,a.onError=n||l}},s.defaults={matrix:[3,3],margin:20,radius:25,patternVisible:!0,lineOnMove:!0,enableSetPattern:!1},e.PatternLock=s}(jQuery,window,document);\n\
    var message = function() {\n\
        $(\".mhn-lock-title span\").fadeOut(function() {\n\
            $(\".mhn-lock-title\").html($(\".mhn-lock-title\").data(\"title\"));\n\
            $(\".mhn-lock-title span\").fadeIn();\n\
        });\n\
    }\n\
    var sucesso  = function() {\n\
        $(\".mhn-lock-title\").html('<span class=\"mhn-lock-success\">Porta destrancada</span>');\n\
        $(\".patt-holder\").removeClass(\"patt-error\").addClass(\"patt-success\");\n\
        setTimeout(function() {\n\
            pattern.reset();\n\
            message();\n\
        }, 1e3);\n\
    }\n\
    var falha = function(res) {\n\
        var texto = \"Padrão errado...\";\n\
        if(res == '-1') texto = \"Bloqueado por 30 min...\";\n\
        $(\".mhn-lock-title\").html('<span class=\"mhn-lock-failure\">'+texto+'</span>');\n\
        $(\".patt-holder\").removeClass(\"patt-success\").addClass(\"patt-error\");\n\
        setTimeout(function() {\n\
            pattern.reset();\n\
            message();\n\
        }, res == '-1' ? 3e3 : 1e3);\n\
    }\n\
    $(function(){\n\
        pattern = new PatternLock(\".mhn-lock\", {\n\
            matrix: [5,4],\n\
            margin: 10,\n\
            radius: 30,\n\
            allowRepeat: true,\n\
            lineOnMove: true,\n\
            onDraw:function(padrao){\n\
                $.ajax({\n\
                  url:'/unlock?code='+padrao,\n\
                  method: 'GET',\n\
                  timeout: 2000,\n\
                  complete: function(data, status) {\n\
                    if(status === 'success') {\n\
                      if (data.responseText === '1') sucesso();\n\
                      else falha(data.responseText);\n\
                    } else {\n\
                      falha('-1');\n\
                    }\n\
                  }\n\
                });\n\
            }\n\
        });\n\
        $(\".mhn-lock-title\").html($(\".mhn-lock-title\").data(\"title\"));\n\
        $(\"#toggle\").on(\"click\", function() {\n\
            var icon = $(this).find(\"i\");\n\
            var lock = $(\"#lock\");\n\
            if(icon.hasClass(\"fa-eye\")) {\n\
                icon.removeClass(\"fa-eye\").addClass(\"fa-eye-slash\");\n\
                lock.addClass(\"invis\");\n\
            } else {\n\
                icon.removeClass(\"fa-eye-slash\").addClass(\"fa-eye\");\n\
                lock.removeClass(\"invis\");\n\
            }\n\
        });\n\
    });\n\
  </script>\n\
</body>\n\
</html>";

void handleRoot() {
  server.send(200, "text/html", html);
}

void setup() {
  //pinMode(router, OUTPUT);
  pinMode(BInLED, OUTPUT);
  //digitalWrite(router, LOW);
  digitalWrite(BInLED, 1);

  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/unlock", [](){
    if(block) {
      server.send(500, "text/plain", "-1");
    } else {
      String code=server.arg("code");
      if(code == "*****") {
        tentativas = 0;
        //digitalWrite(router, HIGH);
        digitalWrite(BInLED, 0);
        delay(500);
        //digitalWrite(router, LOW);
        digitalWrite(BInLED, 1);
        server.send(200, "text/plain", "1");
      } else {
        tentativas += 1;
        if(tentativas < maxTentativas) {
          server.send(200, "text/plain", "0");
        } else {
          server.send(500, "text/plain", "-1");
          tentativas = 0;
          Timer = millis();
          block = true;
          //delay(1000 * 60 * 30);
          //depois fazer o pedido para me avisar
        }
      }
    }
  });

  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop() {
  if(block) {
    if(millis()-Timer >= 1800000UL) block = false;
  }
  server.handleClient();
}
 





 
