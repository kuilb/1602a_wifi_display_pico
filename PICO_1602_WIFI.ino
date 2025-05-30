#include <WiFi.h>
#include <map>
//以下是引脚定义
#define LCD_RS 2                    //数据/指令切换引脚
#define LCD_E 3                     //触发引脚
#define LCD_D4 11                   //四位数据引脚
#define LCD_D5 10
#define LCD_D6 9
#define LCD_D7 8
#define LED_LIGHT 6                 //外接LED提示灯
#define BLA 4                       //背光PWM控制

#define BOTTEN 16                   //按钮引脚

//以下是命令定义
#define CMD 0                       //命令为0
#define CHR 1                       //数据为1
#define char_per_line 16            //每行16个字符
#define LCD_line1 0x80              //第一行地址
#define LCD_line2 0xc0              //第二行地址

const char* ssid = "";              //此处填入你的WiFi SSID
const char* password = "";          //此处填入你的WiFi密码
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;      //连接超时时间2S
String header;
String input = "";
String co="123456789ABCDEF1";
String co2="123456789ABCDEF1";
String IP1="IP:";
char temp[35];
int tail = 0;
int counter=0;
WiFiServer server(80);              //80端口设置服务器
std::map<int, String> kanaMap;      //假名映射表
using namespace std;

