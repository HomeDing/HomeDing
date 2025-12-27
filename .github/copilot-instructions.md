**Purpose**: Quick, actionable guidance to help an AI coding agent become productive in the HomeDing repository.

**Big Picture**:
- **Library role**: `HomeDing` is an Arduino library to build standalone IoT devices (ESP8266 / ESP32 families) that expose a web UI and REST services.
- **Major components**: `src/` (library code and Elements), `examples/` (complete sketches like `standard`, `plug`, `bulb`, `webradio`), `build/` (compile artifacts and logs), and top-level helper scripts (`hd-build.bat`, `hd-upload.cmd`, `hd-start.cmd`).
- **Runtime model**: Devices are assembled from configurable "Elements" that communicate via events/actions; much of the behaviour is configuration-driven (see docs at `homeding.github.io`).

**Documentation Site**:
- Official docs are published at: `https://homeding.github.io`
- Useful pages:
  - Elements overview: `https://homeding.github.io/elements/index.htm` — element types, lifecycle and configuration
  - Examples and how-to: `https://homeding.github.io/examples/index.htm` — device templates and example sketches
  - API / web UI: check device pages linked from examples for REST endpoints and UI behavior
  - Use the site as the authoritative reference for element configuration, REST API details, and examples; the `src/` and `examples/` folders mirror the documentation.

**Developer workflows (explicit commands & files)**:
- Build (Windows wrapper): use `hd-build.bat` which wraps `arduino-cli compile` and `arduino-cli upload`. Common flags: `-c` (compile), `-u` (upload), `-cu` (compile+upload), `-v` (verbose), `--case <name>` (use `hd-build-<case>.bat`), `-fqbn <board>`, `-port <COMx>`.
  - Example: `.\rhd-build --case sc01 examples\standard\standard.ino` (script expects sketch name as last arg).
- Build artifacts: compiled firmware and intermediate outputs are in `./build`; the OTA-ready `.ino.bin` files are typically under `build/` and `./bin` is used by upload helpers.
- OTA Upload (Windows wrapper): `hd-upload.cmd` (aka `hd-upload.bat`) finds the newest `*.ino.bin` in `build/` or a named file in `bin/` and calls `espota.py` from the local ESP toolchain. Usage: `hd-upload <devicename> [firmware]` (supports `-p <password>`).
  - It pings the device first and uses Python `espota.py`; ensure Python and ESP tool paths are available on Windows.
- Quick browse: `hd-start.cmd <devicename>` opens `http://<devicename>/` in the default browser.
- Logs and diagnostics: `hd-build.bat` writes `./build.log`; check `build/compile_result.json` and `build/compile_commands.json` if present.

**Project-specific conventions & patterns**:
- "Elements" pattern: look in `src/` (and `src/lib/README.md`) for element implementations — each element provides inputs/outputs and registers to the event framework. Prefer reading an Element's `.h/.cpp` pair to understand its lifecycle.
- Example tiers: `examples/minimal` for constrained (1MB) devices, `examples/standard` and `DevDing` for full-featured development builds.
- Per-case build configs: `hd-build-<case>.bat` files hold `--build-property` settings used by `arduino-cli` (CPUFreq, FlashSize, UploadSpeed, etc.). Use `--case` to select target-specific settings rather than editing `hd-build.bat`.
- Metadata source of truth: `library.properties` — contains `architectures`, `depends`, and `includes`. Use it to list required libraries and supported boards.

**Integration points & external dependencies**:
- Required tools: `arduino-cli` (used by `hd-build.bat`) and Python for `espota.py` (OTA uploads).
- Manual library: SSDP discovery for ESP32 must be installed manually (see README note). Check `library.properties` for other `depends` that may require installation.
- Build cache: default `%USERPROFILE%\\AppData\\Local\\Temp\\arduino` (script uses `_hd-cachedir`).

**Where to look first (quick navigation for code-modifying agents)**:
- `hd-build.bat`, `hd-upload.cmd`, `hd-start.cmd` — understand build/upload flows and example usage.
- `library.properties` — supported architectures and external dependencies.
- `src/` and `src/lib/README.md` — main implementation and element examples.
- `examples/` — canonical usage patterns and device configurations.
- `build/` — inspect `compile_result.json`, `compile_commands.json`, and `build.log` when troubleshooting compilation errors.

