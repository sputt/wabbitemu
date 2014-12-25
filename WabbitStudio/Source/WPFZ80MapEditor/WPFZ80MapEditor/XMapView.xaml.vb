Public Class XMapView
    Implements IMapLayer

    Private Sub TileButton_Click(Sender As Object, e As RoutedEventArgs)
        'Dim Map As MapData = DataContext

        'Dim TileList = Utils.FindChildren(Of Grid)(Me)
        'Dim Index = TileList.ToList().IndexOf(Sender)

        'If MainWindow.Instance.TileSelectorPanel.Index <> -1 Then
        '    Map.TileData(Index) = MainWindow.Instance.TileSelectorPanel.Index
        '    Map.Tileset = MainWindow.Instance.TileSelectorPanel.CurrentTileset.SelectedValue
        'End If
    End Sub

    Public WriteOnly Property Active As Boolean Implements IMapLayer.Active
        Set(value As Boolean)

        End Set
    End Property

    Public Sub DeselectAll() Implements IMapLayer.DeselectAll
    End Sub

    Private Sub TileGrid_MouseEnter(sender As Object, e As MouseEventArgs)
        Dim result = VisualStateManager.GoToState(sender, "MouseOver", True)
    End Sub

    Private Sub TileGrid_MouseLeave(sender As Object, e As MouseEventArgs)
        Dim result = VisualStateManager.GoToState(sender, "Normal", True)
    End Sub



End Class
