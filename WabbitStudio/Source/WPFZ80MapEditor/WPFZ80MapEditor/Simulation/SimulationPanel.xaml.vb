Public Class SimulationPanel

    Private ReadOnly Property GameModel As GameModel
        Get
            Return DataContext.GameModel
        End Get
    End Property

    Private Sub RetargetButton_Checked(sender As Object, e As RoutedEventArgs) Handles RetargetButton.Checked
        GameModel.EnableRetargetMode(True)
    End Sub

    Private Sub RetargetButton_Unchecked(sender As Object, e As RoutedEventArgs) Handles RetargetButton.Unchecked
        'GameModel.EnableRetargetMode(False)
    End Sub

    Private Sub InventoryListBox_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
        If e.AddedItems.Count > 0 Then
            GameModel.SelectItem(e.AddedItems(0))
            GameMapView.Instance.Focus()
        End If
    End Sub

End Class
