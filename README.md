# WinMac自動確認

WindowsとMacの両方で動くネイティブ部品（DLL / dylib）を、push のたびに **GitHub Actions が Windows と Mac で同時に**組み立て・読み込みテストします。
「片方を直すともう片方が未確認」「DLL が読み込めない」を、人が往復せずに見つけるためのひな形です。

## 中身
| 場所 | 役割 |
|---|---|
| `src/` | 見本のC部品（`winmac_add` / `winmac_platform`） |
| `CMakeLists.txt` | Windows=`.dll`、Mac=`.dylib` を組み立て、`load_test` を登録 |
| `tests/load_test.c` | 作ったライブラリを**実際に開いて**関数を呼ぶ |
| `.github/workflows/ci.yml` | `windows-latest` と `macos-latest`（Apple Silicon）で同時に実行 |
| `scripts/failure_report.py` | 失敗時に、OS・落ちた手順・ログの要点を短い文章にまとめる |

## 自分のSDKへの入れ方
1. `.github/workflows/ci.yml` と `scripts/failure_report.py` を自分のリポジトリへコピー。
2. 自分の `CMakeLists.txt` に、共有ライブラリと `add_test`（読み込みテスト）を足す。見本の `CMakeLists.txt` / `tests/load_test.c` を真似る。
3. Windows は `__declspec(dllexport)` が無い関数は外から見えない（`src/winmac_demo.h` の `WINMAC_API`）。Mac側で動いてWindowsだけ「関数が見つからない」はたいていこれ。
4. CMake を使っていないSDKは、ci.yml の「組み立て」「読み込みテスト」の2手順だけ自分のコマンドに差し替える。

## 失敗したとき
Actions の該当実行の **Summary** に「CI失敗の報告」が出ます。その文章をそのまま Codex / Claude Code に貼れば直させられます。

報告の見本（わざと壊した実行から。Windows と Mac それぞれに出る）:
```text
【CI失敗の報告】Windows（X64）の確認で落ちました。
落ちた手順: 読み込みテスト
対象: <オーナー>/<リポジトリ> / コミット 540c6b3
ログの要点:
    CALL_FAIL: winmac_add(2,3) = -1（期待 5）
    1 - load_test (Failed)

お願い: 上のログを読んで原因を直してください。Windows 側だけの問題かもしれないので、もう片方のOSでも動く形で直してください。
```
手順: Actions の失敗した実行を開く → 各OSのジョブの Summary → 文章をコピー → Codex / Claude Code に貼る → 直させて push → 両OSが緑になるまで繰り返す。

## 限界（Actions でできないこと）
- **機器が要るテスト**（USB機器・カメラ・専用ドライバなど）は Actions にはありません。手元で行う。Actions で確かめるのは、機器なしで済むところ（組み立て・読み込み・純粋な計算）まで。
- 手元の確認: Windows は Visual Studio 付属の CMake で `cmake -S . -B build && cmake --build build --config Release && ctest --test-dir build -C Release`。

## 無料枠
- 公開リポジトリ: Windows・Mac とも無料。
- 非公開リポジトリ: 無料枠は月の分数が決まっており、**Mac は10倍で消費**（Windowsは2倍、Linuxは1倍）。Mac 1分 = 枠10分。頻繁に push するなら、`pull_request` と main だけに絞る。

## 確認状況
- Windows（手元）: 組み立て・読み込みテストとも通過（2026-10-12）。
- Mac（Actions の macos-latest、Apple Silicon）: 組み立て・読み込みテストとも通過（2026-10-12）。
- 失敗報告: わざと壊した実行で、Windows・Mac 両方に報告文が出ることを確認。
