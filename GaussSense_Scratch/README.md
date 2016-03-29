#GaussSense  Scratch

利用Scratch的擴充機制將GaussSense的Arduino mode感測值與運算值，送至Scratch，讓中小學生可以在此基礎，做出Scratch GaussSense互動作品

#Scratch擴充原理

[Scratch http extenstion](http://wiki.scratch.mit.edu/wiki/Scratch_Extension#HTTP_Extensions)

[python實作Scratch擴充文章](http://beardad1975.blogspot.tw/2015/08/pythonscratch2basic.html)

#使用環境

安裝Scratch2

安裝python3

pyserial(用pip安裝)

Arduino Leonardo(Uno未試過)

GaussSense(目前僅試過1顆)

#使用方法

1.將arduino_firmware內的韌體安裝至Arduino Leonardo(韌體的輸出有修改過，與原本的稍不同)

2.在Scratch2載入GaussSense_helper資料夾內的 .s2e積木定義檔

3.執行GaussSense_helper內的 .py程式 (執行命令為 py -3 GaussSense_helper.py com3)
註：com3為Arduino的序列埠號碼，依個人的情況修改

4.在Scratch的更多積木中，有可使用的感測積木




