## 概要
VRChatの起動オプションをGUIベースで変更可能にしたちょっと便利なランチャー。  
いちいちSteamやショートカットを編集しなくても簡単に起動構成がいじれるのでいろいろ捗ります。
![image](Image.png)

## 機能について
GUIメニューの各機能について軽く説明します。
### Display
* DesktopMode : デスクトップモードで起動する
* FullScreen  : フルスクリーンで起動する
* FPS Limit   : FPSを制限する
* Monitor     : VRChatを表示するモニター。モニターのIDについてはWindowsの設定を参照。
* WindowSize  : ウィンドウのサイズを指定。

### Test
* Avatar Test : ローカルテスト中のアバターがアップデートされた場合に自動で再度読み込みを行う。
* World Test  : ローカルテスト中のワールドがアップデートされた場合に自動で再度読み込みを行う。

### Misc
* Profile : アカウントを切り替えることができる。
* CCX Option
```
一部のRyzen CPUユーザー限定。  
CCXをまたいでの処理が行われなくなるのでレイテンシが減りパフォーマンスが向上する（らしい）
```

## その他
### 設定ファイル
config.json という名前で以下の場所にあります。  
また、このLauncherがPC上に残すファイルもこれだけです。
```
%AppData%Local\VRChatAdvancedLauncher
```