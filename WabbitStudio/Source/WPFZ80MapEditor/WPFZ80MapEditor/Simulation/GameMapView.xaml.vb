Public Class GameMapView
    Public Sub New()
        InitializeComponent()

        AddHandler DataContextChanged,
            Sub()
                If DataContext IsNot Nothing Then
                    AddHandler CType(DataContext, GameModel).Started,
                        Sub()
                            Me.Focus()
                        End Sub
                End If
            End Sub
    End Sub

    Private Sub GameWindow_KeyDown(sender As Object, e As KeyEventArgs)
        If Not e.IsRepeat Then
            DataContext.PressKey(e.Key)
        End If
        e.Handled = True
    End Sub

    Private Sub GameWindow_KeyUp(sender As Object, e As KeyEventArgs)
        DataContext.ReleaseKey(e.Key)
        e.Handled = True
    End Sub

    Private Sub GameWindow_MouseLeftButtonDown(sender As Object, e As MouseButtonEventArgs)
        Me.Focus()
    End Sub
End Class
