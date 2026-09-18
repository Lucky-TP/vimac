# HANDOFF: Vimac (macOS Modifications & Windows Port Architecture)

เอกสารส่งมอบงาน (Handoff Document) สรุปประวัติการแก้ไขโปรเจกต์ `Lucky-TP/vimac` และข้อกำหนดสถาปัตยกรรม (Architecture & Design Spec) สำหรับการ Rebuild พอร์ตไปยัง Windows

---

## 1. สรุปการแก้ไขบน macOS (`Lucky-TP/vimac`)

โปรเจกต์เดิมถูก Fork มาจาก `nchudleigh/vimac` และได้รับการปรับปรุงจนทำงานได้สมบูรณ์บน macOS 12+:

1. **ถอนระบบสอดแนมและการเชื่อมต่อภายนอก (100% Offline & Privacy-safe)**:
   - ลบ `Segment Analytics` ออกจาก Podfile และโค้ดทุกส่วน (`ModeCoordinator.swift`, `HintModeController.swift`, `ScrollModeViewController.swift` ฯลฯ)
   - ลบ `Sparkle` (Auto-updater) ออกจากการทำงานเบื้องหลัง
2. **แก้ไขปัญหาการ Build บน macOS รุ่นใหม่**:
   - ปรับ `MACOSX_DEPLOYMENT_TARGET` เป็น `12.0`
   - ปิดการทำงานของ AppleScript เก่าที่พยายามเปิด "System Preferences" (ซึ่งถูกเปลี่ยนชื่อเป็น "System Settings" ใน macOS 13+)
   - แก้ไข Bridging Header `HideCursorGlobally.h` ให้ include `<Cocoa/Cocoa.h>`
3. **ปรับปรุง Smooth Scrolling ใน Scroll Mode**:
   - ปรับความถี่ Timer จาก 50Hz เป็น **120Hz** ใน `ChunkyScroller.swift`
   - เพิ่ม Flag `scrollWheelEventIsContinuous = 1` ให้ระบบ macOS รับรู้ว่าเป็น Trackpad Gesture เพื่อเรนเดอร์ภาพไหลลื่น
4. **เพิ่มฟังก์ชันเปิดแท็บใหม่เบื้องหลัง (Middle Click on Shift)**:
   - ปรับปรุง `HintModeController.swift` และ `Utils.swift`
   - เมื่อกด `Shift + ตัวอักษรป้าย` ระบบจะส่งสัญญาณ `CGMouseButton.center` (`otherMouseDown` / `otherMouseUp`) เพื่อจำลองคลิกลูกกลิ้งเมาส์กลาง ทำให้เบราว์เซอร์ (Chrome, Safari, Edge) เปิดลิงก์ในแท็บใหม่เบื้องหลังได้ 100%
5. **คู่มือพัฒนาในเครื่อง**:
   - บันทึกไว้ใน `DEVELOPMENT.md` พร้อมวิธีแก้ปัญหา Accessibility Glitch ด้วยคำสั่ง `tccutil reset Accessibility dexterleng.vimac`

---

## 2. สถาปัตยกรรม Rebuild สำหรับ Windows (Design Specification)

จากการประเมินและทำ Architecture Grilling ได้ข้อสรุปดังนี้:

### 2.1 Tech Stack & Runtime
* **ภาษาหลัก**: **Rust**
* **Windows API Crate**: `windows-rs` (Official Microsoft crate)
* **ข้อดี**:
  * ตัวไฟล์เดี่ยว `.exe` ขนาดเล็ก (< 15MB)
  * กิน Memory ต่ำมาก (< 15MB RAM)
  * ไม่มี Garbage Collector (GC) ป้องกันปัญหา Hook Delay จนถูกระบบ Windows ปลด

### 2.2 คีย์ลัดและการควบคุม (Keybindings & Ergonomics)
* **Trigger หลัก (เลี่ยงการชนกับคีย์ลัดสากลของ Windows)**:
  * **`Alt + F`** สำหรับเข้า **Hint Mode** (แทน `Ctrl + F` ที่ใช้ค้นหาคำในทุกแอป Windows โดยต้องสั่ง Swallow Key ไม่ให้เปิดเมนู File แถบโปรแกรม)
  * **`Alt + J`** สำหรับเข้า **Scroll Mode** (แทน `Ctrl + J` ที่ชนกับหน้ารายการดาวน์โหลดในเบราว์เซอร์)
