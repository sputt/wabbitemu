Public Class DragAdorner
    Inherits Adorner

    Private _child As UIElement
    Private _owner As UIElement
    Private _brush As VisualBrush

    Private XCenter As Double
    Private YCenter As Double

    Public Sub New(owner As UIElement)
        MyBase.New(owner)
    End Sub

    Public Sub New(owner As UIElement, adornElement As UIElement)
        MyBase.New(owner)

        _owner = owner
        _brush = New VisualBrush(adornElement)
        _brush.Opacity = 0.8
        Dim r As New Rectangle()
        r.RadiusX = 3
        r.RadiusY = 3

        r.Width = adornElement.DesiredSize.Width
        r.Height = adornElement.DesiredSize.Height

        XCenter = adornElement.DesiredSize.Width / 2
        YCenter = adornElement.DesiredSize.Height / 2

        r.Fill = _brush
        _child = r
    End Sub

    Private _leftOffset As Double
    Public Property LeftOffset() As Double
        Get
            Return _leftOffset
        End Get
        Set(ByVal value As Double)
            _leftOffset = value - XCenter
            UpdatePosition()
        End Set
    End Property

    Private _topOffset As Double
    Public Property TopOffset() As Double
        Get
            Return _topOffset
        End Get
        Set(ByVal value As Double)
            _topOffset = value - YCenter
            UpdatePosition()
        End Set
    End Property


    Private Sub UpdatePosition()
        Dim adorner As AdornerLayer = Me.Parent
        If Not adorner Is Nothing Then
            adorner.Update(Me.AdornedElement)
        End If
    End Sub

    Protected Overrides Function GetVisualChild(index As Integer) As Visual
        Return _child
    End Function

    Protected Overrides ReadOnly Property VisualChildrenCount As Integer
        Get
            Return 1
        End Get
    End Property

    Protected Overrides Function MeasureOverride(constraint As System.Windows.Size) As System.Windows.Size
        _child.Measure(constraint)
        Return _child.DesiredSize
    End Function

    Protected Overrides Function ArrangeOverride(finalSize As System.Windows.Size) As System.Windows.Size
        _child.Arrange(New Rect(_child.DesiredSize))
        Return finalSize
    End Function

    Public Overrides Function GetDesiredTransform(transform As System.Windows.Media.GeneralTransform) As System.Windows.Media.GeneralTransform
        Dim result As New GeneralTransformGroup

        result.Children.Add(MyBase.GetDesiredTransform(transform))
        result.Children.Add(New TranslateTransform(_leftOffset, _topOffset))
        Return result
    End Function

End Class