void initKanaMap() {
  // あ行
  kanaMap[354] = "\xb1"; kanaMap[450] = "\xb1";  // あ
  kanaMap[356] = "\xb2"; kanaMap[452] = "\xb2";  // い
  kanaMap[358] = "\xb3"; kanaMap[454] = "\xb3";  // う
  kanaMap[360] = "\xb4"; kanaMap[456] = "\xb4";  // え
  kanaMap[362] = "\xb5"; kanaMap[458] = "\xb5";  // お

  // か行
  kanaMap[363] = "\xb6"; kanaMap[459] = "\xb6";  // か
  kanaMap[365] = "\xb7"; kanaMap[461] = "\xb7";  // き
  kanaMap[367] = "\xb8"; kanaMap[463] = "\xb8";  // く
  kanaMap[369] = "\xb9"; kanaMap[465] = "\xb9";  // け
  kanaMap[371] = "\xba"; kanaMap[467] = "\xba";  // こ

  // が行（浊音）
  kanaMap[364] = "\xb6\xde"; kanaMap[460] = "\xb6\xde";  // が
  kanaMap[366] = "\xb7\xde"; kanaMap[462] = "\xb7\xde";  // ぎ
  kanaMap[368] = "\xb8\xde"; kanaMap[464] = "\xb8\xde";  // ぐ
  kanaMap[370] = "\xb9\xde"; kanaMap[466] = "\xb9\xde";  // げ
  kanaMap[372] = "\xba\xde"; kanaMap[468] = "\xba\xde";  // ご

  // さ行
  kanaMap[373] = "\xbb"; kanaMap[469] = "\xbb";  // さ
  kanaMap[375] = "\xbc"; kanaMap[471] = "\xbc";  // し
  kanaMap[377] = "\xbd"; kanaMap[473] = "\xbd";  // す
  kanaMap[379] = "\xbe"; kanaMap[475] = "\xbe";  // せ
  kanaMap[381] = "\xbf"; kanaMap[477] = "\xbf";  // そ

  // ざ行（浊音）
  kanaMap[374] = "\xbb\xde"; kanaMap[470] = "\xbb\xde";  // ざ
  kanaMap[376] = "\xbc\xde"; kanaMap[472] = "\xbc\xde";  // じ
  kanaMap[378] = "\xbd\xde"; kanaMap[474] = "\xbd\xde";  // ず
  kanaMap[380] = "\xbe\xde"; kanaMap[476] = "\xbe\xde";  // ぜ
  kanaMap[382] = "\xbf\xde"; kanaMap[478] = "\xbf\xde";  // ぞ

  // た行
  kanaMap[383] = "\xc0"; kanaMap[479] = "\xc0";  // た
  kanaMap[385] = "\xc1"; kanaMap[481] = "\xc1";  // ち
  kanaMap[387] = "\xaf"; kanaMap[483] = "\xaf";  // っ
  kanaMap[388] = "\xc2"; kanaMap[484] = "\xc2";  // つ
  kanaMap[390] = "\xc3"; kanaMap[486] = "\xc3";  // て
  kanaMap[392] = "\xc4"; kanaMap[488] = "\xc4";  // と

  // だ行（浊音）
  kanaMap[384] = "\xc0\xde"; kanaMap[480] = "\xc0\xde";  // だ
  kanaMap[386] = "\xc1\xde"; kanaMap[482] = "\xc1\xde";  // ぢ
  kanaMap[388] = "\xc2\xde"; kanaMap[484] = "\xc2\xde";  // づ
  kanaMap[390] = "\xc3\xde"; kanaMap[486] = "\xc3\xde";  // で
  kanaMap[392] = "\xc4\xde"; kanaMap[488] = "\xc4\xde";  // ど

  // な行
  kanaMap[394] = "\xc5"; kanaMap[490] = "\xc5";  // な
  kanaMap[395] = "\xc6"; kanaMap[491] = "\xc6";  // に
  kanaMap[396] = "\xc7"; kanaMap[492] = "\xc7";  // ぬ
  kanaMap[397] = "\xc8"; kanaMap[493] = "\xc8";  // ね
  kanaMap[398] = "\xc9"; kanaMap[494] = "\xc9";  // の

  // は行
  kanaMap[399] = "\xca"; kanaMap[495] = "\xca";  // は
  kanaMap[402] = "\xcb"; kanaMap[498] = "\xcb";  // ひ
  kanaMap[405] = "\xcc"; kanaMap[501] = "\xcc";  // ふ
  kanaMap[408] = "\xcd"; kanaMap[504] = "\xcd";  // へ
  kanaMap[411] = "\xce"; kanaMap[507] = "\xce";  // ほ

  // ば行（浊音）
  kanaMap[400] = "\xca\xde"; kanaMap[496] = "\xca\xde";  // ば
  kanaMap[403] = "\xcb\xde"; kanaMap[499] = "\xcb\xde";  // び
  kanaMap[406] = "\xcd\xde"; kanaMap[502] = "\xcc\xde";  // ぶ
  kanaMap[409] = "\xcd\xde"; kanaMap[505] = "\xcd\xde";  // べ
  kanaMap[412] = "\xce\xde"; kanaMap[508] = "\xce\xde";  // ぼ

  // ぱ行（半浊音）
  kanaMap[401] = "\xca\xdf"; kanaMap[497] = "\xca\xdf";  // ぱ
  kanaMap[404] = "\xcb\xdf"; kanaMap[500] = "\xcb\xdf";  // ぴ
  kanaMap[407] = "\xcc\xdf"; kanaMap[503] = "\xcc\xdf";  // ぷ
  kanaMap[410] = "\xcd\xdf"; kanaMap[506] = "\xcd\xdf";  // ぺ
  kanaMap[413] = "\xce\xdf"; kanaMap[509] = "\xce\xdf";  // ぽ

  // ま行
  kanaMap[414] = "\xcf"; kanaMap[510] = "\xcf";  // ま
  kanaMap[415] = "\xd0"; kanaMap[511] = "\xd0";  // み
  kanaMap[416] = "\xd1"; kanaMap[512] = "\xd1";  // む
  kanaMap[417] = "\xd2"; kanaMap[513] = "\xd2";  // め
  kanaMap[418] = "\xd3"; kanaMap[514] = "\xd3";  // も

  // や行
  kanaMap[420] = "\xd4"; kanaMap[516] = "\xd4";  // や
  kanaMap[422] = "\xd5"; kanaMap[518] = "\xd5";  // ゆ
  kanaMap[424] = "\xd6"; kanaMap[520] = "\xd6";  // よ

  // ら行
  kanaMap[425] = "\xd7"; kanaMap[521] = "\xd7";  // ら
  kanaMap[426] = "\xd8"; kanaMap[522] = "\xd8";  // り
  kanaMap[427] = "\xd9"; kanaMap[523] = "\xd9";  // る
  kanaMap[428] = "\xda"; kanaMap[524] = "\xda";  // れ
  kanaMap[429] = "\xdb"; kanaMap[535] = "\xdb";  // ろ

  // わ行・ん
  kanaMap[431] = "\xdc"; kanaMap[527] = "\xdd";  // わ
  kanaMap[434] = "\xa6"; kanaMap[530] = "\xde";  // を
  kanaMap[435] = "\xdd"; kanaMap[531] = "\xdf";  // ん

  // 标点
  kanaMap[540] = "\xb0"; // ー
  kanaMap[290] = "\xa1"; // 。
  kanaMap[300] = "\xa2"; // 「
  kanaMap[301] = "\xa3"; // 」
  kanaMap[289] = "\xa4"; // 、
  kanaMap[539] = "\xa5"; // ・
}

