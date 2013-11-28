Public Class MapContainer
    Inherits Grid

    Public Shared MapDataProperty As DependencyProperty =
        DependencyProperty.Register("MapData", GetType(MapData), GetType(MapContainer))

    Public Sub New()
        MyBase.New()
        AllowDrop = True
    End Sub

    Public Property MapData As MapData
        Get
            Return GetValue(MapDataProperty)
        End Get
        Set(value As MapData)
            SetValue(MapDataProperty, value)
        End Set
    End Property

    Public Sub New(Map As MapData)
        MyBase.New()

        AllowDrop = True
        Me.DataContext = Map
        SetValue(MapDataProperty, Map)
    End Sub

    Private Sub Map_DragEnter(sender As System.Object, e As System.Windows.DragEventArgs) Handles Me.DragEnter
        Debug.WriteLine("Map_DragEnter!")
        If e.Data.GetDataPresent(GetType(ZDef)) Then
            e.Effects = DragDropEffects.Move
        Else
            Debug.WriteLine("Setting effects to none")
            e.Effects = DragDropEffects.None
        End If
        e.Handled = True
    End Sub

    Private Sub Map_DragOver(sender As System.Object, e As System.Windows.DragEventArgs) Handles Me.DragOver
        Debug.WriteLine("Map_DragOver!")
        If e.Data.GetDataPresent(GetType(ZDef)) Then
            e.Effects = DragDropEffects.Move
        Else
            Debug.WriteLine("Setting effects to none")
            e.Effects = DragDropEffects.None
        End If
        e.Handled = True
    End Sub

    Private Sub Map_Drop(sender As System.Object, e As System.Windows.DragEventArgs) Handles Me.Drop
        If e.Data.GetDataPresent(GetType(ZDef)) Then

            Dim Pos As Point = e.GetPosition(sender)
            Dim Def As ZDef = e.Data.GetData(GetType(ZDef))

            If Not Me.DataContext Is Nothing Then
                CType(Me.DataContext, MapData).AddFromDef(Def, Pos.X, Pos.Y)
            End If
        End If
        e.Handled = True
    End Sub
End Class
