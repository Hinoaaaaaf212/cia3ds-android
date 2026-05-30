# cia3ds-android

[<img src="https://img.shields.io/badge/Obtainium-Add%20to%20Obtainium-blueviolet?style=for-the-badge&logo=android" alt="Add to Obtainium" height="32">](https://apps.obtainium.imranr.dev/redirect?r=obtainium://add/https://github.com/Hinoaaaaaf212/cia3ds-android)
[<img src="https://img.shields.io/github/v/release/Hinoaaaaaf212/cia3ds-android?style=for-the-badge&label=Latest%20APK" alt="Latest release" height="32">](https://github.com/Hinoaaaaaf212/cia3ds-android/releases/latest)
[<img src="https://img.shields.io/github/downloads/Hinoaaaaaf212/cia3ds-android/total?style=for-the-badge&label=Downloads" alt="Total downloads" height="32">](https://github.com/Hinoaaaaaf212/cia3ds-android/releases)

On-device decryption of Nintendo 3DS `.cia` and `.3ds` files. Inspired by the
Windows-only [Batch CIA 3DS Decryptor Redux][inspiration], but a separate
implementation: cia3ds-android wraps the same upstream `ctrtool` and `makerom`
([3DSGuy/Project_CTR][ctr]) in a native Android library, with its own Kotlin
UI and an open-source replacement for the closed `decrypt.exe` the Windows
tool depended on.

Targets phones, tablets, Android TV, and ARM-handheld devices like the
Retroid Pocket and Odin. Output is a fully-decrypted `.cia` (or `.cci` for
games) that installs in any 3DS emulator that accepts plaintext content
(Citra, Lime3DS, [Azahar][azahar]).

## Screenshots

|                          Idle                          |                          Decrypting                          |
| :----------------------------------------------------: | :----------------------------------------------------------: |
| ![Decrypt screen, idle](docs/screenshots/single-idle.png) | ![Decrypt screen, decryption in progress](docs/screenshots/single-progress.png) |

## Status

Stable and feature-complete. The full pipeline (ctrtool extract, NCCH-flags
patch, makerom rebuild) works for `.cia` and `.3ds` input across game, DLC,
update, system, and DSiWare titles. New features land occasionally; see the
[releases](https://github.com/Hinoaaaaaf212/cia3ds-android/releases) for what's
changed.

## Build

Requirements:

- Android Studio Hedgehog (or newer) with NDK `28.2.13676358` and
  CMake `3.22.1` installed via the SDK manager
- JDK 17

```sh
git clone --recurse-submodules https://github.com/Hinoaaaaaf212/cia3ds-android.git
cd cia3ds-android
./gradlew :app:assembleDebug
```

The first build pulls and compiles `ctrtool`, `makerom`, mbedTLS, libfmt,
libyaml, libblz, libtoolchain, libnintendo-n3ds, and libbroadon-es from
vendored sources under `native/third_party/Project_CTR/`. Expect 5–10
minutes on first build per ABI; subsequent builds are incremental.

The output APK is at `app/build/outputs/apk/debug/app-debug.apk`. Sideload
with `adb install` or transfer to the device manually.

## Usage

The app has a single **Decrypt** tab that handles one file or many.

### Single file

1. Tap *Choose a .cia or .3ds file* and pick the file.
2. Pick the output format: `.cia`, `.cci`, or `.3ds` (the latter two only
   apply to game titles; DLC, updates, and system titles fall back to `.cia`
   automatically with a warning).
3. Tap *Decrypt and save*, choose a destination filename, and watch
   progress on the right pane.

### Multiple files (zip)

1. Tap *Choose a .zip file* and pick a zip containing one or more
   `.cia`/`.3ds` files.
   - If the zip has one entry inside, it acts like a single file (save-as).
   - If it has multiple entries, the app switches to batch mode.
2. Pick the output format and the output structure:
   - *No folder*: outputs go directly into the folder you pick.
   - *Grouped*: outputs go into a new subfolder named after the zip.
3. Tap *Decrypt all* and pick an output folder.

Batch jobs run in a foreground service so they survive backgrounding. Zip
entries are extracted lazily, one at a time, so cache pressure stays low
even for large archives.

### Picking from Downloads

Android 11+ blocks granting tree access to the root of `Downloads`. Pick a
`.zip` file (file picker, not folder) to bypass this, or pick a subfolder
of Downloads instead of `Downloads` itself.

### Android TV / handhelds

Shows up on the Android TV home screen and works with a D-pad. The file
pickers are the standard Android ones, so a connected mouse or gamepad makes
them easier to navigate with a TV remote.

## License

MIT, see [LICENSE](LICENSE). Third-party attributions in [NOTICE](NOTICE).

[inspiration]: https://github.com/xxmichibxx/Batch-CIA-3DS-Decryptor-Redux
[ctr]: https://github.com/3DSGuy/Project_CTR
[azahar]: https://github.com/azahar-emu/azahar
