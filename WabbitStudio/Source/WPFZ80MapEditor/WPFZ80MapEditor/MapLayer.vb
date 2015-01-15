
Public Class MapLayer
    Inherits UserControl
    Implements IMapLayer

    Public Overridable Sub DeselectAll() Implements IMapLayer.DeselectAll

    End Sub

    Public Overridable ReadOnly Property LayerType As LayerType Implements IMapLayer.LayerType
        Get
            Return Nothing
        End Get
    End Property

    Public Property Active As Boolean
        Get
            Return GetValue(ActiveProperty)
        End Get

        Set(ByVal value As Boolean)
            SetValue(ActiveProperty, value)
        End Set
    End Property

    Public Shared ReadOnly ActiveProperty As DependencyProperty = _
                           DependencyProperty.Register("Active", _
                           GetType(Boolean), GetType(MapLayer), _
                           New PropertyMetadata(False))

    Protected ReadOnly Property Map As MapData
        Get
            Return DirectCast(DataContext, MapData)
        End Get
    End Property
End Class

Public Interface IMapLayer
    ReadOnly Property LayerType As LayerType

    Sub DeselectAll()
End Interface
