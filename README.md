# Smart POS - نظام نقاط البيع الذكي

نظام نقاط بيع احترافي مبني بـ Qt 5.15.2 مع دعم كامل للغة العربية وواجهة RTL.

## المميزات

### نظام المبيعات (POS)
- شاشة بيع احترافية مع شبكة المنتجات وسلة المشتريات
- البحث عن المنتجات بالاسم أو الباركود
- تصفية المنتجات حسب الأقسام
- حساب الإجمالي والخصم والضريبة والباقي تلقائياً
- دعم طرق دفع متعددة (نقدي، بطاقة، تحويل)
- ربط العميل بالفاتورة
- طباعة الفاتورة تلقائياً أو يدوياً

### إدارة المنتجات
- إضافة وتعديل وحذف المنتجات
- تصنيف المنتجات في أقسام
- تتبع المخزون مع تنبيه الحد الأدنى
- دعم الباركود لكل منتج
- البحث والتصفية المتقدمة

### إدارة العملاء
- قاعدة بيانات العملاء الكاملة
- تتبع مشتريات العملاء والأرصدة
- البحث السريع عن العملاء

### التقارير والأرباح
- ملخص المبيعات (إجمالي المبيعات، صافي الربح، عدد المعاملات)
- سجل المبيعات التفصيلي
- أكثر المنتجات مبيعاً
- المبيعات حسب القسم
- المبيعات اليومية
- طباعة أي تقرير بطابعة GDI (A4)

### الطابعات
- **طابعة حرارية (Thermal):** دعم 58mm و 80mm مع بروتوكول ESC/POS
- **طابعة GDI (A4):** طباعة الفواتير والتقارير بحجم A4
- إعدادات كاملة للمنفذ والسرعة وحجم الورق
- اختبار الطابعة من صفحة الإعدادات

### قارئ الباركود
- دعم قارئ الباركود USB
- إضافة المنتجات تلقائياً عند المسح
- إعدادات الحد الأدنى لطول الباركود

### التفعيل و Firebase
- تفعيل التطبيق عبر Firebase/Firestore
- معرف جهاز فريد (Hardware ID) لكل جهاز
- إمكانية حظر جهاز معين أو جميع الأجهزة
- إدارة الأجهزة المسجلة عبر Firestore

### إدارة المستخدمين
- أدوار متعددة (مدير، مشرف، كاشير)
- إضافة وتعديل وحذف المستخدمين
- إعادة تعيين كلمة المرور
- تسجيل دخول آمن

### المظهر
- **سمة داكنة (Dark Theme)** - احترافية وعصرية
- **سمة فاتحة (Light Theme)** - نظيفة ومريحة للعين
- تبديل فوري بين السمات

### التوافق
- **Windows 7** وأحدث
- Linux و macOS
- C++14 Standard
- Qt 5.15.2

---

## متطلبات التشغيل

