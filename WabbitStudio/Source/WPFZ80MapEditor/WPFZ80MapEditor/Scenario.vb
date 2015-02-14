Imports System.IO
Imports System.Collections.ObjectModel
Imports System.Text.RegularExpressions
Imports System.Threading.Tasks
Imports System.ComponentModel

Public Class Scenario
    Implements INotifyPropertyChanged

    Private Shared ReadOnly ZDefRegex As New Regex("^;(?<Name>[a-zA-Z][a-zA-Z ]+) - (?<Description>.+)\s*" & _
            "(^; (?<ArgName>\w+) - (?<ArgDesc>.+)\s*)*" & _
            "(^;Properties\s*)?" & _
            "(^; [a-zA-Z_]+ = .+\s*)*" & _
            "#macro (?<MacroName>[a-z0-9_]+).*\s*$",
            RegexOptions.Multiline Or RegexOptions.CultureInvariant Or RegexOptions.Compiled)

    Private Shared ReadOnly GraphicsRegex As New Regex("^(?<Name>[a-z_]+_gfx)(\s*|\s+with\s+bm_map\s*=\s*(?<X>\d+)x(?<Y>\d+)\s*)" & _
            "^#include\s+""(?<FileName>.+)""\s*" & _
            "(^\s*|(?<ExtraDefines>(^[a-z0-9_]+\s*=\s*[a-z0-9_]+\s*)+))$",
            RegexOptions.Multiline Or RegexOptions.CultureInvariant Or RegexOptions.Compiled)

    Private Shared ReadOnly MapDataRegex As New Regex("^ANIMATE_SECTION\(\)" & vbLf & _
        "(^\s+(?<AnimName>[A-Z_]+)\((?<AnimArgs>.*)\)\s*)*\s*" & _
        "^OBJECT_SECTION\(\)" & vbLf & _
        "(((^#DEFINE\s+(?<ObjectPropName>\w+)\s+(?<ObjectPropValue>\w+)\s*)|((?!#DEFINE)(?<ObjectPropName>)(?<ObjectPropValue>)))*^\s+(?<ObjectName>[A-Z_]+)\((?<ObjectArgs>.*)\)\s*)*\s*" & _
        "^ENEMY_SECTION\(\)" & vbLf & _
        "(((^#DEFINE\s+(?<EnemyPropName>\w+)\s+(?<EnemyPropValue>\w+)\s*)|((?<EnemyPropName>)(?<EnemyPropValue>)))*^\s+(?<EnemyName>[A-Z_]+)\((?<EnemyArgs>.*)\)\s*)*\s*" & _
        "^MISC_SECTION\(\)" & vbLf & _
        "(((^#DEFINE\s+(?<MiscPropName>\w+)\s+(?<MiscPropValue>\w+)\s*)|((?<MiscPropName>)(?<MiscPropValue>)))*^\s+(?<MiscName>[A-Z_]+)\((?<MiscArgs>.*)\)\s*)*\s*" & _
        "^END_SECTION\(\)",
        RegexOptions.Multiline Or RegexOptions.CultureInvariant Or RegexOptions.Compiled)

    Public ScenarioName As String

    Private _IsLoading As Boolean

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

    Private _NamedSlots As New ObservableCollection(Of String)
    Public Property NamedSlots As ObservableCollection(Of String)
        Get
            Return _NamedSlots
        End Get
        Set(value As ObservableCollection(Of String))
            _NamedSlots = value
            RaisePropertyChanged("NamedSlots")
        End Set
    End Property

    Private _CachedMapHierArray() As Byte
    Public Sub CacheMapHierarchy()
        Dim MapHier As New MapHierarchy
        GetExistingMaps().ToList().ForEach(Sub(m) MapHier.AddMap(m.X, m.Y, m.Index))

        _CachedMapHierArray = MapHier.GetArray()
    End Sub

    Public Function GetMap(HierarchyIndex As Byte) As MapData
        Dim RealIndex = _CachedMapHierArray(HierarchyIndex)
        Return Maps.FirstOrDefault(Function(m) m.Exists And m.Index = RealIndex)
    End Function

    Public Property Images As New List(Of ZeldaImage)
    Public Property AnimDefs As New Dictionary(Of String, ZDef)
    Public Property ObjectDefs As New Dictionary(Of String, ZDef)
    Public Property EnemyDefs As New Dictionary(Of String, ZDef)
    Public Property MiscDefs As New Dictionary(Of String, ZDef)
    Public Property Tilesets As New List(Of Tileset)

    Private _FileName As String

    Private Sub Log(LogStr As String)
        Debug.Write(Now.ToFileTime & ": " & LogStr & vbCrLf)
    End Sub

    Structure DefParams
        Public FileName As String
        Public Collection As Object
        Public Type As Type
        Sub New(FileName, Collection, Type)
            Me.FileName = FileName
            Me.Collection = Collection
            Me.Type = Type
        End Sub
    End Structure

    Private Sub LoadProps(ByRef Obj As IBaseGeneralObject, PropName As String, PropValue As String)
        If PropName IsNot Nothing AndAlso PropName.ToUpper.EndsWith("_SLOT") Then
            Obj.NamedSlot = PropName.ToUpper()
            NamedSlots.Add(Obj.NamedSlot)
        End If
    End Sub

    Public Async Function LoadScenario(FileName As String) As Task
        _IsLoading = True

        Tilesets = New List(Of Tileset)()
        Tilesets.Add(New Tileset("dungeon", IO.Path.Combine(MapEditorControl.ZeldaFolder, "maps\dungeon.bmp")))
        Tilesets.Add(New Tileset("town", IO.Path.Combine(MapEditorControl.ZeldaFolder, "maps\town.bmp")))
        Tilesets.Add(New Tileset("graveyard", IO.Path.Combine(MapEditorControl.ZeldaFolder, "maps\graveyard.bmp")))
        Tilesets.Add(New Tileset("cave", IO.Path.Combine(MapEditorControl.ZeldaFolder, "maps\cave.bmp")))
        Tilesets.Add(New Tileset("indoors", IO.Path.Combine(MapEditorControl.ZeldaFolder, "maps\indoors.bmp")))

        Log("Starting SPASM")
        SPASMHelper.Initialize(MapEditorControl.ZeldaFolder)

        Log("Loading images")
        Await LoadImages(MapEditorControl.ZeldaFolder & "\graphics.asm")

        _FileName = FileName
        SPASMHelper.Defines.Add("INCLUDE_ALL", 1)

        Log("Assembling map")
        Dim Data = SPASMHelper.AssembleFile(FileName)

        Log("Loading all defs")

        Dim Test As New DefParams()
        Dim DefList = {New DefParams("animatedef.inc", AnimDefs, GetType(ZAnim)),
        New DefParams("objectdef.inc", ObjectDefs, GetType(ZObject)),
        New DefParams("miscdef.inc", MiscDefs, GetType(ZMisc)),
        New DefParams("enemydef.inc", EnemyDefs, GetType(ZEnemy))}

        Parallel.ForEach(DefList, Sub(Item)
                                      LoadDefs(MapEditorControl.ZeldaFolder & "\" & Item.FileName, Item.Collection, Item.Type)
                                  End Sub)

        Dim Reader As New StreamReader(FileName)
        Dim ScenarioContents As String = Await Reader.ReadToEndAsync()
        ScenarioContents = ScenarioContents.ToUpper().Replace(vbCrLf, vbLf)
        Reader.Close()

        Dim MaxX = -1
        Dim MaxY = -1
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
                Dim EndSectionString = "END_SECTION()"
                Dim MapMacrosEnd = Math.Max(ScenarioContents.IndexOf(EndSectionString, MapMacrosStart) + EndSectionString.Length, MapMacrosStart)

                Dim MapContents = ScenarioContents.Substring(MapMacrosStart, MapMacrosEnd - MapMacrosStart)

                Log("Loading map " & x & "," & y)

                Log("Running regular expression...")
                Dim Matches = MapDataRegex.Matches(MapContents)
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
                    Dim offset = 0
                    RawData.ReadByte() : RawData.ReadByte()
                    For i = 0 To Groups("ObjectName").Captures.Count - 1
                        Dim ObjectPropName = Groups("ObjectPropName").Captures(i + offset).Value
                        Dim ObjectPropValue = Groups("ObjectPropValue").Captures(i + offset).Value
                        If Not String.IsNullOrEmpty(ObjectPropName) Then
                            offset += 1
                        End If
                        Dim Params = Split(Groups("ObjectArgs").Captures(i).Value, ",")
                        Dim Obj As New ZObject(ObjectDefs(Groups("ObjectName").Captures(i).Value), RawData, Params)
                        LoadProps(Obj, ObjectPropName, ObjectPropValue)
                        MapData.ZObjects.Add(Obj)
                    Next

                    offset = 0
                    RawData.ReadByte() : RawData.ReadByte()
                    For i = 0 To Groups("EnemyName").Captures.Count - 1
                        Dim EnemyPropName = Groups("EnemyPropName").Captures(i + offset).Value
                        Dim EnemyPropValue = Groups("EnemyPropValue").Captures(i + offset).Value
                        If Not String.IsNullOrEmpty(EnemyPropName) Then
                            offset += 1
                        End If
                        Dim Params = Split(Groups("EnemyArgs").Captures(i).Value, ",")
                        Dim Enemy As New ZEnemy(EnemyDefs(Groups("EnemyName").Captures(i).Value), RawData, Params)
                        LoadProps(Enemy, EnemyPropName, EnemyPropValue)
                        MapData.ZEnemies.Add(Enemy)
                    Next

                    offset = 0
                    RawData.ReadByte() : RawData.ReadByte()
                    For i = 0 To Groups("MiscName").Captures.Count - 1
                        Dim MiscPropName = Groups("MiscPropName").Captures(i + offset).Value
                        Dim MiscPropValue = Groups("MiscPropValue").Captures(i + offset).Value
                        If Not String.IsNullOrEmpty(MiscPropName) Then
                            offset += 1
                        End If
                        Dim Params = Split(Groups("MiscArgs").Captures(i).Value, ",")
                        Dim Misc As New ZMisc(MiscDefs(Groups("MiscName").Captures(i).Value), RawData, Params)
                        LoadProps(Misc, MiscPropName, MiscPropValue)
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

        _IsLoading = False
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
        Private _Maps As New Dictionary(Of Point, Integer)

        Public Sub AddMap(X As Integer, Y As Integer, MapIndex As Integer)
            _Maps(New Point(X, Y)) = MapIndex
        End Sub

        Public Sub Write(Stream As StreamWriter, ScenarioName As String)
            Dim Xs = _Maps.Keys.Select(Function(p) p.X)
            Dim Width = Xs.Max + 1 - Xs.Min + 1

            Stream.WriteLine("#ifdef INCLUDE_MAP_HIERARCHY")
            Stream.WriteLine(ScenarioName & "_MAP_HIERARCHY:")
            Stream.WriteLine(ScenarioName & "_MAP_HIERARCHY_WIDTH = " & Width)

            WriteAssemblyData(Stream, GetArray())
            Stream.WriteLine("#endif")
        End Sub

        Public Function GetArray() As Byte()
            Dim Xs = _Maps.Keys.Select(Function(p) p.X)
            Dim Ys = _Maps.Keys.Select(Function(p) p.Y)

            Dim Width = Xs.Max + 1 - Xs.Min + 1
            Dim Height = Ys.Max + 1 - Ys.Min + 2

            Dim Data = Enumerable.Repeat(CByte(255), Width * Height).ToArray()
            _Maps.Keys.ToList().ForEach(Sub(P) Data((P.Y - Ys.Min + 1) * Width + P.X - Xs.Min) = _Maps(P))

            Return Data
        End Function
    End Class

    Public Sub SaveScenario()
        SaveScenario(_FileName)
    End Sub

    Public Sub SaveScenario(fileName As String)
        Dim Stream = New StreamWriter(fileName)

        Stream.WriteLine("#ifdef INCLUDE_ALL")
        Stream.WriteLine("#define INCLUDE_MAPS")
        Stream.WriteLine("#define INCLUDE_DEFAULTS")
        Stream.WriteLine("#define INCLUDE_MAPS_TABLE")
        Stream.WriteLine("#define INCLUDE_DEFAULTS_TABLE")
        Stream.WriteLine("#define INCLUDE_TILESET_TABLE")
        Stream.WriteLine("#define INCLUDE_MAP_HIERARCHY")
        Stream.WriteLine("#endif")

        Dim MapHierarchy As New MapHierarchy
        Dim DefaultsTable As String = ""
        Dim MapsTable As String = ""
        Dim TilesetTable As New List(Of String)

        Dim MapIndex As Integer = 0
        For Each MapData In Maps.ToList().Where(Function(m) m.Exists).OrderBy(Function(m) m.Y).ThenBy(Function(m) m.X)
            Dim MapId = String.Format("{0:D2}", MapIndex)
            Dim MapPrefix = ScenarioName & "_MAP_" & MapId

            MapData.Index = MapIndex

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

            For i = 0 To MapData.ZObjects.Count - 1
                MapData.ZObjects.Cast(Of ZObject)()(i).Write(Stream, i)
            Next

            Stream.WriteLine("enemy_section()")

            For i = 0 To MapData.ZEnemies.Count - 1
                MapData.ZEnemies.Cast(Of ZEnemy)()(i).Write(Stream, i)
            Next

            Stream.WriteLine("misc_section()")

            For i = 0 To MapData.ZMisc.Count - 1
                MapData.ZMisc(i).Write(Stream, i)
            Next

            TilesetTable.Add(MapPrefix & "_TILESET")

            Stream.WriteLine("end_section()")
            Stream.WriteLine("#endif")
            Stream.WriteLine("")

            MapHierarchy.AddMap(X, Y, MapIndex)
            MapIndex = MapIndex + 1
        Next

        MapHierarchy.Write(Stream, ScenarioName)

        Stream.WriteLine("")

        Stream.WriteLine("#ifdef INCLUDE_MAPS_TABLE")
        Stream.WriteLine(ScenarioName & "_MAP_TABLE:")
        Stream.Write(MapsTable)
        Stream.WriteLine("#endif")

        Stream.WriteLine("")

        Stream.WriteLine("#ifdef INCLUDE_DEFAULTS_TABLE")
        Stream.WriteLine(ScenarioName & "_DEFAULTS_TABLE:")
        Stream.Write(DefaultsTable)
        Stream.WriteLine("#endif")

        Stream.WriteLine("")

        Stream.WriteLine("#ifdef INCLUDE_TILESET_TABLE")
        Stream.WriteLine(ScenarioName & "_TILESET_TABLE:")
        For Each Tileset In TilesetTable
            Stream.WriteLine(vbTab & ".db " & Tileset)
        Next
        Stream.WriteLine("#endif")

        Stream.Close()
    End Sub

    Private Sub RaisePropertyChanged(PropName As String)
        If _IsLoading Then
            RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
        End If
    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged

End Class

Public Class TilesetList
    Inherits List(Of Tileset)
End Class