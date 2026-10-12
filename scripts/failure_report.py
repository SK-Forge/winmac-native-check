"""失敗したときだけ呼ぶ。OS・落ちた手順・ログの要点を1つの短い文章にして
Actions の結果画面（Job Summary）に出す。そのまま Codex / Claude Code に貼れる。

使い方: python failure_report.py <ログファイル> <落ちた手順> [<落ちた手順>...]
"""
import os
import platform
import sys

KEYWORDS = ("error", "fail", "cannot", "not found", "undefined", "unresolved", "エラー")
MAX_LINES = 25


def key_lines(text: str) -> list[str]:
    lines = [l.rstrip() for l in text.splitlines() if l.strip()]
    hit = [l for l in lines if any(k in l.lower() for k in KEYWORDS)]
    picked = hit[-MAX_LINES:] if hit else lines[-MAX_LINES:]
    return picked


def main() -> int:
    log_path, steps = sys.argv[1], sys.argv[2:]
    try:
        with open(log_path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    except OSError:
        text = "(ログファイルが無い。手順が始まる前に落ちた可能性)"

    os_name = os.environ.get("RUNNER_OS", platform.system())
    arch = os.environ.get("RUNNER_ARCH", platform.machine())
    repo = os.environ.get("GITHUB_REPOSITORY", "?")
    sha = os.environ.get("GITHUB_SHA", "?")[:7]
    run_url = "{}/{}/actions/runs/{}".format(
        os.environ.get("GITHUB_SERVER_URL", "https://github.com"), repo, os.environ.get("GITHUB_RUN_ID", "")
    )

    report = (
        "【CI失敗の報告】{os_name}（{arch}）の確認で落ちました。\n"
        "落ちた手順: {steps}\n"
        "対象: {repo} / コミット {sha}\n"
        "ログの要点:\n{lines}\n\n"
        "お願い: 上のログを読んで原因を直してください。"
        "{os_name} 側だけの問題かもしれないので、もう片方のOSでも動く形で直してください。\n"
        "詳細: {url}\n"
    ).format(
        os_name=os_name,
        arch=arch,
        steps="、".join(steps) or "不明",
        repo=repo,
        sha=sha,
        lines="\n".join("    " + l for l in key_lines(text)),
        url=run_url,
    )

    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    block = "## ❌ {} で失敗（AIに貼る用）\n\n```text\n{}```\n".format(os_name, report)
    if summary:
        with open(summary, "a", encoding="utf-8") as f:
            f.write(block)
    print(report)
    return 0


if __name__ == "__main__":
    sys.exit(main())
