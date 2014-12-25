Public Class XObjectLayer
    Public Property Tint As Color
        Get
            Return GetValue(TintProperty)
        End Get

        Set(ByVal value As Color)
            SetValue(TintProperty, value)
        End Set
    End Property

    Public Shared ReadOnly TintProperty As DependencyProperty = _
                           DependencyProperty.Register("Tint", _
                           GetType(Color), GetType(XObjectLayer), _
                           New PropertyMetadata(Nothing))


#Region "Canvas events"
    Private _StartDrag As New Point

    Private Sub ObjectCanvas_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs) Handles ObjectLayer.MouseLeftButtonDown
        SelectionRect.Width = 0
        SelectionRect.Height = 0

        _StartDrag = Mouse.GetPosition(ObjectCanvas)

        If sender.CaptureMouse() Then
            SelectionRect.Visibility = Windows.Visibility.Visible
            'DeselectAll()
            e.Handled = True
        End If
    End Sub

    Private Sub ObjectCanvas_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs) Handles ObjectLayer.MouseLeftButtonUp
        If Mouse.Captured Is sender Then
            sender.ReleaseMouseCapture()
            SelectionRect.Visibility = Windows.Visibility.Hidden

            Dim SelRect As New Rect
            SelRect.X = Canvas.GetLeft(SelectionRect)
            SelRect.Y = Canvas.GetTop(SelectionRect)
            SelRect.Width = SelectionRect.Width
            SelRect.Height = SelectionRect.Height

            SelectObjectsInRect(SelRect)

            e.Handled = True
        End If
    End Sub

    Private Sub ObjectCanvas_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs) Handles ObjectLayer.MouseMove
        If Mouse.Captured Is sender Then
            Dim CurPoint = Mouse.GetPosition(ObjectCanvas)
            CurPoint.X = Math.Max(0, Math.Min(Me.Width, CurPoint.X))
            CurPoint.Y = Math.Max(0, Math.Min(Me.Height, CurPoint.Y))

            Canvas.SetLeft(SelectionRect, Math.Min(CurPoint.X, _StartDrag.X))
            Canvas.SetTop(SelectionRect, Math.Min(CurPoint.Y, _StartDrag.Y))
            SelectionRect.Width = Math.Abs(CurPoint.X - _StartDrag.X)
            SelectionRect.Height = Math.Abs(CurPoint.Y - _StartDrag.Y)

            Dim SelRect As New Rect
            SelRect.X = Canvas.GetLeft(SelectionRect)
            SelRect.Y = Canvas.GetTop(SelectionRect)
            SelRect.Width = SelectionRect.Width
            SelRect.Height = SelectionRect.Height

            SelectObjectsInRect(SelRect)

            e.Handled = True
        End If
    End Sub

    Private Sub SelectObjectsInRect(SelRect As Rect)
        ObjectListBox.SelectedItems.Clear()



        For Each ZObj As ZObject In CType(Me.DataContext, MapData).ZObjects
            Dim IntersectResult As Rect = Rect.Intersect(ZObj.Bounds, SelRect)
            If Not IntersectResult.IsEmpty() Then
                If (IntersectResult.Width * IntersectResult.Height) > (ZObj.Bounds.Width * ZObj.Bounds.Height * 0.75) Then
                    ObjectListBox.SelectedItems.Add(ZObj)
                ElseIf IntersectResult.Equals(SelRect) Then
                    ObjectListBox.SelectedItems.Add(ZObj)
                End If
            End If
        Next
    End Sub
#End Region

End Class