**Precise examples to reference in edits or PRs**:
- To add a new board target: add a `hd-build-<case>.bat` with `--build-property` and call `hd-build --case <case> <sketch>`.
- To add an Element: follow existing `src/...Element*.h/.cpp` pairs and update examples in `examples/DevDing`.

**Element Implementation Skeleton & Checklist**
 - **Why**: Elements are the primary extension mechanism. Each Element derives from `Element` (`src/Element.h`) and registers itself with `ElementRegistry` (`src/ElementRegistry.h`). Use `examples/DevDing/DiagElement.*` as a canonical example.

Header skeleton (place under `src/` or `examples/<sketch>/src/` for sketch-local Elements):

```
// MyElement.h
#pragma once
class MyElement : public Element {
public:
  static Element *create();            // factory
  static bool registered;              // triggers registration in static init

  // override only what you need
  virtual void init(Board *board) override;
  virtual bool set(const char *name, const char *value) override;
  virtual void setup() override;
  virtual void start() override;
  virtual void loop() override;
  virtual void term() override;
  virtual void pushState(std::function<void(const char*, const char*)> cb) override;

private:
  // element specific members
  int _pin = -1;
  String _value;
};
```

CPP skeleton (implementation and registration):

```
// MyElement.cpp
#include <HomeDing.h>
#include "MyElement.h"

Element *MyElement::create() {
  MyElement *e = new MyElement();
  e->startupMode = Element::STARTUPMODE::System; // or Manual/Network/Time
  e->loglevel = LOGGER_LEVEL_INFO;
  return e;
}

bool MyElement::registered = ElementRegistry::registerElement("myelement", MyElement::create);

void MyElement::init(Board *board) {
  Element::init(board);
  // read configuration from board or defaults
}

bool MyElement::set(const char *name, const char *value) {
  if (_stricmp(name, "pin") == 0) {
    _pin = _atoi(value);
    return true;
  }
  return Element::set(name, value);
}

void MyElement::start() {
  Element::start();
  // register HTTP handlers: _board->server->on(...)
}

void MyElement::loop() {
  // periodic work
  Element::loop();
}

void MyElement::pushState(std::function<void(const char*, const char*)> cb) {
  cb("value", _value.c_str());
}
```

Checklist when adding a new Element:
- Place the `.h/.cpp` in `src/` for library-wide Elements, or `examples/<sketch>/src/` for sketch-local Elements.
- Provide a `static Element *create()` factory and a `static bool registered` that calls `ElementRegistry::registerElement("<name>", create)`.
- Override only needed virtuals; default behaviour is in `Element` (call `Element::...` when extending).
- Use helpers on `Element` for parsing: `_atoi`, `_atob`, `_atopin`, `_scanEnum`, `_scanDuration`.
- Use `_board` pointer for access to board-wide services: `_board->server`, `_board->deviceName`, `_board->I2cSda/_board->I2cScl`, etc.
- For web endpoints register handlers in `start()` using `_board->server->on()` with a lambda handler (see `examples/DevDing/DiagElement.cpp`).
- Set `startupMode`, `loglevel`, and `category` in `create()` as appropriate.
- Save persistent local state via `saveState(key, value)` if needed.
- Keep element type name (first parameter to `registerElement`) short and lower-case (e.g., `"diag"`).
- Add example usage to `examples/DevDing` or `examples/<appropriate>` and update docs on `homeding.github.io` if element is public.

Files to inspect while implementing: `src/Element.h`, `src/ElementRegistry.h`, `examples/DevDing/src/DiagElement.h`, and `examples/DevDing/src/DiagElement.cpp`.

**Limitations & safe assumptions**:
- This repo targets Windows helper scripts (`.bat`/`.cmd`) — assume contributors use Windows by default but keep `arduino-cli` commands portable where possible.
- No CI-specific agent files were found in the repo; there is no existing `.github/copilot-instructions.md` to merge (so this file seeds that guidance).

If any section is incomplete or you want a different emphasis (for example, deeper notes on Elements internals or preferred PR styles), tell me what to expand and I will iterate.