* **การสั่งการใน Hint Mode**:
  * **กดตัวอักษรธรรมดา**: คลิกซ้าย (`MOUSEEVENTF_LEFTDOWN | UP`)
  * **`Shift + ตัวอักษร`**: คลิกเมาส์กลาง / Middle Click (`MOUSEEVENTF_MIDDLEDOWN | UP` เพื่อเปิดแท็บใหม่)
  * **`Ctrl + ตัวอักษร`**: ดับเบิลคลิกซ้าย
  * **`Alt + ตัวอักษร`**: ย้ายเมาส์ไปชี้เฉยๆ (Hover / Move without click via `SetCursorPos`)

### 2.3 ตารางเทียบเคียง API (macOS vs Windows)

| หน้าที่การทำงาน | macOS (Vimac เดิม) | Windows (Vimac Rebuild) |
| :--- | :--- | :--- |
| **ค้นหาตำแหน่งปุ่ม/ลิงก์** | Accessibility API (`AXUIElement`) | **Windows UI Automation (`IUIAutomation` COM API)** |
| **ดักจับคีย์บอร์ดระดับลึก** | `CGEventTap` / Carbon Events | **`SetWindowsHookExW(WH_KEYBOARD_LL, ...)`** |
| **หน้าต่างใสคลุมหน้าจอ** | `NSWindow` (Floating, Clear color) | **Win32 Layered Window** (`WS_EX_LAYERED \| WS_EX_TRANSPARENT \| WS_EX_NOACTIVATE \| WS_EX_TOPMOST`) |
| **การวาดป้าย Overlay** | AppKit Drawing / CoreAnimation | **Direct2D + DirectWrite** (GPU-accelerated, คมชัด ไม่กระพริบ) |
| **จำลองการคลิกเมาส์** | `CGEvent.post(tap: .cghidEventTap)` | **`SendInput` API** (`INPUT_MOUSE`) |
| **การเลื่อนจอสมูท** | 120Hz Continuous `CGEvent` | **120Hz Fractional `SendInput(MOUSEEVENTF_WHEEL)`** |

---

## 3. ข้อควรระวังและข้อจำกัดระดับวิกฤต (Critical Safeguards)

1. **ปัญหา `LowLevelHooksTimeout`**:
   - บน Windows ถ้า Callback ของ `WH_KEYBOARD_LL` ประมวลผลช้าเกินกำหนด (~200ms) Windows จะปลด Hook ทิ้งทันทีโดยไม่แจ้งเตือน
   - **แนวทางแก้ไข**: ห้ามรัน `IUIAutomation` ภายใน Hook Thread โดยตรง ต้องแยกออกเป็น 3 Threads อิสระ:
     - `Hook Thread`: ดักจับ/บล็อกคีย์ ส่ง Event เข้า Message Channel
     - `UIA Worker Thread`: วิ่งสำรวจ Element ต้นไม้ของหน้าต่างแอป
     - `Render Thread`: วาด Direct2D Overlay บนหน้าจอ
2. **DPI Awareness & Multi-Monitor**:
   - ต้องประกาศเปิด `DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2` เพื่อให้ Overlay ซิงค์พิกัดได้ตรงเป๊ะข้ามจอที่มี Display Scale ไม่เท่ากัน (เช่น 125% และ 100%)
3. **พฤติกรรมกับ Electron / Chromium Apps**:
   - เช่นเดียวกับ macOS แอปอย่าง VS Code หรือ Chrome จะเห็นเป็น Web Area ผืนใหญ่ การคลิกทำงานได้แม่นยำ แต่ Section ใน Scroll Mode จะต้องอาศัยการเลื่อนเมาส์ไปชี้ใน Section นั้นๆ ก่อน

---

## 4. ทักษะที่แนะนำสำหรับ Agent ในเซสชันถัดไป (Suggested Skills)
* `codebase-design`: กำหนด Seam และ Deep Modules แยกความรับผิดชอบของ Input Hook, UIA Walker, และ Overlay Renderer
* `lean-build`: สโคปและเริ่มสร้าง Minimum Viable Product (MVP) ใน Rust
* `grilling`: ใช้ซักถามเพิ่มเติมกรณีต้องการปรับเปลี่ยนรายละเอียดในฝั่ง Config หรือ Multi-monitor support
