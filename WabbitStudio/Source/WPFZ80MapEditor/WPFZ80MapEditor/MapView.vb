Imports System.ComponentModel
Imports System.Collections.ObjectModel
Imports System.Windows.Media.Effects
Imports WPFZ80MapEditor.ValueConverters
Imports System.Windows.Media.Animation

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
        If e.ChangedButton = MouseButton.Right Then Exit Sub

        Dim Tile As Tile = sender

        Dim x = Grid.GetColumn(Tile)
        Dim y = Grid.GetRow(Tile)

        If Keyboard.IsKeyDown(Key.LeftCtrl) Then
            MainWindow.Instance.TileSelectorPanel.Index = Tile.Index
        Else
            If e.ChangedButton = MouseButton.Left Then
                Dim MapData As MapData = DataContext

                Dim SelectedTile = MainWindow.Instance.TileSelectorPanel.SelectedTile
                For Each Anim In MapData.ZAnims
                    If Anim.X = x * 16 And Anim.Y = y * 16 Then
                        MapData.ZAnims.Remove(Anim)
                        Exit For
                    End If
                Next
                Tile.Anim = Nothing
                If SelectedTile IsNot Nothing AndAlso SelectedTile.AnimDef IsNot Nothing Then
                    Tile.Anim = New ZAnim(SelectedTile.AnimDef, x * 16, y * 16)
                    MapData.ZAnims.Add(Tile.Anim)
                End If
                If MainWindow.Instance.TileSelectorPanel.Index <> -1 Then
                    Tile.Index = MainWindow.Instance.TileSelectorPanel.Index
                End If
            ElseIf e.ChangedButton = MouseButton.Middle Then
                Tile.Index = (Tile.Index Xor &H80)
            End If
        End If
    End Sub

    Public Sub New(MapData As MapData, Optional InEditor As Boolean = True)
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
                Dim tile As New XTile()

                'Dim Tile As New Tile(InEditor)

                Dim Index As Integer = (y * LayerContainer.MapSize.Width + x)

                Dim TileBinding As New Binding("TileData[" & Index & "]")
                TileBinding.Mode = BindingMode.TwoWay
                tile.SetBinding(XTile.IndexProperty, TileBinding)

                'Tile.SetBinding(Tile.TilesetProperty, New Binding("Tileset"))

                If MapData IsNot Nothing Then
                    tile.Scenario = MapData.Scenario
                    For Each Anim In MapData.ZAnims
                        If Anim.X = x * 16 And Anim.Y = y * 16 Then
                            tile.SetValue(XTile.IsAnimatedProperty, True)
                            tile.SetValue(XTile.AnimDefProperty, Anim.Definition)

                            Dim Story As Storyboard = New AnimDefStoryboardConverter().Convert1(Anim.Definition, GetType(Storyboard), Nothing, Nothing)
                            Story.Begin(tile)
                            Exit For
                        End If
                    Next
                End If

                If InEditor Then
                    AddHandler tile.MouseDown, AddressOf HandleMouseUp
                End If
                Children.Add(tile)
                Grid.SetColumn(tile, x)
                Grid.SetRow(tile, y)
            Next
        Next
        CacheMode = New BitmapCache()

        If InEditor Then
            Effect = New DropShadowEffect With {.Opacity = 0.4}
        End If
    End Sub

    Public Sub New()
        Me.New(Nothing, False)
    End Sub

    Public Sub DeselectAll() Implements IMapLayer.DeselectAll

    End Sub
End Class
