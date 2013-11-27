Imports System.Windows.Media.Animation
Imports System.Windows.Media.Effects
Imports System.Linq

Public Class TilesPanel
    Implements ComponentModel.INotifyPropertyChanged

    Private LastTab As Object = Nothing

    Protected Sub OnTileSelect(Tile As PanelTile, e As System.Windows.Input.MouseButtonEventArgs)
        LastTab = TileTabs.SelectedValue

        Select Case e.ChangedButton
            Case MouseButton.Left
                If Index = Tile.Index Then
                    Index = -1
                    SelectedTile = Nothing
                Else
                    Index = Tile.Index
                    SelectedTile = Tile
                End If
            Case MouseButton.Middle
                If Tile.Index = Index Then
                    Index = Index Xor &H80
                Else
                    Index = Tile.Index Xor &H80
                End If
                SelectedTile = Tile
        End Select

    End Sub

    Private _Index As Integer = -1
    Public Property Index As Integer
        Get
            Return _Index
        End Get
        Set(value As Integer)
            Dim IsOneSelected As Boolean = False
            _Index = value

            Dim SelectedTile As Tile = Nothing
            Dim Tiles As List(Of PanelTile) = Utils.FindChildren(Of PanelTile)(Me).ToList()
            Tiles.ForEach(Sub(t)
                              t.Index = (t.Index Mod 128)
                              t.IsSelected = t.Index = (_Index Mod 128)
                              If t.IsSelected Then
                                  SelectedTile = t
                              End If
                          End Sub)

            IsOneSelected = (Not SelectedTile Is Nothing)
            If IsOneSelected Then
                SelectedTile.Index = value
                SelectedTile.BringIntoView()
            Else
                Tiles.ForEach(Sub(t) t.OverlayColor = Color.FromArgb(0, 255, 255, 255))
            End If

            Me.SelectedTile = Nothing
            RaiseEvent PropertyChanged(Me, New ComponentModel.PropertyChangedEventArgs("Index"))

        End Set
    End Property

    Private _SelectedTile As Tile
    Public Property SelectedTile As Tile
        Get
            Return _SelectedTile
        End Get
        Set(value As Tile)
            _SelectedTile = value
        End Set
    End Property

    Public Sub Initialize(TilesetIndex As Integer)
        DataContext = Scenario.Instance

        Dim Tileset = Scenario.Instance.Tilesets.Values(TilesetIndex)
        For Index As Integer = 0 To Tileset.Count - 1
            Dim tile = New PanelTile()
            tile.Tileset = TilesetIndex
            tile.Index = Index

            AddHandler tile.TileSelected, AddressOf OnTileSelect
            tile.Margin = New Thickness(2)
            Panel.SetZIndex(tile, 0)
            TilesGrid.Children.Add(tile)
        Next

    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged
End Class
