Imports System.IO
Imports System.Collections.ObjectModel
Imports System.Text.RegularExpressions
Imports System.Threading.Tasks
Imports System.ComponentModel
Imports System.Reflection

Public Class Scenario
    Implements INotifyPropertyChanged

    Public ScenarioName As String

    Public ActiveLayerType As System.Type

    Private _MapCount As Integer

    'Public Shared ReadOnly ImagesProperty As DependencyProperty =
    '    DependencyProperty.Register("Images", GetType(ObservableCollection(Of ZeldaImage)), GetType(Scenario),
    '    New PropertyMetadata(New ObservableCollection(Of ZeldaImage)))
    'Public Shared ReadOnly AnimDefsProperty As DependencyProperty =
    '    DependencyProperty.Register("AnimDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
    '    New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))
    'Public Shared ReadOnly ObjectDefsProperty As DependencyProperty =
    '    DependencyProperty.Register("ObjectDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
    '    New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))
    'Public Shared ReadOnly EnemyDefsProperty As DependencyProperty =
    '    DependencyProperty.Register("EnemyDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
    '    New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))
    'Public Shared ReadOnly MiscDefsProperty As DependencyProperty =
    '    DependencyProperty.Register("MiscDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
    '    New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))

    'Public Shared ReadOnly TilesetsProperty As DependencyProperty =
    '    DependencyProperty.Register("Tilesets", GetType(ObservableCollection(Of Tileset)), GetType(Scenario))

    '' Player starting position and map
    'Public Shared ReadOnly StartXProperty As DependencyProperty =
    '    DependencyProperty.Register("StartX", GetType(Byte), GetType(Scenario))
    'Public Shared ReadOnly StartYProperty As DependencyProperty =
    '    DependencyProperty.Register("StartY", GetType(Byte), GetType(Scenario))
    'Public Shared ReadOnly StartMapIndexProperty As DependencyProperty =
    '    DependencyProperty.Register("StartMapIndex", GetType(Integer), GetType(Scenario))
    'Public Shared ReadOnly MapsProperty As DependencyProperty = _
    '                   DependencyProperty.Register("Maps", _
    '                   GetType(ObservableCollection(Of MapData)), GetType(Scenario), _
    '                   New PropertyMetadata(New ObservableCollection(Of MapData)()))

    Private _Maps As New ObservableCollection(Of MapData)

    Public Property Maps As ObservableCollection(Of MapData)
        Get
            Return _Maps
        End Get
        Set(value As ObservableCollection(Of MapData))
            If value IsNot _Maps Then
                _Maps = value
                RaisePropertyChanged("Maps")
            End If
        End Set
    End Property

    Public Property Images As New ObservableCollection(Of ZeldaImage)

    Public Property AnimDefs As New ObservableDictionary(Of String, ZDef)
    Public Property ObjectDefs As New ObservableDictionary(Of String, ZDef)
    Public Property EnemyDefs As New ObservableDictionary(Of String, ZDef)
    Public Property MiscDefs As New ObservableDictionary(Of String, ZDef)


    Public Property Tilesets As New ObservableCollection(Of Tileset)


    'Public ReadOnly Property AllDefs As Collection(Of ZDef)
    '    Get
    '        Return ObjectDefs.Values.Union(EnemyDefs.Values).Union(MiscDefs.Values).Union(AnimDefs.Values)
    '    End Get
    'End Property

    Public ReadOnly Property ActiveLayer As IMapLayer
        Get
            'Return (From containers As MapContainer In MainWindow.Instance.LayerContainer.Children
            '        From m As IMapLayer In containers.Children
            '        Where m.GetType() = ActiveLayerType
            '        Select m).First()
            Return Nothing
        End Get
    End Property


    Private _FileName As String

    Private Sub Log(LogStr As String)
        Debug.Write(Now.ToFileTime & ": " & LogStr & vbCrLf)
    End Sub

    Public Async Function LoadScenario(FileName As String) As Task
        Tilesets = New ObservableCollection(Of Tileset)()
        Tilesets.Add(New Tileset("dungeon", IO.Path.Combine(MainWindow.ZeldaFolder, "maps\dungeon.bmp")))
        Tilesets.Add(New Tileset("town", IO.Path.Combine(MainWindow.ZeldaFolder, "maps\town.bmp")))
        Tilesets.Add(New Tileset("graveyard", IO.Path.Combine(MainWindow.ZeldaFolder, "maps\graveyard.bmp")))

        Log("Starting SPASM")
        SPASMHelper.Initialize(MainWindow.ZeldaFolder)

        Log("Loading images")
        Await LoadImages(MainWindow.ZeldaFolder & "\graphics.asm")

        _FileName = FileName
        SPASMHelper.Assembler.Defines.Add("INCLUDE_ALL", 1)

        Log("Assembling map")
        Dim Data = SPASMHelper.AssembleFile(FileName)

        Log("Loading animate defs")
        Await LoadDefs(MainWindow.ZeldaFolder & "\animatedef.inc", AnimDefs, GetType(ZAnim))
        Log("Loading object defs")
        Await LoadDefs(MainWindow.ZeldaFolder & "\objectdef.inc", ObjectDefs, GetType(ZObject))
        Log("Loading misc defs")
        Await LoadDefs(MainWindow.ZeldaFolder & "\miscdef.inc", MiscDefs, GetType(ZMisc))
        Log("Loading enemy defs")
        Await LoadDefs(MainWindow.ZeldaFolder & "\enemydef.inc", EnemyDefs, GetType(ZEnemy))

        Dim Reader As New StreamReader(FileName)
        Dim ScenarioContents As String = Await Reader.ReadToEndAsync()
        ScenarioContents = ScenarioContents.ToUpper().Replace(vbCrLf, vbLf)
        Reader.Close()

        'Dim CompList As New List(Of RegexCompilationInfo)
        'CompList.Add(New RegexCompilationInfo("^ANIMATE_SECTION\(\)" & vbLf & _
        '    "(^\s+(?<AnimName>[A-Z_]+)\((?<AnimArgs>.*)\)\s*)*\s*" & _
        '    "^OBJECT_SECTION\(\)" & vbLf & _
        '    "(^\s+(?<ObjectName>[A-Z_]+)\((?<ObjectArgs>.*)\)\s*)*\s*" & _
        '    "^ENEMY_SECTION\(\)" & vbLf & _
        '    "(^\s+(?<EnemyName>[A-Z_]+)\((?<EnemyArgs>.*)\)\s*)*\s*" & _
        '    "^MISC_SECTION\(\)" & vbLf & _
        '    "(^\s+(?<MiscName>[A-Z_]+)\((?<MiscArgs>.*)\)\s*)*\s*",
        '    RegexOptions.Multiline Or RegexOptions.CultureInvariant, _
        '    "MapDataRegex", _
        '    "WPFZ80MapEditor", _
        '    True))
        'CompList.Add(New RegexCompilationInfo("^;(?<Name>[a-zA-Z][a-zA-Z ]+) - (?<Description>.+)\s*" & _
        '    "(^; (?<ArgName>\w+) - (?<ArgDesc>.+)\s*)*" & _
        '    "(^;Properties\s*)?" & _
        '    "(^; [a-zA-Z_]+ = .+\s*)*" & _
        '    "#macro (?<MacroName>[a-z0-9_]+).*\s*$",
        '    RegexOptions.Multiline Or RegexOptions.CultureInvariant, _
        '    "ZDefRegex", _
        '    "WPFZ80MapEditor", _
        '    True))
        'CompList.Add(New RegexCompilationInfo("^(?<Name>[a-z_]+_gfx)(\s*|\s+with\s+bm_map\s*=\s*(?<X>\d+)x(?<Y>\d+)\s*)" & _
        '    "^#include\s+""(?<FileName>.+)""\s*" & _
        '    "(^\s*|(?<ExtraDefines>(^[a-z0-9_]+\s*=\s*[a-z0-9_]+\s*)+))$",
        '    RegexOptions.Multiline Or RegexOptions.CultureInvariant, _
        '    "GraphicsRegex", _
        '    "WPFZ80MapEditor", _
        '    True))
        'Dim Asm As New AssemblyName("RegexLib, Version=1.0.0.1001, Culture=neutral, PublicKeyToken=null")
        'Regex.CompileToAssembly(CompList.ToArray(), Asm)

        Dim MaxX = -1
        Dim MaxY = -1
        _MapCount = 0
        Dim CurShiftX = 0, CurShiftY = 0

        For Each Label In SPASMHelper.Labels.Keys
            If Label Like "*_MAP_##" Then
                Dim x = SPASMHelper.Labels(Label & "_X")
                MaxX = Math.Max(x, MaxX)
                Dim y = SPASMHelper.Labels(Label & "_Y")
                MaxY = Math.Max(y, MaxY)
                Dim Tileset = SPASMHelper.Labels(Label & "_TILESET")
                Dim IsCompressed = Not SPASMHelper.Labels.ContainsKey(Label & "_RAW")

                Dim RawMapData = Data.Skip(SPASMHelper.Labels(Label))

                Dim MapData = New MapData(RawMapData, Me, Tileset, IsCompressed)

                Dim MapMacrosStart = ScenarioContents.IndexOf(Label & ":")
                Dim MapMacrosEnd = Math.Max(ScenarioContents.IndexOf("END_SECTION()", MapMacrosStart), MapMacrosStart)

                Dim MapContents = ScenarioContents.Substring(MapMacrosStart, MapMacrosEnd - MapMacrosStart)

                Log("Loading map " & x & "," & y)
                Dim Rx As New MapDataRegex()

                Log("Running regular expression...")
                Dim Matches = Rx.Matches(MapContents)
                Log("Done")

                Dim RawScenarioData = Data.Skip(SPASMHelper.Labels(Label & "_DEFAULTS"))
                Dim RawData As New MemoryStream(RawScenarioData.ToArray())

                If Matches.Count = 1 Then
                    Dim Groups = Matches(0).Groups

                    Log("Loading animated tiles...")

                    RawData.ReadByte() : RawData.ReadByte()
                    For i = 0 To Groups("AnimName").Captures.Count - 1
                        Dim Params = Split(Groups("AnimArgs").Captures(i).Value, ",")
                        Dim Anim As New ZAnim(AnimDefs(Groups("AnimName").Captures(i).Value), Params(0), Params(1), RawData)
                        MapData.ZAnims.Add(Anim)
                    Next
                    Log("Done")

                    Log("Loading the rest...")
                    RawData.ReadByte() : RawData.ReadByte()
                    For i = 0 To Groups("ObjectName").Captures.Count - 1
                        Dim Params = Split(Groups("ObjectArgs").Captures(i).Value, ",")
                        Dim Obj As New ZObject(ObjectDefs(Groups("ObjectName").Captures(i).Value), RawData, Params)
                        MapData.ZObjects.Add(Obj)
                    Next

                    RawData.ReadByte() : RawData.ReadByte()
                    For i = 0 To Groups("EnemyName").Captures.Count - 1
                        Dim Params = Split(Groups("EnemyArgs").Captures(i).Value, ",")
                        Dim Enemy As New ZEnemy(EnemyDefs(Groups("EnemyName").Captures(i).Value), RawData, Params)
                        MapData.ZEnemies.Add(Enemy)
                    Next

                    RawData.ReadByte() : RawData.ReadByte()
                    For i = 0 To Groups("MiscName").Captures.Count - 1
                        Dim Params = Split(Groups("MiscArgs").Captures(i).Value, ",")
                        Dim Misc As New ZMisc(MiscDefs(Groups("MiscName").Captures(i).Value), RawData, Params)
                        MapData.ZMisc.Add(Misc)
                    Next
                    Log("Done")
                End If

                Log("Creating map...")
                MapData.X = x + CurShiftX
                MapData.Y = y + CurShiftY
                Dim OldX = MapData.X, OldY = MapData.Y
                AddMap(MapData)
                CurShiftX += MapData.X - OldX
                CurShiftY += MapData.Y - OldY
                Log("Done")
                ScenarioName = Left(Label, Len(Label) - 7)
            End If
        Next

        'For x = 0 To MaxX
        '    For y = 0 To MaxY
        '        Dim CurX = x, CurY = y
        '        Dim Exist = (From m In MainWindow.Instance.LayerContainer.Children Where Grid.GetColumn(m) = CurX And Grid.GetRow(m) = CurY).Count() > 0
        '        If Not Exist Then
        '            AddMap(x, y, Nothing)
        '        End If
        '    Next
        'Next


    End Function

    Private Function AddMap(MapCollection As Collection(Of MapData), Map As MapData) As MapData
        Dim OriginalMap As MapData = MapCollection.ToList().Find(Function(m) m.X = Map.X And m.Y = Map.Y)
        If OriginalMap IsNot Nothing AndAlso Not Map.Exists Then
            Return OriginalMap
        End If

        If Map.X < 0 Then
            MapCollection.ToList().ForEach(Sub(m) m.X -= Map.X)
            Map.X = 0
        End If
        If Map.Y < 0 Then
            MapCollection.ToList().ForEach(Sub(m) m.Y -= Map.Y)
            Map.Y = 0
        End If

        If Map.Exists And OriginalMap IsNot Nothing Then
            MapCollection.Remove(OriginalMap)
        End If
        MapCollection.Add(Map)

        Dim AddedMap = Map
        If Map.Exists Then
            AddedMap = AddMap(MapCollection, MapData.NonexistentMap(AddedMap.X - 1, AddedMap.Y))
            AddedMap = AddMap(MapCollection, MapData.NonexistentMap(AddedMap.X + 2, AddedMap.Y))
            AddedMap = AddMap(MapCollection, MapData.NonexistentMap(AddedMap.X - 1, AddedMap.Y - 1))
            AddedMap = AddMap(MapCollection, MapData.NonexistentMap(AddedMap.X, AddedMap.Y + 2))
        End If
        Return Map
    End Function

    Public Function AddMap(Map As MapData) As MapData
        Return AddMap(Maps, Map)
    End Function

    Private Function GetExistingMaps() As ICollection(Of MapData)
        Dim Existing = Maps.ToList().FindAll(Function(m) m.Exists).Select(Function(m) CType(m.Clone(), MapData)).ToList()
        Dim Leftmost = Existing.Min(Function(m) m.X)
        Dim Topmost = Existing.Min(Function(m) m.Y)
        Existing.ForEach(Sub(m)
                             m.X -= Leftmost
                             m.Y -= Topmost
                         End Sub)

        Return Existing
    End Function

    Public Sub RemoveMap(Map As MapData)
        Maps.Remove(Map)

        Dim Existing = GetExistingMaps()
        Maps.Clear()
        Dim CurShiftX = 0, CurShiftY = 0

        Dim TempMaps As New Collection(Of MapData)
        Existing.ToList().ForEach(Sub(m)
                                      Dim m2 = CType(m.Clone(), MapData)
                                      m2.X += CurShiftX
                                      m2.Y += CurShiftY
                                      Dim OldX = m2.X, OldY = m2.Y
                                      AddMap(TempMaps, m2)
                                      CurShiftX += m2.X - OldX
                                      CurShiftY += m2.Y - OldY
                                  End Sub)
        Maps = New ObservableCollection(Of MapData)(TempMaps)
    End Sub

    Private Shared Sub WriteAssemblyData(Stream As StreamWriter, Data As IEnumerable(Of Byte))
        Dim Index = 0
        While True
            Stream.Write(vbTab & ".db ")
            For i = 1 To 16
                Stream.Write(String.Format("${0:X2}", Data(Index)))
                Index += 1
                If Index = Data.Count Then
                    If i <> 16 Then Stream.Write(vbCrLf)
                    Exit Sub
                End If
                If i <> 16 Then Stream.Write(",")
            Next
            Stream.Write(vbCrLf)
        End While

    End Sub


    Private Class MapHierarchy
        Private _Maps(0 To 15, 0 To 15) As Integer
        Private _MaxX As Integer = -1, _MaxY As Integer = -1
        Private _MinX As Integer = Integer.MaxValue, _MinY = Integer.MaxValue

        Public Sub AddMap(X As Integer, Y As Integer, MapIndex As Integer)
            _MaxX = Math.Max(_MaxX, X)
            _MaxY = Math.Max(_MaxY, Y)
            _MinX = Math.Min(_MinX, X)
            _MinY = Math.Min(_MinY, Y)
            _Maps(X, Y + 1) = MapIndex
        End Sub

        Public Sub Write(Stream As StreamWriter)
            Dim Width = _MaxX + 1 - _MinX
            Dim Height = _MaxY + 1 - _MinY

            Stream.WriteLine("#ifdef INCLUDE_MAP_HIERARCHY")
            Stream.WriteLine("#ifndef __MAP_HIERARCHY_WIDTH_DEFINED")
            Stream.WriteLine("#define __MAP_HIERARCHY_WIDTH_DEFINED")
            Stream.WriteLine("map_hierarchy_width = " & Width)
            Stream.WriteLine("#endif")

            Dim Data(0 To Width * (Height + 1) - 1) As Byte
            For Y = 0 To Height
                For X = 0 To Width - 1
                    Data(Y * Width + X) = _Maps(X + _MinX, Y + _MinY)
                Next
            Next

            WriteAssemblyData(Stream, Data)
            Stream.WriteLine("#endif")
        End Sub
    End Class

    Public Sub SaveScenario()
        Dim Stream = New StreamWriter(_FileName)

        Stream.WriteLine("#ifdef INCLUDE_ALL")
        Stream.WriteLine("#define INCLUDE_MAPS")
        Stream.WriteLine("#define INCLUDE_DEFAULTS")
        Stream.WriteLine("#define INCLUDE_MAPS_TABLE")
        Stream.WriteLine("#define INCLUDE_DEFAULTS_TABLE")
        Stream.WriteLine("#define INCLUDE_MAP_HIERARCHY")
        Stream.WriteLine("#endif")

        Dim MapHierarchy As New MapHierarchy
        Dim DefaultsTable As String = ""
        Dim MapsTable As String = ""

        Dim MapIndex As Integer = 0
        For Each MapData In Maps.ToList().Where(Function(m) m.Exists)
            Dim MapId = String.Format("{0:D2}", MapIndex)
            Dim MapPrefix = ScenarioName & "_MAP_" & MapId

            Dim X = MapData.X
            Dim Y = MapData.Y
            Stream.WriteLine("#ifdef INCLUDE_MAPS")
            MapsTable &= vbTab & ".dw " & MapPrefix & vbCrLf

            Stream.WriteLine(MapPrefix & ":")
            Stream.WriteLine(MapPrefix & "_X = " & X)
            Stream.WriteLine(MapPrefix & "_Y = " & Y)
            Stream.WriteLine(MapPrefix & "_TILESET = " & Tilesets.IndexOf(MapData.Tileset))

            Dim CompressedMap = MapCompressor.Compress(MapData.TileData)
            WriteAssemblyData(Stream, CompressedMap)
            Stream.WriteLine("#endif")
            Stream.WriteLine("")

            Dim DefaultsLabel = MapPrefix & "_DEFAULTS"
            DefaultsTable &= vbTab & ".dw " & DefaultsLabel & vbCrLf

            Stream.WriteLine("#ifdef INCLUDE_DEFAULTS")
            Stream.WriteLine(DefaultsLabel & ":")

            Stream.WriteLine("animate_section()")

            For Each Anim In MapData.ZAnims
                Stream.WriteLine(vbTab & Anim.ToMacro())
            Next

            Stream.WriteLine("object_section()")

            For Each Obj In MapData.ZObjects
                Stream.WriteLine(vbTab & Obj.ToMacro())
            Next

            Stream.WriteLine("enemy_section()")

            For Each Enemy In MapData.ZEnemies
                Stream.WriteLine(vbTab & Enemy.ToMacro())
            Next

            Stream.WriteLine("misc_section()")

            For Each Misc In MapData.ZMisc
                Stream.WriteLine(vbTab & Misc.ToMacro())
            Next

            Stream.WriteLine("end_section()")
            Stream.WriteLine("#endif")
            Stream.WriteLine("")

            MapHierarchy.AddMap(X, Y, MapIndex)
            MapIndex = MapIndex + 1
        Next

        MapHierarchy.Write(Stream)

        Stream.WriteLine("")

        Stream.WriteLine("#ifdef INCLUDE_MAPS_TABLE")
        Stream.Write(MapsTable)
        Stream.WriteLine("#endif")

        Stream.WriteLine("")

        Stream.WriteLine("#ifdef INCLUDE_DEFAULTS_TABLE")
        Stream.Write(DefaultsTable)
        Stream.WriteLine("#endif")

        Stream.WriteLine("")

        Stream.WriteLine("#undefine INCLUDE_MAPS")
        Stream.WriteLine("#undefine INCLUDE_DEFAULTS")
        Stream.WriteLine("#undefine INCLUDE_MAPS_TABLE")
        Stream.WriteLine("#undefine INCLUDE_DEFAULTS_TABLE")
        Stream.WriteLine("#undefine INCLUDE_MAP_HIERARCHY")

        Stream.Close()
    End Sub

    Private Sub RaisePropertyChanged(PropName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged

End Class
