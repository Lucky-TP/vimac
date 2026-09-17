# คู่มือการรันแอป Vimac บนเครื่องตัวเอง (Local Development Guide)

เอกสารนี้สรุปขั้นตอนการ Build โปรเจกต์, การแก้ปัญหาติดลายเซ็น (Code Signing), และวิธีแก้ปัญหาจุกจิกของ macOS Accessibility

---

## 1. การเตรียมโปรเจกต์ (Dependencies)

โปรเจกต์นี้ใช้ทั้ง Carthage และ CocoaPods (เราได้นำปลั๊กอินเก่าอย่าง Analytics และ Sparkle ออกไปแล้วเพื่อให้แอปทำงานแบบ Offline 100%)

เปิด Terminal และรันคำสั่งตามลำดับ:
```bash
# 1. โหลด Dependencies ของ Carthage (บังคับใช้ xcframeworks สำหรับ macOS รุ่นใหม่)
carthage bootstrap --platform macOS --use-xcframeworks

# 2. โหลดปลั๊กอินของ CocoaPods
pod install
```

---

## 2. การเปิดและการ Build ใน Xcode

1. **สำคัญ:** ต้องเปิดไฟล์ **`Vimac.xcworkspace`** เท่านั้น (ห้ามเปิด `.xcodeproj`)
2. เปลี่ยน **Team (ลายเซ็น)** เป็นของคุณเอง:
   - สังเกตแถบซ้ายบนสุด คลิกที่ไอคอนโฟลเดอร์สีฟ้า `Vimac`
   - มองตรงกลางจอ เลือกแถบ **Signing & Capabilities**
   - ตรงช่อง **Team** ให้กดเลือกเป็น Apple ID ของคุณเอง (ถ้าไม่มีให้กด Add an Account)
3. กดปุ่ม **Play ▷** หรือกด `Cmd + R` บนคีย์บอร์ดเพื่อรันแอป

---

## 3. วิธีแก้ปัญหา (Troubleshooting)

### ปัญหาที่ 1: กดให้สิทธิ์ Accessibility แล้วแต่แอปยังมองไม่เห็น
**สาเหตุ:** macOS จะจำ "ลายเซ็น (Signature)" ของแอปไว้ เวลาเราแก้ไขโค้ดและ Build ใหม่ ลายเซ็นของไฟล์ `.app` จะเปลี่ยนไป ทำให้ระบบความปลอดภัยของ macOS สับสน (มองว่าเป็นแอปคนละตัวหรือแอปปลอม)

**วิธีแก้ (ผ่าน Terminal - แนะนำ):**
ล้างความจำสิทธิ์ของแอปตัวนี้ทิ้งซะ ด้วยคำสั่ง:
```bash
tccutil reset Accessibility dexterleng.vimac
```
*หลังจากรันคำสั่งนี้ ให้กลับไปกด Play ใน Xcode อีกรอบ ระบบจะเด้งถามสิทธิ์ใหม่แบบใสสะอาด*

**วิธีแก้ (ผ่าน System Settings):**
1. เปิด **System Settings** -> **Privacy & Security** -> **Accessibility**
2. หาชื่อ Vimac ในลิสต์ คลิกเลือกแล้วกดเครื่องหมายลบ **`-`** ทิ้งไปก่อน
3. เปิดแอป Vimac ใหม่อีกครั้ง ค่อยกดเครื่องหมายบวก **`+`** (หรือกดยืนยันในหน้าต่างป๊อปอัป) กลับเข้าไปใหม่

### ปัญหาที่ 2: กด Play แล้วติด Error `LaunchAtLogin` หาไม่เจอ
**สาเหตุ:** ไม่ได้รัน `carthage bootstrap` ให้สมบูรณ์ หรือลืมลง Carthage
**วิธีแก้:** เปิด Terminal รัน `carthage bootstrap --platform macOS --use-xcframeworks` ใหม่อีกรอบ

---

## 4. สิ่งที่ปรับปรุงไปในเวอร์ชันนี้ (Custom Changes)
- **Privacy & Security:** ถอนโค้ด Segment Analytics และ Sparkle Auto-updater ทิ้งทั้งหมด เพื่อให้แอปทำงานได้โดยไม่ต้องต่ออินเทอร์เน็ต (Offline-first)
- **Smooth Scrolling (Scroll Mode):**
  - ปรับความถี่จาก 50Hz เป็น **120Hz**
  - เพิ่ม Flag `scrollWheelEventIsContinuous = 1` เพื่อสั่งให้ macOS ประมวลผลการเลื่อนเสมือนการปาดนิ้วบน Trackpad จริงๆ ทำให้ภาพลื่นไหลไม่กระตุก
- **macOS 12+ Compatibility:**
  - อัปเดต `MACOSX_DEPLOYMENT_TARGET` ของทุก Dependencies เป็น `12.0`
  - ปิดการทำงานของสคริปต์ขอสิทธิ์เก่า (AppleScript) ที่พังบน macOS 13+ (System Preferences ถูกเปลี่ยนชื่อเป็น System Settings)
