# JiYuTrainer 1.7.6 鈥?Jare-Li Refactor

> 鍩轰簬 [imengyu/JiYuTrainer](https://github.com/imengyu/JiYuTrainer) 1.7.6锛圡IT License锛夌殑閲嶆瀯鐗堛€?
> 鍘熺増鏉冨綊 **蹇箰鐨勬ⅵ楸硷紙imengyu锛?* 鎵€鏈夛紱閲嶆瀯銆乁I銆佹瀯寤轰笌娴嬭瘯鐢?**Jare-Li** 缁存姢銆?

## 鏈鏇存柊

- 涓荤晫闈㈡敼涓?**960脳540锛?6:9锛?*锛氬乏渚х姸鎬佸崱鐗?+ 鍔熻兘瀵艰埅锛屽彸渚у姛鑳藉唴瀹?
- UDP 閲嶆斁绐楀彛鏀惧ぇ鍒?**896脳760**锛屽乏鍙冲垎鏍忥紝杈撳叆妗嗕笌鎸夐挳涓嶅啀閲嶅彔
- 灞€鍩熺綉鎵弿绐楀彛鏀惧ぇ鍒?**580脳700**锛岄噸鍋氳〃鍗曘€佺粨鏋滃垪琛ㄤ笌鐘舵€佹樉绀?
- 淇 IP 鎵弿锛?
  - 鎵弿鑼冨洿灏鹃儴鍦板潃涓嶅啀涓㈠け锛?56 涓湴鍧€瀹屾暣瑕嗙洊锛?
  - `SendARP` 澶辫触涓嶅啀璇垽涓哄湪绾夸富鏈?
  - 淇鎵弿绾跨▼璁℃暟绔炴€?
  - 澧炲姞璧峰鍦板潃涓嶈兘澶т簬缁撴潫鍦板潃鐨勬牎楠?
- 淇濈暀鍘?1.7.6 UDP 鍗忚瀛楄妭銆佹敾鍑绘ā鍧椾笌鍏ㄩ儴鍘熸湁鍔熻兘
- 鍥炲綊娴嬭瘯锛?*74/74 閫氳繃锛?00.0%**

## 璧勪骇

| 鏂囦欢 | SHA-256 |
|---|---|
| `JiYuTrainer-JareLi-1.7.6.zip` | `0BD42FCB3EC0CA731B9F3EDEC7DD9D3E6DEF17D79DB3B3F04D3787B9FCC6B0D4` |
| `JiYuTrainer.exe` | `A662B378E37FBFFDA7198F55E1A709CE9ABC6BF76E34EAA20803F76CE82A166B` |

> 濡傛灉浣犱笂浼犵殑鏄?GitHub Actions 閲嶆柊鏋勫缓鐨?zip锛屽搱甯屼細涓嶅悓锛涜鐢ㄤ笅闈㈢殑鍛戒护閲嶆柊璁＄畻骞舵浛鎹€?

## 鏍￠獙 SHA-256

```powershell
Get-FileHash .\JiYuTrainer-JareLi-1.7.6.zip -Algorithm SHA256
Get-FileHash .\dist\JiYuTrainer.exe -Algorithm SHA256
```

鎴栵細

```cmd
certutil -hashfile JiYuTrainer-JareLi-1.7.6.zip SHA256
```

鍝堝笇涓€鑷村彧璇存槑鏂囦欢鏈绡℃敼锛涢娆¤繍琛屼粛鍙兘瑙﹀彂 SmartScreen銆?

## 绯荤粺瑕佹眰

- Windows 10 / 11锛坸86 涓荤▼搴忥紝鍙湪 64 浣嶇郴缁熻繍琛岋級
- 棣栨杩愯寤鸿浠ョ鐞嗗憳韬唤鍚姩锛屼互渚垮畨瑁?Hook/椹卞姩
- 涓嶆敮鎸佸湪鏈巿鏉冪殑绗笁鏂硅澶囦笂浣跨敤 UDP 閲嶆斁鎴栨壂鎻忓姛鑳?

## 瀹夎

1. 涓嬭浇骞惰В鍘?`JiYuTrainer-JareLi-1.7.6.zip`
2. 杩愯 `JiYuTrainer.exe`
3. 棣栨杩愯鍚屾剰鐢ㄦ埛鍗忚
4. 涓荤晫闈㈠彲鏈€灏忓寲鍒版墭鐩橈紱鍏抽棴绐楀彛榛樿闅愯棌鍒版墭鐩?

## Windows SmartScreen 璇存槑

棣栨杩愯浠庣綉缁滀笅杞界殑 exe 鏃讹紝Windows 鍙兘鏄剧ず锛?

> Windows 宸蹭繚鎶や綘鐨勭數鑴?

杩欐槸 SmartScreen 瀵光€滄湭鐭ュ彂甯冭€?浣庝俊瑾夋枃浠垛€濈殑鎻愮ず锛屼笉鏄梾姣掓姤姣掞紝涔熶笉浠ｈ〃鏂囦欢鎹熷潖銆?

### 鏈€蹇鐞?

1. 鐐瑰嚮 **鈥滄洿澶氫俊鎭€?* 鈫?**鈥滀粛瑕佽繍琛屸€?*
2. 濡傛灉娌℃湁鈥滀粛瑕佽繍琛屸€濓細
   - **鍏堝 zip 瑙ｉ攣**锛氬彸閿?`JiYuTrainer-JareLi-1.7.6.zip` 鈫?灞炴€?鈫?甯歌 鈫?鍕鹃€?**鈥滆В闄ら攣瀹氣€?* 鈫?纭畾
   - 鍐嶈В鍘?
3. 宸茬粡瑙ｅ帇鐨勮瘽锛屽 exe 鎵ц锛?

```powershell
Unblock-File -Path .\JiYuTrainer.exe
```

鎴栧鏁翠釜鐩綍鎵ц锛?

```powershell
Get-ChildItem -Recurse | Unblock-File
```

### 涓轰粈涔堜粛浼氭彁绀?

- 鏂囦欢浠庣綉缁滀笅杞斤紝甯︽湁 Mark-of-the-Web
- 褰撳墠浣跨敤 `CN=Jare-Li` 鐨?*鑷鍚?*璇佷功锛學indows 涓嶄俊浠昏鏍硅瘉涔?
- 鏂囦欢娌℃湁 Microsoft SmartScreen 淇¤獕

鑷鍚嶅彲浠ヨ瘉鏄庢枃浠舵湭琚鏀癸紝浣嗕笉鑳借幏寰楀井杞殑鍙戝竷鑰呬俊瑾夈€? 
`Get-AuthenticodeSignature` 鏄剧ず `UnknownError` / `UntrustedRoot` 灞炰簬鑷鍚嶇殑姝ｅ父鐘舵€併€?

### 濡傛灉杩炩€滀粛瑕佽繍琛屸€濋兘娌℃湁

鍙兘鏄?Windows 11 **Smart App Control** 鎴栫粍缁囩瓥鐣ワ細

- 浼樺厛浣跨敤鍙椾俊浠荤殑浠ｇ爜绛惧悕璇佷功
- 浼佷笟鐜浣跨敤 AppLocker / WDAC 鍏佽瑙勫垯
- 涓嶅缓璁负浜嗕竴涓▼搴忓叏灞€鍏抽棴 SmartScreen
- Smart App Control 鍏抽棴閫氬父涓嶅彲閫嗭紝璇疯皑鎱?

鏇村璇存槑瑙?[CODE_SIGNING.md](CODE_SIGNING.md) 涓?[docs/FIRST-RUN.md](docs/FIRST-RUN.md)銆?

## 瀹夊叏涓庢巿鏉?

- 鏈蒋浠剁敤浜庢仮澶嶆湰鏈哄湪鏈烘埧鐜涓殑鎿嶄綔鑷敱锛岃閬靛畧瀛︽牎銆佸崟浣嶅拰瀹為獙瀹よ瀹?
- UDP 閲嶆斁妯″潡鍙悜鐩爣涓绘満鍙戦€佸懡浠ゃ€佸叧鏈烘垨閲嶅惎璇锋眰锛涜浠呭鑷繁鎷ユ湁鎴栧凡鑾锋槑纭巿鏉冪殑璁惧浣跨敤
- 涓嶈瀵规湭鎺堟潈鐨勭涓夋柟涓绘満浣跨敤鎵弿鎴栭噸鏀惧姛鑳?
- 鏈」鐩笌鏋佸煙鐢靛瓙鏁欏銆乮mengyu 瀹樻柟鏃犻毝灞炲叧绯?

## 璁稿彲涓庡綊灞?

- 鍘熼」鐩細<https://github.com/imengyu/JiYuTrainer>锛圡IT锛?
- UDP 閲嶆斁鍘熺悊锛?https://github.com/ht0Ruial/Jiyu_udp_attack>
- 閲嶆瀯锛欽are-Li
- 瀹屾暣璁稿彲璇佽 [LICENSE](LICENSE)锛岀涓夋柟褰掑睘瑙?[NOTICE](NOTICE)
- 涓婃父 README锛歔docs/UPSTREAM-README.md](docs/UPSTREAM-README.md)

