Public Class XMapView
    Inherits MapLayer

    Public Property SourceTile As TileSelection
        Get
            Return GetValue(SourceTileProperty)
        End Get

        Set(ByVal value As TileSelection)
            SetValue(SourceTileProperty, value)
        End Set
    End Property

    Public Shared ReadOnly SourceTileProperty As DependencyProperty = _
                           DependencyProperty.Register("SourceTile", _
                           GetType(TileSelection), GetType(XMapView), _
                           New PropertyMetadata(Nothing))

    Public Property Selection As TilegroupSelection
        Get
            Return GetValue(SelectionProperty)
        End Get

        Set(ByVal value As TilegroupSelection)
            SetValue(SelectionProperty, value)
        End Set
    End Property

    Public Shared ReadOnly SelectionProperty As DependencyProperty = _
                           DependencyProperty.Register("Selection", _
                           GetType(TilegroupSelection), GetType(XMapView), _
                           New PropertyMetadata(Nothing))



    Public Property HotIndex As Integer
        Get
            Return GetValue(HotIndexProperty)
        End Get

        Set(ByVal value As Integer)
            SetValue(HotIndexProperty, value)
        End Set
    End Property

    Public Shared ReadOnly HotIndexProperty As DependencyProperty = _
                           DependencyProperty.Register("HotIndex", _
                           GetType(Integer), GetType(XMapView), _
                           New PropertyMetadata(-1))




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

    Public Property ShowCollisions As Boolean
        Get
            Return GetValue(ShowCollisionsProperty)
        End Get

        Set(ByVal value As Boolean)
            SetValue(ShowCollisionsProperty, value)
        End Set
    End Property

    Public Shared ReadOnly ShowCollisionsProperty As DependencyProperty = _
                           DependencyProperty.Register("ShowCollisions", _
                           GetType(Boolean), GetType(XMapView), _
                           New PropertyMetadata(False))


    Public Overrides Sub DeselectAll()
        Selection = Nothing
    End Sub

    Private Function GetTileIndex(X As Double, Y As Double) As Integer
        If Y >= 256.0 Or X >= 256.0 Or X < 0 Or Y < 0 Then Return -1

        Dim Row = Math.Floor(Y / 16.0)
        Dim Col = Math.Floor(X / 16.0)
        Return Row * 16 + Col
    End Function

    Private Sub MapView_MouseLeftButtonDown(sender As Object, e As MouseButtonEventArgs)
        Dim Entries As New List(Of TilegroupEntry)

        Dim Point = e.GetPosition(sender)
        Dim Index = GetTileIndex(Point.X, Point.Y)
        If Index = -1 Then Exit Sub

        Entries.Add(New TilegroupEntry(Index, Map.TileData(Index)))
        Selection = New TilegroupSelection(Map.Tileset, Entries)

        If Keyboard.IsKeyDown(Key.LeftCtrl) Then
            SourceTile = New TileSelection(Map.Tileset, Map.TileData(Index))
            e.Handled = True
        Else
            If e.MiddleButton = MouseButtonState.Pressed Then
                Map.TileData(Index) = Map.TileData(Index) Xor &H80
            Else
                If SourceTile IsNot Nothing Then
                    If SourceTile.Type = WPFZ80MapEditor.TileSelection.SelectionType.Tile Then

                        If Map.Tileset IsNot SourceTile.Tileset Then
                            Exit Sub
                        End If

                        UndoManager.PushUndoState(Map, UndoManager.TypeFlags.Anims)
                        Map.TileData(Index) = SourceTile.TileIndex

                        Dim X = (Index Mod 16) * 16
                        Dim Y = Math.Floor(Index / 16) * 16
                        Dim MatchingAnim = Map.ZAnims.ToList().Find(Function(m) m.X = X And m.Y = Y)
                        If MatchingAnim IsNot Nothing Then
                            Map.ZAnims.Remove(MatchingAnim)
                        End If
                    ElseIf SourceTile.Type = WPFZ80MapEditor.TileSelection.SelectionType.AnimatedTile Then
                        Dim Anim = ZAnim.FromMacro(Map.Scenario.AnimDefs, SourceTile.AnimatedTileDef.Macro & "(" & (Index Mod 16) * 16 & "," & Math.Floor(Index / 16) * 16 & ")")
                        Map.ZAnims.Add(Anim)
                        Map.TileData(Index) = SourceTile.AnimatedTileDef.DefaultImage
                    End If
                End If
            End If
        End If
    End Sub

    Private Sub MapView_MouseMove(sender As Object, e As MouseEventArgs)
        Dim MousePoint = Mouse.GetPosition(sender)
        Dim Index = GetTileIndex(MousePoint.X, MousePoint.Y)
        HotIndex = Index

        Dim Point As New Point(Math.Round(MousePoint.X), Math.Round(MousePoint.Y))
        Dim Args = New CoordinatesUpdatedArgs(MapLayer.CoordinatesUpdatedEvent, Point)
        MyBase.RaiseEvent(Args)
    End Sub

    Private Sub MapView_MouseLeave(sender As Object, e As MouseEventArgs)
        HotIndex = -1
    End Sub
End Class
