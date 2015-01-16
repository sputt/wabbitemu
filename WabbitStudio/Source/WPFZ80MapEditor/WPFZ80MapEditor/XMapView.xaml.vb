Public Class XMapView
    Inherits MapLayer

    Public Property SelectedTile As TileSelection
        Get
            Return GetValue(SelectedTileProperty)
        End Get

        Set(ByVal value As TileSelection)
            SetValue(SelectedTileProperty, value)
        End Set
    End Property

    Public Shared ReadOnly SelectedTileProperty As DependencyProperty = _
                           DependencyProperty.Register("SelectedTile", _
                           GetType(TileSelection), GetType(XMapView), _
                           New PropertyMetadata(Nothing))

    Public Property ShowNewMaps As Boolean
        Get
            Return GetValue(ShowNewMapsProperty)
        End Get

        Set(ByVal value As Boolean)
            SetValue(ShowNewMapsProperty, value)
        End Set
    End Property

    Public Shared ReadOnly ShowNewMapsProperty As DependencyProperty = _
                           DependencyProperty.Register("ShowNewMaps", _
                           GetType(Boolean), GetType(XMapView), _
                           New PropertyMetadata(False))

    Private Sub TileGrid_MouseEnter(sender As Object, e As MouseEventArgs)
        Dim result = VisualStateManager.GoToState(sender, "MouseOver", True)
    End Sub

    Private Sub TileGrid_MouseLeave(sender As Object, e As MouseEventArgs)
        Dim result = VisualStateManager.GoToState(sender, "Normal", True)
    End Sub

    Public Overrides Sub DeselectAll()

    End Sub

    Public Overrides ReadOnly Property LayerType As LayerType
        Get
            Return WPFZ80MapEditor.LayerType.MapLayer
        End Get
    End Property

    Private Sub TileImage_MouseLeftButtonDown(sender As Object, e As MouseButtonEventArgs)
        Dim Index = TileContainer.ItemContainerGenerator.IndexFromContainer(VisualTreeHelper.GetParent(sender))
        If Index = -1 Then Exit Sub



        Dim Map As MapData = DataContext

        If Keyboard.IsKeyDown(Key.LeftCtrl) Then
            SelectedTile = New TileSelection(Map.Tileset, Map.TileData(Index))
            e.Handled = True
        Else
            If e.MiddleButton = MouseButtonState.Pressed Then
                Map.TileData(Index) = Map.TileData(Index) Xor &H80
            Else
                If SelectedTile IsNot Nothing Then
                    If SelectedTile.Type = TileSelection.SelectionType.Tile Then

                        If Map.Tileset IsNot SelectedTile.Tileset Then
                            Exit Sub
                        End If

                        UndoManager.PushUndoState(Map, UndoManager.TypeFlags.Anims)
                        Map.TileData(Index) = SelectedTile.TileIndex

                        Dim X = (Index Mod 16) * 16
                        Dim Y = Math.Floor(Index / 16) * 16
                        Dim MatchingAnim = Map.ZAnims.ToList().Find(Function(m) m.X = X And m.Y = Y)
                        If MatchingAnim IsNot Nothing Then
                            Map.ZAnims.Remove(MatchingAnim)
                        End If
                    ElseIf SelectedTile.Type = TileSelection.SelectionType.AnimatedTile Then
                        Dim Anim = ZAnim.FromMacro(Map.Scenario.AnimDefs, SelectedTile.AnimatedTileDef.Macro & "(" & (Index Mod 16) * 16 & "," & Math.Floor(Index / 16) * 16 & ")")
                        Map.ZAnims.Add(Anim)
                        Map.TileData(Index) = SelectedTile.AnimatedTileDef.DefaultImage
                    End If
                End If
            End If
        End If
    End Sub
End Class
