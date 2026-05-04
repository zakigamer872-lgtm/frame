# كيف تحصل على FrameX.exe بدون تثبيت أي شي

## الطريقة — GitHub Actions (مجاني 100%)

---

### الخطوة 1 — إنشاء حساب GitHub
اذهب إلى [github.com](https://github.com) وأنشئ حساباً مجانياً.

---

### الخطوة 2 — إنشاء Repository جديد
1. اضغط **New repository**
2. اسم المشروع: `FrameX`
3. اختر **Public**
4. اضغط **Create repository**

---

### الخطوة 3 — رفع الملفات
اضغط **uploading an existing file** ثم اسحب محتويات مجلد `framex-project` كلها.

أو من Command Prompt:
```batch
cd framex-project
git init
git add .
git commit -m "FrameX v1.0.0 - by ZAKI @zg22x"
git remote add origin https://github.com/YOUR_USERNAME/FrameX.git
git push -u origin main
```

---

### الخطوة 4 — انتظر البناء التلقائي
بمجرد رفع الملفات، GitHub سيبدأ البناء تلقائياً!

1. اذهب إلى تبويب **Actions** في مشروعك
2. ستجد **Build FrameX** يعمل (دائرة برتقالية ⟳)
3. انتظر 5-10 دقائق
4. لما يخلص ستجد علامة ✅ خضراء

---

### الخطوة 5 — تحميل الـ EXE
1. اضغط على **Build FrameX** في Actions
2. انزل لأسفل الصفحة إلى **Artifacts**
3. اضغط **FrameX-Setup-Windows**
4. سيتحمّل ملف ZIP فيه `FrameX_Setup_v1.0.0.exe` ✅

---

### إصدار نسخة جديدة (اختياري)
عشان ينشئ GitHub Release عام يقدر الكل يحمّله:
```batch
git tag v1.0.0
git push origin v1.0.0
```
سينشئ GitHub صفحة تحميل عامة تلقائياً!

---

**Developer:** ZAKI | Instagram & TikTok: @zg22x
