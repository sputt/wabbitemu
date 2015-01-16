Imports System.Collections.ObjectModel
Imports System.ComponentModel

''' <summary>
''' 
''' </summary>
''' <remarks></remarks>
Public Class MapData
    Inherits Freezable
    Implements INotifyPropertyChanged

    Private _ID As Guid
    Public ReadOnly Property ID As Guid
        Get
            Return _ID
        End Get
    End Property

    Private _Exists As Boolean = True
    Private _X As Double
    Private _Y As Double

    Public Property Exists As Boolean
        Get
            Return _Exists
        End Get
        Set(value As Boolean)
            If value <> _Exists Then
                _Exists = value
                RaisePropertyChanged("Exists")
            End If
        End Set
    End Property


    Public Property X As Double
        Get
            Return _X
        End Get
        Set(value As Double)
            If (value <> _X) Then
                _X = value
                RaisePropertyChanged("X")
            End If
        End Set
    End Property

    Public Property Y As Double
        Get
            Return _Y
        End Get
        Set(value As Double)
            If (value <> _Y) Then
                _Y = value
                RaisePropertyChanged("Y")
            End If
        End Set
    End Property

    Private _Scenario As Scenario
    Public Property Scenario As Scenario
        Get
            Return _Scenario
        End Get
        Private Set(value As Scenario)
            If value IsNot _Scenario Then
                _Scenario = value
                RaisePropertyChanged("Scenario")
            End If
        End Set
    End Property

    Public Property TileData As ObservableCollection(Of Byte)

    Public Property ZAnims As ObservableCollection(Of ZAnim)

    Private _ZObjects As ObservableCollection(Of ZObject)
    Public Property ZObjects As ObservableCollection(Of ZObject)
        Get
            Return _ZObjects
        End Get
        Set(value As ObservableCollection(Of ZObject))
            If value IsNot _ZObjects Then
                _ZObjects = value
                RaisePropertyChanged("ZObjects")
            End If
        End Set
    End Property

    Public Property ZEnemies As ObservableCollection(Of ZEnemy)

    Public Property ZMisc As ObservableCollection(Of ZMisc)

    Public Sub AddFromDef(Def As ZDef, X As Byte, Y As Byte)
        If Scenario.ObjectDefs.ContainsValue(Def) Then
            Dim ObjTest As New ZObject(Def, X, Y)

            X = X - ObjTest.W / 2
            Y = Y - ObjTest.H / 2

            Dim Obj As New ZObject(Def, X, Y)
            ZObjects.Add(Obj)
        ElseIf Scenario.EnemyDefs.ContainsValue(Def) Then
            Dim ObjTest As New ZEnemy(Def, X, Y)

            X = X - ObjTest.W / 2
            Y = Y - ObjTest.H / 2

            Dim Obj As New ZEnemy(Def, X, Y)
            ZEnemies.Add(Obj)
        ElseIf Scenario.MiscDefs.ContainsValue(Def) Then
            Dim Misc As New ZMisc(Def, X, Y, 16, 16)

            ZMisc.Add(Misc)
        End If
    End Sub

    Private _Tileset As Tileset
    Public Property Tileset As Tileset
        Get
            Return _Tileset
        End Get
        Set(value As Tileset)
            If value IsNot _Tileset Then
                _Tileset = value
                RaisePropertyChanged("Tileset")
            End If
        End Set
    End Property

    Private Sub Initialize(newScenario As Scenario, NewTileset As Integer)
        Scenario = newScenario
        If Scenario IsNot Nothing Then
            Tileset = Scenario.Tilesets(NewTileset)
        End If
        ZAnims = New ObservableCollection(Of ZAnim)
        ZObjects = New ObservableCollection(Of ZObject)
        ZEnemies = New ObservableCollection(Of ZEnemy)
        ZMisc = New ObservableCollection(Of ZMisc)

        _ID = Guid.NewGuid()
    End Sub

    Public Sub New(Data As IEnumerable(Of Byte), newScenario As Scenario, newTileset As Integer, isCompressed As Boolean)
        TileData = New ObservableCollection(Of Byte)(If(isCompressed, MapCompressor.Decompress(Data), Data.Take(16 * 16)))
        Initialize(newScenario, newTileset)
    End Sub

    Public Sub New(newScenario As Scenario, newTileset As Integer)
        TileData = New ObservableCollection(Of Byte)(Enumerable.Repeat(CByte(0), 256))
        Initialize(newScenario, newTileset)
    End Sub

    Public Shared Function NonexistentMap(X As Integer, Y As Integer) As MapData
        Dim MapData As New MapData(Nothing, Nothing)
        MapData.Exists = False
        MapData.X = X
        MapData.Y = Y
        Return MapData
    End Function

    Public Shared Function EmptyMap(NewScenario, NewTileset, X, Y, NumberOfTiles)
        Dim Map As New MapData(NewScenario, NewTileset)
        Map.X = X
        Map.Y = Y
        Map.Exists = True
        Map.TileData = New ObservableCollection(Of Byte)(Enumerable.Repeat(CByte(0), NumberOfTiles).ToList())
        Return Map
    End Function

    Private Sub RaisePropertyChanged(PropName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged


    Private Function CloneAll(Of BaseType As {IBaseGeneralObject, ICloneable})(CollectionToClone As ICollection(Of BaseType)) As ICollection(Of BaseType)
        Return New ObservableCollection(Of BaseType)(CollectionToClone.Select(Function(z) CType(z.Clone(), BaseType)).ToList())
    End Function

    Protected Overrides Function CreateInstanceCore() As Freezable
        Dim Map As New MapData(Scenario, 0)
        Map.Tileset = Tileset

        Map.X = X
        Map.Y = Y
        Map.Exists = Exists
        Map.TileData = New ObservableCollection(Of Byte)(TileData)
        Map.ZAnims = CloneAll(ZAnims)
        Map.ZObjects = CloneAll(ZObjects)
        Map.ZEnemies = CloneAll(ZEnemies)
        Map.ZMisc = CloneAll(ZMisc)
        Map._ID = ID

        Return Map
    End Function
End Class
