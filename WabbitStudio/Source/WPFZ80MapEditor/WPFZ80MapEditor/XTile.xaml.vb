Public Class XTile
    'Inherits UserControl

    Public Shared ReadOnly IndexProperty As DependencyProperty =
        DependencyProperty.Register("Index", GetType(Double), GetType(XTile),
                                    New FrameworkPropertyMetadata(-1.0, FrameworkPropertyMetadataOptions.AffectsRender))
    Public Shared ReadOnly IsAnimatedProperty As DependencyProperty =
        DependencyProperty.Register("IsAnimated", GetType(Boolean), GetType(XTile),
                                    New FrameworkPropertyMetadata(False))
    Public Shared ReadOnly AnimDefProperty As DependencyProperty =
        DependencyProperty.Register("AnimDef", GetType(ZDef), GetType(XTile))

    Public Property IsAnimated
        Get
            Return GetValue(IsAnimatedProperty)
        End Get
        Set(value)
            SetValue(IsAnimatedProperty, value)
        End Set
    End Property
End Class
