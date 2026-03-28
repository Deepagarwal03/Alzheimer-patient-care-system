from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "web_pages.h"


def raw_literal(content: str) -> str:
    return 'R"rawliteral(\n' + content + '\n)rawliteral"'


def main() -> None:
    index_html = (ROOT / "index.html").read_text(encoding="utf-8").rstrip()
    style_css = (ROOT / "style.css").read_text(encoding="utf-8").rstrip()
    script_js = (ROOT / "script.js").read_text(encoding="utf-8").rstrip()

    output = (
        "const char index_html[] PROGMEM = "
        + raw_literal(index_html)
        + ";\n\nconst char style_css[] PROGMEM = "
        + raw_literal(style_css)
        + ";\n\nconst char script_js[] PROGMEM = "
        + raw_literal(script_js)
        + ";\n"
    )
    OUTPUT.write_text(output, encoding="utf-8")


if __name__ == "__main__":
    main()