void init(){                        //初始化引脚
    pinMode(LCD_RS,GPIO_OUT);
    pinMode(LCD_E,GPIO_OUT);
    pinMode(LCD_D4,OUTPUT);
    pinMode(LCD_D5,GPIO_OUT);
    pinMode(LCD_D6,GPIO_OUT);
    pinMode(LCD_D7,GPIO_OUT);
    pinMode(LED_LIGHT,GPIO_OUT);
    pinMode(BOTTEN,INPUT);

    analogWrite(BLA, 255);
}

void triggle_E(){                     //触发E引脚
    delay(1);
    digitalWrite(LCD_E,HIGH);
    delay(1);
    digitalWrite(LCD_E,LOW);
    delay(1);
}

void gpio_write(int data,int mode){   //写入一个数据或指令

    //设定模式(指令/字符)
    if(mode==0){
        digitalWrite(LCD_RS,LOW);
    }
    else if(mode==1){
        digitalWrite(LCD_RS,HIGH);
    }

    //前四位
    digitalWrite(LCD_D7,LOW);
    digitalWrite(LCD_D6,LOW);
    digitalWrite(LCD_D5,LOW);
    digitalWrite(LCD_D4,LOW);

    if((data &0x80) == 0x80){
        digitalWrite(LCD_D7,HIGH);
    }
    if((data &0x40) == 0x40){
        digitalWrite(LCD_D6,HIGH);
    }
    if((data & 0x20) == 0x20){
        digitalWrite(LCD_D5,HIGH);
    }
    if((data & 0x10) == 0x10){
        digitalWrite(LCD_D4,HIGH);
    }
    triggle_E();

    //后四位
    digitalWrite(LCD_D7,LOW);
    digitalWrite(LCD_D6,LOW);
    digitalWrite(LCD_D5,LOW);
    digitalWrite(LCD_D4,LOW);
    
    if((data & 0x08) == 0x08){
        digitalWrite(LCD_D7,HIGH);
    }
    if((data & 0x04) == 0x04){
        digitalWrite(LCD_D6,HIGH);
    }
    if((data & 0x02) == 0x02){
        digitalWrite(LCD_D5,HIGH);
    }
    if((data & 0x01) == 0x01){
        digitalWrite(LCD_D4,HIGH);
    }
    triggle_E();
}

void lcd_init(){                        //初始化1602函数
    gpio_write(0x33,CMD);               //
    delay(50);
    gpio_write(0x32,CMD);               //
    delay(50);
    gpio_write(0x06,CMD);               //设置向右写入字符，设置屏幕内容不滚动
    delay(50);
    gpio_write(0x0C,CMD);               //开启屏幕显示，关闭光标显示，关闭光标闪烁
    delay(50);
    gpio_write(0x28,CMD);               //设定数据总线为四位，显示2行字符，使用5*8字符点阵
    delay(50);
    gpio_write(0x01,CMD);               //清屏
    delay(50);
}

void lcd_text(String ltext,int line){   //显示函数
    gpio_write(line,CMD);               //命令写入:行数
    int tsize=ltext.length();
    for(int size=0;size<16;size++){     //逐字写入
        if(size>tsize-1){
            gpio_write(0x20,CHR);       //若字符串大小小于16，则填充空格
        }
        else{
            gpio_write(int(ltext[size]),CHR); //转换成RAW编码后写入
        }
    }
}

