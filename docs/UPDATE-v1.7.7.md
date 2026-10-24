# v1.7.7 鏇存柊璇存槑

> 鐗堟湰锛歚1.7.7.0 (Jare-Li Refactor)`  
> 鍩虹嚎锛歚imengyu/JiYuTrainer 1.7.6`锛圡IT License锛? 
> 鍙戝竷鏍囩锛歚v1.7.7`  
> 鍙戝竷鍖咃細`JiYuTrainer-JareLi-1.7.7.zip`

## 鏈淇

### 1. 鍏抽棴 / 鏈€灏忓寲鎸夐挳浣嶇疆

- 鍏抽棴 `脳`銆佹渶灏忓寲 `鈭抈 绉诲埌绐楀彛**宸︿笂瑙?*
- 椤哄簭锛氬叧闂湪宸︼紝鏈€灏忓寲鍦ㄥ彸
- 涓嶅啀涓庢爣棰樸€佺姸鎬佸崱鐗囦簤鎶㈠彸涓婅绌洪棿

### 2. UDP 閲嶆斁绐楀彛

- 绐楀彛灏哄淇濇寔鏀惧ぇ鍚庣殑 `896脳760`
- 灞忓箷楂樺害涓嶈冻鏃惰嚜鍔ㄧ缉灏忓埌 `screenHeight - 80`锛岄伩鍏嶈浠诲姟鏍忚鎺?- 鍐呭鍖哄煙鏀寔绾靛悜婊氬姩
- IP 杈撳叆妗嗕笌鈥滄壂鎻忓眬鍩熺綉鈥濇寜閽笉鍐嶉噸鍙?
### 3. IP 鎵弿

- 鈥滃紑濮嬫壂鎻忎富鏈?IP鈥濆悓鏃舵敮鎸?`HYPERLINK_CLICK` 鍜?`BUTTON_CLICK`
- 澧炲姞 `isScanning` 闃查噸澶嶇偣鍑?- 淇濈暀骞剁‘璁や互涓嬩慨澶嶏細
  - 鎵弿鑼冨洿灏鹃儴鍦板潃涓嶅啀涓㈠け锛?56 涓湴鍧€瀹屾暣瑕嗙洊锛?  - `SendARP` 澶辫触涓嶅啀璇垽涓哄湪绾夸富鏈?  - 淇鎵弿绾跨▼璁℃暟绔炴€?  - 璧峰鍦板潃涓嶈兘澶т簬缁撴潫鍦板潃
- 澧炲姞鏃ュ織锛屼究浜庡畾浣嶏細

```text
[UDP] open scan window
[ScanIP] scan link clicked, scanning=0
[ScanIP] start 192.168.x.0 - 192.168.x.255
[ScanIP] range 192.168.x.0-255, count=256
[ScanIP] finish, hosts=N
```

鏃ュ織浣嶄簬 exe 鍚岀洰褰曠殑 `JiYuTrainer.log`銆?
## 浠庢棫鐗堝崌绾?
1. 浠庢墭鐩橀€€鍑烘棫鐗?JiYuTrainer锛堜笉瑕佸彧鍏抽棴绐楀彛锛?2. 澶囦唤鏃х洰褰曚腑鐨?`JiYuTrainer.ini`锛堝彲閫夛紝淇濈暀璁剧疆锛?3. 涓嬭浇 `JiYuTrainer-JareLi-1.7.7.zip`
4. 瑙ｅ帇鍒版柊鐩綍锛屾垨瑕嗙洊鏃х洰褰?5. 濡傛灉瑕嗙洊锛屼繚鐣欏師鏉ョ殑 `JiYuTrainer.ini`
6. 杩愯 `JiYuTrainer.exe`锛岄娆¤繍琛屽缓璁鐞嗗憳鏉冮檺
7. 濡傛灉鍑虹幇 SmartScreen锛屾寜涓嬫柟璇存槑澶勭悊

> 涓嶈鍚屾椂杩愯鏃х増鍜屾柊鐗堬紱涓よ竟鐨?Hook/椹卞姩浼氬啿绐併€?
## 鏍￠獙

```powershell
Get-FileHash .\JiYuTrainer-JareLi-1.7.7.zip -Algorithm SHA256
Get-FileHash .\JiYuTrainer.exe -Algorithm SHA256
```

```text
JiYuTrainer.exe            FEC8C38A867C61D8C154D5482353D605C93253C0DFF9E89B80AF4CF0F8DE0CA4
JiYuTrainer-JareLi zip     410A3763F5C9B858CBCD732511E4AC56C6127603B355B77C61C06E890D6E04AA
```

濡傛灉鍝堝笇涓嶄竴鑷达紝涓嶈杩愯锛岃閲嶆柊涓嬭浇銆?
## SmartScreen

棣栨杩愯鍙兘鎻愮ず鈥淲indows 宸蹭繚鎶や綘鐨勭數鑴戔€濄€傝繖鏄嚜绛惧悕璇佷功 `CN=Jare-Li` 娌℃湁 SmartScreen 淇¤獕瀵艰嚧鐨勶紝涓嶆槸鐥呮瘨鎶ユ瘨銆?
1. 鐐瑰嚮 **鏇村淇℃伅** 鈫?**浠嶈杩愯**
2. 濡傛灉娌℃湁璇ユ寜閽細
   - 鍙抽敭 zip 鈫?灞炴€?鈫?閫氱敤 鈫?鍕鹃€?**瑙ｉ櫎閿佸畾** 鈫?纭畾
   - 鎴栬瑙ｅ帇鍚庣殑 exe 鎵ц `Unblock-File -Path .\JiYuTrainer.exe`
3. `Get-AuthenticodeSignature` 鏄剧ず `UnknownError` / `UntrustedRoot` 灞炰簬鑷鍚嶆甯哥姸鎬?
璇﹁ [FIRST-RUN.md](FIRST-RUN.md) 涓?[CODE_SIGNING.md](../CODE_SIGNING.md)銆?
## 鍐呯疆鏇存柊璇存槑

绋嬪簭鍐呯疆鐨勨€滄鏌ユ洿鏂扳€濅粛鎸囧悜鍘熼」鐩洿鏂版湇鍔★紝涓嶄細鑷姩鍙戠幇 GitHub Release `v1.7.7`銆? 
鏈鏇存柊闇€瑕佹墜鍔ㄤ笅杞?zip 瑕嗙洊銆?
## 宸茬煡闄愬埗

- 鏈増鏈粛浣跨敤鑷鍚嶈瘉涔︼紝涓嶈兘娑堥櫎 SmartScreen 棣栨鎻愮ず
- UDP 閲嶆斁妯″潡浠呴檺鑷繁鎷ユ湁鎴栧凡鑾锋巿鏉冪殑璁惧
- 椹卞姩 `JiYuTrainerDriver.sys` 淇濇寔涓婃父鍘熸枃浠?- 鑷姩娴嬭瘯 74/74 閫氳繃锛屼絾涓嶄唬琛ㄦ墍鏈夋満鎴跨幆澧?100% 鍏煎