### البرمجيات المطلوبة
1. **Qt 5.15.2** - [تحميل](https://download.qt.io/archive/qt/5.15/5.15.2/)
2. **VS Code** - [تحميل](https://code.visualstudio.com/)
3. **MinGW 8.1** أو **MSVC 2019** (مع Qt 5.15.2)

### إعداد VS Code

#### 1. تثبيت الإضافات المطلوبة
```
- C/C++ (Microsoft)
- C/C++ Extension Pack
- Qt tools (optional)
- CMake Tools (optional)
```

#### 2. إعداد متغيرات البيئة
أضف مسار Qt إلى متغير PATH:
```batch
:: لـ MinGW
set PATH=C:\Qt\5.15.2\mingw81_64\bin;C:\Qt\Tools\mingw810_64\bin;%PATH%

:: لـ MSVC
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
```

#### 3. بناء المشروع من سطر الأوامر
```batch
:: إنشاء مجلد البناء
mkdir build
cd build

:: إنشاء Makefile
qmake ../SmartPOS.pro

:: بناء المشروع
:: MinGW:
mingw32-make

:: MSVC:
nmake
```

#### 4. بناء المشروع باستخدام VS Code Tasks
أنشئ ملف `.vscode/tasks.json`:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "qmake",
            "type": "shell",
            "command": "qmake",
            "args": ["../SmartPOS.pro"],
            "options": {
                "cwd": "${workspaceFolder}/build"
            }
        },
        {
            "label": "build",
            "type": "shell",
            "command": "mingw32-make",
            "options": {
                "cwd": "${workspaceFolder}/build"
            },
            "dependsOn": "qmake",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```

#### 5. تشغيل المشروع
```batch
cd build
:: Windows:
release\SmartPOS.exe

:: Linux:
./SmartPOS
```

---

## إعداد Firebase

### المعلمات المطلوبة

| المعلمة | الوصف | مثال |
|---------|-------|------|
| `Project ID` | معرف مشروع Firebase | `my-pos-project` |
| `API Key` | مفتاح واجهة برمجة التطبيقات | `AIzaSyA...` |
| `Collection` | اسم مجموعة Firestore | `devices` |

### خطوات الإعداد

#### 1. إنشاء مشروع Firebase
1. اذهب إلى [Firebase Console](https://console.firebase.google.com/)
2. أنشئ مشروع جديد
3. انسخ **Project ID** من إعدادات المشروع

#### 2. الحصول على API Key
1. اذهب إلى إعدادات المشروع > عام
2. انسخ **Web API Key**

#### 3. إنشاء قاعدة بيانات Firestore
1. اذهب إلى Firestore Database
2. أنشئ قاعدة بيانات في وضع الإنتاج
3. أنشئ مجموعة (Collection) باسم `devices`

#### 4. هيكل مستند الجهاز في Firestore
```json
{
    "hardwareId": "ABC123DEF456...",
    "deviceName": "DESKTOP-PC",
    "activated": true,
    "banned": false,
    "banReason": "",
    "licenseKey": "XXXX-XXXX-XXXX",
    "registeredAt": "2024-01-01T00:00:00Z",
    "lastSeen": "2024-01-01T12:00:00Z"
}
```

#### 5. مستند حظر التطبيق (اختياري)
أنشئ مستند باسم `app_config` في مجموعة `settings`:
```json
{
    "appBanned": false,
    "banReason": ""
}
```

#### 6. قواعد الأمان لـ Firestore
```javascript
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    match /devices/{deviceId} {
      allow read: if true;
      allow write: if true;
    }
    match /settings/{doc} {
      allow read: if true;
      allow write: if false; // Admin only via console
    }
  }
}
```

#### 7. إدخال المعلمات في التطبيق
1. افتح التطبيق
2. اذهب إلى الإعدادات
3. أدخل **Project ID** و **API Key** و **Collection**
4. اضغط حفظ

---

## هيكل المشروع

```
SmartPOS/
├── SmartPOS.pro              # ملف المشروع
├── README.md                 # هذا الملف
├── resources/
│   ├── resources.qrc         # ملف الموارد
│   └── styles/
│       ├── dark.qss          # السمة الداكنة
│       └── light.qss         # السمة الفاتحة
└── src/
    ├── main.cpp              # نقطة الدخول
    ├── models/               # نماذج البيانات
    │   ├── product.h
    │   ├── category.h
    │   ├── customer.h
    │   ├── sale.h
    │   └── user.h
    ├── core/                 # الخدمات الأساسية
    │   ├── database.h/cpp    # قاعدة بيانات SQLite
    │   ├── firebase.h/cpp    # تكامل Firebase
    │   ├── hardwareid.h/cpp  # معرف الجهاز
    │   ├── thermalprinter.h/cpp  # الطابعة الحرارية
    │   ├── gdiprinter.h/cpp  # طابعة GDI
    │   └── barcodehandler.h/cpp  # قارئ الباركود
    └── ui/                   # واجهة المستخدم
        ├── mainwindow.h/cpp
        ├── loginwidget.h/cpp
        ├── activationwidget.h/cpp
        ├── poswidget.h/cpp
        ├── productswidget.h/cpp
        ├── customerswidget.h/cpp
        ├── reportswidget.h/cpp
        ├── settingswidget.h/cpp
        ├── userswidget.h/cpp
        └── widgets/
            └── sidebar.h/cpp
```

---

## تسجيل الدخول الافتراضي

| الحقل | القيمة |
|-------|--------|
| اسم المستخدم | `admin` |
| كلمة المرور | `admin` |

> **مهم:** قم بتغيير كلمة المرور الافتراضية فوراً بعد أول تسجيل دخول!

---

## قاعدة البيانات

يستخدم التطبيق **SQLite** كقاعدة بيانات محلية. يتم إنشاء الجداول تلقائياً عند أول تشغيل:

- `users` - المستخدمون
- `categories` - أقسام المنتجات
- `products` - المنتجات
- `customers` - العملاء
- `sales` - المبيعات
- `sale_items` - عناصر المبيعات
- `settings` - الإعدادات

ملف قاعدة البيانات: `smartpos.db` (يتم إنشاؤه تلقائياً)

---

## الترخيص

MIT License
