Imports System.Windows.Media.Animation

Public Class AnimatedTile

    Public Property CurrentAnimValue As Integer
        Get
            Return GetValue(CurrentAnimValueProperty)
        End Get

        Set(ByVal value As Integer)
            SetValue(CurrentAnimValueProperty, value)
        End Set
    End Property

    Public Shared ReadOnly CurrentAnimValueProperty As DependencyProperty = _
                           DependencyProperty.Register("CurrentAnimValue", _
                           GetType(Integer), GetType(AnimatedTile), _
                           New PropertyMetadata(-1))


    Public Property MapData As MapData
        Get
            Return GetValue(MapDataProperty)
        End Get

        Set(ByVal value As MapData)
            SetValue(MapDataProperty, value)
        End Set
    End Property

    Public Shared MapDataProperty As DependencyProperty =
        DependencyProperty.Register("MapData", GetType(MapData), GetType(AnimatedTile))


    Private Sub AnimatedTile_Loaded(sender As Object, e As RoutedEventArgs) Handles MyBase.Loaded, MyBase.Loaded
        'Dim StoryBoard As Storyboard = CType(Resources("EntryToStoryboardConverter"), IValueConverter).Convert(DataContext.ExtractOnCount,
        '                                                                                         GetType(Storyboard), Nothing, Nothing)
        'StoryBoard.Begin(Me)
    End Sub
End Class
