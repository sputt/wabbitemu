Imports System.IO
Imports System.Collections.ObjectModel
Imports System.Text.RegularExpressions

Public Class Scenario
    Inherits Freezable

    Public Maps As New Dictionary(Of String, MapData)
    Public ScenarioName As String

    Public ActiveLayerType As System.Type

    Private _MapCount As Integer

    Public Shared ReadOnly ImagesProperty As DependencyProperty =
        DependencyProperty.Register("Images", GetType(ObservableCollection(Of ZeldaImage)), GetType(Scenario),
        New PropertyMetadata(New ObservableCollection(Of ZeldaImage)))
    Public Shared ReadOnly AnimDefsProperty As DependencyProperty =
        DependencyProperty.Register("AnimDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
        New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))
    Public Shared ReadOnly ObjectDefsProperty As DependencyProperty =
        DependencyProperty.Register("ObjectDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
        New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))
    Public Shared ReadOnly EnemyDefsProperty As DependencyProperty =
        DependencyProperty.Register("EnemyDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
        New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))
    Public Shared ReadOnly MiscDefsProperty As DependencyProperty =
        DependencyProperty.Register("MiscDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
        New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))

    Public Shared ReadOnly TilesetsProperty As DependencyProperty =
        DependencyProperty.Register("Tilesets", GetType(ObservableCollection(Of Tileset)), GetType(Scenario))

    ' Player starting position and map
    Public Shared ReadOnly StartXProperty As DependencyProperty =
        DependencyProperty.Register("StartX", GetType(Byte), GetType(Scenario))
    Public Shared ReadOnly StartYProperty As DependencyProperty =
        DependencyProperty.Register("StartY", GetType(Byte), GetType(Scenario))
    Public Shared ReadOnly StartMapIndexProperty As DependencyProperty =
        DependencyProperty.Register("StartMapIndex", GetType(Integer), GetType(Scenario))

    Public Property Images As ObservableCollection(Of ZeldaImage)
        Get
            Return GetValue(ImagesProperty)
        End Get
        Set(value As ObservableCollection(Of ZeldaImage))
            SetValue(ImagesProperty, value)
        End Set
    End Property

    Public Property AnimDefs As ObservableDictionary(Of String, ZDef)
        Get
            Return GetValue(AnimDefsProperty)
        End Get
        Set(value As ObservableDictionary(Of String, ZDef))
            SetValue(AnimDefsProperty, value)
        End Set
    End Property

    Public Property ObjectDefs As ObservableDictionary(Of String, ZDef)
        Get
            Return GetValue(ObjectDefsProperty)
        End Get
        Set(value As ObservableDictionary(Of String, ZDef))
            SetValue(ObjectDefsProperty, value)
        End Set
    End Property

    Public Property EnemyDefs As ObservableDictionary(Of String, ZDef)
        Get
            Return GetValue(EnemyDefsProperty)
        End Get
        Set(value As ObservableDictionary(Of String, ZDef))
            SetValue(EnemyDefsProperty, value)
        End Set
    End Property

    Public Property MiscDefs As ObservableDictionary(Of String, ZDef)
        Get
            Return GetValue(MiscDefsProperty)
        End Get
        Set(value As ObservableDictionary(Of String, ZDef))
            SetValue(MiscDefsProperty, value)
        End Set
    End Property

    Public Property Tilesets As ObservableCollection(Of Tileset)
        Get
            Return GetValue(TilesetsProperty)
        End Get
        Set(value As ObservableCollection(Of Tileset))
            SetValue(TilesetsProperty, value)
        End Set
    End Property

    Public ReadOnly Property AllDefs As Dictionary(Of String, ZDef)
        Get
            Return ObjectDefs.Values.Union(EnemyDefs.Values).Union(MiscDefs.Values).Union(AnimDefs.Values)
        End Get
    End Property


    Public Sub AddMap(x As Integer, y As Integer, Map As MapData)

        Dim Container = MainWindow.Instance.LayerContainer.AddMap(x, y, Map)

        Dim EnemyLayer As New EnemyLayer
        Container.Children.Add(EnemyLayer)
        Grid.SetColumn(EnemyLayer, x)
        Grid.SetRow(EnemyLayer, y)
        Panel.SetZIndex(EnemyLayer, 4)

        Dim ObjLayer As New ObjectLayer
        ObjLayer.Scenario = Me
        Container.Children.Add(ObjLayer)
        Grid.SetColumn(ObjLayer, x)
        Grid.SetRow(ObjLayer, y)
        Panel.SetZIndex(ObjLayer, 3)

        Dim MiscLayer As New MiscLayer
        Container.Children.Add(MiscLayer)
        Grid.SetColumn(MiscLayer, x)
        Grid.SetRow(MiscLayer, y)
        Panel.SetZIndex(MiscLayer, 2)

        Dim MapView As New MapView(Map, True)
        Container.Children.Add(MapView)
        Grid.SetColumn(MapView, x)
        Grid.SetRow(MapView, y)
        Panel.SetZIndex(MapView, 1)

        Dim MapSet As New MapSet
        Container.Children.Add(MapSet)
        Grid.SetColumn(MapSet, x)
        Grid.SetRow(MapSet, y)
        Panel.SetZIndex(MapSet, 10)

        Maps(String.Format("{0:D2}", _MapCount)) = Map
        _MapCount += 1
    End Sub

    Public ReadOnly Property ActiveLayer As IMapLayer
        Get
            Return (From containers As MapContainer In MainWindow.Instance.LayerContainer.Children
                    From m As IMapLayer In containers.Children
                    Where m.GetType() = ActiveLayerType
                    Select m).First()
        End Get
    End Property


    Private _FileName As String

    Public Sub LoadScenario(FileName As String)
        Tilesets = New ObservableCollection(Of Tileset)()
        Tilesets.Add(New Tileset("dungeon", IO.Path.Combine(MainWindow.ZeldaFolder, "images\dungeon.bmp")))
        Tilesets.Add(New Tileset("town", IO.Path.Combine(MainWindow.ZeldaFolder, "images\town.bmp")))

        Dim Path As String = Directory.GetParent(FileName).FullName
        SPASMHelper.Initialize(Path)
        LoadImages(Path & "\graphics.asm")

        _FileName = FileName
        SPASMHelper.Assembler.Defines.Add("INCLUDE_ALL", 1)
        Dim Data = SPASMHelper.AssembleFile(FileName)

        LoadDefs(Path & "\animatedef.inc", AnimDefsProperty, GetType(ZAnim))
        'LoadDefs(Path & "\objectdef.inc", ObjectDefsProperty, GetType(ZObject))
        'LoadDefs(Path & "\miscdef.inc", MiscDefsProperty, GetType(ZMisc))
        'LoadDefs(Path & "\enemydef.inc", EnemyDefsProperty, GetType(ZEnemy))

        Dim Reader As New StreamReader(FileName)
        Dim ScenarioContents As String = Reader.ReadToEnd()
        Reader.Close()

        Dim MaxX = -1
        Dim MaxY = -1
        _MapCount = 0
        For Each Label In SPASMHelper.Labels.Keys
            If Label Like "*_MAP_##" Then
                Dim x = SPASMHelper.Labels(Label & "_X")
                MaxX = Math.Max(x, MaxX)
                Dim y = SPASMHelper.Labels(Label & "_Y")
                MaxY = Math.Max(y, MaxY)
                Dim Tileset = SPASMHelper.Labels(Label & "_TILESET")

                Dim MapData = New MapData(Data.Skip(SPASMHelper.Labels(Label)), Me, Tileset)

                Dim Rx As New Regex(
                    "^" & Label & "_DEFAULTS:" & vbCrLf & _
                    "^animate_section\(\)" & vbCrLf & _
                    "(^\s+(?<AnimName>[a-z_]+)\((?<AnimArgs>.*)\)\s*)*\s*" & _
                    "^object_section\(\)" & vbCrLf & _
                    "(^\s+(?<ObjectName>[a-z_]+)\((?<ObjectArgs>.*)\)\s*)*\s*" & _
                    "^enemy_section\(\)" & vbCrLf & _
                    "(^\s+(?<EnemyName>[a-z_]+)\((?<EnemyArgs>.*)\)\s*)*\s*" & _
                    "^misc_section\(\)" & vbCrLf & _
                    "(^\s+(?<MiscName>[a-z_]+)\((?<MiscArgs>.*)\)\s*)*\s*" & _
                    "end_section\(\)", RegexOptions.Multiline Or RegexOptions.Compiled)

                Dim Matches = Rx.Matches(ScenarioContents)
                If Matches.Count = 1 Then
                    Dim Groups = Matches(0).Groups

                    For i = 0 To Groups("AnimName").Captures.Count - 1
                        Dim Params = Split(Groups("AnimArgs").Captures(i).Value, ",")
                        Dim Anim As New ZAnim(AnimDefs(Groups("AnimName").Captures(i).Value), Params(0), Params(1))
                        MapData.ZAnims.Add(Anim)
                    Next

                    For i = 0 To Groups("ObjectName").Captures.Count - 1
                        Dim Params = Split(Groups("ObjectArgs").Captures(i).Value, ",")
                        'Dim Obj As New ZObject(ObjectDefs(Groups("ObjectName").Captures(i).Value), Params)
                        'MapData.ZObjects.Add(Obj)
                    Next

                    For i = 0 To Groups("EnemyName").Captures.Count - 1
                        Dim Params = Split(Groups("EnemyArgs").Captures(i).Value, ",")
                        'Dim Enemy As New ZEnemy(EnemyDefs(Groups("EnemyName").Captures(i).Value), Params)
                        'MapData.ZEnemies.Add(Enemy)
                    Next

                    For i = 0 To Groups("MiscName").Captures.Count - 1
                        Dim Params = Split(Groups("MiscArgs").Captures(i).Value, ",")
                        'Dim Misc As New ZMisc(MiscDefs(Groups("MiscName").Captures(i).Value), Params)
                        'MapData.ZMisc.Add(Misc)
                    Next
                End If

                AddMap(x, y, MapData)
                ScenarioName = Left(Label, Len(Label) - 7)
            End If
        Next

        For x = 0 To MaxX
            For y = 0 To MaxY
                Dim CurX = x, CurY = y
                Dim Exist = (From m In MainWindow.Instance.LayerContainer.Children Where Grid.GetColumn(m) = CurX And Grid.GetRow(m) = CurY).Count() > 0
                If Not Exist Then
                    AddMap(x, y, Nothing)
                End If
            Next
        Next

        ActiveLayerType = GetType(MapSet)
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

        Public Sub AddMap(X As Integer, Y As Integer, MapIndex As Integer)
            _MaxX = Math.Max(_MaxX, X)
            _MaxY = Math.Max(_MaxY, Y)
            _Maps(X, Y + 1) = MapIndex
        End Sub

        Public Sub Write(Stream As StreamWriter)
            Dim Width = _MaxX + 1
            Stream.WriteLine("#ifdef INCLUDE_MAP_HIERARCHY")
            Stream.WriteLine("#ifndef __MAP_HIERARCHY_WIDTH_DEFINED")
            Stream.WriteLine("#define __MAP_HIERARCHY_WIDTH_DEFINED")
            Stream.WriteLine("map_hierarchy_width = " & Width)
            Stream.WriteLine("#endif")

            Dim Data(0 To Width * (_MaxY + 2) - 1) As Byte
            For Y = 0 To _MaxY + 1
                For X = 0 To _MaxX
                    Data(Y * Width + X) = _Maps(X, Y)
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
        Dim PossibleViews = From v In MainWindow.Instance.LayerContainer.Children
                            Where TypeOf v Is MapContainer AndAlso CType(v, MapContainer).MapData IsNot Nothing
                            Order By Grid.GetRow(v), Grid.GetColumn(v)
        For Each View In PossibleViews
            Dim Container As MapContainer = View
            Dim MapData = Container.MapData

            Dim MapId = String.Format("{0:D2}", MapIndex)
            Dim MapPrefix = ScenarioName & "_MAP_" & MapId

            Dim X = Grid.GetColumn(Container)
            Dim Y = Grid.GetRow(Container)
            Stream.WriteLine("#ifdef INCLUDE_MAPS")
            MapsTable &= vbTab & ".dw " & MapPrefix & vbCrLf

            Stream.WriteLine(MapPrefix & ":")
            Stream.WriteLine(MapPrefix & "_X = " & X)
            Stream.WriteLine(MapPrefix & "_Y = " & Y)
            Stream.WriteLine(MapPrefix & "_TILESET = 0")

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

    Protected Overrides Function CreateInstanceCore() As Freezable
        Return New Scenario
    End Function

    Private Shared _Instance As Scenario
    Public Shared ReadOnly Property Instance As Scenario
        Get
            SyncLock GetType(Scenario)
                If _Instance Is Nothing Then
                    _Instance = New Scenario
                End If
            End SyncLock
            Return _Instance
        End Get
    End Property

End Class
