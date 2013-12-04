Imports System.Collections.ObjectModel
Imports System.ComponentModel

''' <summary>
''' 
''' </summary>
''' <remarks></remarks>
Public Class MapData
    Inherits DependencyObject
    Implements INotifyPropertyChanged

    Public Shared ReadOnly TileDataProperty As DependencyProperty =
        DependencyProperty.Register("TileData", GetType(ObservableCollection(Of Byte)), GetType(MapData))

    Public Shared ReadOnly TilesetProperty As DependencyProperty =
        DependencyProperty.Register("Tileset", GetType(Integer), GetType(MapData))

    Public Shared ReadOnly ZAnimsProperty As DependencyProperty =
        DependencyProperty.Register("ZAnims",
                                    GetType(ObservableCollection(Of ZAnim)), GetType(MapData))

    Public Shared ReadOnly ZObjectsProperty As DependencyProperty =
        DependencyProperty.Register("ZObjects",
                                    GetType(ObservableCollection(Of ZObject)), GetType(MapData))

    Public Shared ReadOnly ZEnemiesProperty As DependencyProperty =
        DependencyProperty.Register("ZEnemies",
                                    GetType(ObservableCollection(Of ZEnemy)), GetType(MapData))

    Public Shared ReadOnly ZMiscProperty As DependencyProperty =
        DependencyProperty.Register("ZMisc",
                                    GetType(ObservableCollection(Of ZMisc)), GetType(MapData))

    Public Shared ReadOnly ScenarioProperty As DependencyProperty =
        DependencyProperty.Register("Scenario", GetType(Scenario), GetType(MapData))

    Public Property TileData As ObservableCollection(Of Byte)
        Get
            Return GetValue(TileDataProperty)
        End Get
        Set(value As ObservableCollection(Of Byte))
            SetValue(TileDataProperty, value)
        End Set
    End Property

    Public Property ZAnims As ObservableCollection(Of ZAnim)
        Get
            Return GetValue(ZAnimsProperty)
        End Get
        Set(value As ObservableCollection(Of ZAnim))
            SetValue(ZAnimsProperty, value)
        End Set
    End Property

    Public Property ZObjects As ObservableCollection(Of ZObject)
        Get
            Return GetValue(ZObjectsProperty)
        End Get
        Set(value As ObservableCollection(Of ZObject))
            SetValue(ZObjectsProperty, value)
        End Set
    End Property

    Public Property ZEnemies As ObservableCollection(Of ZEnemy)
        Get
            Return GetValue(ZEnemiesProperty)
        End Get
        Set(value As ObservableCollection(Of ZEnemy))
            SetValue(ZEnemiesProperty, value)
        End Set
    End Property

    Public Property ZMisc As ObservableCollection(Of ZMisc)
        Get
            Return GetValue(ZMiscProperty)
        End Get
        Set(value As ObservableCollection(Of ZMisc))
            SetValue(ZMiscProperty, value)
        End Set
    End Property

    Public Sub AddFromDef(Def As ZDef, X As Byte, Y As Byte)
        If Scenario.Instance.ObjectDefs.ContainsValue(Def) Then
            Dim ObjTest As New ZObject(Def, X, Y)

            X = X - ObjTest.W / 2
            Y = Y - ObjTest.H / 2

            Dim Obj As New ZObject(Def, X, Y)
            ZObjects.Add(Obj)
        ElseIf Scenario.Instance.EnemyDefs.ContainsValue(Def) Then
            Dim ObjTest As New ZEnemy(Def, X, Y)

            X = X - ObjTest.W / 2
            Y = Y - ObjTest.H / 2

            Dim Obj As New ZEnemy(Def, X, Y)
            ZEnemies.Add(Obj)
        ElseIf Scenario.Instance.MiscDefs.ContainsValue(Def) Then
            Dim Misc As New ZMisc(Def, X, Y, 16, 16)

            ZMisc.Add(Misc)
        End If
    End Sub

    Public Property Tileset As Integer
        Get
            Return GetValue(TilesetProperty)
        End Get
        Set(value As Integer)
            SetValue(TilesetProperty, value)
            RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs("Tileset"))
        End Set
    End Property

    'Public Property Scenario As Scenario
    '    Get
    '        Return GetValue(ScenarioProperty)
    '    End Get
    '    Set(value As Scenario)
    '        SetValue(ScenarioProperty, value)
    '    End Set
    'End Property

    Private Sub Initialize(NewTileset As Integer)
        Tileset = NewTileset
        ZAnims = New ObservableCollection(Of ZAnim)
        ZObjects = New ObservableCollection(Of ZObject)
        ZEnemies = New ObservableCollection(Of ZEnemy)
        ZMisc = New ObservableCollection(Of ZMisc)
    End Sub


    Public Sub New(FileName As String, NewTileset As Integer)
        Dim FullPath = System.IO.Path.GetFullPath(FileName)
        Dim CompMapData = SPASMHelper.AssembleFile(FullPath)
        TileData = New ObservableCollection(Of Byte)(MapCompressor.Decompress(CompMapData))

        Initialize(NewTileset)
    End Sub

    Public Sub New(Data As IEnumerable(Of Byte), NewTileset As Integer)
        TileData = New ObservableCollection(Of Byte)(MapCompressor.Decompress(Data))
        Initialize(NewTileset)
    End Sub

    Public Sub New(NewTileset As Integer)
        TileData = New ObservableCollection(Of Byte)(Enumerable.Repeat(CByte(0), LayerContainer.MapSize.Width * LayerContainer.MapSize.Height))
        Initialize(NewTileset)
    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged
End Class
