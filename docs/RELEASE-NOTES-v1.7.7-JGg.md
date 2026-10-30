# JiYu Trainer 1.7.7-JGg 更新文档

> 程序版本：`1.7.7.0 (Jare-Li Refactor)`  
> 基线版本：`imengyu/JiYuTrainer 1.7.6`（MIT License）  
> 发布标签：`v1.7.7-JGg`  
> 发布包：`JiYuTrainer-JareLi-1.7.7-JGg.zip`

## 一、本次更新内容

### 1. 关闭 UDP 不再退出主程序

- 关闭 UDP 窗口改为**隐藏**，不再销毁窗口
- 主界面、托盘、Hook/驱动状态继续运行
- 再次点击主界面的 **“UDP 重放”** 即可重新打开
- 如果扫描窗口还开着，会和 UDP 窗口一起隐藏
- 修复了关闭 UDP 后再次打开可能使用失效窗口指针的问题

### 2. 关闭 / 最小化移到最右上角

- 关闭 `×`、最小化 `−` 固定在窗口**最右上角**
- 控件移出圆角 `window-box`，不会再被圆角裁切
- 标题、图标仍在左上侧，不再和窗口按钮抢位置

### 3. 主窗口可移动

- 无边框主窗口支持标题栏拖动
- 按住标题、图标或标题栏空白处均可移动
- 右上角 80px 保留给关闭/最小化，不会误拖
- 同时保留原生 `WM_NCHITTEST` 处理

### 4. UDP 主界面重做

- 放弃容易错位的 flex / 绝对定位混用
- 改为稳定的表格双栏布局：
  - 左栏：目标 IP、端口、发送结果
  - 右栏：文字消息、CMD 命令、远程操作
- 输入框、按钮改为整行宽度，不再互相覆盖
- 操作按钮改为原生 `<button>`，确保 `BUTTON_CLICK` 生效
- 保留原配色：`#f4f7fb`、白色卡片、`#2ea4f0`
- 按 DPI 缩放，125% / 150% 下保持可读

### 5. 扫描窗口补完整

- 窗口改为 **640×760**
- 本机地址、开始地址、结束地址改为整行控件
- 增加扫描状态：
  - 正在扫描
  - 扫描完成，发现 N 台主机
- 结果列表和底部说明完整显示
- 打开扫描窗口后 450ms 自动开始扫描
- 保留 `isScanning` 防重复
- 保留扫描范围、`SendARP`、线程计数修复

## 二、升级方法

1. 从托盘退出旧版 JiYuTrainer
2. 备份旧目录中的 `JiYuTrainer.ini`（可选）
3. 下载 `JiYuTrainer-JareLi-1.7.7-JGg.zip`
4. 解压到新目录，或覆盖旧目录
5. 覆盖时保留原来的 `JiYuTrainer.ini`
6. 运行 `JiYuTrainer.exe`，首次运行建议管理员权限
7. 出现 SmartScreen 时按下方说明处理

> 不要同时运行旧版和新版，Hook/驱动会冲突。

## 三、SHA-256

```text
JiYuTrainer.exe                    0C89E4D0F4504D738A76517892A4968F326992972C1556DC49B8EC60EC83B054
JiYuTrainer-JareLi-1.7.7-JGg.zip   0FED7C0F77DA1B91A50850176F44E79B553265919BEE3EB40ED70DC92A4A6054
```

校验命令：

```powershell
Get-FileHash .\JiYuTrainer-JareLi-1.7.7-JGg.zip -Algorithm SHA256
Get-FileHash .\JiYuTrainer.exe -Algorithm SHA256
```

哈希不一致时不要运行，请重新下载。

## 四、SmartScreen 说明

首次运行可能提示“Windows 已保护你的电脑”。这是自签名证书 `CN=Jare-Li` 没有 SmartScreen 信誉导致的，不是病毒报毒。

1. 点击 **更多信息** → **仍要运行**
2. 如果没有该按钮：
   - 右键 zip → 属性 → 通用 → 勾选 **解除锁定** → 确定
   - 或对解压后的 exe 执行：

```powershell
Unblock-File -Path .\JiYuTrainer.exe
```

3. `Get-AuthenticodeSignature` 显示 `UnknownError` / `UntrustedRoot` 属于自签名正常状态

## 五、测试结果

```text
74/74 passed, 0 failed, 100.0%
```

覆盖：

- IP 校验、扫描范围、目标列表解析
- UDP 包字节、偏移、UTF-16LE、超长载荷
- PE 架构、版本信息、HTML 资源
- 主界面 DOM id、UDP/扫描窗口布局
- MIT 归属、签名主体、构建产物

## 六、已知限制

- 仍使用自签名证书，不能消除 SmartScreen 首次提示
- UDP 重放模块仅限自己拥有或已获授权的设备
- 驱动 `JiYuTrainerDriver.sys` 保持上游原文件
- 自动测试 74/74 不代表所有机房环境 100% 兼容
- 程序内置更新仍指向原项目服务，不会自动发现 GitHub Release

## 七、发布信息

```text
Tag: v1.7.7-JGg
Title: JiYuTrainer 1.7.7-JGg — Jare-Li Refactor
Asset: JiYuTrainer-JareLi-1.7.7-JGg.zip
```

如果 `v1.7.7-JGg` 已经发布过，请新建 `v1.7.7-JGg2`，不要强推旧 tag。
