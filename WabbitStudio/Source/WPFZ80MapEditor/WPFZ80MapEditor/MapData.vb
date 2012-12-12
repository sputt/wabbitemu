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

    Public Shared ReadOnly ZObjectsProperty As DependencyProperty =
        DependencyProperty.Register("ZObjects", GetType(ObservableCollection(Of ZObject)), GetType(MapData))

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

    Public Property ZObjects As ObservableCollection(Of ZObject)
        Get
            Return GetValue(ZObjectsProperty)
        End Get
        Set(value As ObservableCollection(Of ZObject))
            SetValue(ZObjectsProperty, value)
        End Set
    End Property

    Public Property Tileset As Integer
        Get
            Return GetValue(TilesetProperty)
        End Get
        Set(value As Integer)
            SetValue(TilesetProperty, value)
            RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs("Tileset"))
        End Set
    End Property

    Public Property Scenario As Scenario
        Get
            Return GetValue(ScenarioProperty)
        End Get
        Set(value As Scenario)
            SetValue(ScenarioProperty, value)
        End Set
    End Property

    Private Sub Initialize(NewTileset As Integer)
        Tileset = NewTileset
        ZObjects = New ObservableCollection(Of ZObject)
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
