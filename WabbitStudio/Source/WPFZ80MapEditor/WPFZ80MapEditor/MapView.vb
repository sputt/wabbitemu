Imports System.ComponentModel
Imports System.Collections.ObjectModel
Imports System.Windows.Media.Effects

Public Class MapView
    Inherits Grid
    Implements IMapLayer


    Public WriteOnly Property Active As Boolean Implements IMapLayer.Active
        Set(value As Boolean)
            IsHitTestVisible = value
        End Set
    End Property

    Public ShowCollidable As Boolean = True

    Private Sub HandleMouseUp(sender As Object, e As MouseButtonEventArgs)
        Dim tile As Tile = sender

        Dim x = Grid.GetColumn(tile)
        Dim y = Grid.GetRow(tile)

        If Keyboard.IsKeyDown(Key.LeftCtrl) Then
            MainWindow.Instance.TileSelectorPanel.Index = tile.Index
        Else
            If e.ChangedButton = MouseButton.Left Then
                If MainWindow.Instance.TileSelectorPanel.Index <> -1 Then
                    tile.Index = MainWindow.Instance.TileSelectorPanel.Index
                End If
            ElseIf e.ChangedButton = MouseButton.Middle Then
                tile.Index = (tile.Index Xor &H80)
            End If
        End If
    End Sub

    Public Sub New(Optional InEditor As Boolean = True)
        MyBase.New()

        For x = 1 To LayerContainer.MapSize.Width
            Dim col = New ColumnDefinition With {.Width = New GridLength(LayerContainer.TileSize.Width)}
            ColumnDefinitions.Add(col)
        Next

        For y = 1 To LayerContainer.MapSize.Height
            Dim row = New RowDefinition With {.Height = New GridLength(LayerContainer.TileSize.Height)}
            RowDefinitions.Add(row)
        Next

        MaxHeight = LayerContainer.TileSize.Height * LayerContainer.MapSize.Height
        MaxWidth = LayerContainer.TileSize.Width * LayerContainer.MapSize.Width

        For x = 0 To LayerContainer.MapSize.Width - 1
            For y = 0 To LayerContainer.MapSize.Height - 1
                Dim Tile As New Tile(InEditor)

                Dim Index As Integer = (y * LayerContainer.MapSize.Width + x)

                Dim TileBinding As New Binding("TileData[" & Index & "]")
                TileBinding.Mode = BindingMode.TwoWay
                Tile.SetBinding(Tile.IndexProperty, TileBinding)

                Tile.SetBinding(Tile.TilesetProperty, New Binding("Tileset"))

                If InEditor Then
                    AddHandler Tile.MouseDown, AddressOf HandleMouseUp
                End If
                Children.Add(Tile)
                Grid.SetColumn(Tile, x)
                Grid.SetRow(Tile, y)
            Next
        Next
        CacheMode = New BitmapCache()

        If InEditor Then
            Effect = New DropShadowEffect With {.Opacity = 0.4}
        End If
    End Sub

    Public Sub New()
        Me.New(False)
    End Sub

    Public Sub DeselectAll() Implements IMapLayer.DeselectAll

    End Sub
End Class
