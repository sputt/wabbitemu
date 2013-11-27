Public Class MapSet
    Inherits Grid
    Implements IMapLayer

    Private Shared _CurrentlySelected As MapSet
    Public Shared Property CurrentlySelected As MapSet
        Get
            Return _CurrentlySelected
        End Get
        Set(value As MapSet)
            If Not _CurrentlySelected Is Nothing Then
                _CurrentlySelected.Children.Remove(_CurrentlySelected._SelectionBorder)
                _CurrentlySelected._SelectionBorder = Nothing
            End If

            If Not value Is Nothing Then
                value._SelectionBorder = New Border
                value._SelectionBorder.BorderThickness = New Thickness(4)
                value._SelectionBorder.BorderBrush = New SolidColorBrush(Color.FromRgb(&H40, &H40, &HFF))
                value._SelectionBorder.Opacity = 0.6
                value.Children.Add(value._SelectionBorder)
            End If
            _CurrentlySelected = value
        End Set
    End Property

    Public WriteOnly Property Active As Boolean Implements IMapLayer.Active
        Set(value As Boolean)
            IsHitTestVisible = value
            DeselectAll()
            If Not _SelectionBorder Is Nothing Then
                Children.Remove(_SelectionBorder)
                _SelectionBorder = Nothing
                Me.Background = New SolidColorBrush(Color.FromArgb(IIf(value, 1, 0), 0, 0, 0))
            End If
        End Set
    End Property

    Public Sub DeselectAll() Implements IMapLayer.DeselectAll
        CurrentlySelected = Nothing
    End Sub

    Public Event MapSelected(MapSet As MapSet)
    Private _SelectionBorder As Border




    Protected Overrides Sub OnMouseEnter(e As System.Windows.Input.MouseEventArgs)
        MyBase.OnMouseEnter(e)

        Me.Background = New SolidColorBrush(Color.FromArgb(20, 255, 255, 255))
    End Sub

    Protected Overrides Sub OnMouseLeave(e As System.Windows.Input.MouseEventArgs)
        MyBase.OnMouseLeave(e)

        Me.Background = New SolidColorBrush(Color.FromArgb(1, 0, 0, 0))
    End Sub

    Protected Overrides Sub OnMouseUp(e As System.Windows.Input.MouseButtonEventArgs)

        CurrentlySelected = Me
        RaiseEvent MapSelected(Me)

        e.Handled = True
    End Sub

    Sub New()
        MyBase.New()

        'Dim Rect As New Rectangle
        'Rect.Fill = New SolidColorBrush(Color.FromArgb(40, 255, 0, 0))
        'Rect.Stretch = Stretch.Fill
        '
        'Children.Add(Rect)

        Me.Background = New SolidColorBrush(Color.FromArgb(1, 0, 0, 0))
    End Sub

End Class
