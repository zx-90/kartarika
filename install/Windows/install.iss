; ��� ����������
#define   Name       "���������"
; ������ ����������
#define   Version    "0.1"
; �����-�����������
#define   Publisher  "Kartarika"
; ���� ����� ������������
#define   URL        "http://kartarika.ru"
; ��� ������������ ������
#define   ExeName    "kar.exe"

[Setup]

; ���������� ������������� ����������, 
;��������������� ����� Tools -> Generate GUID
AppId={{B7F1DC74-A423-4002-8567-CBD5E4A19D44}

; ������ ����������, ������������ ��� ���������
AppName={#Name}
AppVersion={#Version}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}

; ���� ��������� ��-���������
DefaultDirName={pf}\{#Name}
; ��� ������ � ���� "����"
DefaultGroupName={#Name}

; �������, ���� ����� ������� ��������� setup � ��� ������������ �����
OutputDir=..\..\build\bin\install
OutputBaseFileName=install

; ���� ������
SetupIconFile=..\Resources\kartarika.ico

; ��������� ������
Compression=lzma
SolidCompression=yes

[Tasks]
; �������� ������ �� ������� �����
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl";
Name: "english"; MessagesFile: "compiler:Default.isl";

[Files]

; ����������� ����
Source: "..\..\build\bin\kar.exe"; DestDir: "{app}"; Flags: ignoreversion

; ������������� �������
Source: "..\..\build\bin\LLVM-C.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]

Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"

Name: "{commondesktop}\{#Name}"; Filename: "{app}\{#ExeName}"; Tasks: desktopicon