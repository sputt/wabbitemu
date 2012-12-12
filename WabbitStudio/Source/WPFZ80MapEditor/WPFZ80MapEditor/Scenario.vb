Imports System.IO
Imports System.Collections.ObjectModel
Imports System.Text.RegularExpressions

Public Class Scenario
    Inherits Freezable

    Public Tilesets As New Dictionary(Of String, Tileset)
    Public Maps As New Dictionary(Of String, MapData)
    Public ScenarioName As String

    Public ActiveLayerType As System.Type

    Private _MapCount As Integer

    Public Shared ReadOnly ImagesProperty As DependencyProperty =
        DependencyProperty.Register("Images", GetType(ObservableCollection(Of ZeldaImage)), GetType(Scenario),
        New PropertyMetadata(New ObservableCollection(Of ZeldaImage)))

    Public Shared ReadOnly ObjectDefsProperty As DependencyProperty =
        DependencyProperty.Register("ObjectDefs", GetType(ObservableDictionary(Of String, ZDef)), GetType(Scenario),
        New PropertyMetadata(New ObservableDictionary(Of String, ZDef)))

    Public Shared ReadOnly TilesetsProperty As DependencyProperty =
        DependencyProperty.Register("Tilesets", GetType(ObservableCollection(Of Tileset)), GetType(Scenario))

    Public Property Images As ObservableCollection(Of ZeldaImage)
        Get
            Return GetValue(ImagesProperty)
        End Get
        Set(value As ObservableCollection(Of ZeldaImage))
            SetValue(ImagesProperty, value)
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

    Public Sub AddMap(x As Integer, y As Integer, Map As MapData)

        Dim Container = MainWindow.Instance.LayerContainer.AddMap(x, y, Map)

        Dim ObjLayer As New ObjectLayer
        Container.Children.Add(ObjLayer)
        Grid.SetColumn(ObjLayer, x)
        Grid.SetRow(ObjLayer, y)
        Panel.SetZIndex(ObjLayer, 2)

        Dim MapView As New MapView
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

    Public Sub ClearMaps()
        MainWindow.Instance.LayerContainer.Children.Clear()
        Maps.Clear()
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

        ClearMaps()

        LoadImages(Directory.GetCurrentDirectory() & "\Scenario\graphics.asm")

        _FileName = FileName
        Dim Data = SPASMHelper.AssembleFile(FileName)

        LoadObjectDefs(Directory.GetCurrentDirectory() & "\Scenario\objectdef.inc")

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

                Dim MapData = New MapData(Data.Skip(SPASMHelper.Labels(Label)), Tileset)

                Dim Rx As New Regex(
                    "^" & Label & "_DEFAULTS:\s*" & _
                    "^.*\s*" & _
                    "^object_section\(\)\s*" & _
                    "(^\s+(?<MacroName>[a-z_]+)\((?<MacroArgs>.*)\)\s*)*" & _
                    "^enemy_section\(\)\s*$", RegexOptions.Multiline Or RegexOptions.Compiled)

                Dim Matches = Rx.Matches(ScenarioContents)
                If Matches.Count = 1 Then
                    Dim Groups = Matches(0).Groups

                    For i = 0 To Groups("MacroName").Captures.Count - 1
                        Dim Params = Split(Groups("MacroArgs").Captures(i).Value, ",")
                        Dim Obj As New ZObject(ObjectDefs(Groups("MacroName").Captures(i).Value), Params)
                        MapData.ZObjects.Add(Obj)
                    Next
                End If

                AddMap(x, y, MapData)
                ScenarioName = Left(Label, Len(Label) - 7)
            End If
        Next

        'For Each Map In Maps.Values
        '    Dim Obj As New ZObject("pot", "48", 48)
        '    Map.ZObjects.Add(Obj)

        '    Dim Obj2 As New ZObject("doortoph", 128, 128)
        '    Map.ZObjects.Add(Obj2)

        '    Dim Obj3 As New ZObject("plant", 200, 200)
        '    Map.ZObjects.Add(Obj3)

        'Next

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
                    Exit Sub
                End If
                If i <> 16 Then Stream.Write(",")
            Next
            Stream.Write(vbCrLf)
        End While

    End Sub


    Public Sub SaveScenario()
        Dim Stream = New StreamWriter(_FileName)

        'For Each MapIndex In Maps.Keys
        '    Dim MapData = Maps(MapIndex)

        '    If Not MapData Is Nothing Then
        '        Dim PossibleViews = From v In MapViewContainer.Children Where TypeOf v Is MapView AndAlso CType(v, MapView).MapData Is MapData
        '        If PossibleViews.Count > 0 Then
        '            Dim MapView = PossibleViews.First()


        '            Dim MapPrefix = ScenarioName & "_MAP_" & MapIndex
        '            Stream.WriteLine(MapPrefix & ":")
        '            Stream.WriteLine(MapPrefix & "_X = " & Grid.GetColumn(MapView))
        '            Stream.WriteLine(MapPrefix & "_Y = " & Grid.GetRow(MapView))
        '            Stream.WriteLine(MapPrefix & "_TILESET = 0")

        '            Dim CompressedMap = MapCompressor.Compress(Maps(MapIndex).TileData)
        '            WriteAssemblyData(Stream, CompressedMap)

        '            Stream.WriteLine("")
        '        End If
        '    End If
        'Next

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
