
Public Class MapLayer
    Inherits UserControl
    Implements IMapLayer

    Public Shared ReadOnly CoordinatesUpdatedEvent As RoutedEvent =
        EventManager.RegisterRoutedEvent("CoordinatesUpdated", RoutingStrategy.Bubble, GetType(RoutedEventHandler), GetType(MapLayer))

    Public Shared ReadOnly LayerChangeRequestedEvent As RoutedEvent =
        EventManager.RegisterRoutedEvent("LayerChangeRequested", RoutingStrategy.Bubble, GetType(RoutedEventHandler), GetType(MapLayer))

    Public Shared ReadOnly SelectionChangeRequested As RoutedEvent =
        EventManager.RegisterRoutedEvent("SelectionChangeRequested", RoutingStrategy.Bubble, GetType(RoutedEventHandler), GetType(MapLayer))

    Public Overridable Sub DeselectAll() Implements IMapLayer.DeselectAll

    End Sub

    Public Shared ReadOnly LayerTypeProperty As DependencyProperty = _
                           DependencyProperty.Register("LayerType", _
                           GetType(LayerType), GetType(MapLayer), _
                           New PropertyMetadata(Nothing))

    Public Overridable Property LayerType As LayerType Implements IMapLayer.LayerType
        Get
            Return GetValue(LayerTypeProperty)
        End Get
        Set(value As LayerType)
            SetValue(LayerTypeProperty, value)
        End Set
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



    Public Property IsSelected As Boolean
        Get
            Return GetValue(IsSelectedProperty)
        End Get

        Set(ByVal value As Boolean)
            SetValue(IsSelectedProperty, value)
        End Set
    End Property

    Public Shared ReadOnly IsSelectedProperty As DependencyProperty = _
                           DependencyProperty.Register("IsSelected", _
                           GetType(Boolean), GetType(MapLayer), _
                           New PropertyMetadata(False))




    Protected ReadOnly Property Map As MapData
        Get
            Return DirectCast(DataContext, MapData)
        End Get
    End Property

    Public Overridable Function CanPaste() As Boolean Implements IMapLayer.CanPaste
        Return False
    End Function

    Public Overridable Sub Paste() Implements IMapLayer.Paste
    End Sub
End Class

Public Interface IMapLayer
    Property LayerType As LayerType

    Sub DeselectAll()
    Function CanPaste() As Boolean
    Sub Paste()
End Interface

Public Class CoordinatesUpdatedArgs
    Inherits RoutedEventArgs

    Public Property Point As Point

    Public Sub New(RoutedEvent As RoutedEvent, Point As Point)
        MyBase.New(RoutedEvent)
        Me.Point = Point
    End Sub
End Class

Public Class LayerChangeRequestedArgs
    Inherits RoutedEventArgs

    Public Property LayerType As LayerType

    Public Sub New(RoutedEvent As RoutedEvent, LayerType As LayerType)
        MyBase.New(RoutedEvent)
        Me.LayerType = LayerType
    End Sub
End Class

Public Class SelectionChangeRequestedArgs
    Inherits RoutedEventArgs

    Public Property Map As MapData

    Public Sub New(RoutedEvent As RoutedEvent, Map As MapData)
        MyBase.New(RoutedEvent)
        Me.Map = Map
    End Sub
End Class