void setup() {
    Serial.begin(115200);
    init();
    initKanaMap();                              //初始化假名表
    lcd_init();
    lcd_text("RaspberryPI PICO",LCD_line1);     //欢迎消息
    lcd_text("1602a WIFI Test",LCD_line2);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);            //点灯
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);

    lcd_text("WIFI connecting",LCD_line1);
    lcd_text(" ",LCD_line2);
    Serial.print("RaspberryPI pico WH");
    WiFi.begin(ssid,password);                  //在屏幕上显示状态
  
    while (WiFi.status() != WL_CONNECTED) {     //连接WiFi
        lcd_text("WIFI connecting",LCD_line1);
        lcd_text(" ",LCD_line2);
        WiFi.begin(ssid,password);
        delay(5000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Time:");
    Serial.println(currentTime);
    Serial.print("WiFi connected at IP Address ");
    Serial.println(WiFi.localIP());
    
    String IP = WiFi.localIP().toString().c_str();
    lcd_text("WIFI connected",LCD_line1);
    IP1.concat(IP);
    lcd_text(IP1,LCD_line2);                    //在屏幕上显示ip

    server.begin();
}

String get_kana_ascii(int utf8){      //假名utf转换成RAW字符函数,查表
  String raw_kana="  ";               //返回用变量
  if (kanaMap.count(utf8)) return kanaMap[utf8];

  return raw_kana;
}

void loop(){
    WiFiClient client = server.accept();        //初始化客户端
    digitalWrite(LED_BUILTIN, LOW);
    if(WiFi.status() != WL_CONNECTED) {         //未连接WiFi则点灯
        digitalWrite(LED_LIGHT, HIGH);
    }
    else{
        digitalWrite(LED_LIGHT, LOW);
    }
    
    if(digitalRead(BOTTEN)==1 && counter==0){   //短按按钮显示IP
        digitalWrite(LED_BUILTIN, HIGH);        //按住是点灯
        lcd_text(ssid,LCD_line1);
        lcd_text(IP1,LCD_line2);
        counter++;
    }
    else if(digitalRead(BOTTEN)==1){            //长按开始计时
        digitalWrite(LED_BUILTIN, HIGH);
        if(counter%50==0)
          Serial.println(counter);

        counter++;
    }


    else if(digitalRead(BOTTEN)==0){            //按键松开清零计时器
        digitalWrite(LED_BUILTIN, LOW);
        counter=0;
    }
    
    if (counter==300){                           //300时刻清屏
        lcd_text(" ",LCD_line1);
        lcd_text(" ",LCD_line2);
    } 
    if (counter==700){                           //调整背光亮度
        lcd_text("Brightness",LCD_line1);
        lcd_text("0%",LCD_line2);
        analogWrite(BLA, 0);
    }
    if (counter==1000){
        lcd_text("Brightness",LCD_line1);
        lcd_text("25%",LCD_line2);
        analogWrite(BLA, 64);
    }
    if (counter==1300){
        lcd_text("Brightness",LCD_line1);
        lcd_text("50%",LCD_line2);
        analogWrite(BLA, 128);
    }
    if (counter==1600){
        lcd_text("Brightness",LCD_line1);
        lcd_text("75%",LCD_line2);
        analogWrite(BLA, 191);
    }
    if (counter==1900){
        lcd_text("Brightness",LCD_line1);
        lcd_text("100%",LCD_line2);
        analogWrite(BLA, 255);
    }


    if (counter==4000){                         //程序消息
        lcd_text("by KULIB 24/12/10",LCD_line1);
        lcd_text("code in arduino",LCD_line2);
    }

    if (client){                       
        currentTime = millis();
        previousTime = currentTime;
        Serial.print("Time:");
        Serial.println(currentTime);
        Serial.println("New Client.");
        String currentLine = "";
        while (client.connected() && currentTime-previousTime<=timeoutTime){
            currentTime = millis();
            if (client.available()){
                char c = client.read();
                
                header += c;
                if (c == '\n'){          
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();
      
                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
            
                        // CSS style
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".inputbox{ width:300px; height:35px; text-align:left;line-height:30px; font-size: 20px;}");
                        client.println("</style></head>");
      
                        client.println("<h1>Raspberry Pico WH 1602A display Control</h1>");
                        client.println("<form method=\"get\" action=\"\">");
                        client.println("<input type=\"text\" class=\"inputbox\" id=\"input\" name=\"inputbox\" placeholder=\"input something\" maxlength=\"32\">");
                        client.println("<p><input type=\"submit\" class=\"button\"/></p>");
   
                        client.println("</form></body></html>");
      
                        client.println();
                        
                        break;
                    } 
                    else{
                        Serial.println(currentLine);
                        if(currentLine.indexOf("GET /?inputbox=") >= 0){    //获取输入
                            digitalWrite(LED_LIGHT, HIGH);
                            Serial.println("FIND!");
                            tail = currentLine.indexOf(" HTTP/1.1");
                            input = currentLine.substring(15,tail);
                        }
                        currentLine = "";
                    }
                } 
                else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.print("input is {");
        Serial.print(input);
        Serial.println("}");

        //--------------------测试用代码-------------------
        // String teststr="                                ";
        // int s=0;
        // for(int i=0;i<=256;i++){
        //   teststr[s]=i;
        //   s++;
        //   if(s%32==0){
        //     s=0;
        //     //Serial.println(teststr);
        //   }
        // }
        //--------------------测试用代码-------------------

        int index0;                       //搜索用坐标
        String input_index0="";           //假名UTF8前子串
        String input_index1="";           //假名UTF8后子串
        index0=input.indexOf("%26%2312"); //假名UTF8特征前缀

        while(input.indexOf("%26%2312")!=-1){   //当输入存在假名时
          index0=input.indexOf("%26%2312");
          int kana_utf8=input.substring(index0+8,index0+11).toInt(); //三位假名utf8编码
          String raw_kana=get_kana_ascii(kana_utf8);                 //假名RAW编码

          Serial.print("utf8 kana:"); 
          Serial.print(kana_utf8);                  //调试   输出当前假名编码值
          Serial.print("  RAW kana:"); 
          Serial.println(raw_kana);                 //调试   输出RAW假名

          input_index0=input.substring(0,index0);
          input_index1=input.substring(index0+14,input.length());

          Serial.print("index0:");
          Serial.print(input_index0);
          Serial.print("  index1:");
          Serial.print(input_index1);
          
          if(input_index0!=0){      //防止index0=0时多一个空格
            if(raw_kana[1]==' ')    //判断是否浊音/半浊音
              input=input_index0+raw_kana[0]+input_index1;  //将字符串假名utf替换为RAW编码
            else
              input=input_index0+raw_kana+input_index1;
          }
          else{
            if(raw_kana[1]==' ')
              input=raw_kana[0]+input_index1;
            else
              input=raw_kana+input_index1;
          }

          Serial.print("      input:");
          Serial.println(input);
        }
        
        for(int i=0;i<32;i++){      //初始化临时数组
            temp[i]=0x0;
        }
        for(int i=0;i<32;i++){      //读入原始字符串
            temp[i]=input[i];
        }

        for(int i=0;i<16;i++){
            if(temp[i]!=0x0 && temp[i]!='+')  //将浏览器空格转换成的"+"转换回空格
                co[i]=temp[i];
            else
                co[i]=0X20;
            }
        lcd_text(co,LCD_line1);   //显示第一行
        
        Serial.print("line1 RAW:");
        Serial.println(co);

        if(temp[16]==0x0)         //若第二行内容为空则直接显示
            lcd_text(" ",LCD_line2);
        else{
            for(int i=16;i<32;i++){
                if(temp[i]!=0x0 && temp[i]!='+')  //替换第二行的“+”
                    co2[i-16]=temp[i];
                else
                    co2[i-16]=0X20;
            }
            lcd_text(co2,LCD_line2);    //显示第二行

            Serial.print("line2 RAW:");
            Serial.println(co2);
       }
       delay(100);
       digitalWrite(LED_LIGHT, LOW);
    }
}
