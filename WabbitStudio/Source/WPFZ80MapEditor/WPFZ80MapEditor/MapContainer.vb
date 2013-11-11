Public Class MapContainer
    Inherits Grid

    Public Shared MapDataProperty As DependencyProperty =
        DependencyProperty.Register("MapData", GetType(MapData), GetType(MapContainer))

    Public Property MapData
        Get
            Return GetValue(MapDataProperty)
        End Get
        Set(value)
            SetValue(MapDataProperty, value)
        End Set
    End Property

    Public Sub New(Map As MapData)
        MyBase.New()

        Me.DataContext = Map
        SetValue(MapDataProperty, Map)
    End Sub
End Class
