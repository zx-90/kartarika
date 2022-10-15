; Имя приложения
#define   Name       "Картарика"
; Версия приложения
#define   Version    "0.1"
; Фирма-разработчик
#define   Publisher  "Kartarika"
; Сафт фирмы разработчика
#define   URL        "http://kartarika.ru"
; Имя исполняемого модуля
#define   ExeName    "kar.exe"

[Setup]

; Уникальный идентификатор приложения, 
;сгенерированный через Tools -> Generate GUID
AppId={{B7F1DC74-A423-4002-8567-CBD5E4A19D44}

; Прочая информация, отображаемая при установке
AppName={#Name}
AppVersion={#Version}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}

; Путь установки по-умолчанию
DefaultDirName={pf}\{#Name}
; Имя группы в меню "Пуск"
DefaultGroupName={#Name}

; Каталог, куда будет записан собранный setup и имя исполняемого файла
OutputDir=..\..\build\bin\install
OutputBaseFileName=install

; Файл иконки
SetupIconFile=..\Resources\kartarika.ico

; Параметры сжатия
Compression=lzma
SolidCompression=yes

[Tasks]
; Создание иконки на рабочем столе
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl";
Name: "english"; MessagesFile: "compiler:Default.isl";

[Files]

; Исполняемый файл
Source: "..\..\build\bin\kar.exe"; DestDir: "{app}"; Flags: ignoreversion

; Прилагающиеся ресурсы
Source: "..\..\build\bin\LLVM-C.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]

Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"

Name: "{commondesktop}\{#Name}"; Filename: "{app}\{#ExeName}"; Tasks: desktopicon