unit LangRes;

{ Runtime translation for SpeedyCenter.

  Replaces the resource-DLL approach (reinit.pas + SpeedyCenter.CHS/CHT/ENU DLLs that
  carried complete translated forms and a string table bound to the exe's resource IDs).
  The language file now only holds DIFFERENCES, keyed by things that do not change when
  the exe is rebuilt:

    * form properties  by class + component path   (O:MDIForm.O:CloseBtn.Caption)
    * resourcestrings  by their original text        ("Disconnected" -> "斷線")

  Forms are always loaded from the exe; a translation is applied only while the
  property still holds the original it was translated from, so a changed form never
  gets a wrong text - it shows the new original until a translator updates the XLIFF.
  Editing a .dfm therefore takes effect with a normal build of SpeedyCenter; the
  language files only change when translations change.

  The language file (<exe name>.<LOCALE_SABBREVLANGNAME>, e.g. SpeedyCenter.CHT) is
  generated from Lang\xliff\<Lang>\ by Lang\Build-LangFiles.ps1. UTF-8 text, one entry
  per line, fields separated by TAB, with \t \n \r \\ escaped:

    SPEEDYCENTER-LANG 1                       header
    P  <FormClass>  <PixelsPerInch>           design PPI of the form (layout values)
    F  <FormClass>  <path>  s|i  <original>  <translation>
    S  <original>  <translation>
    I  <resource ID>  <original>  <translation>
       only for originals that have different translations in different places; used
       only while that ID still loads exactly <original> (else the S entry applies)

  When translations are applied
    * LoadTranslation: all forms that already exist (and the frames they contain).
    * TranslateScreen: call after the auto-created forms exist (SpeedyCenter.cpp).
    * Forms and frames created later: a WH_CBT hook on the VCL thread notices when their
      first window is created and queues them; they are translated right after that, at
      the next safe point before anything is painted - when a window is activated or the
      message loop fetches the next message (WH_GETMESSAGE).
      NOT inside HCBT_CREATEWND itself: anything that creates another window there
      (ClientWidth, ComboBox items, ...) would overwrite Vcl.Controls.CreationControl
      while CreateWindowEx is still running, and the application crashes. }

interface

uses
  Winapi.Windows, System.SysUtils, System.Classes;

{ Loads the language file for Locale and translates the existing forms.
  Returns False when there is no (valid) language file: the original texts stay. }
function LoadTranslation(Locale: LCID): Boolean;

{ Translates every form and data module that currently exists. }
procedure TranslateScreen;

{ Translates one form / frame (and the frames it contains). Safe to call repeatedly. }
procedure TranslateComponent(Root: TComponent);

implementation

uses
  System.TypInfo, System.Generics.Collections, Vcl.Controls, Vcl.Forms;

const
  FileHeader = 'SPEEDYCENTER-LANG 1';

type
  TEntry = record
    Path: string;
    IsInt: Boolean;
    Src, Tgt: string;
    SrcInt, TgtInt: Integer;
  end;

  TFormData = class
  public
    DesignPPI: Integer;
    Entries: TList<TEntry>;
    constructor Create;
    destructor Destroy; override;
  end;

  { Remembers which forms/frames were translated; forgets them when they are freed,
    so a new form that happens to get the same address is translated again. }
  TTracker = class(TComponent)
  protected
    procedure Notification(AComponent: TComponent; Operation: TOperation); override;
  public
    Done: TDictionary<TComponent, Boolean>;
    Pending: TList<TComponent>;            // queued by the CBT hook, not translated yet
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
  end;

var
  Strings: TDictionary<string, string> = nil;
  StringsById: TDictionary<Integer, TPair<string, string>> = nil;   // ID -> (original, translation)
  Forms: TObjectDictionary<string, TFormData> = nil;   // key: upper-case class name
  Tracker: TTracker = nil;
  CbtHook: HHOOK = 0;
  MsgHook: HHOOK = 0;
  Processing: Boolean = False;
  PrevLoadResStringFunc: function(ResStringRec: PResStringRec): string = nil;

{ TFormData }

constructor TFormData.Create;
begin
  inherited Create;
  DesignPPI := 96;
  Entries := TList<TEntry>.Create;
end;

destructor TFormData.Destroy;
begin
  Entries.Free;
  inherited;
end;

{ TTracker }

constructor TTracker.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  Done := TDictionary<TComponent, Boolean>.Create;
  Pending := TList<TComponent>.Create;
end;

destructor TTracker.Destroy;
var
  C: TComponent;
begin
  for C in Done.Keys do
    C.RemoveFreeNotification(Self);
  for C in Pending do
    if not Done.ContainsKey(C) then
      C.RemoveFreeNotification(Self);
  FreeAndNil(Pending);
  FreeAndNil(Done);
  inherited;
end;

procedure TTracker.Notification(AComponent: TComponent; Operation: TOperation);
begin
  inherited;
  if Operation = opRemove then
  begin
    if Done <> nil then Done.Remove(AComponent);
    if Pending <> nil then Pending.Remove(AComponent);
  end;
end;

{ resourcestrings }

{ Same as System.LoadResString (the resource always comes from the exe), then the
  translation is looked up by the original text. Installed as LoadResStringFunc. }
function LangLoadResString(ResStringRec: PResStringRec): string;
var
  Buffer: array[0..4095] of Char;
  T: string;
  P: TPair<string, string>;
begin
  Result := '';
  if ResStringRec = nil then Exit;
  if ResStringRec.Identifier >= 64 * 1024 then
    Exit(PChar(ResStringRec.Identifier));
  SetString(Result, Buffer, LoadString(FindResourceHInstance(ResStringRec.Module^),
    ResStringRec.Identifier, Buffer, Length(Buffer)));
  if (StringsById <> nil) and (ResStringRec.Module^ = HInstance) and
     StringsById.TryGetValue(ResStringRec.Identifier, P) and (P.Key = Result) then
    Exit(P.Value);
  if (Strings <> nil) and Strings.TryGetValue(Result, T) then
    Result := T;
end;

{ The RTL would load "<exe>.<lang>" by itself as a resource DLL the first time a
  resource is needed (System.DelayLoadResourceModule). Forms and strings must always
  come from the exe now - also when an old resource DLL is still lying around - so pin
  the exe as its own resource module. }
procedure PinResourceModule;
var
  M: PLibModule;
begin
  M := LibModuleList;
  while M <> nil do
  begin
    if M.Instance = HInstance then
    begin
      if (M.ResInstance <> 0) and (M.ResInstance <> M.Instance) then
        FreeLibrary(M.ResInstance);
      M.ResInstance := M.Instance;
    end;
    M := M.Next;
  end;
  ResStringCleanupCache;
end;

{ language file }

function Unescape(const S: string): string;
var
  I: Integer;
  B: TStringBuilder;
begin
  if Pos('\', S) = 0 then Exit(S);
  B := TStringBuilder.Create(Length(S));
  try
    I := 1;
    while I <= Length(S) do
    begin
      if (S[I] = '\') and (I < Length(S)) then
      begin
        Inc(I);
        case S[I] of
          't': B.Append(#9);
          'n': B.Append(#10);
          'r': B.Append(#13);
        else
          B.Append(S[I]);
        end;
      end
      else
        B.Append(S[I]);
      Inc(I);
    end;
    Result := B.ToString;
  finally
    B.Free;
  end;
end;

function GetFormData(const ClassName: string): TFormData;
var
  Key: string;
begin
  Key := UpperCase(ClassName);
  if not Forms.TryGetValue(Key, Result) then
  begin
    Result := TFormData.Create;
    Forms.Add(Key, Result);
  end;
end;

function ReadLanguageFile(const FileName: string): Boolean;
var
  Lines: TStringList;
  F: TArray<string>;
  I: Integer;
  E: TEntry;
  Id: Integer;
begin
  Result := False;
  Lines := TStringList.Create;
  try
    try
      Lines.LoadFromFile(FileName, TEncoding.UTF8);
    except
      Exit;
    end;
    if (Lines.Count = 0) or (Trim(Lines[0]) <> FileHeader) then
      Exit;   // not a language file (e.g. an old resource DLL with the same name)
    for I := 1 to Lines.Count - 1 do
    begin
      F := Lines[I].Split([#9]);
      if Length(F) = 0 then Continue;
      if (F[0] = 'S') and (Length(F) = 3) then
        Strings.AddOrSetValue(Unescape(F[1]), Unescape(F[2]))
      else if (F[0] = 'I') and (Length(F) = 4) and TryStrToInt(F[1], Id) then
        StringsById.AddOrSetValue(Id, TPair<string, string>.Create(Unescape(F[2]), Unescape(F[3])))
      else if (F[0] = 'P') and (Length(F) = 3) then
        GetFormData(F[1]).DesignPPI := StrToIntDef(F[2], 96)
      else if (F[0] = 'F') and (Length(F) = 6) then
      begin
        E.Path := F[2];
        E.IsInt := F[3] = 'i';
        E.Src := Unescape(F[4]);
        E.Tgt := Unescape(F[5]);
        E.SrcInt := 0;
        E.TgtInt := 0;
        if E.IsInt and not (TryStrToInt(E.Src, E.SrcInt) and TryStrToInt(E.Tgt, E.TgtInt)) then
          Continue;
        GetFormData(F[1]).Entries.Add(E);
      end;
    end;
    Result := True;
  finally
    Lines.Free;
  end;
end;

function LanguageFileName(Locale: LCID): string;
var
  Abbrev: array[0..15] of Char;
  Base: string;
begin
  Result := '';
  if GetLocaleInfo(Locale, LOCALE_SABBREVLANGNAME, Abbrev, Length(Abbrev)) = 0 then
    Exit;
  Base := ChangeFileExt(GetModuleName(HInstance), '');
  Result := Base + '.' + Abbrev;            // e.g. SpeedyCenter.CHT
  if not FileExists(Result) then
  begin
    Abbrev[2] := #0;                         // language only, e.g. SpeedyCenter.ZH
    Result := Base + '.' + PChar(@Abbrev[0]);
    if not FileExists(Result) then
      Result := '';
  end;
end;

{ applying form translations }

{ "Columns[2]" -> Name = Columns, Index = 2;  "Caption" -> Index = -1 }
function SplitSegment(const Seg: string; out Name: string; out Index: Integer): Boolean;
var
  P: Integer;
begin
  P := Pos('[', Seg);
  if P = 0 then
  begin
    Name := Seg;
    Index := -1;
    Exit(Name <> '');
  end;
  Name := Copy(Seg, 1, P - 1);
  Result := (Name <> '') and (Seg[Length(Seg)] = ']') and
    TryStrToInt(Copy(Seg, P + 1, Length(Seg) - P - 1), Index) and (Index >= 0);
end;

{ O:Form.O:Panel.O:ListView.Columns[1].Caption -> object = Columns[1], property = Caption.
  The first segment is the root itself. Components are looked up by name in their owner:
  the form, or a frame (a frame owns the components inside it). }
function Resolve(Root: TComponent; const Path: string; out Obj: TObject;
  out Prop: string; out Index: Integer): Boolean;
var
  Segs: TArray<string>;
  I, Idx: Integer;
  Scope, C: TComponent;
  Name: string;
  Info: PPropInfo;
begin
  Result := False;
  Segs := Path.Split(['.']);
  if (Length(Segs) < 2) or not Segs[0].StartsWith('O:') then Exit;
  Obj := Root;
  Scope := Root;
  for I := 1 to High(Segs) - 1 do
  begin
    if Segs[I].StartsWith('O:') then
    begin
      Name := Copy(Segs[I], 3, MaxInt);
      if (Name = '') or (Name[1] = '#') then Exit;   // unnamed component
      C := Scope.FindComponent(Name);
      if C = nil then Exit;
      Obj := C;
      if C is TCustomFrame then
        Scope := C;
    end
    else
    begin
      if (Obj is TStrings) or not SplitSegment(Segs[I], Name, Idx) then Exit;
      Info := GetPropInfo(Obj, Name);
      if (Info = nil) or (Info.PropType^.Kind <> tkClass) then Exit;
      Obj := GetObjectProp(Obj, Info);
      if Obj = nil then Exit;
      if Idx >= 0 then
      begin
        if not (Obj is TCollection) or (Idx >= TCollection(Obj).Count) then Exit;
        Obj := TCollection(Obj).Items[Idx];
      end;
    end;
  end;
  Result := SplitSegment(Segs[High(Segs)], Prop, Index);
end;

{ Applies one entry; returns what happened (for the optional log). }
function ApplyEntry(Root: TComponent; const E: TEntry; DesignPPI, CurPPI: Integer): string;
var
  Obj: TObject;
  Prop, S: string;
  Index, Cur: Integer;
  Info: PPropInfo;
begin
  if not Resolve(Root, E.Path, Obj, Prop, Index) then Exit('not found');

  if Obj is TStrings then                     // Items.Strings[n]
  begin
    if E.IsInt or (Prop <> 'Strings') or (Index < 0) or (Index >= TStrings(Obj).Count) then
      Exit('not found');
    S := TStrings(Obj)[Index];
    if S <> E.Src then Exit('differs: ' + S);
    TStrings(Obj)[Index] := E.Tgt;
    Exit('applied');
  end;

  if Index >= 0 then Exit('not found');
  Info := GetPropInfo(Obj, Prop);
  if (Info = nil) or (Info.SetProc = nil) then Exit('not found');
  case Info.PropType^.Kind of
    tkUString, tkLString, tkWString, tkString:
      begin
        if E.IsInt then Exit('type');
        S := GetStrProp(Obj, Info);
        if S <> E.Src then Exit('differs: ' + S);
        SetStrProp(Obj, Info, E.Tgt);
        Result := 'applied';
      end;
    tkInteger:
      begin
        if not E.IsInt then Exit('type');
        Cur := GetOrdProp(Obj, Info);
        if Cur = E.SrcInt then
        begin
          SetOrdProp(Obj, Info, E.TgtInt);
          Result := 'applied';
        end
        else if (CurPPI <> DesignPPI) and (Abs(Cur - MulDiv(E.SrcInt, CurPPI, DesignPPI)) <= 1) then
        begin
          SetOrdProp(Obj, Info, MulDiv(E.TgtInt, CurPPI, DesignPPI));   // form was scaled (DPI)
          Result := 'applied (scaled)';
        end
        else if (SameText(Prop, 'ClientWidth') or SameText(Prop, 'ClientHeight')) and
                (Abs(Cur - E.SrcInt) <= 10) then
        begin
          // The client size of a styled form differs by a few pixels from the .dfm
          // (VCL style borders): apply the translator's change as a difference.
          SetOrdProp(Obj, Info, Cur + E.TgtInt - E.SrcInt);
          Result := 'applied (difference)';
        end
        else if (SameText(Prop, 'ClientWidth') or SameText(Prop, 'ClientHeight')) and
                (CurPPI <> DesignPPI) and (Abs(Cur - MulDiv(E.SrcInt, CurPPI, DesignPPI)) <= 10) then
        begin
          SetOrdProp(Obj, Info, Cur + MulDiv(E.TgtInt - E.SrcInt, CurPPI, DesignPPI));
          Result := 'applied (difference, scaled)';
        end
        else
          Result := 'differs: ' + IntToStr(Cur);
      end;
  else
    Result := 'type';
  end;
end;

{ Optional diagnostics: set SPEEDYCENTER_LANGLOG=<file> to get one line per entry. }
var
  LogFileName: string = '';

procedure LogLine(const S: string);
var
  F: TStreamWriter;
begin
  if LogFileName = '' then Exit;
  try
    F := TStreamWriter.Create(LogFileName, True, TEncoding.UTF8);
    try
      F.WriteLine(S);
    finally
      F.Free;
    end;
  except
  end;
end;

function HasTranslations(C: TComponent): Boolean;
begin
  Result := (Forms <> nil) and Forms.ContainsKey(UpperCase(C.ClassName));
end;

procedure TranslateComponent(Root: TComponent);
var
  Data: TFormData;
  E: TEntry;
  I, CurPPI: Integer;
begin
  if (Root = nil) or (Tracker = nil) then Exit;
  if [csLoading, csReading, csDestroying, csDesigning] * Root.ComponentState <> [] then Exit;
  if Tracker.Done.ContainsKey(Root) then Exit;
  Tracker.Done.Add(Root, True);
  Root.FreeNotification(Tracker);

  // frames inside: their own translations first, then the overrides of this form
  for I := 0 to Root.ComponentCount - 1 do
    if (Root.Components[I] is TCustomFrame) and HasTranslations(Root.Components[I]) then
      TranslateComponent(Root.Components[I]);

  if not Forms.TryGetValue(UpperCase(Root.ClassName), Data) then Exit;
  if Root is TControl then CurPPI := TControl(Root).CurrentPPI else CurPPI := Data.DesignPPI;
  if CurPPI <= 0 then CurPPI := Data.DesignPPI;
  LogLine(Format('== %s (%s) PPI design %d / current %d', [Root.ClassName, Root.Name, Data.DesignPPI, CurPPI]));
  for E in Data.Entries do
    try
      LogLine('  ' + E.Path + ' : ' + ApplyEntry(Root, E, Data.DesignPPI, CurPPI));
    except
      on X: Exception do
        LogLine('  ' + E.Path + ' : error ' + X.Message);   // keep the original
    end;
end;

procedure TranslateScreen;
var
  I: Integer;
begin
  if Tracker = nil then Exit;
  for I := 0 to Screen.CustomFormCount - 1 do
    TranslateComponent(Screen.CustomForms[I]);
  for I := 0 to Screen.DataModuleCount - 1 do
    TranslateComponent(Screen.DataModules[I]);
end;

{ Translates the queued forms / frames. Only called at safe points: never while
  CreateWindowEx is running (see the unit header). }
procedure ProcessPending;
var
  Items: TArray<TComponent>;
  C: TComponent;
begin
  if Processing or (Tracker = nil) or (Tracker.Pending.Count = 0) then Exit;
  if CreationControl <> nil then Exit;         // a VCL window is being created right now
  Processing := True;
  try
    Items := Tracker.Pending.ToArray;
    Tracker.Pending.Clear;
    for C in Items do
    begin
      if Tracker.Done.ContainsKey(C) then Continue;
      if [csLoading, csReading] * C.ComponentState <> [] then
        Tracker.Pending.Add(C)                 // still being created: next time
      else
      begin
        try
          TranslateComponent(C);
        except
          // keep the original texts
        end;
      end;
    end;
  finally
    Processing := False;
  end;
end;

{ A window is about to be created for CreationControl: queue the forms / frames it
  belongs to (walking up the owners). }
procedure QueueOwners(C: TComponent);
begin
  while C <> nil do
  begin
    if HasTranslations(C) and not Tracker.Done.ContainsKey(C) and
       (Tracker.Pending.IndexOf(C) < 0) and not (csDestroying in C.ComponentState) then
    begin
      Tracker.Pending.Add(C);
      C.FreeNotification(Tracker);
    end;
    C := C.Owner;
  end;
end;

function CbtProc(Code: Integer; WParam: WPARAM; LParam: LPARAM): LRESULT; stdcall;
begin
  try
    case Code of
      HCBT_CREATEWND:
        if (CreationControl <> nil) and (Tracker <> nil) then
          QueueOwners(CreationControl);
      HCBT_ACTIVATE:
        ProcessPending;                        // a form is being shown
    end;
  except
    // never let a translation problem break window creation
  end;
  Result := CallNextHookEx(CbtHook, Code, WParam, LParam);
end;

function GetMsgProc(Code: Integer; WParam: WPARAM; LParam: LPARAM): LRESULT; stdcall;
begin
  if (Code = HC_ACTION) and (Tracker <> nil) and (Tracker.Pending.Count > 0) then
    try
      ProcessPending;                          // before the message (e.g. WM_PAINT) is dispatched
    except
    end;
  Result := CallNextHookEx(MsgHook, Code, WParam, LParam);
end;

function LoadTranslation(Locale: LCID): Boolean;
var
  FileName: string;
begin
  Result := False;
  PinResourceModule;
  FileName := LanguageFileName(Locale);
  if FileName = '' then Exit;

  Strings.Clear;
  StringsById.Clear;
  Forms.Clear;
  if not ReadLanguageFile(FileName) then
  begin
    Strings.Clear;
    StringsById.Clear;
    Forms.Clear;
    Exit;
  end;

  if Tracker = nil then
    Tracker := TTracker.Create(nil);
  if CbtHook = 0 then
    CbtHook := SetWindowsHookEx(WH_CBT, @CbtProc, 0, GetCurrentThreadId);
  if MsgHook = 0 then
    MsgHook := SetWindowsHookEx(WH_GETMESSAGE, @GetMsgProc, 0, GetCurrentThreadId);
  TranslateScreen;
  Result := True;
end;

initialization
  Strings := TDictionary<string, string>.Create;
  StringsById := TDictionary<Integer, TPair<string, string>>.Create;
  LogFileName := GetEnvironmentVariable('SPEEDYCENTER_LANGLOG');
  Forms := TObjectDictionary<string, TFormData>.Create([doOwnsValues]);
  PinResourceModule;
  PrevLoadResStringFunc := LoadResStringFunc;
  LoadResStringFunc := LangLoadResString;

finalization
  if CbtHook <> 0 then
    UnhookWindowsHookEx(CbtHook);
  CbtHook := 0;
  if MsgHook <> 0 then
    UnhookWindowsHookEx(MsgHook);
  MsgHook := 0;
  LoadResStringFunc := PrevLoadResStringFunc;
  FreeAndNil(Tracker);
  FreeAndNil(Forms);
  FreeAndNil(StringsById);
  FreeAndNil(Strings);

end.
