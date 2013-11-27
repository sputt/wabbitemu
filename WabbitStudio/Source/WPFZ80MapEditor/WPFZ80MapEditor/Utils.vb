Public Class Utils
    Public Shared Iterator Function FindChildren(Of T As DependencyObject)(source As DependencyObject) As IEnumerable(Of T)
        For Each Child In GetChildObjects(source)
            If Child IsNot Nothing AndAlso TypeOf Child Is T Then
                Yield Child
            End If

            For Each Descendant In FindChildren(Of T)(Child)
                Yield Descendant
            Next
        Next
    End Function

    Public Shared Iterator Function GetChildObjects(Parent As DependencyObject) As IEnumerable(Of DependencyObject)
        Dim HasValue As Boolean = False
        If TypeOf Parent Is ContentElement Or TypeOf Parent Is FrameworkElement Then
            For Each Obj In LogicalTreeHelper.GetChildren(Parent)
                Dim DepObj As DependencyObject = TryCast(Obj, DependencyObject)
                If DepObj IsNot Nothing Then
                    HasValue = True
                    Yield DepObj
                End If

            Next
        End If
        If Not HasValue Then
            Try
                If Not TypeOf Parent Is RowDefinition And Not TypeOf Parent Is ColumnDefinition Then
                    Dim Count = VisualTreeHelper.GetChildrenCount(Parent)

                    For i = 0 To Count - 1
                        Yield VisualTreeHelper.GetChild(Parent, i)
                    Next
                End If
            Catch e As Exception
            End Try
        End If
    End Function

    Public Shared Function FindVisualParent(Of T As DependencyObject)(Child As DependencyObject) As T
        Dim ParentObj = VisualTreeHelper.GetParent(Child)

        If ParentObj Is Nothing Then Return Nothing

        Dim Parent As T = TryCast(ParentObj, T)
        If Parent IsNot Nothing Then
            Return Parent
        Else
            Return FindVisualParent(Of T)(ParentObj)
        End If
    End Function
End Class
