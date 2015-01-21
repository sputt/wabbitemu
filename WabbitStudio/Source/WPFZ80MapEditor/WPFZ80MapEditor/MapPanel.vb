Public Class MapPanel
    Inherits WrapPanel

    Protected Overrides Function MeasureOverride(availableSize As Size) As Size
        Return New Size(256, 256)
    End Function

    Protected Overrides Function ArrangeOverride(finalSize As Size) As Size
        Dim Index As Integer = 0
        Dim Bounds As New Rect(-128, -128, 16, 16)
        For Each Child As UIElement In InternalChildren
            Child.Arrange(Bounds)
            Index += 1
            If Index Mod 16 = 0 Then
                Bounds.Offset(-15 * 16, 16)
            Else
                Bounds.Offset(16, 0)
            End If
        Next
    End Function

End Class
