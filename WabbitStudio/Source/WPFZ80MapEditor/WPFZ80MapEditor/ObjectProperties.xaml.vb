Public Class ObjectProperties

    Private Sub CancelButton_Click(sender As System.Object, e As System.Windows.RoutedEventArgs) Handles CancelButton.Click

    End Sub

    Private Sub OKButton_Click(sender As System.Object, e As System.Windows.RoutedEventArgs) Handles OKButton.Click
        Me.DialogResult = True
        Me.Close()
    End Sub

    Private Sub SelectLabel_Click(sender As Object, e As RoutedEventArgs)

    End Sub

    Private Sub EnableNamedSlot_Click(sender As Object, e As RoutedEventArgs)
        If EnableNamedSlot.IsChecked Then
            NamedSlot.Focus()
        Else
            DataContext.NamedSlot = Nothing
        End If
    End Sub
End Class

Public Class ArgToColumnConverter
    Inherits OneWayConverter(Of Object, Integer)

    Public Overrides Function Convert(Value As Object, Parameter As Object) As Integer
        If TypeOf Value Is ArgNameAndIndex Then
            Return 0
        Else
            Return 1
        End If
    End Function
End Class

Public Class ObjectIDCollection
    Inherits List(Of String)
End Class